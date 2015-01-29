
/* ******************************* c202.h *********************************** */
/*  Pøedmìt: Algoritmy (IAL) - FIT VUT v Brnì                                 */
/*  Úkol: c202 - Zásobník znakù v poli                                        */
/*  Hlavièkový soubor pro c202.c                                              */
/*  Vytvoøil: Václav Topinka, záøí 2005                                       */
/*  Úpravy: Bohuslav Køena, záøí 2011                                         */
/* ************************************************************************** */

/* TENTO SOUBOR, PROSÍME, NEUPRAVUJTE! */

#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>

#define MAX_STACK 20
extern int STACK_SIZE; 
/* 
 * Hodnota MAX_STACK udává skuteènou velikost statického pole pro ulo¾ení
 * hodnot zásobníku. Pøi implementaci operací nad ADT zásobník v¹ak
 * pøedpokládejte, ¾e velikost tohoto pole je pouze STACK_SIZE. Umo¾ní to
 * jednodu¹e mìnit velikost zásobníku v prùbìhu testování. Pøi implementaci
 * tedy hodnotu MAX_STACK vùbec nepou¾ívejte. 
 */

extern int solved;                      /* Indikuje, zda byla operace øe¹ena. */
extern int err_flag;                   /* Indikuje, zda operace volala chybu. */

                                        /* Chybové kódy pro funkci stackError */
#define MAX_SERR    3                                   /* poèet mo¾ných chyb */
#define SERR_INIT   1                                  /* chyba pøi stackInit */
#define SERR_PUSH   2                                  /* chyba pøi stackPush */
#define SERR_TOP    3                                   /* chyba pøi stackTop */

                             /* ADT zásobník implementovaný ve statickém poli */
typedef struct {
	char arr[MAX_STACK];                             /* pole pro ulo¾ení hodnot */
	int top;                                /* index prvku na vrcholu zásobníku */
} tStack;

                                  /* Hlavièky funkcí pro práci se zásobníkem. */
void stackError ( int error_code );
void stackInit ( tStack* s );
int stackEmpty ( const tStack* s );
int stackFull ( const tStack* s );
void stackTop ( const tStack* s, char* c );
void stackPop ( tStack* s );
void stackPush ( tStack* s, char c );

#endif

/* Konec hlavièkového souboru c202.h */
