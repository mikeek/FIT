/* 
 * @file Zasilka.cpp
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida zasilka, projekt do predmetu IMS 2014
 */

#include "Zasilka.h"

/**
 * \brief Konstruktor
 */
Zasilka::Zasilka(int c) : Process(), cil{c}, skutCil{c}
{
	/* Nastav čas vstupu zásilky do systému */
	vstup = Time;
}

/**
 * \brief Hlavní proces zásilky (třídění)
 */
void Zasilka::Behavior()
{
	/* životní cyklus zásilky */
	while (true) {
		/* Zaber třídící stroj */
		Enter(pob->Tridici(), 1);

		/* Vezmi zasilku k pretrideni */
		if (!proslaCentralou) {
			/* Každá zásilka musí projít centrálou */
			if (pob->jeCentrala()) {
				/* Zásilka je na centrální pobočce */
				Wait(T_CENTRALA_ZPRAC);
				cil = skutCil;
				proslaCentralou = true;
			} else {
				/* Přesněruj zásilku na centrální pobočku */
				Wait(T_POBOCKA_ZPRAC);
				cil = pob->NejblizsiCentrala();
			}
		} else {
			/* Cílová pobočka, zpracuj */
			Wait(T_POBOCKA_ZPRAC);
		}

		/* Uvolni třídící stroj */
		Leave(pob->Tridici(), 1);

		if (pob->Id() == cil) {
			/* Zásilka už je na cílové pobočce */
			pob->PridejKvyzvednuti(this);
		} else {
			/* Pošli zásilku na cílovou pobočku */
			pob->PridejKodeslani(this);
		}

		/* Vyčkej na další třídění */
		Passivate();
	}
}

/**
 * \brief Roztřídění zásilky
 */
void Zasilka::RoztridSe(Pobocka* p)
{
	/* Nastav aktuální pobočku a aktivuj proces */
	pob = p;
	Activate();
}
