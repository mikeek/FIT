/* 
 * @file PDoba.h
 * @author Michal Kozubik, xkozub03
 * @author Marek Hurta, xhurta01
 * @date 22.11.2014
 * @brief Hlidac pracovni doby, projekt do predmetu IMS 2014
 */

#ifndef PDOBA_H
#define	PDOBA_H

#include "Defs.h"

/**
 * \brief Hlídač začátku/konce pracovní doby
 */
class PDoba: public Process {
public:
	int  AktualniDen() { return den; }	  // Vrati aktualniy den
	bool JeVikend()	   { return vikend; } // Je víkend? 
	
private:
	void Behavior(); // Hlavní proces
	
	int den{PONDELI};	/**< den v simulaci */
	bool vikend{false}; /**< indikátor víkendu */
};
#endif	/* PDOBA_H */
