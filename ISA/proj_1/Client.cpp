/**
 * \file Client.cpp
 * \author Michal Kozubik <xkozub03@stud.fit.vutbr.cz>
 * \brief Simple TFTP client
 */

#include "Client.h"
#include <algorithm>
#include <unistd.h>
#include <sstream>
#include <arpa/inet.h>
#include <sys/time.h>
#include <libgen.h>
#include <string.h>

/**
 * @brief Opcodes in network byt order
 */
static uint16_t net_opcodes[7];

/* Read 2 bytes from data pointer */
#define read16(_ptr_) (*((uint16_t *) (_ptr_)))

/**
 * @brief Constructor
 */
Client::Client()
{
	/* Convert opcodes to network byte order */
	for (uint16_t i = 0; i <= (uint16_t) opcode::OACK; ++i) {
		net_opcodes[i] = htons(i);
	}
}

/**
 * @brief Destructor - release resources
 */
Client::~Client()
{
	disconnect();
	if (inFile.is_open()) {
		inFile.close();
	}
}

/**
 * @brief Print message with timestamp
 */
void Client::verbose(std::string msg)
{
	/* Get current time */
	timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000.0;

	/* Format time */
	char buffer[80];
	strftime(buffer, 80, "[%F %X.", localtime(&curTime.tv_sec));

	/* Print message */
	std::cout << buffer << milli << "] " << msg << std::endl;
}

/**
 * @brief Print verbose message about DATA packet received
 */
void Client::verboseData(int len)
{
	/* Create message */
	std::stringstream ss;
	ss << "Received DATA ... " << len << " B";

	/* Known total size? */
	if (tsize) {
		ss << " of " << tsize << " B";
	}

	/* Print it */
	this->verbose(ss.str());
}

/**
 * @brief Print verbose message about DATA packet sent
 */
void Client::verboseSend(int len)
{
	/* Create message */
	std::stringstream ss;
	ss << "Sending DATA ... " << len << " B of " << tsize << " B";

	/* Print it */
	this->verbose(ss.str());
}

/**
 * @brief Connect to server
 */
void Client::connect(std::string ip, std::string port)
{
	/* Create connection */
	communicator.createConnection(ip, port);

	/* Check if MTU is greater than block size */
	if (hasBsize) {
		int maxMtu = communicator.lowestMtu();
		if (maxMtu < block_size) {
			setBlockSize(maxMtu);
		}
	}
}

/**
 * @brief Connect data socket for multicast
 */
void Client::connectDataSocket(std::string ip, std::string port)
{
	/* Only if not already connected */
	communicator.createConnection(ip, port, true);
}

/**
 * @brief Disconnect from server
 */
void Client::disconnect()
{
	/* Delete communicators */
	communicator.disconnect();
}

/**
 * @brief Parse opcode
 */
opcode Client::parseOpcode(std::string &data)
{
	/* opcode == first 2 bytes of message */
	return static_cast<opcode> (ntohs(read16(data.c_str())));
}

/**
 * @brief Proccess DATA message
 */
int Client::proccessDatamsg(std::string &data, std::string &result)
{
	const char *ptr = data.c_str();

	/* Skip opcode */
	ptr += 2;

	/* read block number */
	uint16_t block = ntohs(read16(ptr));
	ptr += 2;

	/* read block */
	result = std::string(ptr);

	return block;
}

/**
 * @brief Proccess ACK message
 */
uint16_t Client::proccessAck(std::string &data)
{
	const char *ptr = data.c_str();

	/* Skip opcode */
	ptr += 2;

	/* read block number */
	return ntohs(read16(ptr));
}

/**
 * @brief Proccess OACK message
 */
void Client::proccessOack(std::string &data)
{
	const char *ptr = data.c_str();

	/* Skip opcode */
	ptr += 2;

	while (ptr < data.c_str() + data.length()) {
		/* Get option name */
		std::string opt = ptr;
		ptr += opt.length() + 1;

		/* Get option value */
		std::string val = ptr;
		ptr += val.length() + 1;

		/* Set right value */
		if (opt == OPT_BLOCK_SIZE) {
			setBlockSize(atoi(val.c_str()));
		} else if (opt == OPT_TIMEOUT) {
			setTimeout(atoi(val.c_str()));
		} else if (opt == OPT_TSIZE) {
			setTsize(atoi(val.c_str()));
		} else if (opt == OPT_MULTICAST) {
			/* Get multicast IP */
			int delim = val.find(',');
			std::string ip = val.substr(0, delim);
			val = val.substr(delim + 1);

			/* Get multicast port */
			delim = val.find(',');
			std::string port = val.substr(0, delim);
			val = val.substr(delim + 1);

			/* Connect to multicast address */
			if (!ip.empty() && !port.empty()) {
				connectDataSocket(ip, port);
			}

			/* Is this master client? */
			this->main_client = (val == "1");
			this->multicast = true;
		} else {
			/* Unrequested option */
			this->sendOackErr(opt);
			throw std::runtime_error("Received unrequested option " + opt + ", terminating transfer\n");
		}
	}
}

