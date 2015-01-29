/** \file gamewindow.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "Client.h"
#include "winnerswindow.h"

#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QPushButton>
#include <QGraphicsProxyWidget>

#include <QBrush>

/**
 * \brief Konstruktor herního okna 
 * \param[in] parent Předek okna
 * \param[in] client Klient pro komunikaci
 */
GameWindow::GameWindow(QWidget *parent, Client *client) :
QDialog(parent), cl(client), ui(new Ui::GameWindow)
{
	ui->setupUi(this);

	/* Úprava vzhledu historie */
	ui->horizontalLayout_2->setAlignment(Qt::AlignLeft);
	ui->logTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->logTable->verticalHeader()->hide();

	/* Zobrazení obrázku vlastního hráče */
	ui->p_img->setPixmap(pixmap_name("p" + QString::number(cl->color())));

	/* Nastavení scény a gridlayoutu */
	scene = new QGraphicsScene(this);
	layout = new QGraphicsGridLayout();
	form = new QGraphicsWidget();
	ww = new winnerswindow(this);
	ui->graphicsView->setScene(scene);

	form->setLayout(layout);

	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	ui->graphicsView->setContentsMargins(0, 0, 0, 0);

	/* vykreslení mapy */
	createMap();

	/* vložení hráčů do scény (zatím invisible) */
	for (int i = 0; i < players_cnt; i++) {
		QLabel *lbl = new_label(false, "p" + QString::number(i) + "0");
		players.push_back(lbl);
		scene->addWidget(lbl);
	}
}

/**
 * \brief Vytvoření a inicializace nového labelu
 * \param[in] visibility Viditelnost
 * \param[in] name Název pixmapy
 * \return Ukazatel na nový label
 */
QLabel *GameWindow::new_label(bool visibility, QString name)
{
	QLabel *lbl = new QLabel();
	lbl->setPixmap(pixmap_name(name));
	lbl->setStyleSheet("background: transparent; border: none");
	lbl->setGeometry(0, 0, tile_size, tile_size);
	lbl->setVisible(visibility);
	return lbl;
}

/**
 * \brief Obnovení hry
 * \param[in] ss Stringstream se změnami
 */
void GameWindow::refresh(std::stringstream& ss)
{
	std::string line;
	/* zpracování všech příchozích zpráv */
	while (std::getline(ss, line)) {
		std::stringstream strs(line);
		strs >> code;
		switch (code) {
		case 550:
			/* změny v mapě */
			refresh_changes(strs);
			break;
		case 700:
		{
			/* potvrzení/zamítnutí požadavku */
			int id, res;
			strs >> id >> res;
			logInfo(moves[id] + ": " + result[res], (res == 0) ? log_green : log_red);
			break;
		}
		case 800:
		{
			/* konec hry - seznam výherců */
			int cnt, id;
			strs >> cnt;
			for (int i = 0; i < cnt; ++i) {
				strs >> id;
				winners.push_back(id);
			}
			/* zobrazení statistik */
			ww->show_winners(winners, tile_size);
			break;
		}
		case 600:
			/* konec hry - statistiky */
			ww->set_stats(strs, tile_size);
			end_game();
			break;
		}
	}
}

/**
 * \brief Promítnutí změn do mapy
 * \param[in] ss Stringstream se změnami
 */
void GameWindow::refresh_changes(std::stringstream &ss)
{
	char obj;
	int id, x, y, dir, state;
	while (ss >> obj) {
		switch (obj) {
		case 'P':
			/* informace o hráči */
			ss >> id >> x >> y >> dir >> state;
			if (state != 0) {
				/* hráč byl zabit nebo se odpojil */
				if (id == cl->color()) {
					logInfo("Byl jsi zabit!", log_red);
				} else {
					logInfo("Hráč " + QString::number(id).toStdString() + " byl zabit", log_blue);
				}
				players[id]->setVisible(false);
			} else {
				/* změna pozice hráče */
				if (!(players[id]->isVisible())) {
					players[id]->setVisible(true);
				}
				players[id]->move(y * tile_size, x * tile_size);
				players[id]->setPixmap(pixmap_name("p" + QString::number(id) + QString::number(dir)));
			}
			break;
		case 'H':
			/* změna pozice hlídače */
			ss >> id >> x >> y >> dir;
			while (guards.size() < (id + 1)) {
				/* alokování místa pro hlídače */
				QLabel *lbl = new_label(false, "");
				guards.push_back(lbl);
				scene->addWidget(lbl);
			}
			if (!guards[id]->isVisible()) {
				guards[id]->setVisible(true);
			}

			guards[id]->setPixmap(pixmap_name("h" + QString::number(dir)));
			guards[id]->move(y * tile_size, x * tile_size);
			break;
		case 'K':
			ss >> x >> y >> state;
			if (state == 0) {
				/* někdo zvedl klíč */
				QGraphicsLayoutItem *it = layout->itemAt(x, y);
				if (it != nullptr) {
					layout->removeItem(it);
					scene->removeItem(it->graphicsItem());
					delete it;
				}
			} else {
				/* umístění klíče do mapy */
				QLabel *lbl = new_label(true, "k");
				layout->addItem(scene->addWidget(lbl), x, y);
			}
			break;
		case 'G':
			/* otevření brány */
			/* odebrání uzavřené */
			ss >> x >> y;
			QGraphicsLayoutItem *it = layout->itemAt(x, y);
			if (it != nullptr) {
				layout->removeItem(it);
				scene->removeItem(it->graphicsItem());
				delete it;
			}

			/* přidání otevřené */
			QLabel *lbl = new_label(true, "g1");
			layout->addItem(scene->addWidget(lbl), x, y);
			break;
		}
	}
	ui->graphicsView->repaint();
	ui->graphicsView->update();
}

