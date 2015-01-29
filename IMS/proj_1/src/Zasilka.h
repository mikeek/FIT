/* 
 * @file Zasilka.h
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida zasilka, projekt do predmetu IMS 2014
 */

#ifndef ZASILKA_H
#define	ZASILKA_H

#include "Defs.h"

/**
 * \brief Třída zásilky
 * 
 * Při odeslání zákazníkem má zásilka vždy cil == CENTRALA a skutCil == cíl zadaný odesilatelem
 * Po doručení na centrálu je zásilka přetříděna a jako cil je nasteven skutečný cíl
 * 
 */
class Zasilka: public Process {
public:
	Zasilka(int c);						// Konstruktor, c == cílová pobočka
	
	int Cil()	   { return cil; }		// Aktuální cil zásilky
	double Vstup() { return vstup; }	// Čas vstupu zásilky do systému (jejího vytvoření)
	
	void RoztridSe(Pobocka *p);			// Roztrizeni zásilky na pobočce */
	
private:
	void Behavior();					// Proces třídění zásilky */
	
	int	cil;							/**< ID cílové pobočky */
	int skutCil;						/**< skutečný cíl */
	double vstup;						/**< cas vstupu zasielky do systemu */
	bool proslaCentralou{false};		/**< Prošla už zásilka centrální pobočkou? */
	Pobocka *pob{};						/**< Aktuální pobočka, na které se zásilka nachází */
};

#endif	/* ZASILKA_H */
