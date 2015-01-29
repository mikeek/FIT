/* 
 * @file Pobocka.cpp
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida pobocka, projekt do predmetu IMS 2014
 */

#include "Pobocka.h"
#include <string.h>
#include <algorithm>

#define DEBUG_POB DEBUG(id) << "pobocka " << JMENA_POBOCEK[id] << ": "

/**
 * \brief Konstruktor
 */
Pobocka::Pobocka(int i) : id{i}
{
	std::stringstream name;
	name << "Prepazky " << id << " (" << JMENA_POBOCEK[id] << ")";

	/* Nastav počet zaměstnanců */
	prepazky.SetCapacity(ZAMESTNANCU[id]);
	prepazky.SetName(strdup(name.str().c_str()));

	/* Nastav jméno skladu kNalozeni */
	name.clear();
	name.str("");
	name << "kNalozeni " << id << " (" << JMENA_POBOCEK[id] << ")";
	kNalozeni.SetName(strdup(name.str().c_str()));

	/* Inicializuj sklad */
	kNalozeni.SetCapacity(C_POBOCKY);
	kNalozeni.Enter(nullptr, C_POBOCKY);

	/* Nastav pocet tridicu */
	tridici.SetCapacity(TRIDICU[id]);

	/* Nastav jmeno a pocet vykladacich ramp */
	name.clear();
	name.str("");
	name << "VRampa " << id << " (" << JMENA_POBOCEK[id] << ")";
	vRampa.SetName(strdup(name.str().c_str()));
	vRampa.SetCapacity(VRAMP);

	/* Nastav jmeno a pocet nakladacich ramp */
	name.clear();
	name.str("");
	name << "NRampa " << id << " (" << JMENA_POBOCEK[id] << ")";
	nRampa.SetName(strdup(name.str().c_str()));
	nRampa.SetCapacity(NRAMP);

	/* Pusť zákazníky */
	DEBUG_POB << "vytvarin generator\n";
	gener = new GeneratorZakazniku(id);
	gener->Activate();

	/* Najdi nejbližší centrálu */
	nejblizsiCentrala = CENTRALY[0];
	for (auto c : CENTRALY) {
		if (VZDALENOST[Id()][c] < VZDALENOST[Id()][nejblizsiCentrala]) {
			nejblizsiCentrala = c;
		}

		/* A zjisti jestli se je tato pobočka centrální */
		if (c == Id()) {
			jecentrala = true;
		}
	}

	/* Rezervuj misto pro cetnosti cilu u strategii nakládání CETNOST */
	cetnostiCilu.reserve(POBOCEK);
	VynulujCetnostiCilu();

	DEBUG_POB << "nejblizsi centrala je " << JMENA_POBOCEK[nejblizsiCentrala] << "\n";
}

/**
 * \brief Vynulovani cetnosti
 */
void Pobocka::VynulujCetnostiCilu()
{
	/* Inicializuj vektor */
	while ((int) cetnostiCilu.size() < POBOCEK) {
		cetnostiCilu.push_back(0);
	}

	/* Vynuluj ho */
	for (int i = 0; i < POBOCEK; i++) {
		cetnostiCilu[i] = 0;
	}
}

/**
 * \brief Reakce na začátek pracovní doby
 */
void Pobocka::ZacatekPDoby()
{
	/* Pust zákazníky */
	DEBUG_POB << "zacina pracovni doba\n";
	otevrena = true;
	gener->Activate();
}

/**
 * \brief Reakce na konec pracovní doby
 */
void Pobocka::KonecPDoby()
{
	DEBUG_POB << "konci pracovni doba\n";

	/* Zastav generátor zákazníků */
	gener->Passivate();
	otevrena = false;

	/* pošli všechny zákazníky z fronty u přepážek domů */
	while (prepazky.Q->Length() > 0) {
		Zakaznik *z = static_cast<Zakaznik *> (prepazky.Q->front());
		z->KonecPDoby();
	}
}

/**
 * \brief Vyzvednutí zásilky
 */
Zasilka *Pobocka::VyzvednoutZasilku()
{
	/* Neni co vyzvednout */
	if (kVyzvednuti.empty()) {
		return nullptr;
	}

	/* Vyzvedni zásilku */
	Zasilka *z = kVyzvednuti.back();
	kVyzvednuti.pop_back();
	return z;
}

/**
 * \brief Přidej zásilku k odesílaným
 * 
 *	Info o odesílaných musí být
 * a) Vektor zásilek, jelikož každá zásilka si nese informace o svém cíli
 * b) Sklad zásilek (Store), jelikož musí být možné čekání na naskladnění
 */
void Pobocka::PridejKodeslani(Zasilka* z)
{
	/* Přidání zásilky do vektoru */
	kOdeslani.push_back(z);

	/* Přidání zásilky na sklad */
	kNalozeni.Leave(1);
}

/**
 * \brief Přidání zásilky k vyzvednutí
 */
