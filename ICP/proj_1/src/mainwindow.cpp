/** \file mainwindow.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Client.h"
#include "mapmenu.h"
#include <boost/asio.hpp>

/**
 * \brief Konstruktor
 * \param[in] client Klient pro komunikaci
 */
MainWindow::MainWindow(Client *client) :
cl(client), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

/**
 * \brief Destruktor
 */
MainWindow::~MainWindow()
{
	delete ui;
}

/**
 * \brief Připojení k serveru
 */
void MainWindow::on_connectBtn_clicked()
{
	try {
		cl->connect(ui->serverAddr->text().toStdString().c_str(),
				ui->serverPort->text().toStdString().c_str());
	} catch (std::exception& e) {
		ui->statusBar->showMessage(e.what());
		return;
	}

	/* vytvoření nového podokna */
	mm = new MapMenu(this, cl);
	this->hide();
	try {
		mm->exec();
	} catch (std::exception& e) {
		ui->statusBar->showMessage(e.what());
	}
}
