/** \file winnerswindow.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#ifndef WINNERSWINDOW_H
#define WINNERSWINDOW_H

#include <QDialog>
#include <QLabel>
#include "infolabel.h"

namespace Ui {
	class winnerswindow;
}

/**
 * \class winnerswindow
 * \brief Třída reprezentující okno informující o konci hry
 */
class winnerswindow : public QDialog {
	Q_OBJECT

public:
	winnerswindow(QWidget *parent = 0);
	void set_stats(std::stringstream& ss, int tile_size);
	void show_winners(QVector<int> winners, int tile_size);
	~winnerswindow();

private:
	QVector<QLabel *> labvec = QVector<QLabel *>(4);
	QVector<infoLabel *> infovec = QVector<infoLabel *>(4);
	Ui::winnerswindow *ui;
};

#endif // WINNERSWINDOW_H
