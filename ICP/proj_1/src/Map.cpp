/** \file Map.cpp
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */

#include <vector>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <deque>
#include <ctime>
#include <cstdlib>
#include <iostream>

// TODO: projít všechna TODO v tomto souboru :)
// TODO: vyřešít situaci, že hráč narazí na hlídače a zabije ho to...
//       zatím funguje pouze pokud narazí hlídač na hráče

// TODO: odebrat
#include <unistd.h>

#include "Map.h"

const uint16_t MIN_MAP_SIZE = 20;
const uint16_t MAX_MAP_SIZE = 50;


// -----------------------------------------------------------------------------

/** \brief Obecný popis objektu na mapě pro klienta
 * \return Řetězec se souřadnicemi, kde se objekt nachází
 */
std::string Map_object::get_description()
{
	map_position_t pos = {0, 0};
	Map_field *field = get_field_ptr();
	if (field != nullptr) {
		pos = field->get_position();
	}

	std::stringstream ss;
	ss << (int) pos.first << " " << (int) pos.second;
	return ss.str();
}

/** \brief Použítí klíče
 * Pokud hráč vlastní klíč vrací odpověď je kladná a dojde ke snížení počtu 
 * vlastněných klíčů.
 * \return True, pokud vlastnil nějaký klíč, jinak false.
 */
bool Character::use_key()
{
	if (_keys.size() > 0) {
		_keys.pop_back();
		return true;
	} else {
		return false;
	}
}

/** \brief Usmrcení postavy
 * Odebere hráče ze hry a vrátí na hrací plochu všechny klíče, které nepoužil.
 * \param[in] new_state Typ usmrcení hráče (zabit / odpojen)
 * \return Pří úspěchu true, jinak false.
 */
bool Character::kill(Status new_state)
{
	if (new_state != KILLED && new_state != DISCONNECTED) {
		return false;
	}

	if (get_property(CAN_DIE)) {
		_status = new_state;
		Map_field *field = get_field_ptr();
		field->leave_character();
		/* vrácení nepoužítých klíčů */
		for (auto it = _keys.begin(); it != _keys.end(); ++it) {
			(*it)->drop_key();
		}
		set_modified();
		return true;
	}

	return false;
}

/** \brief Chování objektu
 * Podle stavu postavy určí, zda může být zabit nebo zabijet.
 * \param[in] p Dotazovaná vlastnost
 * \return Při platnosti vlastnosti true, jinak false.
 */
bool Character::get_property(Properties p)
{
	if (_status == GUARD) {
		return p & CAN_KILL;
	}

	if (_status == ALIVE) {
		return p & CAN_DIE;
	}

	return false;
}

/** \brief Popis hráče na mapě pro klienta
 * \return Pozice postavy a otočení na mapě, u hráče navíc ještě stav (žívý,...)
 */
std::string Character::get_description()
{
	map_position_t pos = {0, 0};
	Map_field *field = get_field_ptr();
	if (field != nullptr) {
		pos = field->get_position();
	}
	Direction dir = get_direction();

	std::stringstream ss;
	ss << (int) get_character_color_id() << " ";
	ss << (int) pos.first << " " << (int) pos.second << " " << (int) dir;
	if (_status != Character::GUARD) {
		ss << " " << (int) _status;
	}
	return ss.str();
}

/** \brief Popis klíče na mapě
 * \return Řetězec s pozicí klíče a stavem klíče (položený/sebraný)
 */
std::string Key::get_description()
{
	int key_status = _picked_up ? 0 : 1;
	map_position_t pos = _field->get_position();

	std::stringstream ss;
	ss << (int) pos.first << " " << (int) pos.second << " " << key_status;
	return ss.str();
}

/** \brief Zvednutí klíče
 * Pokud klíč je nebyl zvednut, provede jeho zvednutí.
 * \return True, při neúspěchu false
 */
bool Key::pick_up()
{
	if (_picked_up) {
		return false;
	} else {
		_picked_up = true;
		_field->leave_passive();
		set_modified();
		return true;
	}
}

/** \brief Položení klíče 
 * Pokud klíč byl zvednut, provede jeho položení.
 * \return True, při neúspěchu false
 */
bool Key::drop_key()
{
	if (_picked_up && _field->drop_key(this)) {
		_picked_up = false;
		set_modified();
		return true;
	} else {
		return false;
	}
}

