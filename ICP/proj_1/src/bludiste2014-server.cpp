/** \file bludiste2014-server.cpp 
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "Map.h"
#include "Server.h"

/* výchozí číslo portu */
const uint16_t default_port = 4568;

/**
 * \brief Nápověda
 * \param[in] name Jméno programu
 */
void usage(char *name)
{
	std::cout << "\nPouziti: " << name << " [port | -h]\n";
	std::cout << "Vychozi port: 4568\n\n";
}

/**
 * \brief Hlavní funkce
 * \param argc Počet parametrů
 * \param argv Parametry
 * \return 0 pokud je vše v pořádku
 */
int main(int argc, char *argv[])
{
	/* zpracování parametrů */
	int port{default_port};
	if (argc > 2 || (argc == 2 && !strcmp(argv[1], "-h"))) {
		usage(argv[0]);
		return 1;
	} else if (argc == 2) {
		port = std::atoi(argv[1]);
	}

	/* vytvoření serveru */
	boost::asio::io_service io;
	Server server(io, port);

	try {
		/* nahrátí map a spuštění serveru */
		server.load_maps();
		server.start();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
