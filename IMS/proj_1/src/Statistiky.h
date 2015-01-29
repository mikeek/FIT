/* 
 * @file Statistiky.h
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Trida statistiky, projekt do predmetu IMS 2014
 */

#ifndef STATISTIKY_H
#define	STATISTIKY_H

#include "Defs.h"

/**
 * \brief Trida pro sběr a tisk statistik
 */
 class Statistiky {
 public:
 	void ZobrazStatistiky();	// Zobrazenie statistik
 	void SimlibStatistky();		// Zobrazenie SIMLIB statistik

	void NovyPracovniDen();		// oznameni od casovace, ze je novy pracovni den
	
	void PridajStatistiku(Zasilka *z);	// Pridanie statistiky zo zasielky
	
	void ZakaznikUspech()	{ uspechu++; }		// Úspěšny pokus o vyzvednutí zásilky
	void ZakaznikNeuspech() { neuspechu++; }	// Neuspěšny pokus
	void ZakaznikOdeslal()  { odeslanych++; }	// Odeslani zasilky zakaznikem

 private:
	 double TyzdenVsVikend(int,int);	// Podíl tydne / víkendu
	 std::string Cas(double c);			// Naformátování času (v minutách)
	 
	int uspechu{0};			/**< uspěšnych pokusu o vyzvednutí zásilky */
	int neuspechu{0};		/**< neuspěšnych pokusu o vyzvednutí zásilky */
	int odeslanych{0};		/**< pocet odeslanych zasilek */
	
	std::vector<double> statistikaZasielok{}; /**< cas v systemu pro kazdou zasilku */
 };

 #endif	/* STATISTIKY_H */