/** \brief Otevření brány
 * \return Pokud se brána otevřela vrací true, jinak false.
 */
bool Gate::open()
{
	if (_closed) {
		_closed = false;
		set_modified();
		return true;
	}

	return false;
}


// -----------------------------------------------------------------------------
// Implementace třídy jednoho pole mapy

/** \brief Ověření možnosti vložit objekt na pole
 * Ověří, že vložené položky dovolují umístnit objekt na pole. Pokud stojí 
 * na poli postava, lze pod ní vložit pasivní objekt (např. klíč)
 * \return true, při nemožnosti vložit položku false
 */
bool Map_field::can_seize(Map_object *object)
{
	if (object == nullptr) {
		return false;
	}

	if (typeid (Character) == typeid (*object)) {
		if (_character_object != nullptr) {
			return false;
		}

		if (_passive_object != nullptr &&
				!_passive_object->get_property(CAN_SEIZE)) {
			return false;
		}
	} else {
		if (_passive_object != nullptr) {
			return false;
		}
	}

	return true;
}

/** \brief Obsazení pole objektem
 * Ověří se možnost vložení objektu na pole a provede se jeho vložení.
 * \param[in] object Vkládaný objekt
 * \return true, pokud nelze objekt vložit false
 */
bool Map_field::seize(Map_object *object)
{
	if (!can_seize(object)) {
		return false;
	}

	if (typeid (Character) == typeid (*object)) {
		/* aktivní objekt */
		_character_object = object;
	} else {
		/* pasivní objekt */
		_passive_object = object;
	}

	object->change_field(this);

	return true;
}

/** \brief Odebrání postavy (hráč/hlídač) z pole
 * Odebere postavu z pole a vrací ukazatel na tento objekt. Objektu je odebrána
 * informace, na které položce leží.
 * \return Ukazatel na objekt. Pokud žádná postava nebyla na poli, vrací nullptr.
 */
Map_object *Map_field::leave_character()
{
	Map_object *tmp = _character_object;
	_character_object = nullptr;
	if (tmp != nullptr) {
		tmp->change_field(nullptr);
	}

	return tmp;
}

/** \brief Odebraní neaktivního objektu (zeď/brána/klíč/cíl) z pole
 * Odebere objekt z pole a vrací ukazatel na tento objekt. Objektu je odebrána
 * informace, na které položce leží.
 * \return Ukazatel na objekt. Pokud žádný objekt neležel na poli, vrací nullptr.
 */
Map_object *Map_field::leave_passive()
{
	Map_object *tmp = _passive_object;
	_passive_object = nullptr;
	if (tmp != nullptr) {
		tmp->change_field(nullptr);
	}

	return tmp;
}

/** \brief Ověření otevíratelnosti objektu na poli
 * Zjistí, zda pasivní položka má vlastnost umožňující otevření
 * \return true, jinak false
 */
bool Map_field::can_open()
{
	if (_passive_object != nullptr &&
			_passive_object->get_property(CAN_OPEN)) {
		return true;
	}

	return false;
}

/** \brief Otevření položky na poli
 * Pokud daný objekt lze otevřít a jedná se o bránu, provede její otevření.
 * \return true - brána se otevřela, false - objekt nelze otevřít
 */
bool Map_field::open()
{
	if (!can_open()) {
		return false;
	}

	Gate *gate = dynamic_cast<Gate*> (_passive_object);
	if (gate == nullptr) {
		return false;
	}

	return gate->open();
}

/** \brief Ověření zvednutelnosti objektu na poli
 * Zjistí, zda pasivní položka má vlastno umožňující zvednutí.
 * \return true, jinak false
 */
bool Map_field::can_pick_up()
{
	if (_passive_object != nullptr &&
			_passive_object->get_property(CAN_PICK_UP)) {
		return true;
	}

	return false;
}

/** \brief Zvednutí položky (klíče) na poli
 * Pokud daný objekt lze zvednout a jedná se o klíč, provede zvednutí
 * \param[out] key_ptr Ukazatel na zvednutý klíč
 * \return Pokud se podařilo zvedou klíč vrací true a odkaz na klíč, jinak vrací false a nullptr
 */
bool Map_field::pick_up(Key **key_ptr)
{
	if (!can_pick_up()) {
		return false;
	}

	Key *key = dynamic_cast<Key*> (_passive_object);
	if (key == nullptr) {
		return false;
	}

	if (key->pick_up()) {
		*key_ptr = key;
		return true;
	} else {
		*key_ptr = nullptr;
		return false;
	}
}

