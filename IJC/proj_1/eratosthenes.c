/* eratosthenes.c
 * Reseni IJC-DU1, priklad a) b), 28.2.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Modul s implementaci Eratosthenova sita pro zjisteni prvocisel.
 * Pracuji nad bitovym polem, tudiz k praci potrebuje hlavickovy soubor
 * bit-array.h. Pracuje na klasickem principu Erat. sita
 */

#include "eratosthenes.h"

void Eratosthenes(BitArray_t pole)
{
	unsigned long n = BitArraySize(pole); // celkovy pocet bitu

	// Aby se nemusel nulovat bit po bitu, tak se zjisti, kolik prvku typu
	// BitArray_t pole obsahuje a vynuluje vzdy cele prvky
	unsigned long prvku = n / (sizeof (pole[0])*8) + 1L;
	for (unsigned long i = 1L; i <= prvku; i++)
		pole[i] = 0L;

	for (unsigned long i = 2L; i <= (unsigned long) sqrt(n); i++)
		if (GetBit(pole, i) == 0)
			for (unsigned long j = i * i; j < n; j += i)
				SetBit(pole, j, 1);
	return;
}