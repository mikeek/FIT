/**
 * @file Client_class.cpp
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 2
 */

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Client_class.h"
#include "Parser.h"

using namespace std;

/**
 * Poslani dat ulozenych v bufferu str_buff
 */
void Client::cl_write()
{
	ssize_t len{0}, towrite{0}, todo = str_buff.length();
	clock_t begin, end;
	double elapsed{1000.0};
	while (todo > 0) {
		if (elapsed < 1000.0) {
			this_thread::sleep_for(chrono::milliseconds(1000 - static_cast<int> (elapsed)));
		}
		towrite = (todo > max_speed) ? max_speed : todo;
		begin = clock();
		len = write(sockfd, str_buff.c_str(), towrite);

		if (len != towrite) {
			throw ("write()");
		}
		todo -= len;
		str_buff = str_buff.substr(len);
		end = clock();
		elapsed = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
	}
	if (len == max_speed) {
		str_buff = '\0';
		len = write(sockfd, str_buff.c_str(), 1);
		if (len != 1) {
			throw ("write()");
		}
	}
}

/**
 * Precteni zpravy do bufferu str_buff
 */
void Client::cl_read()
{
	char *buff = new char[max_speed + 1];
	ssize_t len = 0;
	str_buff.clear();
	do {
		len = read(sockfd, buff, max_speed);
		buff[len] = '\0';
		if (len == -1) {
			delete[] buff;
		}
		str_buff += buff;
	} while (len == max_speed);
	delete[] buff;
}

void Client::cl_check_code(int expected)
{
	code = atoi(str_buff.substr(0, 3).c_str());
	if (code != expected) {
		if (code == 300) {
			cerr << "Trouble with file on server!\n";
		} else {
			cerr << "Unknown code accepted (" << code << ")!\n";
		}
		throw ("Communication with server");
	}

}

/**
 * Vytvoreni socketu pro ridici a datovy prenos
 */
void Client::cl_socket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		throw ("socket()");
	}
}

/**
 * Pripojeni k serveru
 * 
 * @param server Adresa serveru
 */
void Client::cl_connect(hostent* server)
{
	cl_socket();

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(parser.get_port());
	memcpy((char *) &server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
		throw ("connect()");
	}
}

void Client::cl_get_file()
{
	cl_read();
	if (str_buff.empty()) {
		cerr << "Malformed message accepted!\n";
		throw ("cl_get_file()");
	}

	max_speed = atoi(str_buff.c_str());

	if (max_speed <= 0) {
		cerr << "Wrong response from server!\n";
		throw ("cl_get_file()");
	}

	str_buff = "200 " + parser.get_file();
	cl_write();

	cl_read();
	if (str_buff.length() < 3) {
		cerr << "Malformed message accepted!\n";
		throw ("cl_get_file()");
	}

	cl_check_code(210);
	file_content = str_buff.substr(4);
}

/**
 * Uzavreni socketu
 */
Client::~Client()
{
	if (sockfd >= 0) {
		close(sockfd);
	}
}
