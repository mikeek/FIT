/**
 * \file Client.h
 * \author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * \brief Simple TFTP client
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "Communicator.h"

/**
 * @brief The opcode enum
 */
enum opcode {
    RRQ   = 1,
    WRQ   = 2,
    DATA  = 3,
    ACK   = 4,
    ERROR = 5,
    OACK  = 6
};

/* OACK error code */
#define OACK_ERR 8

/* Option names */
#define OPT_BLOCK_SIZE "blksize"
#define OPT_TIMEOUT    "timeout"
#define OPT_MULTICAST  "multicast"
#define OPT_TSIZE      "tsize"

#define MODE_OCTET    "octet"
#define MODE_NETASCII "netascii"

/* Default values */
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "69"
#define DEFAULT_TIMEOUT 2
#define DEFAULT_BLOCK_SIZE 512
#define DEFAULT_MODE MODE_OCTET

/**
 * @brief The Client class
 */
class Client
{
public:
    /**
     * @brief Constructor
     */
    Client();

    /**
     * @brief Destructor
     */
    ~Client();

    /**
     * @brief Connect client to server
     *
     * @param ip Server IP address
     * @param port Destination port
     */
    void connect(std::string ip, std::string port);

    /**
     * @brief disconnect Disconnect client from server
     */
    void disconnect();

    /**
     * @brief Send read request
     *
     * @param file path to file
     */
    void receiveData(std::string file);

    /**
     * @brief Send write request
     *
     * @param file path to file
     */
    void sendData(std::string file);

    /**
     * @brief Set timeout interval
     *
     * @param n_timeout timeout interval
     */
    void setTimeout(int n_timeout)   { timeout = n_timeout; hasTimeout = true; }

    /**
     * @brief Set block size
     *
     * @param n_size block size
     */
    void setBlockSize(int n_size)    { block_size = n_size; hasBsize = true; }

    /**
     * @brief Set transfer mode
     *
     * @param n_mode transfer mode
     */
    void setMode(std::string n_mode) { mode = n_mode; hasMode = true; }

    /**
     * @brief Set multicast option
     *
     * @param mc true/false
     */
    void setMulticast(bool mc = true){ hasMcast = mc; }

private:
    /**
     * @brief Set transfer size
     *
     * @param n_size new transfer size
     */
    void setTsize(int n_size) { tsize = n_size; }

    /**
     * @brief Create request message
     *
     * @param opc Request code (RRQ | WRQ)
     * @param file Valid path to file
     */
    void createRequest(opcode opc, std::string file);

    /**
     * @brief Get received message
     *
     * @return received message
     */
    std::string getMessage() { return this->message; }

    /**
     * @brief Print verbose message
     *
     * @param msg message
     */
    void verbose(std::string msg);

    /**
     * @brief Print verbose message about DATA receiving
     *
     * @param len data length
     */
    void verboseData(int len);

    /**
     * @brief Print verbose message about DATA sending
     *
     * @param len data length
     */
    void verboseSend(int len);

    /**
     * @brief Connect data socket
     *
     * @param ip IP address
     * @param port Port number
     */
    void connectDataSocket(std::string ip, std::string port);

    /**
     * @brief Parse opcode from received message
     *
     * @param data Message
     * @return opcode
     */
    opcode parseOpcode(std::string &data);

    /**
     * @brief Proccess DATA message
     *
     * @param data Message
     * @param result received block
     * @return block number
     */
    int proccessDatamsg(std::string &data, std::string &result);

    /**
     * @brief Proccess ACK message
     *
     * @param data Message
     * @return Block number
     */
    uint16_t proccessAck(std::string &data);

    /**
     * @brief Proccess ERROR message
     *
     * @param data Message
     */
    void proccessErrmsg(std::string &data);

    /**
     * @brief Proccess OACK message
     *
     * @param data Message
     */
    void proccessOack(std::string &data);

    /**
     * @brief Send ACK packet
     */
    void sendAckPacket();

    /**
     * @brief Send DATA packet
     */
    void sendDataPacket(std::string block);

    /**
     * @brief Receive data from host
     */
    void receiveData();

    /**
     * @brief Send data to host
     *
     * @param msg Data to be send
     */
    void sendFile();

    /**
     * @brief Send error about unrequested option
     *
     * @param opt unrequested option
     */
    void sendOackErr(std::string opt);

    /**
     * @brief Create blocks from input file
     *
     * @return blocks
     */
    void createBlocks(std::vector<std::string> &blocks);

    /**
     * @brief Get file name from path
     *
     * @param path path to file
     * @return file name
     */
    static std::string baseName(std::string path);

    /**
     * @brief Replace all occurences of substring
     *
     * @param where main string
     * @param what substring to be replaced
     * @param repl replacement
     */
    static void replaceAll(std::string &where, std::string what, std::string repl);

    /**
     * @brief Load whole file into memory
     */
    void loadFile();

    /**
     * @brief Clear stringstream for request
     */
    void clearPacket() { packet.str(""); packet.clear(); }

    /**
     * @brief Convert message to netascii format
     */
    void convertToNetascii();

    /**
     * @brief Convert message from netascii format
     */
    void convertFromNetascii();

    std::string mode;                   /**< transfer mode */
	
    bool multicast = false;              /**< info about active multicast */
    bool main_client = true;             /**< info for multicast */
	
    uint16_t block_size = DEFAULT_BLOCK_SIZE; /**< block size */
    int tsize = 0;                       /**< transfer size */
    int timeout = DEFAULT_TIMEOUT;       /**< timeout */
    uint16_t next_block;                 /**< number of last received block */
	
    std::string message;                 /**< received message */
    std::ifstream inFile;                /**< input file when sending data */
    std::stringstream packet;            /**< request message */
	
    Communicator communicator;           /**< Network communicator */
    bool hasTimeout{false}, hasBsize{false}, hasMode{false}, hasMcast{false};
};

#endif // CLIENT_H
