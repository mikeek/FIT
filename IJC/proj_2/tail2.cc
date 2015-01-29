/* tail2.cc
 * Reseni IJC-DU2, priklad a) 30.3.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: g++-4.7 4.7.2
 * Program vypise ze zadaneho vstupniho souboru n radku. Pri nezadani souboru
 * cte ze stdin. Pokud je zadan parametr -n +X tak vypisuje od radku X az
 * do konce souboru. Pri nezadani poctu radku je implicitne nastaveno 10
 */

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <deque>

#define NOT_SET -1

using namespace std;

void printFrom(std::istream *in, int fromline)
{
	string line;
	while (getline(*in, line)) {
		if (fromline <= 0) {
			cout << line << endl;
		} else {
			fromline--;
		}
	}
}

void printLines(std::istream *in, unsigned int lines)
{
	deque<string> buff;
	string line;
	while (getline(*in, line)) {
		buff.push_back(line);
		if (buff.size() > lines) {
			buff.pop_front();
		}
	}
	for (deque<string>::iterator it = buff.begin(); it != buff.end(); ++it) {
		cout << *it << endl;
	}
}

int main(int argc, char *argv[])
{
	ios::sync_with_stdio(false);
	string filename;
	char *endptr;
	int fromline = NOT_SET;
	int lines = NOT_SET;
	switch (argc) {
	case 1:
		lines = 10;
		break;
	case 2:
		filename = argv[1];
		break;
	case 3:
		if (strcmp(argv[1], "-n") != 0) {
			cerr << "CHYBA: spatne zadane parametry!" << endl;
			return -1;
		}
		if (argv[2][0] == '+') {
			fromline = strtol(argv[2] + 1, &endptr, 10);
		} else {
			lines = strtol(argv[2], &endptr, 10);
		}
		if ((*endptr != '\0') || ((lines < 0) && (fromline < 0))) {
			cerr << "CHYBA: zadana spatna hodnota (" << argv[2] << ")!" << endl;
			return -1;
		}
		break;
	case 4:
		if (strcmp(argv[1], "-n") != 0) {
			if (strcmp(argv[2], "-n") != 0) {
				cerr << "CHYBA: spatne zadane parametry!" << endl;
				return -1;
			}
			filename = argv[1];
			if (argv[3][0] == '+') {
				fromline = strtol(argv[3] + 1, &endptr, 10);
			} else {
				lines = strtol(argv[3], &endptr, 10);
			}
		} else {
			filename = argv[3];
			if (argv[2][0] == '+') {
				fromline = strtol(argv[2] + 1, &endptr, 10);
			} else {
				lines = strtol(argv[2], &endptr, 10);
			}
		}
		if ((*endptr != '\0') || ((lines < 0) && (fromline < 0))) {
			cerr << "CHYBA: zadana spatna hodnota!" << endl;
			return -1;
		}
		break;
	default:
		cerr << "CHYBA: prilis mnoho parametru (" << argc - 1 << ")!" << endl;
		return -1;
	}

	istream *in;
	ifstream f;
	if (!filename.empty()) {
		f.open(filename, ifstream::in);
		if (!f.is_open()) {
			cerr << "CHYBA: chyba pri otevirani souboru \"" << filename << "\"!" << endl;
			return -1;
		}
		in = &f;
	} else {
		in = &cin;
	}
	//     Zjisteni pozadovane akce
	if (fromline != NOT_SET) {
		printFrom(in, --fromline);
	} else {
		if (lines == NOT_SET) {
			lines = 10;
		}
		if (lines > 0) {
			printLines(in, lines);
		}
	}
	if (f.is_open()) {
		f.close();
	}
	return 0;
}