/** \brief Položení klíče
 * Odkazovaný klíč je položen na dané pole.
 * \param[in] key_ptr Pokládaný klíč
 * \return Při úspěchu true, jinak false.
 */
bool Map_field::drop_key(Key *key_ptr)
{
	if (!can_seize(key_ptr)) {
		return false;
	}

	return seize(key_ptr);
}

/** \brief Ověření smrtelnosti objektu na poli
 * Zjistí, zda případná postava má vlastnost umožňující usmrcení
 * \return true, jinak false
 */
bool Map_field::can_kill()
{
	if (_character_object != nullptr &&
			_character_object->get_property(CAN_DIE)) {
		return true;
	}

	return false;
}

/** \brief Usmrcení objektu
 * Ověří, že objekt na poli je hráč a lze zabít.
 * \return Pokud je na poli hráč a je usmrcen vrací true, jinak false.
 */
bool Map_field::kill()
{
	if (!can_kill()) {
		return false;
	}

	Character *player = dynamic_cast<Character*> (_character_object);
	if (player == nullptr) {
		return false;
	}

	return player->kill(Character::KILLED);
}

/** \brief Ověření, že na poli je cíl
 * \return Pokud cíl je přítomen vrací true, jinak false.
 */
bool Map_field::is_goal()
{
	if (_passive_object != nullptr &&
			dynamic_cast<Goal*> (_passive_object) != nullptr) {
		return true;
	}

	return false;
}

// -----------------------------------------------------------------------------
// Implementace třídy mapy

/** \brief Konstruktor mapy
 * Na základě rozměru a řetězcového popisu mapy provede její inicializaci
 * \param[in] size Rozměry mapy
 * \param[in] map_des Řetězcový popis mapy, kde položky jsou odděleny mezerou
 */
Map::Map(map_size_t size, std::string map_des)
{
	if (size.first > MAX_MAP_SIZE || size.second > MAX_MAP_SIZE ||
			size.first < MIN_MAP_SIZE || size.second < MIN_MAP_SIZE) {
		throw std::invalid_argument("Chybna velikost mapy");
	}

	this->size = size;

	/* tvorba herniho pole */
	_map.resize(size.first);
	for (uint16_t row = 0; row < size.first; ++row) {
		_map[row].resize(size.second);

		for (uint16_t col = 0; col < size.second; ++col) {
			_map[row][col] = new Map_field({row, col});
		}
	}

	/* aplikace mapy */
	std::istringstream ss(map_des);
	std::ostringstream client_map;
	client_map << (int) size.first << " " << (int) size.second << " ";
	char item;

	uint16_t count = 0;
	while (ss.get(item) && count < size.first * size.second) {
		Map_object *new_object = nullptr;
		map_position_t pos = {count / size.second, count % size.second};
		++count;

		/* vytvoření příslušného typu objektu mapy */
		switch (item) {
		case '\n': // slouží pouze pro nepovinné vizuální formátování mapy
			--count;
			continue;
		case ' ': // prázdné pole
			client_map << ".";
			continue;
		case 'O': // otevřená brána
			new_object = new Gate(false);
			_gates.push_back((Gate*) new_object);
			client_map << "O";
			break;
		case 'G': // zavřená brána
			new_object = new Gate();
			_gates.push_back((Gate*) new_object);
			client_map << "C";
			break;
		case 'C': // cíl hry
			new_object = new Goal();
			_goals.push_back((Goal*) new_object);
			client_map << "G";
			break;
		case '+': // klíč
			new_object = new Key(get_field_ptr(pos));
			_keys.push_back((Key*) new_object);
			client_map << "+";
			break;
		case '#': // zeď
			new_object = new Wall();
			_walls.push_back((Wall*) new_object);
			client_map << "#";
			break;
		case 'S': // start hrače
			_spos_player_list.push_back(pos);
			client_map << ".";
			continue;
		case 'H': // počáteční pozice hlídače
			_spos_guard_list.push_back(pos);
			client_map << ".";
			continue;
		default:
			throw std::invalid_argument("Neplatný formát vstupní mapy!");
			break;
		}

		/* obsazení pole objektem */
		Map_field *field = get_field_ptr(pos);
		field->seize(new_object);
	}

	if (count != size.first * size.second) {
		throw std::invalid_argument("Mapa není úplná");
	}

	_client_map = client_map.str();

	/* přidání všech hlídačů */
	while (_spos_guard_list.size() > _guards.size()) {
		/* vytvoření objektu a získání pozice, kde by se měl umístnit */
		uint16_t guard_id = _guards.size();
		map_position_t start_pos = _spos_guard_list[guard_id];
		Character *new_guard = new Character(RIGHT, Character::GUARD,
				guard_id);

		Map_field *field = get_field_ptr(start_pos);
		if (!field->seize(new_guard)) {
			throw std::runtime_error("Nepodařilo se umístnit hlídače!");
		}
		_guards.push_back(new_guard);
	}

}

