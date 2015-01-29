/**
 * @file Server_class.h
 * @author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * @brief IPK projekt c. 2
 */


#ifndef SERVER_CLASS_H
#define	SERVER_CLASS_H

class Server {
public:
	Server(uint32_t p, uint32_t s): port{p}, max_speed{s} { init(); }
	virtual ~Server();
	
	void s_listen();
	
private:
	void init();
	void s_socket();
	void s_bind();
	void s_serve(int sock);
	
	int sockfd = -1;
	
	uint32_t port;
	uint32_t max_speed;
};

#endif	/* SERVER_CLASS_H */

