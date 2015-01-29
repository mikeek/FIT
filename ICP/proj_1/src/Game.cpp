/** \file Game.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#include "Game.h"
#include "Server.h"

/* maximální počet hráčů */
const int max_players_limit{4};
const int min_players_limit{1};
const float min_delay_limit{0.5};
const float max_delay_limit{5.0};

/**
 * \brief Konstruktor, vytvoření hry
 * \param[in] max Max. počet hráčů
 * \param[in] delay Refresh
 * \param[in] name Název hry
 */
Game::Game(int max, float delay, std::string name, map_form_t map, Server *server)
{
	if (max < min_players_limit || max > max_players_limit) {
		throw std::runtime_error("Počet povolených hráčů nespadá do limitu");
	}

	if (delay < min_delay_limit || delay > max_delay_limit) {
		throw std::runtime_error("Doba obnovy nespadá do limitu");
	}

	_act_players = 0;
	_max_players = max;
	_name = name;
	_delay = delay;
	_game_map = new Map({map.rows, map.cols}, map.items);
	_game_start_time = bpt::second_clock::local_time();
	_server = server;
}

/**
 * \brief Destruktor
 */
Game::~Game()
{
	delete _game_map;
}

/**
 * \brief Získání informací o hře
 * \return string s informacemi
 */
std::string Game::info()
{
	std::stringstream ss;
	ss << _act_players << " " << _max_players << " " << _name;
	return ss.str();
}

/**
 * \brief Přidání hráče do hry
 * Při úspěšném založení přesune socket hráče do hry
 * \param[in] sock Socket hráče
 * \return Info, jestli by hráč přidán do hry
 */
bool Game::add_player(tcp::socket& sock)
{
	if (full()) {
		return false;
	}

	_game_lock.lock();

	/* umístění hráče do mapy */
	uint16_t color;
	if (!_game_map->add_player(color)) {
		/* umístění hráče selhalo */
		_game_lock.unlock();
		return false;
	}

	_players.push_back(player_info(color, sock));

	Server::write(_players.back().sock, Message_server::map_description(
			_players.back().player_color, _game_map->get_map_description()));

	/* první vstupující hráč spouští obslužné vlákno hry */
	if (_players.size() == 1) {
		std::thread new_thread(&Game::game_thread, this);
		new_thread.detach();
	}

	_act_players++;
	_game_lock.unlock();
	return true;
}

/**
 * \brief Vlákno řídící činnost aktivní hry
 * Zpracovává požadavky od klientů (změna směru, zahájení a ukončení pohybu,
 * případně žádost o odechod ze hry), provádí všechny pohyby na mapě včetně 
 * stráží, odesílá změny vzniklé v daném "kroku" hry, kontoroluje ukončení hry
 * a rozesílá závěrečné statistiky.
 */
void Game::game_thread()
{
	bool game_over = false;

	for (;;) {
		_game_lock.lock();

		if (_act_players == 0) {
			/* ve hře se nikdo nenachází -> smazání mapy */
			_server->game_delete(this);
			_game_lock.unlock();

			/* spáchání sebevraždy této třídy */
			delete this;
			return;
		}

		/* obsloužení všech požadavků klientů */
		for (auto it = _players.begin(); it != _players.end(); ++it) {
			if (it->status == player_info::DISCONNECTED) {
				continue;
			}

			serve_client(*it);

			/* případné posunutí na mapě */
			if (it->move == player_info::GO) {
				if (_game_map->player_action(it->player_color, Map::A_STEP)) {
					/* hráč se posunul po poli */
					it->steps += 1;
				}
			}
		}

		/* ověření, jestli již bylo oznámeno ukončení hry */
		if (game_over) {
			_game_lock.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}

		/* posun hlídačů na mapě */
		_game_map->guards_move();

		/* získání informace jestli hráč byl zabit strážcem - pro statistiku */
		for (auto it = _players.begin(); it != _players.end(); ++it) {
			if (_game_map->is_player_killed(it->player_color)) {
				it->kill_count++;
				it->move = player_info::STOP;
			}
		}

		/* odeslání změn na mapě klientům */
		std::string map_changes = _game_map->get_objects_config(true);
		for (auto it = _players.begin(); it != _players.end(); ++it) {
			switch (it->status) {
			case player_info::NEW:
				/* nový hráč - dostane veškeré změny oproti původní mapě */
				Server::write(it->sock, Message_server::map_changes(
						_game_map->get_objects_config(false)));
				it->status = player_info::IN_GAME;
				break;
			case player_info::IN_GAME:
				/* stávající hráč - dostane jen rozdílové změny */
				Server::write(it->sock, Message_server::map_changes(map_changes));
				break;
			case player_info::DISCONNECTED:
				/* nic se neodesílá */
				break;
			}
		}

		/* oživení mrtvých hráčů */
		_game_map->revive_dead_players();

		/* test ukončení hry */
		if (_game_map->is_game_over()) {
			/* hra končí */
			game_over = true;
			std::string statistics = generate_statistics();
			std::string end_game_report = _game_map->game_over_report();
			for (auto it = _players.begin(); it != _players.end(); ++it) {
				if (it->status == player_info::DISCONNECTED) {
					continue;
				}

				/* odeslání závěrečných zpráv */
				Server::write(it->sock, Message_server::game_statistics(
						statistics));
				Server::write(it->sock, Message_server::game_over_report(
						end_game_report));
			}
		}

		_game_lock.unlock();

		/* čekání dokud nemá dojít k dalšímu kroku hrys */
		std::this_thread::sleep_for(std::chrono::milliseconds(
				static_cast<int> (1000 * _delay)));
	}
}

