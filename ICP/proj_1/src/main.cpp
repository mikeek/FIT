/** \file main.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#include "mainwindow.h"
#include "Client.h"
#include <boost/asio.hpp>
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>

/**
 * \brief Hlavní funkce QT části
 * \param[in] argc Počet parametrů
 * \param[in] argv Parametry
 * \return 0 pokud je vše v pořádku
 */
int main(int argc, char *argv[])
{
	qRegisterMetaType<QVector<int> >("QVector<int>");
	QApplication a(argc, argv);

	/* vytvoření klienta */
	boost::asio::io_service io_service;
	Client cl(io_service);

	/* vytvoření hlavního okna */
	MainWindow w(&cl);

	/* vycentrovani okna */
	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	int x = (screenGeometry.width() - w.width()) / 2;
	int y = (screenGeometry.height() - w.height()) / 2;
	w.move(x, y);

	/* spuštění */
	w.show();

	return a.exec();
}
