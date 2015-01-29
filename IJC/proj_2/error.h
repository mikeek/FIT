/* error.h
 * Reseni IJC-DU1, priklad a) b), 28.2.2013
 * Autor: Michal Kozubík, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Hlavickovy soubor pro modul error.c s definici funkci pro vypis chybovych
 * hlasek Error a FatalError s promennym poctem parametru.
 */

#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void Error(const char *fmt, ...);
void FatalError(const char *fmt, ...);

#endif