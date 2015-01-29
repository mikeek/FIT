/**
 * @file Parser.h
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 2
 */


#ifndef PARSER_H
#define	PARSER_H

using namespace std;

class Parser {
public:
	Parser() {};
	virtual ~Parser() {};
	
	void parse(string &input);
	void parse(const char *input);
	
	string get_host()	{ return host; }
	string get_file()	{ return file; }
	int get_port()		{ return port; }
	
private:
	void parse_old(string &input);
	
	string host;
	string file;
	
	int port;
};

#endif	/* PARSER_H */

