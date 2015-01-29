/** \file bludiste2014-cli.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#include <boost/asio.hpp>
#include <ncurses.h>
#include <menu.h>
#include <iostream>
#include <vector>

#include "Client.h"

/**
 * \enum
 * \brief Typ operace
 */
enum {
	CREATE_GAME = 0,
	JOIN_GAME,
	EXIT_APP
};

/**
 * \enum
 * \brief Vykreslované části mapy
 */
enum tiles {
	TILE_SPACE = ' ',
	TILE_WALL = '#',
	TILE_OP_GATE = 'O',
	TILE_CL_GATE = 'A',
	TILE_KEY = '&',
	TILE_GOAL = 'X'
};

/**
 * \enum
 * \brief ID barvy hlídače
 */
enum {
	GUARD_COLOR = 5
};

/**
 * \brief Zavření a uvolnění okna
 * \param[in] win ukazatel na okno
 */
void close_free(WINDOW *win)
{
	wclear(win);
	refresh();
	wrefresh(win);
	delwin(win);
}

/**
 * \class Label
 * \brief Pohyblivý prvek na mapě
 */
class Label {
public:

	/* konstruktor - vytvoření okna, nastavení barvy */
	Label(int col)
	{
		pwin = newwin(1, 1, 0, 0);
		_color = col;
		wattron(pwin, COLOR_PAIR(_color));
		wattron(pwin, A_BOLD);
	}

	bool visible()
	{
		return _visible;
	}

	/* nastavení pozice */
	void move(int y, int x)
	{
		mvwin(pwin, y, x);
	}

	/* nastavení viditelnosti */
	void set_visible(bool val)
	{
		_visible = val;
	}

	/* nastavení symbolu - při změně směru */
	void set_symbol(char symbol)
	{
		_symbol = symbol;
	}

	/* překreslení okna */
	void refresh()
	{
		if (_visible) {
			wclear(pwin);
			waddch(pwin, _symbol);
			wrefresh(pwin);
		}
	}

	~Label()
	{
		close_free(pwin);
	}
private:
	bool _visible{false};
	char _symbol;
	int _color;
	WINDOW *pwin;
};

static bool in_game = false; /* indikátor spuštěné hry */
static Client *cl; /* klient pro komunikaci se serverem */
static Client::mapvec maps; /* vektor pro načtení map */
static Client::gamevec games; /* vektor pro načtení her */
static WINDOW *bar_win; /* okno navigace */
static WINDOW *backg; /* herni okno s pozadim */
static WINDOW *cmd_win; /* herni okno pro příkazy */
static std::vector<Label *> players(4); /* vektor hráčů ve hře */
static std::vector<int> winners; /* vektor výherců */
static std::vector<Label *> guards; /* vektor hlídačů */
static const char dirs[] = {/* symboly pro jednotlivé směry */
	'^', '>', 'v', '<'
};
static const char *colors[] = {/* string hodnota barev pro výpis */
	"red", "green", "blue", "yellow"
};

/**
 * \brief Zobrazení okna s chybovou hláškou
 * \param[in] msg Chybová zpráva
 */
void msg_box(const char *msg)
{
	int row, col;
	getmaxyx(stdscr, row, col);
	WINDOW *mbox = newwin(1, strlen(msg) + 1, row - 4, 10);
	wprintw(mbox, msg);
	refresh();
	wrefresh(mbox);
	getch();
	close_free(mbox);
}

/**
 * \brief Převedení vektorů položek a popisu menu na pole ITEMS pro ncurses
 * \param[in] opts Položky menu
 * \param[in] descs Popis položek
 * \return pole položek menu
 */
ITEM **items_from_vec(std::vector<std::string> opts, std::vector<std::string> descs)
{
	ITEM **it = (ITEM **) calloc(opts.size() + 1, sizeof (ITEM *));
	if (it == nullptr) {
		return nullptr;
	}
	for (uint i = 0; i < opts.size(); ++i) {
		it[i] = new_item(opts[i].c_str(), descs[i].c_str());
	}

	it[opts.size()] = (ITEM *) nullptr;
	return it;
}

