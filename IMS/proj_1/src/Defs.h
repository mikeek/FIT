/* 
 * @file Defs.h
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Soubor s definicemi konstant pro projekt do predmetu IMS 2014
 */

#ifndef DEFS_H
#define	DEFS_H

#include <iostream>
#include <sstream>
#include <simlib.h>
#include <string>
#include <vector>
#include <array>
#include <tuple>

/**
 * Pro učehčení includování veškeré podstatné informace obsahuje Defs.h, který includují ostatní soubory
 */
class Auto;
class Pobocka;
class Zakaznik;
class Zasilka;
class GeneratorZakazniku;
class Statistiky;
class PDoba;

/* Seznam dnu v tydnu */
enum dny {
	PONDELI, UTERY, STREDA, CTVRTEK, PATEK, SOBOTA, NEDELE
};

/* Statisticka obdobi */
enum obdobi {
	TYDEN, VIKEND, VSE
};

/**
 * 
 * 1.strategia(zakladna): POPORADE
 * -auto pride, zoberie baliky podla poradia ako boli prijate a odide
 * 
 * 2.strategia: CETNOST
 * -auto pride, pozre sa na ktore pobocky ide najvac balikov a tam ide
 * 
 * 3.strategia: NEJBLIZSI
 * -auto pride, pozre sa ktore baliky su najblizsie na dorucenie a ide 
 * 
 * 4.strategia: NEJSTARSI
 * - auto pride, pozre ktore baliky su najstarsie na pobocke a ide
 */
enum metodyNakladani {
	POPORADE, CETNOST, NEJBLIZSI, NEJSTARSI
};

extern bool DEBUG_EN;
extern bool STATS_EN;
extern std::ostringstream nullstream;

/* Vyčisti stream přes jeho použitím, aby nezabíral místo v paměti*/
inline std::ostringstream& Nullstream() { nullstream.clear(); nullstream.str(""); return nullstream; }

extern const double T_VYRIZENI;			/**< Doba zpracování požadavku zákazníka (minuty) */
extern const double T_VYLOZENI;			/**< Doba vyložení zásilky (minuty) */
extern const double T_NALOZENI;			/**< Doba naložení zásilky (minuty) */
extern const double TIMEOUT_NALOZENI;	/**< Doba potřebná pro naložení 1 balíku do auta */
extern const double T_POBOCKA_ZPRAC;	/**< Zpracovani baliku pobočkou */
extern const double T_CENTRALA_ZPRAC;	/**< Zpracování balíků centrálou */
extern const int POBOCEK;				/**< Počet poboček */
extern const int AUT;					/**< Počet aut */
extern const int C_POBOCKY;				/**< Kapacita pobočky pro odesílané balíky */
extern const int C_AUTA;				/**< Kapacita auta */
extern const int P_OD;					/**< Začátek pracovní doby (hodiny) */
extern const int P_DO;					/**< Konec pracovní doby   (hodiny) */
extern const int SIM_START;				/**< Začátek simulace */
extern const int SIM_END;				/**< Konec simulace */
extern const int METODA_NAKL;			/**< Metoda nakládání zásilek na auto */
extern const int METODA_PREVOZ;			/**< Metoda výběru dalšího cíle při transportu */
extern const int NRAMP;					/**< Počet nakládacích ramp na každé pobočce */
extern const int VRAMP;					/**< Počet vykládacích ramp na každé pobočce */
extern const std::vector<int> ZAMESTNANCU;					/**< Počet zaměstnanců na každé pobočce */
extern const std::vector<double> T_ZAKAZNIK;				/**< střední hodnota mezi příchody zákazníků na pobočky */
extern const std::vector<std::string> JMENA_METOD;			/**< Názvy metod pro nakládání zboží/plánování trasy */
extern const std::vector<std::string> JMENA_POBOCEK;		/**< Názvy poboček */
extern const std::vector<std::vector<int>> VZDALENOST;		/**< Doba přejezdu mezi pobočkami (minuty) */
extern const std::vector<std::vector<int>> VZDALENOST_KM;	/**< Vzdialenost medzi jednotlivymi pobockami v km */
extern const std::vector<int> CENTRALY;	/**< ID Centralnich pobocek */
extern const std::vector<int> TRIDICU;	/**< Počet třidičů na pobočkach */
extern std::vector<Pobocka *> POBOCKY;	/**< Všechny pobočky */
extern std::vector<Auto *> AUTA;		/**< Všechna auta */
extern Statistiky *hlavneStat;			/**< Statistiky */
extern PDoba *PDOBA;					/**< Pracovni doba, čítač dnů atp. */


#include <iomanip>
#define DEBUG(_id_) (DEBUG_EN ? std::cout : Nullstream()) << std::setw(10) <<  Time << "| " << std::setw(2) << (_id_) << ": "
#define STATS(_id_) (STATS_EN ? std::cout : Nullstream()) << std::setw(2) << (_id_) << ": "

#include "Auto.h"
#include "Pobocka.h"
#include "Zakaznik.h"
#include "Zasilka.h"
#include "Statistiky.h"
#include "PDoba.h"

#endif	/* DEFS_H */
