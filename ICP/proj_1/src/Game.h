/** \file Game.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#ifndef GAME_H
#define	GAME_H

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Map.h"
#include "Client.h"
#include "Server.h"

using boost::asio::ip::tcp;

namespace bpt = boost::posix_time;

/**
 * \struct player_info
 * \brief Informace o hráči
 * 
 * Struktura obsahuje informace o aktuálním stavu záznamu (status), jestli se 
 * pohybuje (move), barvu hráče (player_color), počet ušlých kroků (steps),
 * socket na klienta (sock), čas připojení a případného odpojení hráče 
 * (begin_time, end_time)
 * \warning end_time je platný jen pokud byl hráč odpojen (status == disconnected). 
 * Sock je platný jen pokud hráč není odpojen (status != disconnected) !!!
 */
struct player_info {

	enum Status {
		NEW, IN_GAME, DISCONNECTED
	} status;

	enum Move {
		STOP, GO
	} move;
	uint16_t player_color;
	uint32_t steps;
	uint32_t kill_count;
	tcp::socket sock;
	bpt::ptime begin_time;
	bpt::ptime end_time;

	player_info(uint16_t color, tcp::socket& player_sock)
	: sock(std::move(player_sock)) {
		player_color = color;
		status = NEW;
		move = STOP;
		steps = 0;
		kill_count = 0;
		begin_time = end_time = bpt::second_clock::local_time();
	}
};

/**
 * \class Game
 * \brief Třída reprezentující spuštěnou hru
 */
class Game {
public:
	Game(int max, float delay, std::string name, map_form_t map, Server *server);
	virtual ~Game();

	/* informace o hře */
	std::string info();

	/* zaplněnost */
	bool full() {
		return _act_players == _max_players;
	}

	/* vložení nového hráče */
	bool add_player(tcp::socket& sock);


private:
	/* herní vlákno */
	void game_thread();

	/* obsloužení požadavků od klientů */
	void serve_client(player_info &player);

	/* vytvoření statistiky o hře a hráčích */
	std::string generate_statistics();

	int _act_players{0};
	int _max_players{4};
	float _delay{0.5};
	std::string _name{};
	std::vector<player_info> _players{};
	Map *_game_map;
	bpt::ptime _game_start_time;

	/* zámek pro přidávání/odebírání hráčů do/ze hry */
	boost::mutex _game_lock;

	/* instance třídy server, ke které hra patří*/
	Server *_server{nullptr};
};

#endif	/* GAME_H */

