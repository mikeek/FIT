/* 
 * @file PDoba.cpp
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Hlidac pracovni doby, projekt do predmetu IMS 2014
 */

#include "PDoba.h"

void PDoba::Behavior()
{
	/* Počet minut otvírací doby */
	double do_zavreni = (P_DO - P_OD) * 60.0;

	/* Počet minut mezi zavřením a znovuotevřením pobočky */
	double do_otevreni = (24.0 * 60.0) - do_zavreni;

	/* denní cyklus, na začátku je simulační čas == začátek pracovní doby */
	while (true) {
		/* čekej do konce pracovní doby */
		Wait(do_zavreni);

		/* zastav procesy */
		for (auto p : POBOCKY) {
			p->KonecPDoby();
		}
		for (auto a : AUTA) {
			a->KonecPDoby();
		}

		/* čekej do začátku pracovní doby */
		Wait(do_otevreni);
		den++;

		/* Je víkend? */
		vikend = (den % (NEDELE + 1)) > PATEK;

		/* spusť procesy */
		for (auto p : POBOCKY) {
			p->ZacatekPDoby();
		}
		for (auto a : AUTA) {
			a->ZacatekPDoby();
		}
	}
}
