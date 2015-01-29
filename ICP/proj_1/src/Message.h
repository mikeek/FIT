/** \file Message.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#ifndef MESSAGE_H
#define	MESSAGE_H


#include <iosfwd>
#include <sstream>

/**
 * \class Message
 * \brief Zprávy zasílané klientem
 */
class Message {
public:
    static std::string welcome()   { return std::string("100 WELCOME\n");  }
	static std::string get_maps()  { return std::string("111 GET_MAPS\n"); }
    static std::string get_games() { return std::string("112 GET_GAMES\n");}
    static std::string refresh()   { return std::string("122 REFRESH\n");  }
	
    static std::string go()    { return std::string("150 GO\n");   }
	static std::string stop()  { return std::string("151 STOP\n"); }
	static std::string left()  { return std::string("152 LEFT\n"); }
    static std::string right() { return std::string("153 RIGHT\n");}
    static std::string take()  { return std::string("154 TAKE\n"); }
    static std::string open()  { return std::string("155 OPEN\n"); }
	
	static std::string bye()   { return std::string("195 BYEBYE\n"); }
	static std::string leave() { return std::string("199 LEAVE_GAME\n"); }
	static std::string join_game(int id) {
		std::stringstream ss;
		ss << "130 " << id << "\n";
		return ss.str();
	}

	static std::string new_game(int id, double delay, int max_players, const char *name) {
		std::stringstream ss;
		ss << "120 " << id << " " << delay << " " << max_players << " " << name << "\n";
		return ss.str();
	}
};

/**
 * \class Message_server
 * \brief Zprávy zasílané serverem
 */
class Message_server {
public:
	static std::string unkwnown()      { return std::string("599 UNKNOWN_MESSAGE\n"); }
	static std::string cant_realize()  { return std::string("559 CANNOT_REALIZE\n"); }
	static std::string map_info_end()  { return std::string("511\n");}
	static std::string game_info_end() { return std::string("512\n");}
	static std::string request_denied() {return std::string("559 REQUEST_DENIED\n");}
	
	static std::string ok() { return std::string("501 OK"); }
	static std::string welcome(int id) {
		std::stringstream ss;
		ss << "500 " << id << "\n";
		return ss.str();
	}

	static std::string map_info(int id, std::string name) {
		std::stringstream ss;
		ss << "511 " << id << " " << name << "\n";
		return ss.str();
	}

	static std::string game_info(int id, std::string info) {
		std::stringstream ss;
		ss << "512 " << id << " " << info << "\n";
		return ss.str();
	}

	static std::string map_description(int color, std::string map_str) {
		std::stringstream ss;
		ss << "530 " << color << " " << map_str << "\n";
		return ss.str();
	}

	enum operation {
		GO = 0, STOP, LEFT, RIGHT, TAKE, OPEN
	};

	static std::string player_oper_confirm(operation oper, bool success) {
		std::stringstream ss;
		ss << "700 " << oper << " " << (success ? "0" : "1") << "\n";
		return ss.str();
	}

	static std::string map_changes(std::string description) {
		std::stringstream ss;
		ss << "550 " << description << "\n";
		return ss.str();
	}

	static std::string game_statistics(std::string log_message) {
		std::stringstream ss;
		ss << "600 " << log_message << "\n";
		return ss.str();
	}

	static std::string game_over_report(std::string report) {
		std::stringstream ss;
		ss << "800 " << report << "\n";
		return ss.str();
	}

	static std::string player_gexit_confirm() {
		return "501 OK\n";
	}
};

#endif	/* MESSAGE_H */

