/**
 * \file Communicator.h
 * \author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * \brief Network communicator
 */

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <iostream>
#include <stdexcept>
#include <vector>

/* Empty runtime exception */
#define RUNTIME_EMPTY std::runtime_error(std::string())

/* Default buffer size */
#define BUFF_SIZE 2048

/**
 * @brief Class for sending/receiving messages
 */
class Communicator
{
public:
    /**
     * @brief Constructor
     */
    Communicator();

    /**
      * @brief Destructor
      */
    ~Communicator();

    /**
     * @brief Connect to host
     *
     * @param ip Server IP address
     * @param port Destination port
     * @param mcast True when connecting multicast socket
     */
    void createConnection(std::string ip, std::string port, bool mcast = false);

    /**
     * @brief Disconnect socket
     *
     * @param sockfd socket descriptor
     */
    void disconnect();

    /**
     * @brief Receive new message
     *
     * @param timeout timeout interval
     * @param block_size length of block to be received
     * @return received message
     */
    std::string receiveData(int timeout, uint16_t block_size = BUFF_SIZE);

    /**
     * @brief Send message
     *
     * @param socket descriptor
     * @param msg Message to be send
     */
    void sendData(std::string msg);

    /**
     * @brief Resend last message
     */
    void resendLast();

    /**
     * @brief Get server IP address
     *
     * @return IP
     */
    std::string getIP()   { return ip; }

    /**
     * @brief Get port number
     *
     * @return port number
     */
    std::string getPort() { return port; }

    /**
     * @brief Get multicast IP address
     *
     * @return multicast IP
     */
    std::string getMIP()  { return m_ip; }

    /**
     * @brief Get multicast port number
     *
     * @return multicast port
     */
    std::string getMport(){ return m_port; }

    /**
     * @brief Get lowest MTU of all interfaces
     *
     * @return MTU
     */
    int lowestMtu();

private:

    /**
     * @brief Set multicast socket
     *
     * @param ip Multicast group IP
     * @param port port number
     */
    void createMulticastSocket(std::string ip, std::string port);

    /**
     * @brief Wait for data on socket
     *
     * @param sockfd socket
     * @param timeout timeout interval
     * @return true when data are waiting on socket
     */
    bool waitForData(int timeout);

    fd_set readset;             /**< set for select */
    int main_sockfd{-1};        /**< main socket */
    int mcast_sockfd{-1};       /**< multicast socket */
	
    std::string m_ip, m_port;   /**< IP and PORT for multicast */
    std::string ip, port;       /**< IP and PORT values */
    std::string last_sent;      /**< last sent message */
	
    std::vector<char> buffer;   /**< receive buffer */
	
    struct addrinfo *servinfo;  /**< server information */
    struct addrinfo *mcastinfo; /**< server information */
};

#endif // COMMUNICATOR_H
