/** \file mapmenu.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#ifndef MAPMENU_H
#define MAPMENU_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStatusBar>
#include "Client.h"
#include "gamewindow.h"

namespace Ui {
	class MapMenu;
}

/**
 * \class MapMenu
 * \brief Třída reprezentující okno pro výběr map a her
 */
class MapMenu : public QDialog {
	Q_OBJECT

public:
	MapMenu(QWidget *parent, Client *client);
	~MapMenu();

private slots:
	void on_createBtn_clicked();

	void on_joinBtn_clicked();

	void on_refreshMapsBtn_clicked();

	void on_refreshGamesBtn_clicked();

	void on_mapList_itemClicked(QListWidgetItem *item);

	void on_gameList_itemClicked(QListWidgetItem *item);

	void on_MapMenu_finished(int result);

private:
	void showMsgBox(QString msg, QString title = "Bludiště 2014 ");
	void startGame(QString game_name);

	QStatusBar *bar;
	GameWindow *gw;
	Client *cl;
	Ui::MapMenu *ui;
};

#endif // MAPMENU_H
