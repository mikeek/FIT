/** \file mapmenu.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#include "emitter.h"
#include "mapmenu.h"
#include "ui_mapmenu.h"
#include "gamewindow.h"
#include "Client.h"
#include <QVariant>
#include <QStringList>
#include <QMessageBox>

/**
 * \brief Konstruktor
 * \param[in] parent Předek
 * \param[in] client Klient pro komunikaci
 */
MapMenu::MapMenu(QWidget *parent, Client *client) :
QDialog(parent), cl(client), ui(new Ui::MapMenu)
{
	ui->setupUi(this);
}

/**
 * \brief Destruktor
 */
MapMenu::~MapMenu()
{
	delete ui;
}

/**
 * \brief Obnovení seznamu map
 */
void MapMenu::on_refreshMapsBtn_clicked()
{
	Client::mapvec maps;
	try {
		ui->mapList->clear();
		maps = cl->get_maps();
	} catch (std::exception &e) {
		this->showMsgBox(QString(e.what()));
		return;
	}

	/* vyplnění list boxu */
	int index = 0;
	for (auto& map_info : maps) {
		ui->mapList->addItem(map_info.name.c_str());
		QVariant var(map_info.id);
		ui->mapList->item(index)->setData(Qt::UserRole, var);
		index++;
	}
}

/**
 * \brief Obnovení seznamu her
 */
void MapMenu::on_refreshGamesBtn_clicked()
{
	Client::gamevec games;
	try {
		ui->gameList->clear();
		games = cl->get_games();
	} catch (std::exception &e) {
		this->showMsgBox(QString(e.what()));
		return;
	}

	/* vyplnění list boxu */
	int index = 0;
	for (auto& game_info : games) {
		ui->gameList->addItem(game_info.name.c_str());
		QList<int> lst;
		lst << game_info.id << game_info.act_players << game_info.max_players;
		QVariant var = QVariant::fromValue(lst);
		ui->gameList->item(index)->setData(Qt::UserRole, var);
		index++;
	}
}

/**
 * \brief Výběr mapy -> zobrazení informací
 * \param[in] item Vybraná položka
 */
void MapMenu::on_mapList_itemClicked(QListWidgetItem *item)
{
	ui->mapIdTxt->setText(item->data(Qt::UserRole).toString());
	ui->nameTxt->setText(item->text());
}

/**
 * \brief Výběr hry -> zobrazení informací
 * \param[in] item Vybraná položka
 */
void MapMenu::on_gameList_itemClicked(QListWidgetItem *item)
{
	QList<int> lst = item->data(Qt::UserRole).value<QList<int>>();
	ui->gameIdTxt->setText(QString::number(lst[0]));
	ui->showPlayersBox->setValue(lst[1]);
	ui->showMaxPlayersBox->setValue(lst[2]);
}

/**
 * \brief Zobrazení okna se zprávou
 * \param[in] msg Zpráva
 * \param[in] title Titulek okna
 */
void MapMenu::showMsgBox(QString msg, QString title)
{
	QMessageBox *mbox = new QMessageBox(this);
	QSpacerItem* horizontalSpacer = new QSpacerItem(300, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	QGridLayout* layout = (QGridLayout*) mbox->layout();
	layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

	mbox->setWindowTitle(title);
	mbox->setText(msg);
	mbox->exec();
}

/**
 * \brief Žádost o vytvoření nové hry
 */
void MapMenu::on_createBtn_clicked()
{
	if (ui->nameTxt->text().isEmpty()) {
		showMsgBox("Nezadán název hry");
		return;
	}

	try {
		int ret = cl->create(ui->mapIdTxt->text().toInt(),
				ui->delayBox->value(),
				ui->playersBox->value(),
				ui->nameTxt->text().toStdString().c_str());
		if (ret != 0) {
			showMsgBox(QString::fromStdString(cl->last()));
			return;
		}
	} catch (std::exception &e) {
		showMsgBox(QString(e.what()));
		return;
	}

	/* spuštění hry */
	startGame(ui->nameTxt->text());
}

/**
 * \brief Žádost o připojení ke hře
 */
void MapMenu::on_joinBtn_clicked()
{
	if (ui->gameList->selectedItems().empty()) {
		showMsgBox("Nevybrána hra");
		return;
	}

	try {
		int ret = cl->join(ui->gameIdTxt->text().toInt());
		if (ret != 0) {
			showMsgBox(QString::fromStdString(cl->last()));
			return;
		}
	} catch (std::exception &e) {
		showMsgBox(QString(e.what()));
		return;
	}

	/* spuštění hry */
	startGame(ui->gameList->currentItem()->text());
}

/**
 * \brief Spuštění hry
 * \param[in] game_name Název hry
 */
void MapMenu::startGame(QString game_name)
{
	emitter *e = new emitter(cl);
	gw = new GameWindow(this, cl);

	/* propojení vlákna přijímajícího změny s refresh slotem herního okna */
	connect(gw, SIGNAL(windowTitleChanged(QString)), e, SLOT(start()));
	connect(e, SIGNAL(signalGUI(std::stringstream&)), gw, SLOT(refresh(std::stringstream&)), Qt::BlockingQueuedConnection);
	connect(e, SIGNAL(finished()), e, SLOT(deleteLater()));
	gw->setWindowTitle(windowTitle() + ": " + game_name);
	try {
		/* spuštění hry */
		gw->exec();
	} catch (std::exception &e) {
		showMsgBox(QString(e.what()));
	}
	//    delete e;

}

/**
 * \brief Uzavření okna
 * \param[in] result Výsledek (ignorován)
 */
void MapMenu::on_MapMenu_finished(int result)
{
	(void) result;

	/* odpojení od serveru */
	cl->disconnect();

	/* zobrazení rodičovského okna */
	this->parentWidget()->show();
}
