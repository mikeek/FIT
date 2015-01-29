/* 
 * @file PDoba.h
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Hlidac pracovni doby, projekt do predmetu IMS 2014
 */

#include <iostream>
#include "Defs.h"

/**
 * \brief Vytvoření poboček a aut
 */
void vytvor_zdroje()
{
	/* Vytvoření poboček */
	for (int i = 0; i < POBOCEK; ++i) {
		POBOCKY.push_back(new Pobocka(i));
	}

	/* Vytvoření aut */
	for (int i = 0; i < AUT; ++i) {
		AUTA.push_back(new Auto(i % POBOCEK, i));
		AUTA[i]->Activate();
	}

	/* Hlídač pracovní doby */
	PDOBA = new PDoba;
	PDOBA->Activate();

	/* Statistiky */
	hlavneStat = new Statistiky;
}

/**
 * \brief Zrušení poboček a aut
 */
void zrus_zdroje()
{
	/* Zrušení poboček */
	for (auto p : POBOCKY) {
		delete p;
	}

	/* Zrušení statistik */
	delete hlavneStat;
}

/**
 * \brief Hlavní funkce
 */
int main()
{
	/* Povolení výpisů */
	//	DEBUG_EN = true;
	STATS_EN = true;

	/* Inicializace generátoru náhodných čísel */
	RandomSeed(time(NULL));

	/* Přesměrování výsupu SIMLIB statistik do souboru */
	SetOutput("output");

	/* Inicializace simlibu */
	Init(SIM_START, SIM_END);

	/* Vytvoření poboček a aut */
	vytvor_zdroje();

	/* Spuštění simulace */
	Run();

	/* Zobrazení statistik */
	hlavneStat->ZobrazStatistiky();
	hlavneStat->SimlibStatistky();

	/* Uvolnění zdrojů */
	zrus_zdroje();
}