/**
 * \brief Zobrazení menu
 * \param[in] menu_items Položky menu
 * \param[in] w0 Gemetrie okna
 * \param[in] w1 Gemetrie okna
 * \param[in] w2 Gemetrie okna
 * \param[in] w3 Gemetrie okna
 * \param[in] handler Funkce reagující na výběr některé položky
 */
void show_menu(ITEM **menu_items, int w0, int w1, int w2, int w3, bool (*handler)(int))
{
	int c;
	bool more = true;
	WINDOW *menu_win;
	MENU *menu;

	/* vytvoření nového okna pro menu */
	menu = new_menu(menu_items);
	menu_win = newwin(w0, w1, w2, w3);
	keypad(menu_win, TRUE);

	set_menu_win(menu, menu_win);
	set_menu_sub(menu, derwin(menu_win, w0 - 1, w1 - 1, 1, 1));
	set_menu_format(menu, w0 - 1, 1);
	set_menu_mark(menu, ">");
	box(menu_win, 0, 0);

	noecho();
	refresh();
	post_menu(menu);
	wrefresh(menu_win);

	/* pohyb mezi položkami menu */
	while ((c = wgetch(menu_win)) != KEY_LEFT) {
		switch (c) {
		case KEY_DOWN:
			menu_driver(menu, REQ_DOWN_ITEM);
			break;
		case KEY_UP:
			menu_driver(menu, REQ_UP_ITEM);
			break;
		case KEY_NPAGE:
			menu_driver(menu, REQ_SCR_DPAGE);
			break;
		case KEY_PPAGE:
			menu_driver(menu, REQ_SCR_UPAGE);
			break;
		case '\n':
		case KEY_RIGHT:
			/* potvrzení - vyvolání handleru */
			box(menu_win, ' ', ' ');
			wrefresh(menu_win);
			more = handler(item_index(current_item(menu)));
			box(menu_win, 0, 0);
			break;
		}
		wrefresh(menu_win);
		if (!more) {
			break;
		}
	}
	/* uvolnění menu */
	unpost_menu(menu);
	free_menu(menu);

	/* uvolnění položek menu */
	int index = 0;
	while (menu_items[index]) {
		free_item(menu_items[index++]);
	}

	free(menu_items);

	/* zrušení okna */
	wclear(menu_win);
	wrefresh(menu_win);
	delwin(menu_win);
}

/**
 * \brief Vytvoření okna s mapou
 */
void create_map()
{
	/* vytvoření okna */
	map_form_t *act_map = cl->get_map();
	backg = newwin(act_map->rows + 2, act_map->cols + 2, 0, 0);
	box(backg, 0, 0);
	char tile;
	/* vyplnění */
	int index = 0;
	for (int i = 0; i < act_map->rows; ++i) {
		for (int j = 0; j < act_map->cols; ++j) {
			switch (act_map->items[index + 1]) {
			case '#':
				tile = TILE_WALL;
				break;
			case 'O':
				tile = TILE_OP_GATE;
				break;
			case 'C':
				tile = TILE_CL_GATE;
				break;
			case 'G':
				tile = TILE_GOAL;
				break;
			case '+':
				tile = TILE_KEY;
				break;
			default:
				tile = TILE_SPACE;
				break;
			}
			mvwaddch(backg, i + 1, j + 1, tile);
			index++;
		}
	}
	refresh();
	wrefresh(backg);
}

/**
 * \brief Zaslání požadavku serveru
 * \param[in] cmd Požadavek
 */
void send_request(const char *cmd)
{
	std::string str_cmd{cmd};
	if (str_cmd == "go") {
		cl->go();
	} else if (str_cmd == "stop") {
		cl->stop();
	} else if (str_cmd == "left") {
		cl->left();
	} else if (str_cmd == "right") {
		cl->right();
	} else if (str_cmd == "take") {
		cl->take();
	} else if (str_cmd == "open") {
		cl->open();
	}
}