void Pobocka::PridejKvyzvednuti(Zasilka* z)
{
	kVyzvednuti.push_back(z);
}

/**
 * \brief Vyložení zásilky z auta a její roztřídění
 */
void Pobocka::VylozZAuta(Zasilka* z)
{
	z->RoztridSe(this);
}

/**
 * \brief Odeslání zásilky (zákazníkem)
 */
void Pobocka::OdeslatZasilku(Zasilka* z)
{
	z->RoztridSe(this);
}

/**
 * \brief Auto začalo nakládat
 */
void Pobocka::ZacatekNakladani()
{
	cilPosledni = -1;
}

/**
 * \brief Auto ukončilo nakládání
 */
void Pobocka::KonecNakladani()
{
}

/**
 * \brief Oddělej zásilku (danou iteratorem) z vektoru kOdeslani a vrať ji
 */
Zasilka *Pobocka::VydejZOdesVec(zasvec::iterator it)
{
	Zasilka *z = (*it);
	kOdeslani.erase(it);
	return z;
}

/**
 * \brief Vydej první zásilku v pořadí
 */
Zasilka *Pobocka::VydejPrvni()
{
	return VydejZOdesVec(kOdeslani.begin());
}

/**
 * \brief Vydej nejstarší zásilku
 */
Zasilka *Pobocka::VydejNejstarsi()
{
	/* Init - nejstarsi == prvni */
	zasvec::iterator it, nejstarsi = kOdeslani.begin();

	for (it = kOdeslani.begin(); it != kOdeslani.end(); ++it) {
		if ((*it)->Vstup() > (*nejstarsi)->Vstup()) {
			nejstarsi = it;
		}
	}

	/* Oddelej nejstarsi z vektoru a dej ji autu */
	return VydejZOdesVec(nejstarsi);
}

/**
 * \brief Vydej zásilku s nejbližší cílovou pobočkou
 */
Zasilka *Pobocka::VydejNejblizsi()
{
	/* Init - nejstarsi == prvni */
	zasvec::iterator it, nejblizsi = kOdeslani.begin();

	for (it = kOdeslani.begin(); it != kOdeslani.end(); ++it) {
		if (VZDALENOST[id][(*it)->Cil()] < VZDALENOST[id][(*nejblizsi)->Cil()]) {
			nejblizsi = it;
		}
	}

	/* Oddelej nejblizsi z vektoru a dej ji autu */
	return VydejZOdesVec(nejblizsi);
}

/**
 * \brief Vrať zásilku (a vyndej ji z vektoru), která má danou cílovou adresu
 */
Zasilka *Pobocka::VydejProCil(int p)
{
	/* Najdi zásilku s danou cílovou pobočkou */
	for (zasvec::iterator it = kOdeslani.begin(); it != kOdeslani.end(); ++it) {
		if ((*it)->Cil() == p) {
			return VydejZOdesVec(it);
		}
	}

	return nullptr;
}

/**
 * \brief Vydej zásilku s nejčetnější cílovou adresou
 */
Zasilka *Pobocka::VydejNejcetnejsi()
{
	if (cilPosledni >= 0) {
		/* Do tohoto auta uz se nejaka z nejčetnějších naložila, najdi jinou se stejným cílem */
		Zasilka *z = VydejProCil(cilPosledni);
		if (z) {
			/* Mame ji, nalozit */
			return z;
		}

		/* Zasilka pro nejcetnejsi cil nenalezena */
	}

	/* Je potreba najit nejcetnejsi cil */
	VynulujCetnostiCilu();

	/* Zjisti cetnost cilu */
	for (auto z : kOdeslani) {
		cetnostiCilu[z->Cil()]++;
	}

	/* Zjisti nejvetsi cetnsot */
	cilPosledni = 0;
	for (int i = 0; i < (int) cetnostiCilu.size(); ++i) {
		if (cetnostiCilu[i] > cetnostiCilu[cilPosledni]) {
			cilPosledni = i;
		}
	}

	return VydejProCil(cilPosledni);
}

/**
 * \brief Odeslání 1 zásilky (předání autu)
 * 
 *	O snížení zásilek na skladě se stará samo auto
 */
Zasilka *Pobocka::VydejKOdeslani()
{
	/* Jakou metodu zvolit? */
	switch (METODA_NAKL) {
	case metodyNakladani::NEJSTARSI: return VydejNejstarsi();
	case metodyNakladani::NEJBLIZSI: return VydejNejblizsi();
	case metodyNakladani::CETNOST: return VydejNejcetnejsi();
	default: return VydejPrvni();
	}
}

/* Statistické funkce */

/**
 * \brief Zákazník vtoupil do pobočky
 */
void Pobocka::Vstup()
{
}

/**
 * \brief Zákazník vyšel z pobočky
 */
void Pobocka::Odchod()
{
}

/**
 * \brief Příjezd auta
 */
void Pobocka::Prijezd()
{
}

/**
 * \brief Odjezd auta
 */
void Pobocka::Odjezd()
{
}
