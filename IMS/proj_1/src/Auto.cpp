/* 
 * @file Auto.cpp
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida auto, projekt do predmetu IMS 2014
 */

#include "Auto.h"
#include <string.h>

/* Debug makro pro auto */
#define DEBUG_AUTO DEBUG("") << "auto " << (id) << " (" << JMENA_POBOCEK[pobocka] << "): "

/**
 * \brief Konstruktor
 */
Auto::Auto(int p, int i) : Process(), id{i}, pobocka{p}
{
	if (i < 0) {
		i = p;
	}

	/* Nastav jméno pro výpisy */
	std::stringstream name;
	name << "Auto " << id;
	SetName(strdup(name.str().c_str()));

	DEBUG_AUTO << "vytvoreno\n";
}

/**
 * \brief Pripocitanie casu na vykladacej rampe k celkovemu casu na rampe
 */
void Auto::NaVRampeUp(double casNaRampe)
{
	if (PDOBA->JeVikend()) { // Víkendový čas
		VRampaCasVikend += casNaRampe;
	}

	VRampaCas += casNaRampe; // Celkový čas
}

/**
 * \brief Vrati cas straveny na vykladacej rampe v zavislosti na dni v tyzdni
 */
double Auto::NaVRampe(int kedy)
{
	switch (kedy) {
	case TYDEN: return VRampaCas - VRampaCasVikend;
	case VIKEND: return VRampaCasVikend;
	default: return VRampaCas; /* SUHRN */
	}
}

/**
 * \brief Pripocitanie casu na nakladacej rampe k celkovemu casu na rampe
 */
void Auto::NaNRampeUp(double casNaRampe)
{
	if (PDOBA->JeVikend()) { // Víkendový čas
		NRampaCasVikend += casNaRampe;
	}

	NRampaCas += casNaRampe; // Celkový čas
}

/**
 * \brief Vrati cas straveny na nakladacej rampe v zavislosti na dni v tyzdni
 */
double Auto::NaNRampe(int kedy)
{
	switch (kedy) {
	case TYDEN: return NRampaCas - NRampaCasVikend;
	case VIKEND: return NRampaCasVikend;
	default: return NRampaCas; /* SUHRN */
	}
}

/**
 * \brief Pripocitanie casu na ceste k celkovemu casu
 */
void Auto::NaCesteUp(double casCesty)
{
	if (PDOBA->JeVikend()) { // Víkendový čas
		dobaNaCesteVikend += casCesty;
	}

	dobaNaCeste += casCesty; // Celkový čas
}

/**
 * \brief Vrati cas straveny na ceste v zavislosti na dni v tyzdni
 */
double Auto::NaCeste(int kedy)
{
	switch (kedy) {
	case TYDEN: return dobaNaCeste - dobaNaCesteVikend;
	case VIKEND: return dobaNaCesteVikend;
	default: return dobaNaCeste; /* SUHRN */
	}
}

/**
 * \brief Pripocitanie najazdenych km k celkovemu poctu
 */
void Auto::VzdialenostUp(int km)
{
	if (PDOBA->JeVikend()) { // Víkendový čas
		celkovaVzdialenostVikend += km;
	}

	celkovaVzdialenost += km; // Celkový čas
}

/**
 * \brief Vrati celkovu vzdialenost v zavislosti na dni v tyzdni
 */
int Auto::Vzdialenost(int kedy)
{
	switch (kedy) {
	case TYDEN: return celkovaVzdialenost - celkovaVzdialenostVikend;
	case VIKEND: return celkovaVzdialenostVikend;
	default: return celkovaVzdialenost; /* SUHRN */
	}
}

/**
 * \brief Pripocitanie zasielky k celkovemu poctu prepravenych
 */
void Auto::PrepraveneUp()
{
	if (PDOBA->JeVikend()) { // Víkendový čas
		celkovyPocetVikend++;
	}

	celkovyPocet++; // Celkový čas
}

/**
 * \brief Vrati celkovy pocet prepravenych zasielok v zavislosti na dni v tyzdni
 */
int Auto::Prepravene(int kedy)
{
	switch (kedy) {
	case TYDEN: return celkovyPocet - celkovyPocetVikend;
	case VIKEND: return celkovyPocetVikend;
	default: return celkovyPocet; /* SUHRN */
	}
}

