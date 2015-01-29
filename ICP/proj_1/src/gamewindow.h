/** \file gamewindow.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QColor>
#include <QVector>
#include <QPixmap>
#include <QDialog>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include "Client.h"
#include "winnerswindow.h"

namespace Ui {
	class GameWindow;
}

/**
 * \class GameWindow
 * \brief Třída pro herní okno
 */
class GameWindow : public QDialog {
	Q_OBJECT

public:
	GameWindow(QWidget *parent, Client *client);
	~GameWindow();

private slots:
	void on_sendBtn_clicked();

	void on_cmdTxt_returnPressed();

	void on_GameWindow_finished(int result);

	void on_takeBtn_clicked();

	void on_goBtn_clicked();

	void on_openBtn_clicked();

	void on_leftBtn_clicked();

	void on_stopBtn_clicked();

	void on_rightBtn_clicked();

public slots:

	void refresh(std::stringstream& ss);


private:
	QLabel *new_label(bool visibility, QString name);
	void refresh_changes(std::stringstream& ss);
	void end_game();
	void logInfo(std::string msg, QColor col);
	void sendRequest();
	void createMap();

	QString pixmap_name(QString name) {
		return QString(":/new/images/img/" + name + ".png");
	}

	QColor log_red{QColor::fromRgb(150, 50, 50)};
	QColor log_green{QColor::fromRgb(50, 150, 50)};
	QColor log_blue{QColor::fromRgb(50, 50, 150)};

	u_int16_t players_cnt = 4;
	u_int16_t tile_size = 24;
	u_int16_t map_w{};
	u_int16_t map_h{};
	u_int16_t code{};

	QGraphicsScene *scene{};
	QGraphicsGridLayout *layout{};
	QGraphicsWidget *form{};
	QVector<QLabel *> players{};
	QVector<QLabel *> guards{};

	Client *cl{};
	winnerswindow *ww{};
	QVector<int> winners{};
	QVector<std::string> moves{"go", "stop", "left", "right", "take", "open"};
	QVector<std::string> result{"ok", "fail"};
	Ui::GameWindow *ui{};
};

#endif // GAMEWINDOW_H