/**
 * @brief Proccess ERROR message
 */
void Client::proccessErrmsg(std::string &data)
{
	const char *ptr = data.c_str();

	/* Skip opcode */
	ptr += 2;

	/* read ErrorCode */
	uint16_t errCode = ntohs(read16(ptr));
	ptr += 2;

	/* read ErrMsg */
	std::cerr << "Error code " << errCode << ": " << ptr << std::endl;
}

/**
 * @brief Create request message
 */
void Client::createRequest(opcode opc, std::string file)
{
	/* Clear buffer */
	clearPacket();

	/* Opcode is in binary format */
	packet.write(reinterpret_cast<char *> (&net_opcodes[opc]), sizeof (uint16_t));

	/* Add file name and transfer mode */
	packet << file << '\0' << mode << '\0';

	/* Add multicast option (only for RRQ) */
	if (hasMcast && opc == opcode::RRQ) {
		packet << OPT_MULTICAST << '\0' << '\0';
	}

	/* Add block size option */
	if (hasBsize) {
		packet << OPT_BLOCK_SIZE << '\0' << block_size << '\0';
	}

	/* Add timeout option */
	if (hasTimeout) {
		packet << OPT_TIMEOUT << '\0' << timeout << '\0';
	}

	/* Add transfer size option */
	packet << OPT_TSIZE << '\0' << tsize << '\0';
}

/**
 * @brief Get file name from path
 */
std::string Client::baseName(std::string path)
{
	char *ptr = strdup(path.c_str());
	std::string bname = std::string(basename(ptr));
	free(ptr);
	return bname;
}

/**
 * @brief Receive data
 */
void Client::receiveData(std::string file)
{
	/* Create read request and send it */
	this->verbose("Requesting READ from " + communicator.getIP());

	/* Get only file from path */
	std::string only_file = baseName(file);

	/* RRQ -> zero transfer size */
	setTsize(0);

	/* Send request */
	createRequest(opcode::RRQ, only_file);
	communicator.sendData(packet.str());

	/* Receive data */
	receiveData();

	/* Save data */
	std::ofstream oFile(file, std::ios::binary);
	if (!oFile.is_open()) {
		throw std::runtime_error("Cannot open specified file!\n");
	}

	if (this->mode == MODE_NETASCII) {
		convertFromNetascii();
	}

	oFile << this->message;
	oFile.close();

	this->verbose("Transfer completed without errors");
}

/**
 * @brief Send data
 */
void Client::sendData(std::string file)
{
	/* Check whether file exists and get its size */
	inFile.open(file, std::ios::ate | std::ios::binary);
	if (!inFile.is_open()) {
		throw std::invalid_argument("Cannot open file \"" + file + "\"\n");
	}

	/* Get file size */
	setTsize(inFile.tellg());
	inFile.seekg(0);

	/* Load file into memory */
	loadFile();

	/* Close file */
	inFile.close();

	/* Get only file from path */
	std::string only_file = baseName(file);

	/* Create write request and send it */
	this->verbose("Requesting WRITE to " + communicator.getIP());

	/* Send request */
	createRequest(opcode::WRQ, only_file);
	communicator.sendData(packet.str());

	/* Send data */
	sendFile();

	this->verbose("Transfer completed without errors");
}

/**
 * @brief Load file content into memory
 */
void Client::loadFile()
{
	/* Reserve space */
	message.reserve(tsize);

	/* Load file */
	message.assign((std::istreambuf_iterator<char>(inFile)),
			std::istreambuf_iterator<char>());

	/* Convert to netascii if needed */
	if (mode == MODE_NETASCII) {
		convertToNetascii();
	}

	/* Set new transfer size */
	setTsize(message.length());
}

/**
 * @brief Replace all substrings in string
 */
void Client::replaceAll(std::string &where, std::string what, std::string repl)
{
	for (size_t pos = 0;; pos += repl.length()) {
		/* find substring */
		pos = where.find(what, pos);

		if (pos == std::string::npos) {
			break;
		}

		/* remove old substring and insert new one */
		where.erase(pos, what.length());
		where.insert(pos, repl);
	}
}

/**
 * @brief Convert message from netascii to octet format
 */
void Client::convertFromNetascii()
{
	replaceAll(message, "\r\0", "\r");
	replaceAll(message, "\r\n", "\n");
}

/**
 * @brief Convert message from octet to netascii format
 */

void Client::convertToNetascii()
{
	replaceAll(message, "\r", "\r\0");
	replaceAll(message, "\n", "\r\n");
}

/**
 * @brief Send ACK packet
 */
void Client::sendAckPacket()
{
	/* Create packet */
	clearPacket();
	uint16_t net_block = htons(next_block);
	packet.write(reinterpret_cast<char*> (&net_opcodes[opcode::ACK]), sizeof (uint16_t));
	packet.write(reinterpret_cast<char*> (&net_block), sizeof (uint16_t));

	/* Send it */
	communicator.sendData(packet.str());
}

