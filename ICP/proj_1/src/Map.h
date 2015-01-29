/** \file Map.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#ifndef MAP_H
#define MAP_H

#include <utility>
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

using map_size_t = std::pair<uint16_t, uint16_t>;
using map_position_t = std::pair<uint16_t, uint16_t>;

/** \enum Direction
 * Výčet směrů
 */
enum Direction {
	UP = 0,
	RIGHT,
	DOWN,
	LEFT
};

/** \enum Direction
 * Výčet vlastností, kterých objekt může nebývat
 */
enum Properties {
	CAN_OPEN = (0x1 << 0), // otevřitelný
	CAN_PICK_UP = (0x1 << 1), // zvednutelný
	CAN_SEIZE = (0x1 << 2), // dovoluje obsazení postavou
	CAN_DIE = (0x1 << 3), // dovoluje smrt objektu
	CAN_KILL = (0x1 << 4) // dovoluje objektu zabijet
};

class Map_field;
class Key;

// -----------------------------------------------------------------------------
// Třídy s objekty na mapě

/** \class Map_object
 *  \brief Abstraktní třída s informacemi objektu na hracím poli
 */
class Map_object {
private:
	/* pole na kterém se objekt nachází */
	Map_field *_field;

	/* informace o existenci změny */
	bool _modified;

public:

	/* konstruktor */
	Map_object() : _field(nullptr), _modified(false) {
	};

	virtual ~Map_object() {
	};

	/* změna ukazatelen na pole, na nemž se objekt nachází */
	void change_field(Map_field *field) {
		_field = field;
	};

	/* získání ukazatele pole, ve kterém se objekt nachází */
	Map_field *get_field_ptr() {
		return _field;
	};

	/* získání vlastností objektu dle výčtu Properties */
	bool virtual get_property(Properties p) = 0;

	/* status změny objektu, POZOR: volání resetuje hodnotu na "nezměněno" */
	bool is_modified() {
		bool s = _modified;
		_modified = false;
		return s;
	};

	/* označení objektu jako modifikován */
	void set_modified() {
		_modified = true;
	};

	/* získání stavu objektu pro jeho jednoznačné určení v rámci mapy */
	std::string virtual get_description();
};

/** \class Character
 *  \brief Třída s postavou (hráč/hlídač) na hracím poli
 */
class Character : public Map_object {
public:

	/* výčet stavů objektu */
	enum Status {
		ALIVE = 0, KILLED, DISCONNECTED, GUARD
	};

	/* konstruktor */
	Character(Direction dir, Status stat, uint16_t color_id)
	: _dir(dir), _status(stat), _color_id(color_id) {
		set_modified();
	};

	/* přidání klíče  */
	void add_key(Key *key) {
		_keys.push_back(key);
	};

	/* použití (odebrání) klíče */
	bool use_key();

	/* zabití postavy */
	bool kill(Status new_state);

	/* oživení postavy - nutno prve přiřadit pozici na mapě přes seize */
	void revive() {
		_status = ALIVE;
		set_modified();
	};

	/* test, že postava je žijící hráč */
	bool is_alive() {
		return _status == Character::ALIVE;
	};

	/* test, že postava je zabita, ale hráč stále neopustil hru*/
	bool is_killed() {
		return _status == Character::KILLED;
	};

	/* získání / nastavení směru */
	Direction get_direction() {
		return _dir;
	};

	void set_direction(Direction dir) {
		_dir = dir;
		set_modified();
	};

	/* chování objektu */
	bool get_property(Properties p);

	/* popis objektu */
	std::string get_description();

	/* získání identifikátoru barvy */
	uint16_t get_character_color_id() {
		return _color_id;
	};

private:
	/* směr otočení objektu */
	Direction _dir;

	/* seznam klíčů, které postava vlastní */
	std::vector<Key *> _keys;

	/* stav objektu */
	Status _status;

	/* barevná identifikace postavy */
	uint16_t _color_id;
};

/** \class Wall
 *  \brief Třída představující zeď na hracím poli
 */
class Wall : public Map_object {
public:

	Wall() {
	};

	/* chování objektu */
	bool get_property(Properties p) {
		(void) p;
		return false;
	};
};

/** \class Goal
 *  \brief Třída představující cíl na hracím poli
 */
class Goal : public Map_object {
public:

	Goal() {
	};

	/* chování objektu */
	bool get_property(Properties p) {
		(void) p;
		return false;
	};
};

/** \class Key
 *  \brief Třída představující klíč na hracím poli
 */
class Key : public Map_object {
private:
	/* pozice klíče, na které se nachází (/nacházel i pokud byl zvednut) */
	Map_field* _field;

	/* informace o jestli byl klíč sebrán */
	bool _picked_up;

public:

	Key(Map_field *field) : _field(field), _picked_up(false) {
	};

	/* informace, jestli byl klíč zvednut */
	bool is_picked_up() {
		return _picked_up;
	};