/**
 * \brief Reakce na zahájení pracovní doby
 */
void Auto::ZacatekPDoby()
{
	/* aktivuj proces */
	pdoba = true;
	Activate();
}

/**
 * \brief Reakce na ukončení pracovní doby
 */
void Auto::KonecPDoby()
{
	pdoba = false;

	/* Vyhození z fronty */
	if (isInQueue()) {
		Out();
		Activate();
	}
}

/**
 * \brief Hlavní proces
 */
void Auto::Behavior()
{
	while (true) {
		/* Na začátku je auto na pobočce */
		NalozZasilky();
		if (!pdoba) Passivate();

		/* Jeď na další pobočku */
		Odjezd();
		if (!pdoba) Passivate();

		/* Vylož zásilky */
		VylozZasilky();
		if (!pdoba) Passivate();
	}
}

/**
 * \brief Vypršení timeoutu při nakládání
 */
void Auto::timeout()
{
	/* Vytáhni auto z čekání na zásilku k naložení */
	if (isInQueue()) {
		Out();
		timeouted = true;

		/* Aktivuj proces */
		Activate();
	}
}

/**
 * \brief Naložení zásilek z pobočky
 */
void Auto::NalozZasilky()
{
	/* Zaber si nakládací rampu */
zabern:
	Enter(POBOCKY[pobocka]->NRampa(), 1);
	if (!pdoba) {
		/* Během čekání na rampu skončila pracovní doba */
		Passivate();
		goto zabern;
	}

	DEBUG_AUTO << "mam NRampu\n";

	/* Informuj pobočku o začátku nakládání */
	POBOCKY[pobocka]->ZacatekNakladani();

	/* Nalož zásilky */
	while (!Plne()) {
		/* Aktivuj timeout - pokud se po danou dobu nenaloží žádná zásilka, auto odjíždí */
		timeouted = false;
		Timeout *tm = new Timeout(this);
		tm->Activate(Time + TIMEOUT_NALOZENI);

		/* Vyzvedni zásilku k naložení */
		Enter(POBOCKY[pobocka]->KNalozeni());
		if (!pdoba) {
			/* Během čekání na zásilku skončila pracovní doba */
			tm->Cancel();
			Passivate();
			continue;
		}

		/* Uplynul timeout */
		if (Timeouted()) {
			DEBUG_AUTO << "uplynul timeout\n";
			/* Prazdne auto čeká na nějakou zásilku (kam by jelo?) */
			if (Prazdne()) {
				continue;
			}

			/* Auto neni prazdne, může odjet */
			break;
		}

		/* Zruš timeout */
		tm->Cancel();

		/* Statistika prepravenych zasielok */
		PrepraveneUp();

		/* Nalož zásilku */
		Nalozenie(POBOCKY[pobocka]->VydejKOdeslani());
	}

	/* Informuj pobočku o ukončení nakládání */
	POBOCKY[pobocka]->KonecNakladani();

	/* Uvolni nakládací rampu */
	DEBUG_AUTO << "uvolnuju NRampu\n";
	Leave(POBOCKY[pobocka]->NRampa(), 1);

}

/**
 * \brief Vyložení zásilek na pobočce
 */
void Auto::VylozZasilky()
{
	/* Zaber si vykládací rampu */
zaberv:
	Enter(POBOCKY[pobocka]->VRampa(), 1);
	if (!pdoba) {
		/* Během čekání na rampu skončila pracovní doba */
		Passivate();
		goto zaberv;
	}

	DEBUG_AUTO << "mam VRampu\n";

	/* Vylož všechny zásilky pro tuto pobočku */
	Zasilka *z;
	while ((z = Vylozenie())) {
		POBOCKY[pobocka]->VylozZAuta(z);
	}

	/* Uvolni rampu */
	DEBUG_AUTO << "uvolnuju VRampu\n";
	Leave(POBOCKY[pobocka]->VRampa(), 1);
}

/**
 * \brief Urči nejbližší cílovou pobočku ze všech zásilek
 */
int Auto::NejblizsiCil()
{
	/* Init - nejblizsi cil ma první zásilka */
	int nejblizsi = 0;

	for (int i = 0; i < (int) zasilky.size(); ++i) {
		if (VZDALENOST[pobocka][zasilky[i]->Cil()] < VZDALENOST[pobocka][nejblizsi]) {
			nejblizsi = i;
		}
	}

	return zasilky[nejblizsi]->Cil();
}

