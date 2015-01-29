/* ******************************* c202.c *********************************** */
/*  Pï¿½edmï¿½t: Algoritmy (IAL) - FIT VUT v Brnï¿½                                 */
/*  ï¿½kol: c202 - Zï¿½sobnï¿½k znakï¿½ v poli                                        */
/*  Referenï¿½nï¿½ implementace: Petr Pï¿½ikryl, 1994                               */
/*  Vytvoï¿½il: Vï¿½clav Topinka, zï¿½ï¿½ï¿½ 2005                                       */
/*  ï¿½pravy: Bohuslav Kï¿½ena, ï¿½ï¿½jen 2013                                        */
/* ************************************************************************** */
/*
 ** Implementujte datovï¿½ typ zï¿½sobnï¿½k znakï¿½ ve statickï¿½m poli. ADT zï¿½sobnï¿½k je
 ** reprezentovï¿½n zï¿½znamem typu tStack. Statickï¿½ pole 'arr' je indexovï¿½no
 ** do maximï¿½lnï¿½ hodnoty STACK_SIZE. Poloï¿½ka zï¿½znamu 'top' ukazuje na prvek
 ** na vrcholu zï¿½sobnï¿½ku. Prï¿½zdnï¿½mu zï¿½sobnï¿½ku odpovï¿½dï¿½ hodnota top == -1,
 ** zï¿½sobnï¿½ku s jednï¿½m prvkem hodnota 0, atd. Pï¿½esnou definici typï¿½ neleznete
 ** v hlaviï¿½kovï¿½m souboru c202.h. 
 **
 ** Implementujte nï¿½sledujï¿½cï¿½ funkce:
 **
 **    stackInit .... inicializace zï¿½sobnï¿½ku
 **    stackEmpty ... test na prï¿½zdnost zï¿½sobnï¿½ku
 **    stackFull .... test na zaplnï¿½nost zï¿½sobnï¿½ku
 **    stackTop ..... pï¿½eï¿½te hodnotu z vrcholu zï¿½sobnï¿½ku
 **    stackPop ..... odstranï¿½ prvek z vrcholu zï¿½sobnï¿½ku
 **    stackPush .... vloï¿½ï¿½ prvku do zï¿½sobnï¿½ku
 **
 ** Svï¿½ ï¿½eï¿½enï¿½ ï¿½ï¿½elnï¿½ komentujte!
 **
 ** Terminologickï¿½ poznï¿½mka: Jazyk C nepouï¿½ï¿½vï¿½ pojem procedura.
 ** Proto zde pouï¿½ï¿½vï¿½me pojem funkce i pro operace, kterï¿½ by byly
 ** v algoritmickï¿½m jazyce Pascalovskï¿½ho typu implemenovï¿½ny jako
 ** procedury (v jazyce C procedurï¿½m odpovï¿½dajï¿½ funkce vracejï¿½cï¿½ typ void).
 **
 **/

#include "c202.h"

int STACK_SIZE = MAX_STACK;
int solved;

void stackError(int error_code)
{
	/*   ----------
	 ** Vytiskne upozornï¿½nï¿½, ï¿½e doï¿½lo k chybï¿½ pï¿½i prï¿½ci se zï¿½sobnï¿½kem.
	 **
	 ** TUTO FUNKCI, PROSï¿½ME, NEUPRAVUJTE!
	 */
	static const char* SERR_STRINGS[MAX_SERR + 1] = { "Unknown error",
			"Stack error: INIT", "Stack error: PUSH", "Stack error: TOP" };
	if (error_code <= 0 || error_code > MAX_SERR)
		error_code = 0;
	printf("%s\n", SERR_STRINGS[error_code]);
	err_flag = 1;
}

void stackInit(tStack* s)
{
	/*   ---------
	 ** Provede inicializaci zï¿½sobnï¿½ku - nastavï¿½ vrchol zï¿½sobnï¿½ku.
	 ** Hodnoty ve statickï¿½m poli neupravujte - po inicializaci zï¿½sobnï¿½ku
	 ** jsou nedefinovanï¿½.
	 **
	 ** V pï¿½ï¿½padï¿½, ï¿½e funkce dostane jako parametr s == NULL,
	 ** volejte funkci stackError(SERR_INIT). U ostatnï¿½ch funkcï¿½ pro zjednoduï¿½enï¿½
	 ** pï¿½edpoklï¿½dejte, ï¿½e tato situace nenastane. 
	 */
    if (s == NULL) {
        stackError(SERR_INIT);
    } else {
        /* Na zaèátku je zásobník prázdný -> top ukazuje na -1*/
        s->top = -1;
    }
}