	/* zvednutí klíče */
	bool pick_up();

	/* položení klíče */
	bool drop_key();

	/* chování objektu */
	bool get_property(Properties p) {
		return p & (CAN_PICK_UP);
	};

	/* popis objektu */
	std::string get_description();
};

/** \class Gate
 *  \brief Třída představující bránu na hracím poli
 */
class Gate : public Map_object {
private:
	/* stav brány */
	bool _closed;
public:

	Gate(bool is_closed = true) : _closed(is_closed) {
	};

	/* pokus o odemknutí zámku */
	bool open();

	/* získání stavu brány */
	bool is_closed() {
		return _closed;
	};

	/* chování objektu */
	bool get_property(Properties p) {
		return p & (_closed ? CAN_OPEN : CAN_SEIZE);
	};
};


// -----------------------------------------------------------------------------
// Třída jednoho pole mapy

/** \class Map_field
 *  \brief Třída s informacemi o jednom poli na mapě
 */
class Map_field {
private:
	/* pozice objektu na mapě */
	map_position_t _position;

	/* objekt, který se nachází na poli */
	Map_object *_character_object; // hráč/hlídač může stát na otevřené bráně
	Map_object *_passive_object; // brána/klíč/cíl/zeď
public:

	/* konstruktory/destruktory */
	Map_field(map_position_t pos) :
	_position(pos), _character_object(nullptr), _passive_object(nullptr) {
	};

	/* pozice pole na mapě */
	map_position_t get_position() const {
		return _position;
	};

	/* test možnosti obsadit pole */
	bool can_seize(Map_object *object);

	/* obsazení pole objektem */
	bool seize(Map_object *object);

	/* opuštění pole objektem */
	Map_object * leave_character();
	Map_object * leave_passive();

	/* test možnosti otevřít pole */
	bool can_open();

	/* otevření objektu (jen brány) na poli */
	bool open();

	/* test možnosti provést zvednutí nad danou položkou */
	bool can_pick_up();

	/* zvednutí klíče */
	bool pick_up(Key **key_ptr);

	/* vrácení klíče na pozici */
	bool drop_key(Key *key_ptr);

	/* text možnosti provést zabití objektu (hráče) */
	bool can_kill();

	/* zabití objektu (hráče) */
	bool kill();

	/* text výskytu cíle */
	bool is_goal();


	// TODO: odebrat - jen pro testovací účely !!!!!

	Map_object *get_character() {
		return _character_object;
	};

	Map_object *get_passive() {
		return _passive_object;
	};

};

//------------------------------------------------------------------------------
// Třídy mapy

/** \class Map
 *  \brief Třída pro herní mapu
 */
class Map {
private:
	/* mapa s umístněním objektů */
	std::vector< std::vector<Map_field*> > _map;

	/* rozměry mapy */
	map_size_t size;

	/* seznam startovacích míst hráčů a hlídačů*/
	std::vector<map_position_t> _spos_player_list;
	std::vector<map_position_t> _spos_guard_list;

	/* seznam objektů hráčů, hlídačů, bran, klíčů, zdí a cílů */
	std::vector<Character*> _players;
	std::vector<Character*> _guards;
	std::vector<Gate*> _gates;
	std::vector<Key*> _keys;
	std::vector<Wall*> _walls;
	std::vector<Goal*> _goals;

	/* mapa v podobě pro klienta */
	std::string _client_map;

	/* seznam hráčů, kteří se dostali do cíle */
	std::vector<uint16_t> _winners;

	/* získání ukazatele pole na zadané pozici */
	Map_field *get_field_ptr(const map_position_t &pos);

	/* získání ukazatele vedlejšího */
	Map_field *get_field_adjacent(const Map_field *field, Direction dir);


public:
	/* konstruktor a destruktor mapy */
	Map(map_size_t size, std::string map_des);
	~Map();

	/** \enum Action
	 * Výčet činností, které lze provést nad postavou hráče
	 */
	enum Action {
		A_STEP = 0, A_LEFT, A_RIGHT, A_PICK_UP, A_OPEN
	};

	/* přidání hráče */
	bool add_player(uint16_t &id);

	/*  odebrání hráče ze hry při odpojení */
	bool remove_player(uint16_t id);

	/* informace jestli byl hráč zabit */
	bool is_player_killed(uint16_t id);

	/* provedení operace s hráčem */
	bool player_action(uint16_t id, Action action);

	/* zjištěný stavu mapy v podobě určené klientovi */
	std::string get_objects_config(bool only_changes = true);

	/* provedení posunu hlídačů */
	void guards_move();

	/* podoba mapy pro hráče */
	std::string get_map_description() {
		return _client_map;
	};

	/* test konce hry */
	bool is_game_over();

	/* výpis stavu konce hry */
	std::string game_over_report();

	/* obnovení mrvých hráčů*/
	void revive_dead_players();

	/* testovací vykreslení mapy */
	void print_map();
};


#endif /* MAP_H */