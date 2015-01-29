/* steg-decode.c
 * Reseni IJC-DU1, priklad b), 28.2.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * steg-decode.c resi rozlusteni zakodovane zpravy v souboru *.ppm, jehoz
 * cely nazev je prijiman jako jediny argument. Po nacteni tohoto souboru
 * do struktury (definovane v ppm.h), vytvori binarni pole s pocten bitu
 * podle poctu bytu barev v souboru ppm, z tohoto pole pomoci
 * Eratosthenova sita (eratosthenes.h) ziska prvocisla (prvociselne indexy).
 * Z bytu ve strukture ppm na techto prvociselnych pozicich postupne vytahuje
 * posledni bity, ze kterych sklada znaky, dokud nenarazi na koncovou nulu.
 * Tim je vypsana cela zasifrovana zprava
 */

#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"
#include "bit-array.h"
#include "eratosthenes.h"

#define BITS 8 //pocet bitu v charu

int main(int argc, char *argv[])
{
	// je vyzadovan prave jeden parametr (nazev souboru) => argc==2
	if (argc != 2) {
		FatalError("Zadan spatny pocet parametru (vyzadovan 1)");
		return -1;
	}

	// struktura pro uchovani ppm souboru
	struct ppm *p = ppm_read(argv[1]);
	if (p == NULL)
		FatalError("Nacteni souboru se nezdarilo");

	// celkovy pocet barevnych bytu v souboru
	unsigned long size = p->xsize * p->ysize * 3;

	// vytvoreni bitoveho pole s poctem bitu rovnym s pocten bytu v souboru
	// ulozeni teto velikosti do prvni polozky pole a aplikace Erat. sita
	unsigned long *prims = malloc(size + sizeof (unsigned long));
	prims[0] = size;
	Eratosthenes(prims);

	/* Pole pro uchovavani aktualniho znaku. Musi se pouzit pole, jelikoz pri
	 * praci s GetBit a SetBit se na nulte pozici musi nachazet rozmer pole.
	 * Kdybychom pouzili obycejny char, museli bychom pouzivat makra 
	 * DU1_SET_BIT a DU1_GET_BIT bez kontroly mezi pole. Zde jsem vsak pouzil
	 * bezpecnejsi variantu (za cenu 1B pro uchovani velikosti)
	 */
	char messChar[2] = {8, 0};
	int j = 0;
	for (int i = 2; i < prims[0]; i++)
		if (GetBit(prims, i) == 0) { // Kdyz je prvociselny index, nastavime j-ty bit v charu na hodnotu
			// LSB prislusne polozky v poli bytu
			SetBit(messChar, j, (p->data[i] & 1));
			if (++j == BITS) { // Pri dosazeni koncove nuly se s vypisem konci
				if (messChar[1] == '\0') {
					printf("\n");
					break;
				}
				printf("%c", messChar[1]);
				messChar[1] = j = 0;
			}
		}
	if (prims != NULL) free(prims);
	if (p != NULL) free(p);
	return 0;
}