/**
 * \brief Konec hry - vypnutí GUI prvků
 */
void GameWindow::end_game()
{
	ui->cmdTxt->setEnabled(false);
	ui->sendBtn->setEnabled(false);

	/* zastavení vlákna pro příjem zpráv ze serveru */
	//     cl->stop_listen();
}

/**
 * \brief Logování informací
 * \param[in] msg Zpráva
 * \param[in] col Barva
 */
void GameWindow::logInfo(std::string msg, QColor col)
{
	/* přidání řádku do tabulky */
	ui->logTable->setRowCount(ui->logTable->rowCount() + 1);

	/* vytvoření nového řádku */
	QTableWidgetItem *twi = new QTableWidgetItem(QString::fromStdString(msg));
	twi->setBackgroundColor(col);

	ui->logTable->resizeRowsToContents();
	ui->logTable->setItem(ui->logTable->rowCount() - 1, 0, twi);
}

/**
 * \brief Vytvoření mapy
 */
void GameWindow::createMap()
{
	QLabel *label;
	QGraphicsProxyWidget *widget;
	QString pix_str{};

	map_form_t *act_map = cl->get_map();
	int index = 0;

	/* získání rozměrů mapy */
	map_w = act_map->cols * tile_size;
	map_h = act_map->rows * tile_size;

	/* pozadí mapy */
	scene->addRect(0, 0, map_w, map_h, QPen(Qt::NoPen), QBrush(Qt::black));
	scene->addItem(form);

	/* nastavení šířky sloupců gridlayoutu */
	for (int j = 0; j < act_map->cols; ++j) {
		layout->setColumnFixedWidth(j, tile_size);
	}

	/* vykreslení jednotlivých dlaždic */
	for (int i = 0; i < act_map->rows; ++i) {
		layout->setRowFixedHeight(i, tile_size);
		for (int j = 0; j < act_map->cols; ++j) {
			switch (act_map->items[index + 1]) {
			case '#':
				pix_str = "w";
				break;
			case 'O':
				pix_str = "g1";
				break;
			case 'C':
				pix_str = "g0";
				break;
			case 'G':
				pix_str = "f";
				break;
			case '+':
				pix_str = "k";
				break;
			default:
				pix_str = "";
			}
			if (!pix_str.isEmpty()) {
				label = new QLabel();
				label->setGeometry(0, 0, tile_size, tile_size);
				label->setPixmap(QPixmap(pixmap_name(pix_str)));
				label->setStyleSheet("background: transparent; border: none");
				widget = scene->addWidget(label);
				layout->addItem(widget, i, j);
			}
			index++;
		}
	}
	ui->graphicsView->setGeometry(0, 0, map_w, map_h);
}

/**
 * \brief Zaslání požadavku serveru
 */
void GameWindow::sendRequest()
{
	/* zaslání požadavku na server */
	if (ui->cmdTxt->text() == "go") {
		cl->go();
	} else if (ui->cmdTxt->text() == "stop") {
		cl->stop();
	} else if (ui->cmdTxt->text() == "left") {
		cl->left();
	} else if (ui->cmdTxt->text() == "right") {
		cl->right();
	} else if (ui->cmdTxt->text() == "take") {
		cl->take();
	} else if (ui->cmdTxt->text() == "open") {
		cl->open();
	}

	ui->cmdTxt->setText("");
}

/**
 * \brief Potvrzení požadavku
 */
void GameWindow::on_sendBtn_clicked()
{
	sendRequest();
}

/**
 * \brief Potvrzení požadavku
 */
void GameWindow::on_cmdTxt_returnPressed()
{
	sendRequest();
}

/**
 * \brief Uzavření okna
 * \param[in] result výsledek (ignorován)
 */
void GameWindow::on_GameWindow_finished(int result)
{
	(void) result;
	/* zaslání leave zprávy */
	cl->leave();
}

/**
 * \breif Destruktor
 */
GameWindow::~GameWindow()
{
	delete ui;
}

/**
 * \brief Tlačítko TAKE
 */
void GameWindow::on_takeBtn_clicked()
{
	cl->take();
}

/**
 * \brief Tlačítko GO
 */
void GameWindow::on_goBtn_clicked()
{
	cl->go();
}

/**
 * \brief Tlačítko OPEN
 */
void GameWindow::on_openBtn_clicked()
{
	cl->open();
}

/**
 * \brief Tlačítko LEFT
 */
void GameWindow::on_leftBtn_clicked()
{
	cl->left();
}

/**
 * \brief Tlačítko STOP
 */
void GameWindow::on_stopBtn_clicked()
{
	cl->stop();
}

/**
 * \brief Tlačátko RIGHT
 */
void GameWindow::on_rightBtn_clicked()
{
	cl->right();
}
