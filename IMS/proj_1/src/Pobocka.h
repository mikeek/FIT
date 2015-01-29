/* 
 * @file Pobocka.h
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida pobocka, projekt do predmetu IMS 2014
 */

#ifndef POBOCKA_H
#define	POBOCKA_H

#include <vector>
#include "Defs.h"


/**
 * \brief Třída pobočky
 */
class Pobocka {
	/* alias pro vektor zásilek */
	using zasvec = std::vector<Zasilka *>;
public:
	Pobocka(int i);		// Konstruktor, i == ID pobočky
	
	/* Změna pracovní doby */
	void KonecPDoby();		// Ukončení pracovní doby
	void ZacatekPDoby();	// Začátek nového pracovního dne
	
	/* Požadavky zákazníka */
	Zasilka *VyzvednoutZasilku();		// Vyzvednutí 1 zásilky zákazníkem
	void OdeslatZasilku(Zasilka *z);	// Žádost o odeslání 1 zásilky zákazníkem
	
	/* Požadavky auta */
	void VylozZAuta(Zasilka *z);		// Přijmutí zásilky voložené z auta
	void PridejKvyzvednuti(Zasilka *z);	// Přidání zásilky k čekajícím na vyzvednutí
	Zasilka *VydejKOdeslani();			// Vyjmutí 1 zásilky z čekajících na odeslání (autem)
	
	/* Informační  funkce */
	void Vstup();			// Vstup zákazníka
	void Odchod();			// Odchod zákazníka
	void Prijezd();			// Příjezd auta
	void Odjezd();			// Odjezd auta
	void ZacatekNakladani();// Auto začalo nakládat zásilky
	void KonecNakladani();	// Auto dokončilo nakládání
	
	/* Gettery */
	Store &Prepazky()	{ return prepazky; }  // Sklad přepážek
	Store &KNalozeni()	{ return kNalozeni; } // Sklad zásilek k naložení
	Store &VRampa()		{ return vRampa; }	  // Vykládací rampa
	Store &NRampa()		{ return nRampa; }	  // Nákladová rampa
	Store &Tridici()	{ return tridici; }	  // Třídící stroje
	
	/* Info o pobočce */
	int  Id()		  { return id; }		 // ID pobočky
	bool jeCentrala() { return jecentrala; } // Jedna se o centralni pobocku?
	bool Otevrena()	  { return otevrena; }	 // je pobočka otevřená? 
	int	 NejblizsiCentrala() { return nejblizsiCentrala; } // ID nejbližší centraly

	/* Manipulace se zásilkami */
	void PridejKodeslani(Zasilka *z);	// Přidání 1 zásilky k odesílaným
	void PridejKroztrideni(Zasilka *z); // Přidání 1 zásilky k neroztříděným
	
private:
	/* Vydání zásilky kOdeslani */
	Zasilka *VydejPrvni();			// Vydání první zásilky z fronty
	Zasilka *VydejNejstarsi();		// Vydání nejstarší zásilky
	Zasilka *VydejNejblizsi();		// Vydání zásilky s nejbližší cílovou pobočkou
	Zasilka *VydejNejcetnejsi();	// Vydání zásilky s nejčetnější cílovou pobočkou
	Zasilka *VydejProCil(int p);	// Vydání zásilky s danou cílovou pobočkou
	Zasilka *VydejZOdesVec(zasvec::iterator it);	// Vydání zásilky (dle iteratoru) z vektoru kOdeslani
	void VynulujCetnostiCilu();		// Vynulovani vektoru cetnosti
	
	int id;						/**< ID pobočky */
	int nejblizsiCentrala;		/**< ID nejblizsi centraly */
	bool jecentrala{false};		/**< Jedna se o centralni pobočku? */
	bool otevrena{true};		/**< Je pobočka otevřená? */

	zasvec kVyzvednuti{};		/**< Seznam zásilek k vyzvednutí */
	zasvec kOdeslani{};			/**< Seznam zásilek k odeslání */
	
	Store prepazky;				/**< Přepážky */
	Store kNalozeni;			/**< Zásilky připravené k naložení */
	Store vRampa;				/**< Vykladací rampy */
	Store nRampa;				/**< Nakládací rampy */
	Store tridici;				/**< Třídící stroje */
	
	GeneratorZakazniku *gener{};/**< generátor zákazníků */
	
	/* Atributy pro VydejNejcetnejsi */
	int cilPosledni;				/**< Cíl poslední vydané zásilky */
	std::vector<int> cetnostiCilu{};/**< Cetnost cílů pro VydejNejcetnejsi */
};

#endif	/* POBOCKA_H */
