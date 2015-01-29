/**
 * @file ftpclient.cpp
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 1
 */

#include <iostream>
#include <string>

#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

/**
 * Kody odpovedi serveru
 */
enum codes {
	DATA_OK = 150,
	READY = 220,
	CTRL_CLOSE = 221,
	DATA_CLOSE = 226,
	EXT_PASV = 229,
	NEED_PASS = 331,
	LOGIN_OK = 230,
	NOT_LOGGED = 530,
};

/**
 * Trida reprezentujici parser vstupnich informaci
 */
class Parser {
public:
	Parser() {}
	Parser(string& address): addr{address} {}
	Parser(char *address): addr{address} {}
	
	void parse();
	void parse_data_port(string &msg);
	
	string get_host()	{ return host; }
	string get_login()	{ return login; }
	string get_passwd() { return passwd; }
	string get_pwd()	{ return pwd; }
	int get_port()		{ return port; }
	int get_data_port() { return data_port; }
	
private:
	void parse_old();
	int port = 21;
	int data_port = 20;
	
	string addr, host, login, passwd;
	string pwd = "/";
};

/**
 * Trida reprezentujici klienta pripojujiciho se na server
 */
class Client {
public:
	Client(Parser &p) { parser = p; }
	~Client();
	
	void cl_socket();
	void cl_connect(struct hostent *server, bool data);
	void cl_log_in();
	void cl_log_out();
	void cl_ls();
	
private:
	void cl_write(int sock);
	void cl_read(int sock);
	void cl_check_code();
	void cl_wrong_code(codes expected);
	void cl_enter_passive();
	
	static const int buff_size = 2048;
	char buff[buff_size] = {0};
	
	int code = 0;
	int sockdata = -1;
	int sockfd = -1;
	
	Parser parser;
	string cmd;
	string str_buff;
	string list;
};

void Parser::parse()
{
	/* regex nefunguje spravne v g++ 4.8 - parsovani "po staru" */
	parse_old();
}

/**
 * Zpracovani vstupniho argumentu programu
 */
void Parser::parse_old()
{
	/* Odstraneni pocatecniho prefixu a zjisteni prihlasovacich udaju */
	if (addr.substr(0, 6) == "ftp://") {
		addr = addr.substr(6);
		host = addr;
		size_t at_pos = addr.find("@");
		if (at_pos != string::npos) {
			int colon_pos = addr.find(":");
			login = addr.substr(0, colon_pos);
			passwd = addr.substr(colon_pos + 1, at_pos - colon_pos - 1);
			host = addr.substr(at_pos + 1);
		}
	} else {
		host = addr;
	}

	/* Zjisteni portu */
	size_t colon_pos = host.find(":");
	if (colon_pos != string::npos) {
		uint8_t len = 0;
		while (isdigit(host[colon_pos + 1 + len++]));
		port = atoi(host.substr(colon_pos + 1, len).c_str());
		host = host.replace(colon_pos, len, "");
	}

	/* Zjisteni ciloveho adresare */
	size_t slash_pos = host.find("/");
	if (slash_pos != string::npos) {
		pwd = host.substr(slash_pos);
		host = host.substr(0, slash_pos);
	}

	/* Nezadane prihlasovaci udaje -> anonymni prihlaseni */
	if (login.empty()) {
		login = "anonymous";
		passwd = "secret";
	}
}

/**
 * Zpracovani zpravy obsahujici port pro datove spojeni se serverem
 * 
 * @param msg Zprava od serveru
 */
void Parser::parse_data_port(string& msg)
{
	size_t pos_left = msg.find("|||");
	size_t pos_right = msg.rfind("|)");

	if (pos_left != string::npos && pos_right != string::npos) {
		data_port = atoi(msg.substr(pos_left + 3, pos_right - pos_left - 3).c_str());
	}
}

/**
 * Test na chybovy kod zpravy
 */
void Client::cl_check_code()
{
	if (code >= 400 && code < 500) {
		cerr << str_buff;
		throw ("communication with server");
	}
}

/**
 * Vypis chyby pri neocekavanem kodu
 * 
 * @param expected Ocekavany kod
 */
void Client::cl_wrong_code(codes expected)
{
	cerr << "ERROR:\n";
	cerr << "Expected code: " << expected << endl;
	cerr << "Accepted code: " << code << endl;
	cerr << "Message: " << str_buff;
}