/**
 * \brief Rozpoznání a vykonání požadavků od klienta v rámci aktivní hry
 * Čte ze soketu klienta a rozpoznává a vykonává požadavky na zastavení/spuštění
 * pohybu, změnu směru, otevření brány, zvednutí klíče a odpojení ze hry
 * \param player Struktura s informacemi o klientovi
 */
void Game::serve_client(player_info &player)
{
	/* test existence dat připravených na socketu */
	if (!player.sock.available()) {
		return;
	}

	/* přečtení všech příchozích zpráv od klienta */
	boost::asio::streambuf b;
	boost::asio::streambuf::mutable_buffers_type bufs =
			b.prepare(player.sock.available());
	size_t n = player.sock.receive(bufs);
	b.commit(n);

	/* zpracování jednotlivých zpráv */
	std::istream is(&b);
	std::string line;
	while (getline(is, line)) {
		std::stringstream ss(line);
		int code;
		if (!(ss >> code)) {
			continue;
		}

		switch (code) {
		case 150:
			/* žádost GO */
			player.move = player_info::GO;
			Server::write(player.sock, Message_server::player_oper_confirm(
					Message_server::GO, true));
			break;
		case 151:
			/* žádost STOP */
			player.move = player_info::STOP;
			Server::write(player.sock, Message_server::player_oper_confirm(
					Message_server::STOP, true));
			break;
		case 152:
			/* žádost LEFT */
			Server::write(player.sock, Message_server::player_oper_confirm(
					Message_server::LEFT,
					_game_map->player_action(player.player_color, Map::A_LEFT)));
			break;
		case 153:
			/* žádost RIGHT */
			Server::write(player.sock, Message_server::player_oper_confirm(
					Message_server::RIGHT,
					_game_map->player_action(player.player_color, Map::A_RIGHT)));
			break;
		case 154:
			/* žádost TAKE */
			Server::write(player.sock, Message_server::player_oper_confirm(
					Message_server::TAKE,
					_game_map->player_action(player.player_color, Map::A_PICK_UP)));
			break;
		case 155:
			/* žádost OPEN */
			Server::write(player.sock, Message_server::player_oper_confirm(
					Message_server::OPEN,
					_game_map->player_action(player.player_color, Map::A_OPEN)));
			break;
		case 199:
			/* žádost LEAVE_GAME*/
			player.status = player_info::DISCONNECTED;
			player.end_time = bpt::second_clock::local_time();
			player.move = player_info::STOP;
			_act_players--;
			_game_map->remove_player(player.player_color);
			Server::write(player.sock, Message_server::player_gexit_confirm());
			_server->lobby_add_client(player.sock);
			return;
		default:

			Server::write(player.sock, Message_server::unkwnown());
			break;
		}
	}
}

/**
 * \brief Vytvoření statistiky o hře a hráčích
 * Vygeneruje řetězec určený pro klienta při ukončení hry s informacemi 
 * o době hry + času stráveném hráči ve hře a počtu jejich provedených kroků.
 * Formát: cas_hry P barva_hrace cas_hrace pocet_kroku P barva_hrace ....
 * \return Řetězec s formátovanou statistikou
 */
std::string Game::generate_statistics()
{
	std::stringstream ss;

	/* doba trvání hry */
	bpt::time_duration dur = bpt::second_clock::local_time() - _game_start_time;
	ss << dur.total_seconds();

	/* informace o jednotlivých hráčích */
	for (auto it = _players.begin(); it != _players.end(); ++it) {
		bpt::time_duration player_dur;

		/* výpočet doby strávené hráčem ve hře */
		if (it->status == player_info::DISCONNECTED) {
			/* hráč opustil hru dříve než zkončila */
			player_dur = it->end_time - it->begin_time;
		} else {
			/* hráč zůstal ve hře až do jejího konce */
			player_dur = bpt::second_clock::local_time() - it->begin_time;
		}

		ss << " P " << it->player_color << " " << player_dur.total_seconds() <<
				" " << it->steps << " " << it->kill_count;
	}

	return ss.str();
}
