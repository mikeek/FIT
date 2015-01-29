/**
 * @file server.cpp
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 1
 */

#include <cstdlib>
#include <iostream>
#include <string>

#include <unistd.h>

#include "Server_class.h"

using namespace std;

int main(int argc, char *argv[])
{
	int c;
	bool opt_p = false, opt_d = false;
	uint32_t port = 0, max_speed = 0;

	const char *optstring = ":p:d:";
	while ((c = getopt(argc, argv, optstring)) != -1) {
		switch (c) {
		case 'p':
			port = atoi(optarg);
			opt_p = true;
			break;
		case 'd':
			max_speed = atoi(optarg);
			opt_d = true;
			break;
		default:
			std::cerr << "Error when parsing arguments!\n";
			return 1;
		}
	}
	if (!opt_p || !opt_d) {
		cerr << "All parameters (-p <port> -d <speed_limit>) must be set!\n";
		return 1;
	}

	try {
		Server server(port, max_speed * 1000);
		server.s_listen();
	} catch (const char *what) {
		cerr << what << " failed!\n";
		return 1;
	}
}