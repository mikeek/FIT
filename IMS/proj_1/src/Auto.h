/* 
 * @file Auto.h
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida auto, projekt do predmetu IMS 2014
 */

#ifndef AUTO_H
#define	AUTO_H

#include "Defs.h"

/**
 * \brief Proces auta převážejícího zásilky
 */
class Auto: public Process {
public:
	Auto(int p, int i = -1);	// Konstruktor, p = ID pobočky, kde se auto nachází, i == id auta (nezadano -> stejne jako p)
	
	void timeout();				// Vypršení timeoutu při čekání na naložení zásilky
	
	/* Stavové informace */
	bool Plne()		{ return zasilky.size() == static_cast<size_t>(C_AUTA); } // Je auto plné?
	bool Prazdne()	{ return zasilky.empty(); }	// Je auto prázdné?
	int  Zasilek()	{ return zasilky.size(); }	// Počet zásilek v autě
	bool Timeouted(){ return timeouted; }		// Vypršel timeout?
	
	/* Změna pracovní doby */
	void KonecPDoby();			// Reakce na konec pracovní doby
	void ZacatekPDoby(); 		// Reakce na začátek pracovní doby

	/* Statisticke informace */
	int  Prepravene(int); 		// Pocet prepravenych zasielok
	int  Vzdialenost(int); 		// Celkova vzdialenost 
	double NaCeste(int); 		// Celkovy cas na ceste
	double NaNRampe(int); 		// Celkovy cas na nakladacej rampe
	double NaVRampe(int); 		// Celkovy cas na vykladecej rampe

	int Id() { return id; }		// ID auta
	
private:
	void Behavior();			// Hlavní proces
	
	/* Manipulace se zásilkami */
	void Nalozenie(Zasilka *);	// Naložení 1 zásilky do auta
	Zasilka *Vylozenie();		// Vyložení 1 zásilky z auta dle aktuální pobočky
	
	void NalozZasilky();		// Naložení zásilek z aktuální pobočky
	void VylozZasilky();		// Vyložení zásilek na aktuální pobočku
	
	void Odjezd();				// Přejezd mezi pobočkami
	
	/* Určení cíle */
	int DalsiCil();			// Urči další cílovou pobočku
	int NejblizsiCil();		// Urči nejbližší pobočku (ze zásilek)
	int NejstarsiCil();		// Urči cíl nejstarší zásilky v autě
	int NejcetnejsiCil();	// Urči nejčetnější cíl zásilek v autě
	void VynulujCetnosti();	// Vynuluj četnosti poboček

	/* Update statistických informací */
	void NaVRampeUp(double); 			// Aktualizacia hodnoty casu na vykladacej rampe
	void NaNRampeUp(double); 			// Aktualizacia hodnoty casu na nakladacej rampe 
	void NaCesteUp(double); 			//Aktualizacia hodnoty casu na ceste
	void VzdialenostUp(int); 			// Aktualizacia hodnoty vzdialenosti
	void PrepraveneUp(); 				// Aktualizacia hodnoty prepravenych zasielok
	
	int id;								/**< ID auta */
	int pobocka;						/**< aktuální pobočka */
	bool timeouted{false};				/**< Vypršení timeoutu */
	bool pdoba{true};					/**< je pracovni doba? */

	std::vector<Zasilka *> zasilky{};	/**< Seznam zásilek v autě */
	std::vector<int> cetnostiCilu{};	/**< Četnosti jednotlivých poboček u zásilek */
	
	/* Statistické informace */
	int celkovyPocet{0};				/**< Celkovy pocet zasielok prepravenych autom */
	int celkovyPocetVikend{0};			/**< Celkovy pocet zasielok prepravenych za vikend */

	int celkovaVzdialenost{0};			/**< Celkova najazdena vzdialenost v Km */
	int celkovaVzdialenostVikend{0}; 	/**< Celkova najazdena vzdialenost v Km za vikend */

	double dobaNaCeste{0};				/**< Celkova doba na ceste v minutach */ 
	double dobaNaCesteVikend{0};		/**< Celkova doba na ceste v minutach za vikend */

	double NRampaCas{0};				/**< Celkova doba stravena na nakladecej rampe v minutach */
	double NRampaCasVikend{0};			/**< Celkova doba stravena na nakladecej rampe za vikend v minutach */

	double VRampaCas{0};				/**< Celkova doba stravena na vykladacej rampe v minutach */
	double VRampaCasVikend{0};			/**< Celkova doba stravena na vykladacej rampe za vikend v minutach */
};

/**
 * \brief Timeout pro čekání na naložení
 */
class Timeout: public Event {
public:
	Timeout(Auto *a): Event(), A(a) {}; // Konstruktor, a == auto
	
private:
	void Behavior()
	{
		A->timeout();	// Pošli autu zprávu o timeoutu
		Cancel();		// Ukončí timer
	}
	
	Auto *A;	/**< Auto, které čeká na naložení */
};

#endif	/* AUTO_H */
