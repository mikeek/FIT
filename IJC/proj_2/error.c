/* error.c
 * Reseni IJC-DU1, priklad a) b), 28.2.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Soubor obsahuje funkce pro vypis chybovych hlasek na standardni chybovy
 * vystup. U obou se pracuje s promennym poctem parametru (pomoci stdarg.h)
 * a s funkci vfprintf.
 * Error: vypisuje chybovou hlasku na standardni chybovy vystup
 * FatalError: vypisuje chybovou hlasku na standardni chybovy vystup a 
 * 		navic ukonci program pomoci exit(1)
 */

#include "error.h"

void Error(const char *fmt, ...)
{
	/* V zadani je zmineno, ze se ma pouzit funkce vfprint. Neni uvedeno, zda je
	 * pro vypsani uvdni hlasky "CHYBA: " mozne pouzit fprintf(). Kazdopadne mi
	 * to prislo rozumnejsi. Krkolomejsi verze s vfprintf by vypadala takto:
	 * vfprintf(stderr, "CHYBA: ", NULL);
	 */
	va_list args;
	fprintf(stderr, "CHYBA: ");
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	return;
}

void FatalError(const char *fmt, ...)
{
	va_list args;
	fprintf(stderr, "CHYBA: ");
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(1);
}