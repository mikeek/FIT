/** \file winnerswindow.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#include "winnerswindow.h"
#include "ui_winnerswindow.h"
#include <sstream>

/**
 * \brief Konstruktor
 * \param[in] parent Předek
 */
winnerswindow::winnerswindow(QWidget *parent) :
QDialog(parent), ui(new Ui::winnerswindow)
{
	ui->setupUi(this);
	setWindowTitle("Konec hry");
	setWindowModality(Qt::WindowModal);

	/* inicializace labelů */
	labvec[0] = ui->w0;
	labvec[1] = ui->w1;
	labvec[2] = ui->w2;
	labvec[3] = ui->w3;

	infovec[0] = ui->info0;
	infovec[1] = ui->info1;
	infovec[2] = ui->info2;
	infovec[3] = ui->info3;

	for (int i = 0; i < 4; ++i) {
		labvec[i]->setVisible(false);
		infovec[i]->setVisible(false);
	}
}

/**
 * \brief Destruktor
 */
winnerswindow::~winnerswindow()
{
	delete ui;
}

/**
 * \brief Nastavení statistik
 * \param[in] ss Stringstream s informacemi o hře a hráčích
 * \param[in] tile_size Velikost herního políčka
 */
void winnerswindow::set_stats(std::stringstream& ss, int tile_size)
{
	/* nastavení rozměrů */
	ui->frame_2->setFixedHeight(tile_size * 2);
	ui->frame_2->setFixedWidth(tile_size);

	/* zjištění herní doby */
	char obj;
	int id, player_time, game_time, steps, killed;
	ss >> game_time;
	ui->gameTime->setText(QString::number(game_time) + "s");

	/* zjištění informací o hráčích */
	while (ss >> obj) {
		if (obj != 'P') {
			break;
		}
		ss >> id >> player_time >> steps >> killed;
		infovec[id]->setGeometry(0, 0, tile_size, tile_size);
		infovec[id]->setVisible(true);
		infovec[id]->setToolTip("Čas hráče: " + QString::number(player_time) +
				"s, kroků: " + QString::number(steps) +
				", zabit: " + QString::number(killed) + " krát");
		ui->frame_2->setFixedWidth(ui->frame_2->width() + ui->horizontalLayout_2->spacing() + tile_size);
	}
}

/**
 * \brief Zobrazení výherců
 * \param[in] winners vektor výherců
 * \param[in] tile_size Velikost herního políčka
 */
void winnerswindow::show_winners(QVector<int> winners, int tile_size)
{
	ui->frame->setFixedHeight(tile_size * 2);
	ui->frame->setFixedWidth(tile_size);

	for (auto& id : winners) {
		labvec[id]->setGeometry(0, 0, tile_size, tile_size);
		labvec[id]->setStyleSheet("background: transparent; border: none");
		labvec[id]->setVisible(true);
		ui->frame->setFixedWidth(ui->frame->width() + ui->horizontalLayout->spacing() + tile_size);
	}

	/* zobrazení okna */
	exec();
}
