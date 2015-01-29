/**
 * @file client.cpp
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 2
 */

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

#include <errno.h>
#include <netdb.h>

#include "Client_class.h"
#include "Parser.h"

using namespace std;

/**
 * Hlavni funkce
 */
int main(int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "Wrong number of arguments (1 required)\n";
		return 1;
	}

	/* Zpracovani parametru prikazove radky */
	Parser parser{};
	try {
		parser.parse(argv[1]);
	} catch (invalid_argument &e) {
		cerr << "Wrong parameter format: " << e.what() << "!\n";
		return 1;
	}

	/* Preklad adresy serveru */
	struct hostent *server = gethostbyname(parser.get_host().c_str());
	if (!server) {
		cerr << "gethostbyname() failed!\n";
		return 1;
	}

	/* Vytvoreni klienta */
	Client client(parser);

	try {
		/* Navazani spojeni a zaslani pozadavku na server */
		client.cl_connect(server);
		client.cl_get_file();
	} catch (const char *exc) {
		cerr << exc << " failed!\n";
		return 1;
	}

	ofstream outfile(parser.get_file());

	if (!outfile.is_open()) {
		cerr << "Error while opening output file!";
		return 1;
	}

	outfile << client.cl_get_file_content();
}
