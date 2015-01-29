/* ppm.h
 * Reseni IJC-DU1, priklad b), 28.2.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Hlavickovy soubor pro modul ppm.c
 * Definice struktury pro uchovani ppm souboru a funkce pro praci s nim
 */


#ifndef _PPM_H_
#define _PPM_H_

#include <stdio.h>
#include "error.h"
#include <stdlib.h>
#include <string.h>

// Identifikator PPM je zde uveden jen proto, aby zbytecne prekladac
// nevypisoval warning. Dale jsem se drzel zadani a pouzival "struct ppm"

typedef struct ppm {
	unsigned xsize;
	unsigned ysize;
	char data[];
} PPM;

struct ppm *ppm_read(const char *filename);
int ppm_write(struct ppm *p, const char *filename);

#endif