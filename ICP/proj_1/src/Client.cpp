/** \file Client.cpp 
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#include <iostream>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <thread>
#include <memory>

#include "Client.h"

/**
 * \brief Vyčištění stringstreamu
 */
void Client::clear_stream()
{
	_ss.clear();
	_ss.str("");
}

/**
 * \brief Spuštění asynchronního čtení v aktuálním vlákně
 * \param[in] handler Obslužná funkce pro zpracování dat
 */
void Client::listen_this_thread(std::function<void(std::stringstream &) > handler)
{
	read_async(handler);
	_listening = true;
	_io.reset();
	_io.run();
}

/**
 * \brief Spuštění asynchronního čtení v novém vlákně
 * \param[in] handler Obslužná funkce pro zpracování dat
 */
void Client::listen(std::function<void(std::stringstream &) > handler)
{
	read_async(handler);
	_listening = true;
	_io_thread = new std::thread([this]() {
		_io.reset(); _io.run();
	});
}

/**
 * \brief Zastavení asynchronního čtení
 */
void Client::stop_listen()
{
	_socket.cancel();
	_listening = false;
}

/**
 * \brief Asynchronní čtení na socketu
 * \param[in] handler Obslužná funkce pro zpracování dat
 */
void Client::read_async(std::function<void (std::stringstream &) > handler)
{
	boost::asio::async_read_until(_socket, _strbuf, _ending, [this, handler](const boost::system::error_code ec, std::size_t s) {
		(void) s;
		if (!ec) {
			std::istream is(&_strbuf);
					clear_stream();
					is >> _ss.rdbuf();
					handler(_ss);
					this->read_async(handler);
		} else {
			//            std::cout << "fail: " << ec.message() << std::endl;
		}
	});
}

/**
 * \brief Připojení klienta k serveru
 * \param host[in] adresa serveru
 * \param port[in] cílový port
 */
void Client::connect(const char* host, const char* port)
{
	using boost::asio::ip::tcp;

	/* získání adres cílového serveru */
	tcp::resolver resolver(_io);
	tcp::resolver::query query(host, port);
	auto endpoint_iterator = resolver.resolve(query);

	/* připojení k serveru */
	boost::asio::connect(_socket, endpoint_iterator);
	_connected = true;

	/* Zaslání welcome zprávy*/
	this->write(_message.welcome());

	/* získání ID klienta */
	clear_stream();
	this->read();
	/* parsování zprávy */
	_ok = (_ss >> _code >> _id);
	check_code(500);
}

/**
 * \brief Připojení klienta k serveru
 * \param host[in] adresa serveru
 * \param port[in] cílový port
 */
void Client::connect(std::string &host, std::string &port)
{
	connect(host.c_str(), port.c_str());
}

/**
 * \brief Zápis dat do socketu
 * \param[in] msg data
 */
void Client::write(std::string msg)
{
	boost::asio::write(_socket, boost::asio::buffer(msg));
}

/**
 * \brief Blokující čtení dat ze socketu
 */
void Client::read()
{
	/* načtení dat do streambufferu */
	_socket.non_blocking(false);
	boost::asio::streambuf buf;

	boost::asio::read_until(_socket, buf, "\n");

	/* vyčtení dat z bufferu do stringstreamu */
	clear_stream();
	std::istream is(&buf);
	is >> _ss.rdbuf();
}

/**
 * \brief Kontrola přečtené zprávy
 * \param[in] expected Očekávaný kód
 */
void Client::check_code(int expected)
{
	if (_code != expected) {
		std::stringstream ss;
		ss << "Wrong server response (" << _code << "), expected " << expected << "\n";
		throw Client_e(ss.str().c_str());
	}
}

/**
 * \brief Odpojení klienta od serveru
 */
void Client::disconnect()
{
	this->write(_message.bye());
}

/**
 * \brief Získání seznamu map
 * \return vektor map
 */
Client::mapvec Client::get_maps()
{
	/* zaslání požadavku*/
	this->write(_message.get_maps());
	Client::mapvec vec;
	do {
		/* čtení dokud nepříjde prázdná zpráva 511 */
		this->read();
		do {
			_ss >> _code;
			check_code(511);
			struct map_info_t map_info;
			_ok = (_ss >> map_info.id);
			if (_ok) {
				std::getline(_ss, map_info.name);
				boost::algorithm::trim(map_info.name);
				vec.push_back(map_info);
			}
		} while (_ok && !_ss.str().empty());
	} while (_ok);
	return vec;
}

/**
 * \brief Získání seznamu rozehraných her
 * \return vektor her
 */
Client::gamevec Client::get_games()
{
	/* zaslání požadavku */
	this->write(_message.get_games());
	Client::gamevec vec;
	do {
		/* čtení dokud nepříjde prázdná zpráva 512 */
		this->read();
		do {
			_ss >> _code;
			check_code(512);
			struct game_info_t game_info;
			_ok = (_ss >> game_info.id >> game_info.act_players >> game_info.max_players);
			if (_ok) {
				std::getline(_ss, game_info.name);
				boost::algorithm::trim(game_info.name);
				vec.push_back(game_info);
			}
		} while (_ok && !_ss.str().empty());
	} while (_ok);
	return vec;
}

/**
 * \brief Připojení se k rozehrané hře
 * \param[in] game ID hry
 * \return 1 při neúspěchu, jinak 0
 */
int Client::join(int game)
{
	this->write(_message.join_game(game));
	this->read();

	_ok = (_ss >> _code);
	if (!_ok || _code == 599) {
		return 1;
	}

	check_code(530);
	read_map();
	return 0;
}

/**
 * \brief Žádost o vytvoření nové hry
 * \param[in] game ID mapy
 * \param[in] delay Refresh doba
 * \param[in] max_players Max. počet hráčů
 * \param[in] name Název hry
 * \return 1 při neúspěchu, jinak 0
 */
int Client::create(int game, double delay, int max_players, const char* name)
{
	this->write(_message.new_game(game, delay, max_players, name));
	this->read();

	_ok = (_ss >> _code);
	if (!_ok || _code == 599) {
		return 1;
	}

	check_code(530);
	read_map();
	return 0;
}

/**
 * \brief Opuštění hry
 */
void Client::leave()
{
	/* ukončení asynchronního čtení */
	_socket.cancel();
	if (_io_thread != nullptr) {
		_io.stop();
		_io_thread->join();
		delete _io_thread;
		_io_thread = nullptr;
	}
	_socket.cancel();

	if (_listening) {
		_listening = false;

		this->write(_message.leave());
		this->read();

		_ok = (_ss >> _code);
		check_code(501);
	}
}

/**
 * \brief Načtení dat mapy
 */
void Client::read_map()
{
	_ss >> _color >> _map.rows >> _map.cols;
	std::getline(_ss, _map.items);
}

/**
 * \brief Destruktor -> uzavření socketu
 */
Client::~Client()
{
	try {
		if (_connected) {
			_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_type::shutdown_both);
		}
		_socket.close();
	} catch (std::exception &e) {
	}
}
