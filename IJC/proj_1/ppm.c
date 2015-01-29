/* ppm.c
 * Reseni IJC-DU1, priklad b), 28.2.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Modul obsahuje 2 funkce:
 * ppm_read: funkce nacte ppm soubor, jehoz nazev je specifikovan v parametru
 * 	funkce, zkontroluje, zda-li se jedna o validni ppm soubor (v zapornem
 * 	pripade vypise prislusne chybove hlaseni a navrati hodnotu NULL) a 
 * 	navrati ukazatel na takto inicializovanou strukturu typu struct ppm
 * ppm_write: funkce jako jeden parametr prijima ukazatel na strukturu s daty
 * 	potrebnymi pro vytvoreni souboru ppm, jehoz nazev je specifikovan 
 * 	druhym parametrem. Pri chybe vypisuje chybove hlaseni.
 */


#include "ppm.h"

struct ppm *ppm_read(const char *filename)
{
	if (filename == NULL) { // kontrola nazvu souboru
		Error("Spatny nazev souboru");
		return NULL;
	}
	FILE *fr = fopen(filename, "rb");
	if (fr == NULL) { // kontrola spravneho otevreni souboru
		Error("Chyba pri otevirani souboru");
		return NULL;
	}

	// Soubor musi zacinat znaky "P6"
	if (getc(fr) != 'P' || getc(fr) != '6') {
		Error("Spatny format souboru");
		fclose(fr);
		return NULL;
	}

	unsigned x, y;
	int clr;
	if (fscanf(fr, "%u %u %d", &x, &y, &clr) != 3 || (clr != 255)) { // Nacteni poctu pixelu a nejvyssi hodnoty barev
		Error("Spatny format souboru");
		fclose(fr);
		return NULL;
	}
	// Preskoceni bilych znaku do konce radku
	while (getc(fr) != '\n');

	// Alokovani pameti pro strukturu pro nacteni ppm souboru
	struct ppm *readed = (struct ppm *) malloc(sizeof (struct ppm)+x * y * 3);

	if (readed == NULL) {
		Error("Chyba pri alokaci pameti");
		fclose(fr);
		return NULL;
	}
	readed->xsize = x;
	readed->ysize = y;

	// Nacteni hodnot jednotlivych bytu ze souboru do struktury
	unsigned long cnt = fread(readed->data, 1, x * y * 3, fr);
	if (cnt < x * y * 3) { // Kontrola, zda-li soubor obsahoval ocekavany pocet bytu
		printf("chyba");
		free(readed);
		fclose(fr);
		return NULL;
	}

	fclose(fr);
	return readed;
}

int ppm_write(struct ppm *p, const char *filename)
{
	if (p == NULL) { // test platneho ukazatele na strukturu
		Error("Chyba pri praci se strukturou");
		return -1;
	}
	if (filename == NULL) { // test platneho nazvu souboru
		Error("Chybne zadan nazev souboru pro zapis");
		return -1;
	}
	if (p->xsize <= 0 || p->ysize <= 0) { // test rozmeru dat
		Error("Chybne rozmery datove struktury");
		return -1;
	}
	FILE *fw = fopen(filename, "wb");
	if (fw == NULL) {
		Error("Chyba pri otevirani souboru");
		return -1;
	}

	// vytisknuti hlavicky souboru
	fprintf(fw, "P6 \n%u %u\n255\n", p->xsize, p->ysize);

	// zapis dat do souboru
	unsigned long cnt = fwrite(p->data, 1, p->xsize * p->ysize * 3, fw);
	if (cnt < (p->xsize * p->ysize * 3)) { // v pripade zapsani mene dat nez stanovuji rozmery
		Error("Chyba pri zapisu dat do souboru");
		fclose(fw);
		return -1;
	}
	fclose(fw);
	return 0;
}