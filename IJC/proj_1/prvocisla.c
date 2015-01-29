/* prvocisla.c
 * Reseni IJC-DU1, priklad a), 28.2.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Program vypise na standardni vystup poslednich 10 prvocisel ze zadaneho
 * celkoveho rozsahu cisel. Pracuje s bitovym polem (bit-array.h) a s 
 * Eratosthenovym sitem (eratosthenes.h). V pripade, ze je zadana mala
 * velikost bitoveho, jenz tedy nebude obsahovat 10 prvocisel (ale napr.
 * jen 5), je vypsano jen techto 5.
 */

#include <stdio.h>
#include "bit-array.h"
#include "error.h"
#include "eratosthenes.h"

int main(void)
{
	BitArray(pole, 89000000L); // vytvoreni bitoveho pole
	Eratosthenes(pole); // zjisteni prvocisel v tomto poli

	unsigned long prims[10] = {0}; // pole pro uchovani poslednich 10 prvocisel
	int primIndex = 10;

	/* Bitove pole se prochazi od konce a pri nalezeni prvociselneho indexu
	 * (GetBit(pole, i) == 0) je tento index zaznamenan do pole prvocisel
	 * prims[]. primIndex se postupne zmensuje, takze je do pole take zapisovano
	 * od konce a navic po skonceni cyklu se uchova nejnizsi index prvociselneho
	 * pole, takze se vyhneme pripadnemu vypisovani nul, kdyby bitove pole
	 * obsahovalo mene nez 10 prvocisel
	 */
	for (unsigned long i = BitArraySize(pole) - 1L; (i > 1L && primIndex > 0); i--)
		if (GetBit(pole, i) == 0) prims[--primIndex] = i;

	for (int i = primIndex; i < 10; i++) // vypis prvocisel
		printf("%lu\n", prims[i]);

	return 0;
}
