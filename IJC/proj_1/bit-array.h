/* bit-array.h
 * Reseni IJC-DU1, priklad a) b), 28.2.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Hlavickovy soubor s definicemi maker a inline funkci pro praci s bitovym
 * polem. Vyber maker/inline funkci je realizovan definovanim USE_INLINE pri
 * prekladu programu
 */


#ifndef _BIT_ARRAY_H_
#define _BIT_ARRAY_H_

#include "error.h"

// Typ bitoveho pole
typedef unsigned long BitArray_t[];

/* Vytvoreni bitoveho pole typu BitArray_t.
 * Pocet polozek BitArray_t[n] nutnych pro praci s polem je vypocitan pomoci
 * pozadovaneho poctu bitu a sizeof(BitArray_t) (+1 kvuli indexaci od nuly
 * a +1 kvuli tomu, ze v arr[0] je ulozena informace o velikosti pole)
 */
#define BitArray(arr, size) unsigned long arr[(size-1L)/(sizeof(unsigned long)*8)+2L] = {0}; \
  arr[0]=(unsigned long)size

// Navraceni velikosti pole (pocet bitu)
#define BitArraySize(arr) (arr[0])

// Ziskani hodnoty bitu na pozici index, pomoci pomocne promenne mask, ktera
// je nastavena a pouzita vyuzitim operatoru carka. Neobsahuje kontrolu mezi.
#define DU1_GET_BIT_(arr, index) (arr[INX(index, sizeof(arr[0]))]&(1L<<position(index,sizeof(arr[0]))))\
  ? 1 : 0

// Nastaveni hodnoty bitu na pozici index, pomoci pomocne promenne mask, ktera
// je nastavena a pouzita vyuzitim operatoru carka. Neobsahuje kontrolu mezi.
#define DU1_SET_BIT_(arr, index, expr) (expr==0)\
  ?(arr[INX(index, sizeof(arr[0]))]&=~(1L<<position(index, sizeof(arr[0]))))\
  :(arr[INX(index, sizeof(arr[0]))]|=1L<<position(index, sizeof(arr[0])))

/* Zjisti pozici hledaneho bitu v pouzitem datovem typu (sizeof(BitArray_t)).
 * Napr. chceme-li 10. bit v poli deklarovanem jako unsigned char, je jeho 
 * pozice rovna 2. bitu v 2. polozce pole => position==1 (idnexace od nuly) 
 */
#define position(index,size) (index-((index/(size*8))*(size*8)))

/* Zjisteni indexu pole, ve kterem se nachazi hledany bit. Napr. pro hledani
 * 10. bitu v poli unsigned charu => INX == 2 (zde jiz neni odexace od nuly),
 * jelikoz arr[0] je vyhrazeno pro velikost pole 
 */
#define INX(index, size) ((index)/(size*8)+1L)

#ifdef USE_INLINE // Pri definovanem makru USE_INLINE pri prekladu

// Testovani mezi pole a pripadne chybove hlaseni nebo nastaveni bitu
// pres inline funkci

static inline void SetBit(BitArray_t arr, unsigned long index, int expr) {
	if (index < 0 || index >= arr[0])
		FatalError("Index %ld mimo rozsah 0..%ld", (long) index, (long) (arr[0] - 1));
	DU1_SET_BIT_(arr, index, expr);
}

// Testovani mezi pole a pripadne chybove hlaseni nebo ziskani hodnoty bitu
// pres inline funkci

static inline int GetBit(BitArray_t arr, unsigned long index) {
	if (index < 0 || index >= arr[0])
		FatalError("Index %ld mimo rozsah 0..%ld", (long) index, (long) (arr[0] - 1));
	return DU1_GET_BIT_(arr, index);
}
#else

// Testovani mezi pole a pripadne chybove hlaseni nebo nastaveni bitu
#define SetBit(arr, index, expr) (index<0 || index>=arr[0])\
  ?(FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)(arr[0]-1)),2)\
  :(DU1_SET_BIT_(arr, index, expr))

// Testovani mezi pole a pripadne chybove hlaseni nebo ziskani hodnoty bitu
#define GetBit(arr, index) (index<0 || index>=arr[0])\
  ?(FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)(arr[0]-1)), 2)\
  :(DU1_GET_BIT_(arr,index))
#endif

#endif