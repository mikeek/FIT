/* 
 * @file Zakaznik.cpp
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida zakaznik, projekt do predmetu IMS 2014
 */

#ifndef ZAKAZNIK_H
#define	ZAKAZNIK_H

#include "Defs.h"

/**
 * \brief Třída zákazníka
 */
class Zakaznik : public Process {
public:
	Zakaznik(int p);	// Konstruktor, p == ID pobočky
	
	void KonecPDoby();	// Informování o konci pracovní doby
	
private:
	void Behavior();	// Hlavní proces zákazníka
	
	void Vyzvednuti();	// Vyzvednutí zásilky z pobočky
	void Odeslani();	// Odeslání zásilky
	
	bool pdoba{true};	/**< Je pracovní doba? */
	Pobocka *pobocka{};	/**< Pobočka na které se zákazník nachází */
};

/**
 * \brief Generátor příchodu zákazníků na pobočku
 */
class GeneratorZakazniku: public Event {
public:
	GeneratorZakazniku(int p): Event(), pobocka{p} {}
	
private:
	void Behavior() {
		/* Vytvoř zákazníka a aktivuj se za exp(T_ZAKAZNIK) */
		DEBUG(pobocka) << "generuji zakaznika\n";
		(new Zakaznik(pobocka))->Activate();
		Activate(Time + Exponential(T_ZAKAZNIK[pobocka]));
	}
	
	int pobocka; /**< ID pobočky */
};

#endif	/* ZAKAZNIK_H */
