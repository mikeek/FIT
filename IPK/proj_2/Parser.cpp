/**
 * @file Parser.cpp
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 2
 */

#include <cstdlib>
#include <string>
#include <stdexcept>

#include "Parser.h"

using namespace std;

void Parser::parse(string &input)
{
	/* regex nefunguje spravne v g++ 4.8 - parsovani "po staru" */
	parse_old(input);
}

void Parser::parse(const char* input)
{
	std::string in{input};
	parse_old(in);
}

/**
 * Zpracovani vstupniho argumentu programu
 */
void Parser::parse_old(string &input)
{
	size_t colon_pos = input.find(":");
	size_t slash_pos = input.find("/");

	if (colon_pos == string::npos) {
		throw std::invalid_argument("colon not found");
	}
	if (slash_pos == string::npos) {
		throw std::invalid_argument("slash not found");
	}

	host = input.substr(0, colon_pos);
	port = atoi(input.substr(colon_pos + 1, slash_pos).c_str());
	file = input.substr(slash_pos + 1);

	/* Odstraneni pocatecniho prefixu a zjisteni prihlasovacich udaju */
	if (host.empty()) {
		throw std::invalid_argument("host is empty");
	}
	if (file.empty()) {
		throw std::invalid_argument("file name is empty");
	}
}
