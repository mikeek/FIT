
/* ******************************* c204.h *********************************** */
/*  Pøedmìt: Algoritmy (IAL) - FIT VUT v Brnì                                 */
/*  Úkol: c204 - Pøevod infixového výrazu na postfixový (s vyu¾itím c202)     */
/*  Referenèní implementace: Petr Pøikryl, listopad 1994                      */
/*  Pøepis do jazyka C: Luká¹ Mar¹ík, prosinec 2012                           */
/*  Upravil: Bohuslav Køena, øíjen 2013                                       */
/* ************************************************************************** */

/* TENTO SOUBOR, PROSÍME, NEUPRAVUJTE! */

#ifndef _INFIX2POSTFIX_H_
#define _INFIX2POSTFIX_H_

#include <stdio.h>
#include <stdlib.h>

#include "c202.h"                          /* vyu¾ívá pøíklad c202 - zásobník */

#define MAX_LEN 64                          /* maximální délka øetìzce výrazu */

extern int solved;                      /* Indikuje, zda byla operace øe¹ena. */

/* Konverzní funkce */
char* infix2postfix (const char* infExpr);

#endif

/* Konec hlavièkového souboru c204.h */
