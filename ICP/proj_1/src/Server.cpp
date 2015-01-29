/** \file Server.cpp 
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#include "Server.h"
#include "Map.h"
#include "Message.h"
#include "Game.h"

#include <thread>
#include <chrono>
#include <fstream>

Server::~Server()
{
}

/**
 * \brief Načte mapu ze zadaného souboru, vytvoří a uloží strukturu do vektoru
 * \param[in] map_file název mapy
 */
void Server::load_map(std::string map_file)
{
	std::ifstream map_stream("maps/" + map_file);
	if (!map_stream.is_open()) {
		throw std::runtime_error("Nelze otevrit soubor \"" + map_file + "\"!");
	}

	bool ok{true};
	std::string line;
	std::stringstream map_des;
	struct map_form_t new_map;

	new_map.name = map_file;
	try {
		/* načtení rozměrů mapy */
		ok = map_stream >> new_map.rows >> new_map.cols;
		if (!ok) {
			throw std::runtime_error("Spatny format souboru \"" + map_file + "\"!");
		}

		/* načtení samotného herního pole */
		while (std::getline(map_stream, line)) {
			map_des << line;
		}

		new_map.items = map_des.str();

	} catch (std::exception &e) {
		map_stream.close();
		throw;
	}

	map_stream.close();
	_map_vec.push_back(new_map);
}

/**
 * \brief Načte všechny mapy z adresáře maps a vytvoří vektor struktur map
 */
void Server::load_maps()
{
	std::string map_file;
	std::ifstream map_list("maps/maps");

	if (!map_list.is_open()) {
		throw std::runtime_error("Nepodarilo se otevrit seznam map!");
	}

	try {
		while (std::getline(map_list, map_file)) {
			load_map(map_file);
		}
	} catch (std::exception& e) {
		map_list.close();
		throw;
	}

	map_list.close();
}

/**
 * \brief Přečte zprávu ze socketu do zadaného streamu
 * \param[in] sock Socket klienta
 * \param[out] ss Přečtená zpráva
 */
void Server::read(tcp::socket& sock, std::stringstream& ss)
{
	boost::asio::streambuf buf;

	boost::asio::read_until(sock, buf, "\n");

	std::istream is(&buf);
	is >> ss.rdbuf();
}

/**
 * \brief Odešle zprávu klientovi
 * \param[in] sock Socket klienta
 * @param[in] msg Zpráva pro klienta
 */
void Server::write(tcp::socket& sock, std::string msg)
{
	boost::asio::write(sock, boost::asio::buffer(msg));
}

/**
 * \brief Obsloužení socketu, na kterém je příchozí zpráva
 * \param sock Socket klienta
 * \return Info, jestli klient opustil lobby
 */
bool Server::serve_client(tcp::socket& sock)
{
	int code;
	std::stringstream request;
	read(sock, request);
	request >> code;

	switch (code) {
	case 100:
		/* welcome */
		write(sock, _message.welcome(_ids++));
		break;
	case 111:
		/* žádost o seznam map */
		for (uint16_t i = 0; i < _map_vec.size(); ++i) {
			write(sock, _message.map_info(i, _map_vec[i].name));
		}
		write(sock, _message.map_info_end());
		break;
	case 112:
		/* žádost o seznam her */
		//_all_games_lock.lock();
		for (uint16_t i = 0; i < _game_vec.size(); ++i) {
			write(sock, _message.game_info(i, _game_vec[i]->info()));
		}
		write(sock, _message.game_info_end());
		//_all_games_lock.unlock();
		break;
	case 120:
		/* žádost o vytvoření hry */
		//_all_games_lock.lock();
		if (game_create(request, sock) >= 0) {
			//	_all_games_lock.unlock();
			return true;
		} else {
			write(sock, _message.request_denied());
		}
		//_all_games_lock.unlock();
		break;
	case 130:
		/* žádost o připojení do hry */
		//_all_games_lock.lock();
		if (game_add_player(request, sock)) {
			//	_all_games_lock.unlock();
			return true;
		} else {
			write(sock, _message.request_denied());
		}
		//_all_games_lock.lock();
		break;
	case 195:
		/* bye bye */
		sock.close();
		return true;
	default:
		/* neznámý požadavek */
		write(sock, _message.unkwnown());
		break;
	}

	return false;
}