/** \brief Destruktor mapy
 * Provede zrušení všech položek, které jsou součástí mapy
 */
Map::~Map()
{
	/* uvolnění všech polí na mapě */
	for (auto it_row = _map.begin(); it_row != _map.end(); ++it_row) {
		for (auto it_col = it_row->begin(); it_col != it_row->end(); ++it_col) {
			delete (*it_col);
		}
	}

	/* uvolnění všech hráčů */
	for (auto it = _players.begin(); it != _players.end(); ++it) {
		delete *it;
	}

	/* uvolnění všech hlídačů */
	for (auto it = _guards.begin(); it != _guards.end(); ++it) {
		delete *it;
	}

	/* uvolnění všech bran */
	for (auto it = _gates.begin(); it != _gates.end(); ++it) {
		delete *it;
	}

	/* uvolnění všech klíčů */
	for (auto it = _keys.begin(); it != _keys.end(); ++it) {
		delete *it;
	}

	/* uvolnění všech zdí */
	for (auto it = _walls.begin(); it != _walls.end(); ++it) {
		delete *it;
	}

	/* uvolnění všech cílů */
	for (auto it = _goals.begin(); it != _goals.end(); ++it) {
		delete *it;
	}
}

/** \brief Přístup k poli na adrese
 * \param[in] pos Pozice na mapě
 * \return Ukazatel na pole na mapě. Při přístupu mimo mapu, vrací nullptr.
 */
Map_field *Map::get_field_ptr(const map_position_t &pos)
{
	if (pos.first >= size.first || pos.second >= size.second) {
		return nullptr;
	}

	return _map[pos.first][pos.second];
}

/** \brief Přístup k sousednímu poli
 * \param[in] field Ukazatel na výchozí pole
 * \param[in] dir Směr sousedního pole
 * \return Ukazatel na sousední pole. Pokud sousední pole neexistuje, vrací nullptr.
 */
Map_field *Map::get_field_adjacent(const Map_field *field, Direction dir)
{
	if (field == nullptr) {
		return nullptr;
	}

	map_position_t pos = field->get_position();
	map_position_t new_pos;

	switch (dir) {
	case UP:
		new_pos = {pos.first - 1, pos.second};
		break;
	case RIGHT:
		new_pos = {pos.first, pos.second + 1};
		break;
	case DOWN:
		new_pos = {pos.first + 1, pos.second};
		break;
	case LEFT:
		new_pos = {pos.first, pos.second - 1};
		break;
	default:
		return nullptr;
	}

	return get_field_ptr(new_pos);
}

/** \brief Přidat hráče do hry
 * Pokud existuje na mapě volná pozice určená pro hráče, dojde k jeho umístění
 * a parametru id je přiděleno jednoznačné označení hráče
 * \param[out] id Přidělené ID hráče
 * \return V případě úspěchu vrací true, jinak false.
 */
bool Map::add_player(uint16_t &id)
{
	/* test existence místa, kam lze hráče umístnit */
	if (_spos_player_list.size() <= _players.size()) {
		return false;
	}

	/* vytvoření objektu a získání pozice, kde by se měl úmístnit */
	uint16_t player_id = _players.size();
	Character *new_character = new Character(RIGHT, Character::ALIVE, player_id);
	map_position_t start_pos = _spos_player_list[player_id];

	/* uzmístění objektu */
	std::deque<map_position_t> pos_candidate;
	std::deque<map_position_t> pos_visited;
	pos_candidate.push_back(start_pos);

	while (!pos_candidate.empty()) {
		map_position_t tmp = pos_candidate.front();
		pos_candidate.pop_front();

		Map_field *field = get_field_ptr(tmp);
		if (field->seize(new_character)) {
			break;
		}

		/* snaha o budoucí umístnění objektu do jednoho z okolních polí */
		pos_visited.push_back(tmp);
		for (int dir = UP; dir != LEFT + 1; ++dir) {
			Map_field *next = get_field_adjacent(field, static_cast<Direction> (dir));
			if (next == nullptr) {
				continue;
			}

			map_position_t next_pos = next->get_position();
			if (std::find(pos_visited.begin(), pos_visited.end(),
					next_pos) == pos_visited.end()) {
				pos_candidate.push_back(next_pos);
			}
		}
	}

	if (new_character->get_field_ptr() == nullptr) {
		throw std::runtime_error("Nezdařilo se umístit hráče na hrací plochu");
	}

	_players.push_back(new_character);
	id = player_id;
	return true;
}