/**
 * \brief Zobrazení statistik
 * \param[in] ss Stringstream se statistikami
 */
void show_stats(std::stringstream& ss)
{
	std::stringstream stats;
	int game_time;
	in_game = false;

	ss >> game_time;
	stats << "Game over! Game time: " << game_time << "s. ";

	noecho();
	wclear(cmd_win);
	box(cmd_win, 0, 0);
	wmove(cmd_win, 1, 1);
	wprintw(cmd_win, stats.str().c_str());
}

/**
 * \brief Zobrazení výherců
 * \param[in] winners Vektor ID výherců
 */
void show_winners(std::vector<int> winners)
{
	//    cl->stop_listen();
	std::stringstream stats;
	stats << "Winners: ";
	if (winners.size() == 0) {
		stats << "nobody";
	}

	for (auto& id : winners) {
		if (id == cl->color()) {
			stats << "YOU, ";
		} else {
			stats << colors[id] << ", ";
		}
	}

	wprintw(cmd_win, stats.str().c_str());
	wrefresh(cmd_win);
}

/**
 * \brief Promítnutí změn do mapy
 * \param[in] ss Stringstream se změnami
 */
void refresh_changes(std::stringstream& ss)
{
	char obj;
	int id, x, y, dir, state;

	std::vector<WINDOW *> to_ref;
	while (ss >> obj) {
		switch (obj) {
		case 'P':
			/* Změna stavu hráče */
			ss >> id >> x >> y >> dir >> state;
			if (state != 0) {
				if (id == cl->color()) {
					//                    log_info(0, 0);
				} else {
					//                    log_info(0, 0);
				}
				players[id]->set_visible(false);
			} else {
				players[id]->set_visible(true);
				players[id]->move(x + 1, y + 1);
				players[id]->set_symbol(dirs[dir]);
			}
			break;
		case 'H':
			/* změna stavu hlídače */
			ss >> id >> x >> y >> dir;
			while (guards.size() < static_cast<uint> (id + 1)) {
				guards.push_back(new Label(GUARD_COLOR));
			}
			guards[id]->set_visible(true);
			guards[id]->move(x + 1, y + 1);
			guards[id]->set_symbol(dirs[dir]);
			break;
		case 'K':
			/* změna stavu klíče */
			ss >> x >> y >> state;
			if (state == 0) {
				mvwprintw(backg, x + 1, y + 1, "%c", TILE_SPACE);
			} else {
				mvwprintw(backg, x + 1, y + 1, "%c", TILE_KEY);
			}
			break;
		case 'G':
			/* otevření brány */
			ss >> x >> y;
			mvwprintw(backg, x + 1, y + 1, "%c", TILE_OP_GATE);
			break;
		}
	}

	/* překreslení mapy */
	box(backg, 0, 0);
	wrefresh(backg);

	/* překreslení všech hlídačů */
	for (auto& guard : guards) {
		guard->refresh();
	}

	/* překreslení hráčů */
	for (auto& player : players) {
		player->refresh();
	}
}

/**
 * \brief Logování informací - pro cli verze pravděpodobně zbytečnost
 * \param[in] what akce
 * \param[in] result výsledek
 */
void log_info(int what, int result)
{
	(void) what;
	(void) result;
}

/**
 * \brief Zpracování zprávy se změnami od serveru 
 * \param[in] ss Stringstream se změnami
 */
void refresh_game(std::stringstream& ss)
{
	int code;
	std::string line;
	while (std::getline(ss, line)) {
		std::stringstream strs(line);
		strs >> code;
		switch (code) {
		case 550:
			/* změna na mapě */
			refresh_changes(strs);
			break;
		case 700:
		{
			/* logování informací */
			int id, res;
			strs >> id >> res;
			log_info(id, res);
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
			show_winners(winners);
			break;
		}
		case 600:
			/* konec hry - statistiky */
			show_stats(strs);
			break;
		}
	}
}

/**
 * \brief Zobrazení herního okna
 */
