/* 
 * @file Zakaznik.cpp
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida zakaznik, projekt do predmetu IMS 2014
 */

#include "Zakaznik.h"

#define DEBUG_ZAK DEBUG(pobocka->Id()) << "zakaznik: "

/**
 * \brief Konstruktor
 */
Zakaznik::Zakaznik(int p) : Process(), pobocka{POBOCKY[p]}
{
}

/**
 * \brief Konec pracovni doby
 */
void Zakaznik::KonecPDoby()
{
	pdoba = false;
	if (isInQueue()) {
		/* Odejdi z fronty */
		Out();
		Activate();
	}
}

/**
 * \brief Vyzvednuti zasilky z pobočky
 */
void Zakaznik::Vyzvednuti()
{
	/* Vyřizování požadavku  */
	Wait(Exponential(T_VYRIZENI));

	/* Vyzvednutí */
	Zasilka *z = pobocka->VyzvednoutZasilku();

	if (z) {
		/* Zasilka byla připravena */
		DEBUG_ZAK << "vyzvednuti - mam zasilku\n";
		hlavneStat->PridajStatistiku(z);
		hlavneStat->ZakaznikUspech();
	} else {
		/* Zásilku nelze vyzvednout */
		DEBUG_ZAK << "vyzvednuti - nemam zasilku\n";
		hlavneStat->ZakaznikNeuspech();
	}
}

/**
 * \brief Odeslání zásilky
 */
void Zakaznik::Odeslani()
{
	/* Vyřizování požadavku  */
	Wait(Exponential(T_VYRIZENI));

	/* Kam zásilku odeslat? */
	Zasilka *z = new Zasilka(Random() * POBOCEK);

	/* Odeslání zásilky */
	DEBUG_ZAK << "odesilam na " << JMENA_POBOCEK[z->Cil()] << "\n";
	pobocka->OdeslatZasilku(z);
	hlavneStat->ZakaznikOdeslal();
}

/**
 * \brief Proces zákazníka
 */
void Zakaznik::Behavior()
{
	/* Vstup do pobocky */
	pobocka->Vstup();

	/* Čekej na přepážku */
	DEBUG_ZAK << "cekam na prepazku\n";
	Enter(pobocka->Prepazky());
	if (!pdoba) {
		DEBUG_ZAK << "nedockal jsem se, zaviraji, du dom\n";
		return;
	}

	/*
	 *					Procento zakazniku
	 *				Vyzvednuti		Odeslani
	 * prvni 2 dny		10%				90%
	 * dalsi dny		50%				50%
	 * 
	 * proc? sklady jsou na zacatku prazdne, musi se naplnit
	 */
	double threshold = (PDOBA->AktualniDen() > 2) ? 0.5 : 0.1;

	/* Proveď akci */
	if (Random() <= threshold) {
		Vyzvednuti();
	} else {
		Odeslani();
	}

	/* Uvolni přepážku */
	DEBUG_ZAK << "uvolnuju prepazku\n";
	Leave(pobocka->Prepazky());

	/* Odchod z pobocky */
	pobocka->Odchod();
}