/** \brief Odebrání hráče
 * Provede odebrání hráče tak, že jej označí jako odpojeného
 * \param[in] id ID odebíraného hráče
 * \return V případě úspěchu true, jinak false.
 */
bool Map::remove_player(uint16_t id)
{
	if (id >= _players.size()) {
		return false;
	}

	Character *character = _players[id];
	if (!character->is_alive()) {
		return false;
	}

	return character->kill(Character::DISCONNECTED);
}

/** \brief Informace jestli byl hráč zabit
 * \param[in] id ID hráče
 * \return V případě, že je mrtvý true, jinak false.
 */
bool Map::is_player_killed(uint16_t id)
{
	if (id >= _players.size()) {
		return false;
	}

	Character *character = _players[id];
	if (character->is_killed()) {
		return true;
	}

	return false;
}

/** \brief Provedení operace s hráčem
 * Na základě ID hráče a typu operace se pokusí provést požadovaný krok
 * \param[in,out] id ID hráče
 * \param[in,out] action Požadovaná akce
 * \return V případě, že se operace zdařila vrací true, jinak false.
 */
bool Map::player_action(uint16_t id, Action action)
{
	if (id >= _players.size()) {
		return false;
	}

	bool action_status = false;
	Character *character = _players[id];
	if (!character->is_alive()) {
		return false;
	}

	Map_field *c_pos = character->get_field_ptr();
	Map_field *n_pos = nullptr;

	switch (action) {
	case A_STEP:
		/* provedení jednoho kroku přímým směrem */
		n_pos = get_field_adjacent(c_pos, character->get_direction());
		if (n_pos != nullptr && n_pos->can_seize(character)) {
			c_pos->leave_character();
			n_pos->seize(character);
			action_status = true;
			character->set_modified();
		}

		/* pokud nešlo krok provést, ověří se zda chtěl vstoupit na cíl */
		if (n_pos != nullptr && n_pos->is_goal()) {
			_winners.push_back(id);
			action_status = true;
		}

		break;
	case A_LEFT:
		/* otočení doleva */
		character->set_direction(
				static_cast<Direction> ((character->get_direction() + 3) % 4));
		action_status = true;
		character->set_modified();
		break;
	case A_RIGHT:
		/* otočení doprava */
		character->set_direction(
				static_cast<Direction> ((character->get_direction() + 1) % 4));
		action_status = true;
		character->set_modified();
		break;
	case A_PICK_UP:
		/* zvednutí klíče */
		n_pos = get_field_adjacent(c_pos, character->get_direction());
		if (n_pos != nullptr && n_pos->can_pick_up()) {
			Key *new_key;
			n_pos->pick_up(&new_key);
			character->add_key(new_key);
			action_status = true;
		}
		break;
	case A_OPEN:
		/* otevření brány */
		n_pos = get_field_adjacent(c_pos, character->get_direction());
		if (n_pos != nullptr && n_pos->can_open() && character->use_key()) {
			n_pos->open();
			action_status = true;
		}
		break;
	default:
		return false;
	}

	return action_status;
}

/** \brief Výpis konfigurace objektů na mapě pro klienta
 * Pro sledované položky (hráči, hlídači, klíče a brány) vrací jejich celkový
 * popis. V případě volání s parametrem only_changes == true zobrazí jen změny,
 * které byly zaznamenány a provede resetování záznamu změn. V případě volání 
 * s parametrem only_changes == false, zobrazí veškeré změny oproti výchozí
 * podobě mapy.
 * \param[in] only_changes Zobrazit jen zaznamenané změny
 * \retrun Řetězec se změnami v podobě vhodné pro odeslání klientovi
 */