/**
 * \brief Lobby, ve kterém klienti žádají o mapy, vytvářejí nové hry atp.
 */
void Server::lobby_thread()
{
	for (;;) {
		for (uint16_t i = 0; i < _lobby_vec.size(); ++i) {
			/* pokud se některý klient odpojil nebo se přesunul do hry, odebere
			 * se z klientů v lobby
			 */
			if (!_lobby_vec[i].is_open() ||
					(_lobby_vec[i].available() && serve_client(_lobby_vec[i]))) {
				lobby_remove_client(i);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

/**
 * \brief Přidání klienta do lobby
 * \param sock Socket klienta
 */
void Server::lobby_add_client(tcp::socket& sock)
{
	_lobby_lock.lock();
	_lobby_vec.push_back(std::move(sock));
	_lobby_lock.unlock();
}

/**
 * \brief Odebrání klienta z lobby
 * \param index Index do vektoru klientu v lobby
 */
void Server::lobby_remove_client(int index)
{
	_lobby_lock.lock();
	_lobby_vec[index] = std::move(_lobby_vec.back());
	_lobby_vec.pop_back();
	_lobby_lock.unlock();
}

/**
 * \brief Připojení nových klientů
 */
void Server::start()
{
	tcp::acceptor acc(_io_service, tcp::endpoint(tcp::v4(), _port));
	std::thread(&Server::lobby_thread, this).detach();
	for (;;) {
		tcp::socket sock(_io_service);
		acc.accept(sock);
		lobby_add_client(sock); // socket MOVED !!
	}
}

/**
 * \brief Vytvoření nové hry
 * Vytvořená hry, bude vložena jako poslední do vektro her _game_vec.
 * Pozn: je nutné, aby při použití byl vlastněn zámek _all_games_lock
 * \param[in] ss Požadavky na novou hru
 * \return Při úspěšném vytvoření vrací index hry, jinak zápornou hodnotu.
 */
int Server::game_create(std::stringstream& ss, tcp::socket& sock)
{
	uint16_t map_id, max_players;
	float delay;
	std::string name;
	bool msg_status;

	msg_status = (ss >> map_id >> delay >> max_players >> name);
	if (!msg_status || map_id >= _map_vec.size()) {
		return -1;
	}

	Game *new_game;

	/* vytvoření třídy nové hry */
	try {
		map_form_t map_config = _map_vec[map_id];
		new_game = new Game(max_players, delay, name, map_config, this);
	} catch (std::exception& e) {
		/* vytváření hry selhalo */
		return -1;
	}

	/* uložení hry a připojení zakládajícího hráče */
	_game_vec.push_back(new_game);
	int game_id = _game_vec.size() - 1;
	if (!game_add_player(game_id, sock)) {
		_game_vec.pop_back();
		delete new_game;
		return -1;
	}

	return game_id;
}

/**
 * \brief Odebrání hry ze seznamu dostupných her
 * \param[in] game_ptr Ukazatel hry, která se má odebrat
 */
void Server::game_delete(Game *game_ptr)
{
	//_all_games_lock.lock();
	for (auto it = _game_vec.begin(); it != _game_vec.end(); ++it) {
		if (*it == game_ptr) {
			*it = _game_vec.back();
			_game_vec.pop_back();
			return;
		}
	}
	//_all_games_lock.unlock();
}

/**
 * \brief Připojení hráče ke hře
 * Pozn: je nutné vlastnit zámek _all_games_lock
 * \param[in] index Index hry
 * \param[in] sock Socket klienta
 * \return Info, jestli se podařilo připojit klienta do hry
 */
bool Server::game_add_player(int index, tcp::socket& sock)
{
	if (index >= static_cast<int> (_game_vec.size()) || index < 0) {
		return false;
	}

	return _game_vec[index]->add_player(sock);
}

/**
 * \brief Připojení hráče ke hře dle požadavku klienta
 * Pozn: je nutné vlastnit zámek _all_games_lock
 * \param[in] ss Stream zprávy, kterou poslal klient
 * \param[in] sock Socket klienta
 * \return Info, jestli se podařilo připojit klienta do hry
 */
bool Server::game_add_player(std::stringstream& ss, tcp::socket& sock)
{
	int game_id;
	bool status = (ss >> game_id);
	if (status) {
		return game_add_player(game_id, sock);
	}

	return false;
}