/**
 * \brief Urči nejstarší zásilku v autě, vrať její cíl
 */
int Auto::NejstarsiCil()
{
	/* Init - nejstarší je první zásilka */
	int nejstarsi = 0;

	for (int i = 0; i < (int) zasilky.size(); ++i) {
		if (zasilky[i]->Vstup() > zasilky[nejstarsi]->Vstup()) {
			nejstarsi = i;
		}
	}

	return zasilky[nejstarsi]->Cil();
}

/**
 * \brief Vynuluj četnosti jednotlivých poboček
 */
void Auto::VynulujCetnosti()
{
	/* Inicializace vektoru */
	while ((int) cetnostiCilu.size() < POBOCEK) {
		cetnostiCilu.push_back(0);
	}

	/* Vynulování */
	for (int i = 0; i < (int) cetnostiCilu.size(); ++i) {
		cetnostiCilu[i] = 0;
	}
}

/**
 * \brief Urči pobočku, kam směřuje nejvíce zásilek
 */
int Auto::NejcetnejsiCil()
{
	VynulujCetnosti();

	/* Spočítej četnosti */
	for (auto z : zasilky) {
		cetnostiCilu[z->Cil()]++;
	}

	/* Zjisti nejcetnejsi, init - nejčetnější je první cíl */
	int nejcetnejsi = 0;
	for (int i = 0; i < (int) cetnostiCilu.size(); ++i) {
		if (cetnostiCilu[i] > cetnostiCilu[nejcetnejsi]) {
			nejcetnejsi = i;
		}
	}

	return nejcetnejsi;
}

/**
 * \brief Urči další cíl cesty
 */
int Auto::DalsiCil()
{
	switch (METODA_PREVOZ) {
	case metodyNakladani::POPORADE: return zasilky[0]->Cil();
	case metodyNakladani::NEJBLIZSI: return NejblizsiCil();
	case metodyNakladani::NEJSTARSI: return NejstarsiCil();
	case metodyNakladani::CETNOST: return NejcetnejsiCil();
	default: return zasilky[0]->Cil();
	}
}

/**
 * \brief Odjezd z pobočky
 */
void Auto::Odjezd()
{
	/* Odjed z pobocky */
	POBOCKY[pobocka]->Odjezd();

	/* Zjisti dalsi pobocku */
	int dalsi = DalsiCil();

	/* Aktualizuj statistiky */
	VzdialenostUp(VZDALENOST_KM[pobocka][dalsi]);
	NaCesteUp(VZDALENOST[pobocka][dalsi]);

	/* Jed */
	DEBUG_AUTO << "jedu " << JMENA_POBOCEK[pobocka] << " -> " << JMENA_POBOCEK[dalsi] << "\n";
	Wait(Exponential(VZDALENOST[pobocka][dalsi]));

	/* Příjezd na pobočku */
	POBOCKY[dalsi]->Prijezd();
	pobocka = dalsi;
	DEBUG_AUTO << "prijel jsem\n";
}

/**
 * \brief Nalozeni jedne zasilky
 */
void Auto::Nalozenie(Zasilka *z)
{
	/* Nalož zásilku */
	zasilky.push_back(z);

	/* Aktualizuj statistiky */
	NaNRampeUp(T_NALOZENI);

	/* Nakládej */
	Wait(Exponential(T_NALOZENI));
}

/**
 * \brief Vylozeni jedne zasilky (dle pobocky)
 */
Zasilka *Auto::Vylozenie()
{
	Zasilka * z{nullptr};

	/* Najdi zásilku pro aktualni pobočku */
	for (std::vector<Zasilka *>::iterator it = zasilky.begin(); it != zasilky.end(); ++it) {
		/* Vylož balíky pro danou pobočku */
		if ((*it)->Cil() == pobocka) {
			/* Cakanie po dobu vylozenia */
			NaVRampeUp(T_VYLOZENI);
			Wait(Exponential(T_VYLOZENI));

			/* Vylož zásilku */
			z = *it;
			zasilky.erase(it);
			break;
		}
	}

	return z;
}