void game_window()
{
	clear();
	refresh();

	int row, col;
	char cmd[6];

	/* vykreslení mapy */
	getmaxyx(stdscr, row, col);
	create_map();

	/* vykreslení příkazového řádku */
	cmd_win = newwin(3, col, row - 3, 0);
	box(cmd_win, 0, 0);
	wrefresh(cmd_win);

	/* vytvoření hráčů */
	for (int i = 0; i < 4; ++i) {
		players[i] = new Label(i + 1);
	}

	in_game = true;
	cl->listen(refresh_game);

	echo();
	int i, c;
	keypad(cmd_win, TRUE);

	/* přijímání příkazů od uživatele */
	std::string prefix = std::string(colors[cl->color()]) + std::string(": ");
	while (in_game) {
		wclear(cmd_win);
		box(cmd_win, 0, 0);
		mvwprintw(cmd_win, 1, 1, prefix.c_str());
		wrefresh(cmd_win);
		i = 0;
		/* buď zadání příkazu nebo reakce na šipky */
		while ((c = wgetch(cmd_win)) != '\n') {
			switch (c) {
			case KEY_DOWN:
				send_request("stop");
				break;
			case KEY_UP:
				send_request("go");
				break;
			case KEY_LEFT:
				send_request("left");
				break;
			case KEY_RIGHT:
				send_request("right");
				break;
			default:
				if (isalpha(c) && i < 5) {
					cmd[i++] = c;
				}
				break;
			}
		}
		cmd[i] = '\0';
		if (!strcmp(cmd, "exit")) {
			break;
		}
		send_request(cmd);
	}
	noecho();
	curs_set(0);

	cl->leave();

	/* zrušení hráčů */
	for (i = 0; i < static_cast<int> (players.size()); ++i) {
		delete players[i];
		players[i] = nullptr;
	}

	/* zrušení hlídačů */
	for (i = guards.size() - 1; i >= 0; --i) {
		delete guards[i];
		guards.pop_back();
	}

	/* zrušení oken */
	close_free(backg);
	close_free(cmd_win);

	/* vykreslení navigačního okna */
	box(bar_win, 0, 0);
	wrefresh(bar_win);
}

/**
 * \brief Připojení k rozehrané hře 
 * \param[in] choice ID hry
 * \return info o tom, zda-li má navigace zůstat v menu pro výběr hry
 */
bool join_win(int choice)
{
	try {
		int ret = cl->join(games[choice].id);
		if (ret != 0) {
			msg_box(cl->last().c_str());
			return true;
		}
	} catch (std::exception &e) {
		msg_box(e.what());
		return true;
	}

	/* spuštění hry */
	game_window();

	return false;
}

/**
 * \brief Vytvoření nové hry
 * \param[in] choice ID mapy
 * \return info o tom, zda-li má navigace zůstat v menu pro výběr mapy 
 */
bool create_win(int choice)
{
	double delay;
	int max_players;
	char sdelay[3], smax_players[1], name[30];
	WINDOW *input_win = newwin(5, 50, 15, 4);
	box(input_win, 0, 0);

	/* popisky */
	mvwprintw(input_win, 1, 1, "   Delay (s): ");
	mvwprintw(input_win, 2, 1, "Max. players: ");
	mvwprintw(input_win, 3, 1, "   Game name: ");

	refresh();
	wrefresh(input_win);

	/* input boxy */
	echo();
	curs_set(1);

	/* zadání delay */
	mvwgetnstr(input_win, 1, 15, sdelay, 3);
	delay = std::atof(sdelay);
	delay = (delay < 0.5) ? 0.5 : (delay > 5.0) ? 5.0 : delay;

	/* zadání max. počtu hráčů */
	mvwgetnstr(input_win, 2, 15, smax_players, 1);
	max_players = std::atoi(smax_players);
	max_players = (max_players < 1) ? 1 : (max_players > 4) ? 4 : max_players;

	/* zadání názvu hry */
	mvwgetnstr(input_win, 3, 15, name, 30);
	curs_set(0);
	noecho();

	if (strlen(name) == 0) {
		msg_box("Empty name!");
		close_free(input_win);
		return true;
	}

	/* vytvoření hry */
	try {
		int ret = cl->create(maps[choice].id, delay, max_players, name);
		if (ret != 0) {
			msg_box(cl->last().c_str());
			return true;
		}
	} catch (std::exception &e) {
		msg_box(e.what());
		return true;
	}

	/* zobrazení herního okna */
	game_window();

	/* zrušení okna pro zadávání informací */
	close_free(input_win);
	return false;
}