// pozor: při konfiguraci only_changes == false NEDOJDE k resetovaní detekce změn

std::string Map::get_objects_config(bool only_changes)
{
	std::ostringstream ss;

	/* změny u hráčů */
	auto player_size = _players.size();
	for (unsigned i = 0; i < player_size; ++i) {
		if ((only_changes && _players[i]->is_modified()) || !only_changes) {
			ss << "P " << _players[i]->get_description() << " ";
		}
	}

	/* změny u hlídačů */
	auto guard_size = _guards.size();
	for (unsigned i = 0; i < guard_size; ++i) {
		if ((only_changes && _guards[i]->is_modified()) || !only_changes) {
			ss << "H " << _guards[i]->get_description() << " ";
		}
	}

	/* změny u klíčů */
	for (auto it = _keys.begin(); it != _keys.end(); ++it) {
		if ((only_changes && (*it)->is_modified()) ||
				(!only_changes && (*it)->is_picked_up())) {
			ss << "K " << (*it)->get_description() << " ";
		}
	}

	/* změny u bran */
	for (auto it = _gates.begin(); it != _gates.end(); ++it) {
		if ((only_changes && !(*it)->is_closed() && (*it)->is_modified()) ||
				(!only_changes && !(*it)->is_closed())) {
			ss << "G " << (*it)->get_description() << " ";
		}
	}

	return ss.str();
}

/** \brief Náhodný posun hlídačů po mapě
 * Pro každého hlídači vybere jeden vhodný následující krok podle aktuální 
 * pozice a možných směrů v okolí. Krok provede přičemž může dojít k zabití 
 * hráče.
 */
void Map::guards_move()
{
	srand(time(NULL));

	/* každému hlídači se určuje nová činnost */
	for (auto it = _guards.begin(); it != _guards.end(); ++it) {
		/* určení hlídače a jeho pozice */
		Character *guard = *it;
		Map_field *field = guard->get_field_ptr();
		if (field == nullptr) {
			continue;
		}

		/* získání směrů kam se může hlídač posunout */
		std::vector<Direction> dir_list;
		for (int dir = UP; dir != LEFT + 1; ++dir) {
			Map_field *next = get_field_adjacent(field, static_cast<Direction> (dir));
			if (next == nullptr) {
				continue;
			}

			if (next->can_kill()) {
				/* je-li v přímém okolí nějaký hráč, zaměr se jeho směrem */
				dir_list.clear();
				dir_list.push_back(static_cast<Direction> (dir));
				break;
			}

			if (!next->can_seize(guard)) {
				continue;
			}

			dir_list.push_back(static_cast<Direction> (dir));
		}

		if (dir_list.empty()) {
			/* hlídač se nemůže nikam posunout */
			continue;
		}

		/* výběr směru chůze */
		Direction c_dir = guard->get_direction();
		Direction n_dir = c_dir;
		if (dir_list.size() > 1) {
			/* lze jít více směry - preferované jsou ty, které se nevrací */
			int index_straight = -1;
			std::vector<Direction> next_list;

			for (auto dir_it = dir_list.begin(); dir_it != dir_list.end(); ++dir_it) {
				int tmp = (int) (*dir_it) - (int) c_dir;
				if (tmp < 0) {
					/* hodnota je záporná -> korekce */
					tmp += 4;
				}

				/* nový směr */
				int val = ((int) (c_dir) + tmp) % 4;

				if (val - (int) (c_dir) == 2 || (int) (c_dir) - val == 2) {
					/* označuje směr, který se vrací o 180 stupňů - je jen jeden */
					continue;
				}

				/* přidání do seznamu směrů, které se nevrací */
				if (val - (int) (c_dir) == 0) {
					index_straight = next_list.size();
				}

				next_list.push_back(static_cast<Direction> (val));
			}

			if (index_straight >= 0 && rand() % 4 > 0) {
				/* je znám přímý směr, a proto má mít větší náhodu.*/
				n_dir = next_list[index_straight];
			} else {
				/* není znám přímý směr */
				n_dir = next_list[rand() % next_list.size()];
			}


		} else {
			/* lze jít pouze jedním směrem */
			int tmp = (int) (dir_list.front()) - (int) c_dir;
			if (tmp < 0) {
				/* hodnota je záporná -> korekce */
				tmp += 4;
			}

			/* nový směr */
			int val = ((int) c_dir + tmp) % 4;
			if (val - (int) (c_dir) == 2 || (int) (c_dir) - val == 2) {
				/* pokud se musí otočit o 180 stupňů, rotuje nejprve doprava */
				val = (val + 1) % 4;
			}

			n_dir = static_cast<Direction> (val);
		}

		/* určení typu akce - krok nebo rotace */
		if (c_dir == n_dir) {
			/* směr obsazení */
			Map_field *next = get_field_adjacent(field, static_cast<Direction> (c_dir));

			// TODO: po řádném otestování tuto podmínku odebrat
			if (next == nullptr) {
				throw std::runtime_error("Chyba výpočtu kroku hlídače");
			}

			if (!next->can_seize(guard)) {
				/* na pozici je hráč - zabij ho */
				if (!next->kill()) {
					throw std::runtime_error("Ve směru hlídače je "
							"neodebratelný objekt!");
				}
			}

			field->leave_character();
			next->seize(guard);
		} else {
			guard->set_direction(n_dir);
		}

		guard->set_modified();
	}
}

