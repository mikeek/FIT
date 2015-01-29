/**
 * @file Client_class.h
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 2
 */

#ifndef CLIENT_H
#define	CLIENT_H

#include "Parser.h"

using namespace std;

class Client {
public:
	Client(Parser &p) { parser = p; }
	~Client();

	void cl_connect(struct hostent *server);
	void cl_get_file();

	string cl_get_file_content() { return file_content; }
private:
	void cl_socket();
	void cl_write();
	void cl_read();
	void cl_check_code(int expected);
	
	int max_speed{1000};
	int code = 0;
	int sockfd = -1;
	
	Parser parser;
	string str_buff{}, file_content{};
};

#endif	/* CLIENT_H */

