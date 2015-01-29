/** \file Server.h 
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#ifndef SERVER_H
#define SERVER_H

#include <mutex>
#include <boost/asio.hpp>
//#include <boost/thread/mutex.hpp>

#include "Client.h"
#include "Map.h"
#include "Message.h"

/** \file Sever.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

using boost::asio::ip::tcp;
class Game;

/**
 * \class Server
 * \brief Třída pro komunikaci s klienty
 */
class Server {
public:
	Server(boost::asio::io_service& io, int port): _io_service(io), _port{port} {};

	/* načtení map z disku */
	void load_maps();

	/* spuštění obsluhy klientů */
	void start();

	/* čtení zprávy od klienta*/
	static void read(tcp::socket& sock, std::stringstream& ss);
	/* zaslání zprávy */
	static void write(tcp::socket& sock, std::string msg);

	~Server();
private:
	/* umožnění vracet hře klienty do lobby */
	friend class Game;

	/* načtení jedné mapy */
	void load_map(std::string map_file);

	/* lobby */
	void lobby_thread();

	/* přidání klienta do lobby */
	void lobby_add_client(tcp::socket& sock);

	/* odebrání klienta z lobby*/
	void lobby_remove_client(int index);

	/* vytvoření hry */
	int game_create(std::stringstream& ss, tcp::socket& sock);

	/* odebrání hry */
	void game_delete(Game *game_ptr);

	/* přidání hráče do hry */
	bool game_add_player(int index, tcp::socket& sock);

	/* přidání hráce do hry na základě požadavků od uživatele */
	bool game_add_player(std::stringstream& ss, tcp::socket& sock);

	/* odebrání hráče ze hry */
	void game_remove_player(int index, tcp::socket& sock);

	/* obsluha požadavku klienta*/
	bool serve_client(tcp::socket& sock);

	Message_server _message;

	boost::asio::io_service& _io_service;

	int _ids{};
	int _port;
	std::vector<struct map_form_t> _map_vec{};
	std::vector<Game *> _game_vec{};
	std::vector<tcp::socket> _lobby_vec{};

	/* zámek pro přidávání/odebírání klientů do/z lobby */
	std::mutex _lobby_lock;

	/* zámek pro přídávání/odebírání her */
	//std::mutex _all_games_lock;
};

#endif /* SERVER_H */