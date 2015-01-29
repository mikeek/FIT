/** \file emitter.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#ifndef EMITTER_H
#define EMITTER_H

#include <QThread>
#include "Client.h"

/**
 * \class emitter
 * \brief QThread s přepsanou funkcí run()
 */
class emitter : public QThread {
	Q_OBJECT

public:
	emitter(Client *cl);
protected:
	virtual void run();

signals:
	/* signál, který je napojen na refresh socket herního okna v GUI vlákně */
	void signalGUI(std::stringstream&);

public slots:
private:
	/* klient pro komunikaci */
	Client *cl;
};

#endif // EMITTER_H
