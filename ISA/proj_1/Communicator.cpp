/**
 * \file Communicator.cpp
 * \author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * \brief Network communicator
 */

#include "Communicator.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX(_frst_, _scnd_) ((_frst_) < (_scnd_)) ? (_scnd_) : (_frst_)
#define MIN(_frst_, _scnd_) ((_frst_) > (_scnd_)) ? (_scnd_) : (_frst_)

/**
 * @brief Constructor
 */
Communicator::Communicator()
{
	/* Allocate space in buffer */
	buffer.reserve(BUFF_SIZE);
}

/**
 * @brief Destructor
 */
Communicator::~Communicator()
{
	disconnect();
}

/**
 * @brief Get lowest MTU of all interfaces
 */
int Communicator::lowestMtu()
{
	struct ifaddrs *ifs = NULL;
	int mtu = -1;

	/* Get all interfaces */
	if (getifaddrs(&ifs)) {
		perror("getifaddrs");
		throw RUNTIME_EMPTY;
	}

	/* Go through all interfaces */
	for (struct ifaddrs *ifa = ifs; ifa; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != servinfo->ai_family) {
			continue;
		}

		/* Get interface name  */
		struct ifreq ifr;
		size_t if_name_len = strlen(ifa->ifa_name);
		if (if_name_len > sizeof (ifr.ifr_name)) {
			continue;
		}

		memcpy(ifr.ifr_name, ifa->ifa_name, if_name_len);
		ifr.ifr_name[if_name_len] = 0;

		/* Get interface MTU */
		if (ioctl(main_sockfd, SIOCGIFMTU, &ifr) == -1) {
			perror("ioctl");
			throw RUNTIME_EMPTY;
		}

		if (mtu < 0) {
			mtu = ifr.ifr_mtu;
		} else {
			mtu = MIN(mtu, ifr.ifr_mtu);
		}
	}

	/* Free resources */
	freeifaddrs(ifs);

	return (mtu == -1) ? 1500 : mtu;
}

/**
 * @brief Get addrinfo structure for given ip and port number
 */
struct addrinfo *getInfo(std::string ip, std::string port)
{
	struct addrinfo hints;
	struct addrinfo *info;
	int rv;

	/* Get server address */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(ip.c_str(), port.c_str(), &hints, &info)) != 0) {
		perror("getaddrinfo");
		throw RUNTIME_EMPTY;
	}

	return info;
}

/**
 * @brief Set multicast socket (join group etc.)
 */
void Communicator::createMulticastSocket(std::string ip, std::string port)
{
	u_int yes = 1;

	/* get info */
	mcastinfo = getInfo(ip, port);

	/* Create socket */
	mcast_sockfd = socket(mcastinfo->ai_family, mcastinfo->ai_socktype, 0);
	if (mcast_sockfd == -1) {
		perror("socket");
		throw RUNTIME_EMPTY;
	}

	/* allow multiple sockets to use the same port number */
	if (setsockopt(mcast_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) < 0) {
		perror("setsockopt");
		throw RUNTIME_EMPTY;
	}

	/* set up destination address */
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(atoi(port.c_str()));

	/* bind to receive address */
	if (bind(mcast_sockfd, (struct sockaddr *) &addr, sizeof (addr)) < 0) {
		perror("bind");
		throw RUNTIME_EMPTY;
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(ip.c_str());
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(mcast_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof (mreq)) < 0) {
		perror("setsockopt2");
		throw RUNTIME_EMPTY;
	}

	/* Set socket nonblocking - we're using select() with timeout */
	fcntl(mcast_sockfd, F_SETFL, O_NONBLOCK);

	/* Store values */
	m_ip = ip;
	m_port = port;
}

/**
 * @brief Create new connection to host
 */
void Communicator::createConnection(std::string ip, std::string port, bool mcast)
{
	if (mcast) {
		createMulticastSocket(ip, port);
		return;
	}

	servinfo = getInfo(ip, port);

	/* Create socket */
	main_sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (main_sockfd == -1) {
		perror("socket");
		throw RUNTIME_EMPTY;
	}

	/* Set socket nonblocking - we're using select() with timeout */
	fcntl(main_sockfd, F_SETFL, O_NONBLOCK);

	/* Store descriptors */
	mcast_sockfd = main_sockfd;

	/* Store values */
	this->ip = ip;
	this->port = port;
}

/**
 * @brief Disconnect socket
 */
void Communicator::disconnect()
{
	/* Close multicast socket */
	if (mcast_sockfd != main_sockfd) {
		close(mcast_sockfd);
	}

	/* Close main socket */
	if (main_sockfd > 0) {
		close(main_sockfd);
	}
}

/**
 * @brief Wait for data on socket
 */
bool Communicator::waitForData(int timeout)
{
	int ret;
	struct timeval tv;

	/* Watch only specified socket */
	FD_ZERO(&readset);
	FD_SET(main_sockfd, &readset);

	/* Add multicast socket if set */
	if (mcast_sockfd != main_sockfd) {
		FD_SET(mcast_sockfd, &readset);
	}

	/* Set timeout */
	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	/* Wait for data */
	ret = select(MAX(mcast_sockfd, main_sockfd) + 1, &readset, NULL, NULL, (timeout != -1) ? &tv : NULL);

	/* Check return value */
	if (ret == 0) {
		return false;
	} else if (ret < 0) {
		perror("select");
		throw RUNTIME_EMPTY;
	}

	return true;
}

/**
 * @brief Receive data from socket
 */
std::string Communicator::receiveData(int timeout, uint16_t block_size)
{
	int ret;

	/* Add header length to block size */
	block_size += 5;

	/* Wait for data */
	if (!this->waitForData(timeout)) {
		/* timeouted */
		return "";
	}

	int sockfd;
	struct addrinfo *info;

	/* Get socket with data */
	if (FD_ISSET(main_sockfd, &readset)) {
		sockfd = main_sockfd;
		info = servinfo;
	} else {
		sockfd = mcast_sockfd;
		info = mcastinfo;
	}

	/* Make enough space */
	if (buffer.capacity() < block_size) {
		buffer.reserve(block_size);
	}

	/* Clear buffer */
	memset(&buffer[0], 0, buffer.size());

	/* Receive data */
	ret = recvfrom(sockfd, &buffer[0], block_size, 0, info->ai_addr, &(info->ai_addrlen));

	/* Check for errors */
	if (ret < 0) {
		perror("recvfrom");
		throw RUNTIME_EMPTY;
	}

	/* Return received message */
	return std::string(buffer.data(), ret);
}

/**
 * @brief Write data into socket
 */
void Communicator::sendData(std::string msg)
{
	int sent = 0, sent_now = 0;
	int len = msg.length();

	while (sent < len) {
		/* Drop sent data */
		msg = msg.substr(sent_now);

		/* Send data */
		sent_now = sendto(main_sockfd, msg.c_str(), msg.length(), 0, servinfo->ai_addr, servinfo->ai_addrlen);

		/* Check for errors */
		if (sent_now == -1) {
			perror("sendto");
			throw RUNTIME_EMPTY;
		}

		sent += sent_now;
	}

	/* Save values for resending */
	last_sent = msg;
}

/**
 * @brief Resend last message
 */
void Communicator::resendLast()
{
	this->sendData(last_sent);
}