/**
 * \brief Handler okna pro vytvoření nové hry nebo připojení k existující
 * \param[in] choice Zvolená volba
 * \return Info o tom, zda-li má navigace pokračovat
 */
bool map_win(int choice)
{
	std::vector<std::string> opts, descs;
	bool (*handler)(int) = nullptr;
	switch (choice) {
	case CREATE_GAME:
		/* vytvoření hry -> zobrazit mapy */
		maps = cl->get_maps();
		for (auto& m : maps) {
			opts.push_back(m.name);
			descs.push_back("");
		}
		handler = create_win;
		break;
	case JOIN_GAME:
		/* připojení ke hře -> zobrazit hry */
		games = cl->get_games();
		for (auto& g : games) {
			std::stringstream ss;
			ss << "(" << g.act_players << "/" << g.max_players << ")";
			opts.push_back(g.name);
			descs.push_back(ss.str());
		}
		handler = join_win;
		break;
	case EXIT_APP:
		/* ukončit aplikaci */
		return false;
		break;
	}

	/* vytvoření položek nového menu a jeho zobrazení */
	ITEM **it = items_from_vec(opts, descs);
	show_menu(it, 10, 30, 4, 20, handler);
	return true;
}

/**
 * \brief Hlavní okno pro připojení k serveru
 */
void main_win()
{
	char host[30], port[10];
	int row, col, midrow, midcol;

	border(0, 0, 0, 0, 0, 0, 0, 0);

	getmaxyx(stdscr, row, col);
	midrow = row / 2;
	midcol = col / 2;

	/* popisky */
	mvprintw(midrow - 1, 10, "Server: ");
	mvprintw(midrow, 12, "Port: ");

	/* input boxy pro server a port */
	mvgetnstr(midrow - 1, 18, host, 30);
	mvgetnstr(midrow, 18, port, 10);

	/* připojení k serveru */
	cl->connect(host, port);

	/* zobrazení navigačního okna */
	clear();
	curs_set(0);
	char bar_txt[] = "<- back | next -> | PGUP/PGDOWN scroll menu";
	int txt_len = strlen(bar_txt);

	bar_win = newwin(3, txt_len + 2, row - 3, midcol - txt_len / 2);
	mvwprintw(bar_win, 1, 1, bar_txt);
	box(bar_win, 0, 0);
	refresh();
	wrefresh(bar_win);

	/* zobrazení hlavního menu */
	std::vector<std::string> opts{"create", "join", "exit"};
	std::vector<std::string> descs(opts.size());
	ITEM **it = items_from_vec(opts, descs);
	show_menu(it, 5, 12, 5, 5, map_win);

	/* zrušení navigačního okna */
	delwin(bar_win);
}

/**
 * \brief Hlavní funkce
 * \return 0 pokud je vše v pořádku
 */
int main(void)
{
	/* vytvoření klienta */
	boost::asio::io_service io;
	cl = new Client(io);

	/* inicializace ncurses */
	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_RED);
	init_pair(2, COLOR_WHITE, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_BLUE);
	init_pair(4, COLOR_WHITE, COLOR_YELLOW);
	init_pair(GUARD_COLOR, COLOR_BLACK, COLOR_WHITE);

	/* zobrazení hlavního okna */
	try {
		main_win();
	} catch (std::exception& e) {
		msg_box(e.what());
	}

	/* vyčištění okna */
	endwin();

	/* zrušení klienta*/
	delete cl;
}
