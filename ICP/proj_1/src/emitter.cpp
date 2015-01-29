/** \file emitter.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#include "emitter.h"
#include <boost/asio.hpp>

/**
 * \brief Konstruktor
 * \param[in] client Klient pro komunikaci se serverem
 */
emitter::emitter(Client *client) :
cl(client)
{
}

/**
 * \brief Hlavní funkce vlákna
 */
void emitter::run()
{
	/* naslouchání serveru a poslání signálu vláknu s GUI */
	cl->listen_this_thread([this](std::stringstream & ss) {
		emit signalGUI(ss);
	});
}
