/** \file Client.h 
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#ifndef CLIENT_H
#define	CLIENT_H

#include <thread>
#include <boost/asio.hpp>
#include "Message.h"

/**
 * \struct map_info_t
 * \brief struktura se základními informacemi o mapě
 */
struct map_info_t {
	int id;
	std::string name;
};

/**
 * \struct map_form_t
 * \brief struktura reprezentujici podobu mapy
 */
struct map_form_t {
	int rows;
	int cols;
	std::string items;
	std::string name;
};

/**
 * \struct game_info_t
 * \brief struktura s informacemi o hře
 */
struct game_info_t {
	int id;
	int act_players;
	int max_players;
	std::string name;
};

/**
 * \class Client
 * \brief Třída pro komunikaci klienta se serverem
 */
class Client {
public:
	using mapvec = std::vector<struct map_info_t>;
	using gamevec = std::vector<struct game_info_t>;

	Client(boost::asio::io_service& io) : _io(io), _socket(io) {
	}
	~Client();

	/* připojení k serveru */
	void connect(std::string &host, std::string &port);
	void connect(const char *host, const char *port);

	/* odpojení od serveru */
	void disconnect();

	/* žádost o seznam map a her */
	mapvec get_maps();
	gamevec get_games();

	/* pripojeni do bezici hry */
	int join(int game);

	/* vytvoreni nové hry */
	int create(int game, double delay, int max_players, const char *name);

	/* žádost o provedení činnosti */
	void left() {
		this->write(_message.left());
	}

	void right() {
		this->write(_message.right());
	}

	void go() {
		this->write(_message.go());
	}

	void stop() {
		this->write(_message.stop());
	}

	void take() {
		this->write(_message.take());
	}

	void open() {
		this->write(_message.open());
	}
	void leave();

	/* čtení změn na napě */
	void listen(std::function<void(std::stringstream&) > handler);
	void listen_this_thread(std::function<void(std::stringstream&) > handler);

	/* konec čtení */
	void stop_listen();

	/* gettery */
	int id() {
		return _id;
	}

	int color() {
		return _color;
	}

	struct map_form_t *get_map() {
		return &_map;
	}

	std::string last() {
		return _ss.str();
	}

private:
	/* zápis do socketu */
	void write(std::string msg);
	void write_once(std::string msg);

	/* vyčištění streamu */
	void clear_stream();

	/* čtení ze socketu */
	void read();
	void read_async(std::function<void(std::stringstream&) > handler);

	/* kontrola kódu */
	void check_code(int expected);

	/* načtení informací o mapě */
	void read_map();

	/* zprávy pro komunikaci*/
	Message _message;

	/* prostředky pro komunikaci */
	boost::asio::io_service& _io;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::streambuf _strbuf;

	bool _connected = false;
	bool _ok = true;
	bool _listening = false;
	int _id = 0;
	int _color;
	int _code = 0;
	char _ending = '\n';
	std::string _msg{};
	std::stringstream _ss;
	struct map_form_t _map;

	std::thread *_io_thread{};
};

/**
 * \class Client_e
 * \brief Vyjimka při operacich klienta
 */
class Client_e : public std::runtime_error {
public:

	Client_e(const char *msg) : std::runtime_error(msg) {
	}
};

#endif	/* CLIENT_H */