/** \brief Test konce hry
 * Hra je označena jako ukončena, pokud žádný hráč nežije nebo některý hráč 
 * došel do cíle.
 * \return Pokud hra skončila vrací true, jinak false.
 */
bool Map::is_game_over()
{
	if (_winners.size() > 0) {
		/* už existuje nějaký vítěz */
		return true;
	}

	for (auto it = _players.begin(); it != _players.end(); ++it) {
		if ((*it)->is_alive()) {
			/* existuje ještě někdo naživu */
			return false;
		}
	}

	return true;
}

/** \brief Zpráva o stavu ukončené hry
 * \return Řetězec obsahující počet hráčů, kteří vyhráli následovaný jejich ID
 */
std::string Map::game_over_report()
{
	std::ostringstream ss;
	ss << _winners.size();
	for (auto it = _winners.begin(); it != _winners.end(); ++it) {
		ss << " " << (int) (*it);
	}
	return ss.str();
}

/**
 * \brief Obnovení mrtvým neodpojených hráčů
 */
void Map::revive_dead_players()
{
	for (auto it = _players.begin(); it != _players.end(); ++it) {
		if ((*it)->is_killed()) {
			for (auto pos = _spos_player_list.begin(); pos !=
					_spos_player_list.end(); ++pos) {
				Map_field *field = get_field_ptr(*pos);
				if (field->seize(*it)) {
					(*it)->revive();
					break;
				}
			}
		}
	}
}

/** \brief Testovací vykeslení mapy
 * Na standardní výstup vykreslí aktuální pozice všech objektů na mapě.
 * U postav zobrazí i jejich směr.
 */
void Map::print_map()
{
	for (std::vector< std::vector<Map_field*> >::iterator it_row = _map.begin();
			it_row != _map.end(); ++it_row) {
		for (std::vector<Map_field*>::iterator it_col = it_row->begin();
				it_col != it_row->end(); ++it_col) {
			Map_field *tmp = *it_col;

			Map_object *char_obj = tmp->get_character();
			if (char_obj != nullptr) {
				if (typeid (*char_obj) == typeid (Character)) {
					// postava
					if (((Character*) char_obj)->get_property(CAN_DIE)) {
						std::cout << "P";
					} else {
						std::cout << "H";
					}

					switch (((Character*) char_obj)->get_direction()) {
					case UP: std::cout << "^";
						break;
					case RIGHT: std::cout << ">";
						break;
					case DOWN: std::cout << "v";
						break;
					case LEFT: std::cout << "<";
						break;
					}
				} else {
					std::cout << "! ";
				}
			}

			Map_object *pass_obj = tmp->get_passive();
			if (pass_obj != nullptr) {
				if (typeid (*pass_obj) == typeid (Gate)) {
					//brána
					if (pass_obj->get_property(CAN_SEIZE)) {
						std::cout << "O ";
					} else {
						std::cout << "C ";
					}
				} else if (typeid (*pass_obj) == typeid (Goal)) {
					// cíl
					std::cout << "G ";
				} else if (typeid (*pass_obj) == typeid (Wall)) {
					// zeď
					std::cout << "# ";
				} else if (typeid (*pass_obj) == typeid (Key)) {
					// klíč
					std::cout << "+ ";
				} else {
					// nerozpoznáno
					std::cout << "! ";
				}
			}

			if (char_obj == nullptr && pass_obj == nullptr) {
				// volné pole
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
}

