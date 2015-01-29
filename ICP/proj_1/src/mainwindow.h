/** \file mainwindow.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Client.h"
#include "mapmenu.h"

namespace Ui {
	class MainWindow;
}

/**
 * \class MainWindow
 * \brief Třída pro hlavní okno programu
 */
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(Client *client);
	~MainWindow();

private slots:
	void on_connectBtn_clicked();

private:
	Client *cl;
	MapMenu *mm{};
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