/**
 * @brief Send DATA packet
 */
void Client::sendDataPacket(std::string data)
{
	/* Create packet */
	clearPacket();
	uint16_t net_block = htons(next_block);
	packet.write(reinterpret_cast<char*> (&net_opcodes[opcode::DATA]), sizeof (uint16_t));
	packet.write(reinterpret_cast<char*> (&net_block), sizeof (uint16_t));
	packet.write(const_cast<char*> (data.c_str()), data.length());

	/* Send it */
	communicator.sendData(packet.str());
}

/**
 * @brief Send error about unrequested option
 */
void Client::sendOackErr(std::string opt)
{
	/* Create packet */
	clearPacket();
	uint16_t net_err = htons(OACK_ERR);
	packet.write(reinterpret_cast<char*> (&net_opcodes[opcode::ERROR]), sizeof (uint16_t));
	packet.write(reinterpret_cast<char*> (&net_err), sizeof (uint16_t));
	packet << "Unrequested option " << opt << '\0';

	/* Send it */
	communicator.sendData(packet.str());
}

/**
 * @brief Create blocks
 */
void Client::createBlocks(std::vector<std::string> &blocks)
{
	int total = message.length();

	/* Go through all blocks */
	for (int done = 0; done < total; done += block_size) {
		/* Get difference between total and processed size */
		int diff = total - done;

		/* At least whole one block remaining */
		if (diff <= block_size) {
			blocks.push_back(message.substr(done, block_size));
		} else if (diff > 0) {
			/* Less than one block */
			blocks.push_back(message.substr(done, diff));
		} else {
			/* sizeof(last block) == block_size, add ending block */
			blocks.push_back("");
		}
	}

	/* Free memory */
	message.erase();
	message.clear();
}

/**
 * @brief Send file to host
 */
void Client::sendFile()
{
	uint16_t block_num;
	std::string response;
	opcode opc;
	next_block = 0;
	std::vector<std::string> blocks;

	/* Send blocks */
	do {
		/* Wait for ACK */
		response = communicator.receiveData(timeout, block_size);
		if (response.empty()) {
			/* Timeouted */
			communicator.resendLast();
			continue;
		}

		/* Parse response */
		opc = this->parseOpcode(response);
		switch (opc) {
		case opcode::ACK:
			/* ACK received */
			block_num = this->proccessAck(response);
			next_block = block_num + 1;
			break;
		case opcode::OACK:
			/* parse it */
			this->proccessOack(response);
			next_block = 1;
			break;
		case opcode::ERROR:
			/* Something is wrong */
			this->proccessErrmsg(response);
			throw std::runtime_error(std::string());
		default:
			/* Wrong server response */
			throw std::runtime_error("Unexpected opcode (" + static_cast<int> (opc) + std::string(")!\n"));
		}

		/*
		 * Blocks must be created AFTER first response
		 *  - it could be OACK with block size
		 */
		if (blocks.empty()) {
			createBlocks(blocks);
		}

		/* Out of blocks */
		if (next_block > blocks.size()) {
			break;
		}

		/* Print status */
		if (next_block == (int) blocks.size()) {
			verboseSend(blocks[next_block - 1].length());
		} else {
			verboseSend(block_size);
		}

		/* Send next block */
		this->sendDataPacket(blocks[next_block - 1]);
	} while (next_block <= blocks.size());
}

/**
 * @brief Receive data from host
 */
void Client::receiveData()
{
	opcode opc;
	std::string block, dataPart;
	int block_num;
	next_block = 1;

	this->message = std::string("");

	do {
		/* Receive block */
		block = communicator.receiveData(timeout, block_size);
		if (block.empty()) {
			/* Timeouted */
			communicator.resendLast();
			continue;
		}

		/* Parse opcode */
		opc = this->parseOpcode(block);
		switch (opc) {
		case opcode::DATA:
			/* OK */
			break;
		case opcode::OACK:
			/* parse it and get data */
			this->proccessOack(block);
			next_block = 0;
			this->sendAckPacket();
			next_block = 1;
			continue;
		case opcode::ERROR:
			/* Something is wrong */
			this->proccessErrmsg(block);
			throw std::runtime_error(std::string());
		default:
			/* Wrong server response */
			throw std::runtime_error("Unexpected opcode (" + static_cast<int> (opc) + std::string(")!\n"));
		}

		/* Get data from block */
		block_num = this->proccessDatamsg(block, dataPart);
		if (block_num == next_block) {
			/* Everything is ok */
			this->message += dataPart;
		} else {
			/* Wrong block number, resend last request */
			communicator.resendLast();
			continue;
		}

		/* ACK block (if this is the main client) */
		if (main_client) {
			sendAckPacket();
		}

		/* Print verbose message */
		this->verboseData(dataPart.length());

		/* Increase block counter */
		++next_block;
		if (dataPart.length() < this->block_size) {
			break;
		}
	} while (true);
}
