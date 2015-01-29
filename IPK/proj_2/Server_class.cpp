/**
 * @file Server_class.cpp
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 2
 */

#include <chrono>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Server_class.h"

using namespace std;

int s_read(int sock, string &in_msg, int max_speed)
{
	char *buff = new char[max_speed + 1];
	memset(buff, 0, max_speed);
	ssize_t len = 0;
	do {
		len = read(sock, buff, max_speed);
		buff[len] = '\0';
		if (len == -1) {
			cerr << "Error while receiving message, terminating thread.\n";
			delete[] buff;
			return -1;
		}
		in_msg += buff;
	} while (len == max_speed);

	if (in_msg.length() < 3) {
		cerr << "Malformed message accepted, terminating thread.\n";
		delete[] buff;
		return -1;
	}

	delete[] buff;
	return 0;
}

int s_write(int sock, string &msg, int max_speed)
{
	ssize_t len{0}, towrite{0}, todo = msg.length();
	clock_t begin, end;
	double elapsed{1000.0};

	while (todo > 0) {
		if (elapsed < 1000.0) {
			this_thread::sleep_for(chrono::milliseconds(1000 - static_cast<int> (elapsed)));
		}
		towrite = (todo > max_speed) ? max_speed : todo;
		begin = clock();
		len = write(sock, msg.c_str(), towrite);
		if (len != towrite) {
			cerr << "Error while sending message to client, terminating thread.\n";
			return -1;
		}
		todo -= len;
		msg = msg.substr(len);
		end = clock();
		elapsed = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
	}

	if (len == max_speed) {
		msg = '\0';
		len = write(sock, msg.c_str(), 1);
		if (len != 1) {
			cerr << "Error while sending message to client, terminating thread.\n";
			return -1;
		}
	}

	return 0;
}

void Server::s_serve(int sock)
{
	string in_msg{}, out_msg{};
	stringstream ss;
	ss << max_speed;
	out_msg = ss.str();
	if (s_write(sock, out_msg, max_speed)) {
		close(sock);
		return;
	}

	if (s_read(sock, in_msg, max_speed)) {
		close(sock);
		return;
	}

	int code = atoi(in_msg.substr(0, 3).c_str());
	string file = in_msg.substr(4);

	if (code != 200) {
		cerr << "Unknown code received (" << code << "), terminating thread.\n";
		close(sock);
		return;
	}

	ifstream infile(file);

	if (!infile.is_open()) {
		cerr << "Can't open file '" << file << "', terminating thread.\n";
		out_msg = "300";
		s_write(sock, out_msg, max_speed);
		close(sock);
		return;
	}

	out_msg = "210 " + string(std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>());

	s_write(sock, out_msg, max_speed);

	close(sock);
	infile.close();
	return;
}

void Server::init()
{
	s_socket();
	s_bind();
}

void Server::s_socket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		throw ("socket()");
	}

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) != 0) {
		throw ("setsockopt()");
	}
}

void Server::s_bind()
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *) &addr, sizeof (addr)) != 0) {
		throw ("bind()");
	}
}

void Server::s_listen()
{
	if (listen(sockfd, SOMAXCONN) != 0) {
		throw ("listen()");
	}

	vector<thread> v{};

	struct sockaddr_in addr;

	while (true) {
		socklen_t len = sizeof (addr);
		int thread_sock = accept(sockfd, (struct sockaddr*) &addr, &len);

		if (thread_sock < 0) {
			throw ("accept()");
		}

		thread t(&Server::s_serve, this, thread_sock);
		v.push_back(move(t));
	}
	for (auto& t : v) {
		t.join();
	}
}

Server::~Server()
{
	if (sockfd > 0) {
		close(sockfd);
	}
}