/**
 * Poslani dat ulozenych v bufferu str_buff
 * 
 * @param sock Socket do ktereho se ma zprava zapsat
 */
void Client::cl_write(int sock)
{
	int len = write(sock, str_buff.c_str(), str_buff.length());

	if (len == -1) {
		throw ("write()");
	}

	if (static_cast<size_t> (len) != str_buff.length()) {
		throw ("write()");
	}
}

/**
 * Precteni zpravy do bufferu str_buff
 * 
 * @param sock Socket ze ktereho se ma cist
 */
void Client::cl_read(int sock)
{
	memset(buff, 0, buff_size);
	str_buff.clear();
	bool first = true;
	string str_code = "";

	while (1) {
		int len = read(sock, buff, buff_size - 1);
		if (len == -1) {
			throw ("read()");
		} else if (len == 0) {
			break;
		}
		buff[len] = '\0';
		str_buff += buff;
		if (sock == sockfd) {
			if (first) {
				first = false;
				if (str_buff[3] != '-') {
					break;
				}
				str_code = "\r\n" + str_buff.substr(0, 3) + " ";
			} else {
				if (str_buff.find(str_code) != string::npos) {
					break;
				}
			}
		}
	}

	code = atoi(str_buff.substr(0, 3).c_str());
	cl_check_code();
}

/**
 * Vytvoreni socketu pro ridici a datovy prenos
 */
void Client::cl_socket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	sockdata = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0 || sockdata < 0) {
		throw ("socket()");
	}
}

/**
 * Pripojeni k serveru
 * 
 * @param server Adresa serveru
 * @param data Indikuje jestli se pripojit na datovy nebo ridici port
 */
void Client::cl_connect(struct hostent *server, bool data)
{
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons((data) ? parser.get_data_port() : parser.get_port());
	memcpy((char *) &server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

	if (connect((data) ? sockdata : sockfd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
		throw ("connect()");
	}

	if (!data) {
		str_buff = "EPSV\r\n";
		cl_write(sockfd);
		cl_read(sockfd);
	}
}

/**
 * Prihlaseni se na server
 */
void Client::cl_log_in()
{
	if (code == READY) {
		cl_read(sockfd);
	}
	if (code == NOT_LOGGED) {
		str_buff = "USER " + parser.get_login() + "\r\nPASS " + parser.get_passwd() + "\r\n";
		cl_write(sockfd);
		cl_read(sockfd);
		cl_read(sockfd);
	}
	cl_enter_passive();
}

/**
 * Prepnuti do pasivniho rezimu
 */
void Client::cl_enter_passive()
{
	str_buff = "EPSV\r\n";
	cl_write(sockfd);
	cl_read(sockfd);
	if (code != EXT_PASV) {
		cl_wrong_code(EXT_PASV);
		throw ("communication with server");
	}
	parser.parse_data_port(str_buff);
}

/**
 * Odhlaseni klienta
 */
void Client::cl_log_out()
{
	str_buff = "QUIT\r\n";
	cl_write(sockfd);
	cl_read(sockfd);
	if (code != CTRL_CLOSE) {
		cl_wrong_code(CTRL_CLOSE);
		throw ("QUIT");
	}
}

/**
 * Vypis adresare
 */
void Client::cl_ls()
{
	str_buff = "LIST " + parser.get_pwd() + "\r\n";
	cl_write(sockfd);
	cl_read(sockfd);
	if (code != DATA_OK) {
		cl_wrong_code(DATA_OK);
		throw ("LIST");
	}

	cl_read(sockdata);
	close(sockdata);
	cout << str_buff;

	cl_read(sockfd);
	if (code != DATA_CLOSE) {
		cl_wrong_code(DATA_CLOSE);
		throw ("LIST");
	}

}

/**
 * Uzavreni socketu
 */
Client::~Client()
{
	if (sockfd >= 0) {
		close(sockfd);
	}
	if (sockdata >= 0) {
		close(sockdata);
	}
}

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
	Parser parser(argv[1]);
	parser.parse();

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
		client.cl_socket();
		client.cl_connect(server, false);
		client.cl_log_in();
		client.cl_connect(server, true);
		client.cl_ls();
		client.cl_log_out();

	} catch (const char *exc) {
		cerr << exc << " failed!\n";
		return 1;
	}
}