int stackEmpty(const tStack* s)
{
	/*  ----------
	 ** Vracï¿½ nenulovou hodnotu, pokud je zï¿½sobnï¿½k prï¿½zdnï¿½, jinak vracï¿½ hodnotu 0.
	 ** Funkci implementujte jako jedinï¿½ pï¿½ï¿½kaz. Vyvarujte se zejmï¿½na konstrukce
	 ** typu "if ( true ) b=true else b=false".
	 */
    
    /* Pokud top == -1, je zásobník prázdný */
    return (s->top == -1);
}

int stackFull(const tStack* s)
{
	/*  ---------
	 ** Vracï¿½ nenulovou hodnotu, je-li zï¿½sobnï¿½k plnï¿½, jinak vracï¿½ hodnotu 0.
	 ** Dejte si zde pozor na ï¿½astou programï¿½torskou chybu "o jedniï¿½ku"
	 ** a dobï¿½e se zamyslete, kdy je zï¿½sobnï¿½k plnï¿½, jestliï¿½e mï¿½e obsahovat
	 ** nejvï¿½e STACK_SIZE prkvï¿½ a prvnï¿½ prvek je vloï¿½en na pozici 0.
	 **
	 ** Funkci implementujte jako jedinï¿½ pï¿½ï¿½kaz.
	 */

    /* 
     * Pokud bylo dosa¾eno STACK_SIZE - 1, je zásobník plný (alokováno je STACK_SIZE 
     * polo¾ek, indexuje se od 0 *
     */
	return (s->top >= (STACK_SIZE - 1));
}

void stackTop(const tStack* s, char* c)
{
	/*   --------
	 ** Vracï¿½ znak z vrcholu zï¿½sobnï¿½ku prostï¿½ednictvï¿½m parametru c.
	 ** Tato operace ale prvek z vrcholu zï¿½sobnï¿½ku neodstraï¿½uje.
	 ** Volï¿½nï¿½ operace Top pï¿½i prï¿½zdnï¿½m zï¿½sobnï¿½ku je nekorektnï¿½
	 ** a oï¿½etï¿½ete ho volï¿½nï¿½m funkce stackError(SERR_TOP). 
	 **
	 ** Pro ovï¿½ï¿½enï¿½, zda je zï¿½sobnï¿½k prï¿½zdnï¿½, pouï¿½ijte dï¿½ï¿½ve definovanou
	 ** funkci stackEmpty.
	 */

	if (stackEmpty(s)) {
        /* Zásobník je prázdný */
        stackError(SERR_TOP);
    } else {
        /* Jako hodnota c se nastaví hodnota z pole arr indexovaného promìnnou top*/
        *c = s->arr[s->top];
    }
}

void stackPop(tStack* s)
{
	/*   --------
	 ** Odstranï¿½ prvek z vrcholu zï¿½sobnï¿½ku. Pro ovï¿½ï¿½enï¿½, zda je zï¿½sobnï¿½k prï¿½zdnï¿½,
	 ** pouï¿½ijte dï¿½ï¿½ve definovanou funkci stackEmpty.
	 **
	 ** Vyvolï¿½nï¿½ operace Pop pï¿½i prï¿½zdnï¿½m zï¿½sobnï¿½ku je sice podezï¿½elï¿½ a mï¿½e
	 ** signalizovat chybu v algoritmu, ve kterï¿½m je zï¿½sobnï¿½k pouï¿½it, ale funkci
	 ** pro oï¿½etï¿½enï¿½ chyby zde nevolejte (mï¿½eme zï¿½sobnï¿½k ponechat prï¿½zdnï¿½).
	 ** Spï¿½e neï¿½ volï¿½nï¿½ chyby by se zde hodilo vypsat varovï¿½nï¿½, coï¿½ vï¿½ak pro
	 ** jednoduchost nedï¿½lï¿½me.
	 ** 
	 */

	if (!stackEmpty(s)) {
        /* Sní¾ení ukazatele do pole (zásobníku) */
        s->top--;
    }
}

void stackPush(tStack* s, char c)
{
	/*   ---------
	 ** Vloï¿½ï¿½ znak na vrchol zï¿½sobnï¿½ku. Pokus o vloï¿½enï¿½ prvku do plnï¿½ho zï¿½sobnï¿½ku
	 ** je nekorektnï¿½ a oï¿½etï¿½ete ho volï¿½nï¿½m procedury stackError(SERR_PUSH).
	 **
	 ** Pro ovï¿½ï¿½enï¿½, zda je zï¿½sobnï¿½k plnï¿½, pouï¿½ijte dï¿½ï¿½ve definovanou
	 ** funkci stackFull.
	 */

	if (stackFull(s)) {
        /* Zásobník je plný -> nelze vkládat dal¹í prvky */
        stackError(SERR_PUSH);
    } else {
        /* Zvý¹ení ukazatele do pole (zásobníku) + ulo¾ení hodnoty na tuto pozici */
        s->top++;
        s->arr[s->top] = c;
    }
}

/* Konec c202.c */
