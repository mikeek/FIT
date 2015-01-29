/**
 * \file mytftpclient.cpp
 * \author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * \brief Simple TFTP client
 */

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include <wordexp.h>

#include "Client.h"

#define OPTSTRING ":hmRWd:t:s:a:c:"

#define PROMPT ">> "

/* R/W mode */
enum rw_modes {
	MODE_READ,
	MODE_WRITE,
	MODE_NONE
};

/**
 * @brief Print usage
 */
void usage()
{
	std::cout << std::endl
			<< "Parameters at runtime:\n"
			<< "  -h           show this text\n"
			<< "  -R           set read mode\n"
			<< "  -W           set write mode\n"
			<< "  -d path      path to file to transfer to/from server\n"
			<< "  -t timeout   timeout for server\n"
			<< "  -s size      block size\n"
			<< "  -a ip[,port] IPv4/IPv6 server address and port\n"
			<< "  -m           enable multicast option\n"
			<< "  -c mode      transfer mode (octet | netascii)\n"
			<< "\n"
			<< "To quit application, just send EOF or enter command \"exit\".\n"
			<< "\n";
}

/**
 * @brief Execute entered command
 *
 * @param argc
 * @param argv
 * @return 0 on success
 */
int execute_command(int argc, char *argv[])
{
	Client cl;

	/* Set default values */
	int c{0}, rw_mode{MODE_NONE};
	std::string ip{DEFAULT_IP}, path{}, port{DEFAULT_PORT};

	int addrind{0};
	bool port_given = false;

	/* Reset getopt */
	optind = 0;
	opterr = 0;

	/* Parse command */
	while ((c = getopt(argc, argv, OPTSTRING)) != -1) {
		switch (c) {
		case 'h':
			usage();
			return 0;
		case 'R':
			if (rw_mode == MODE_WRITE) {
				std::cerr << "You cannot combide R/W modes in one command!\n";
				return 0;
			}
			rw_mode = MODE_READ;
			break;
		case 'W':
			if (rw_mode == MODE_READ) {
				std::cerr << "You cannot combide R/W modes in one command!\n";
				return 0;
			}
			rw_mode = MODE_WRITE;
			break;
		case 'd':
			path = optarg;
			break;
		case 't':
			cl.setTimeout(std::atoi(optarg));
			break;
		case 's':
			cl.setBlockSize(std::atoi(optarg));
			break;
		case 'a':
		{
			addrind = optind;
			std::string addr = optarg;
			size_t tmp = addr.find(',');
			if (tmp == addr.npos) {
				/* ip (maybe "ip , port", parse it later) */
				ip = optarg;
			} else if (tmp == addr.length() - 1) {
				/* ip, port (port parse later) */
				ip = addr.substr(0, tmp);
			} else {
				/* ip,port */
				ip = addr.substr(0, tmp);
				port_given = true;
				port = addr.substr(tmp + 1);
			}
			break;
		}
		case 'c':
			cl.setMode(std::string(optarg));
			break;
		case 'm':
			cl.setMulticast();
			break;
		case ':':
			std::cerr << "Command " << (char) optopt << " requires an argument!\n";
			return 1;
		default:
			std::cerr << "Unknown command " << (char) optopt << std::endl;
			return 1;
		}
	}

	/* Find missing port */
	if (optind < argc && !port_given) {
		std::string tmp{argv[addrind]};
		if (tmp == ",") {
			/* ip , port */
			if (optind < argc - 1) {
				port = argv[addrind + 1];
			} else {
				throw std::invalid_argument("Bad format of IP and port\n");
			}
		} else if (tmp[0] == ',' && tmp.length() > 1) {
			/* ip ,port */
			port = tmp.substr(1);
		} else {
			/* ip, port */
			port = tmp;
		}
	}

	/* Check mode and path */
	if (rw_mode == MODE_NONE) {
		std::cerr << "Read/Write mode not set!\n";
		return 1;
	}

	if (path.empty()) {
		std::cerr << "Path to file not set!\n";
		return 1;
	}

	/* Connect to host */
	cl.connect(ip, port);

	/* Proccess operation */
	if (rw_mode == MODE_READ) {
		cl.receiveData(path);
	} else {
		cl.sendData(path);
	}

	/* Disconnect client */
	cl.disconnect();

	return 0;
}

/**
 * @brief Get line from std::cin
 *
 * @param line
 * @return false when program should exit
 */
bool get_line(std::string& line)
{
	/* Print prompt */
	std::cout << PROMPT;

	/* EOF? */
	if (!std::getline(std::cin, line)) {
		std::cout << std::endl;
		return false;
	}

	/* exit command? */
	if (line == "exit") {
		return false;
	}

	/* Add argv[0] */
	line = std::string("mytftclient ") + line;
	return true;
}

/**
 * @brief Main function
 * @return 0 on success
 */
int main()
{
	std::string line;

	/* Print usage */
	usage();

	/* Read line */
	while (get_line(line)) {
		/* Split to argc and argv */
		wordexp_t wexp{};
		wordexp(line.c_str(), &wexp, 0);

		/* Parse it */
		try {
			execute_command(wexp.we_wordc, wexp.we_wordv);
		} catch (std::exception &e) {
			std::cerr << e.what();
		}

		/* Free resources */
		wordfree(&wexp);
	}

	return 0;
}

