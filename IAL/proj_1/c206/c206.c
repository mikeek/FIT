	
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                             Úpravy: Bohuslav Køena, øíjen 2013
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu, 
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem, 
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu, 
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam 
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* This program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
    /* Inicializace seznamu - v¹echny ukazatele nastaveny na NULL */
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L)	{
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free. 
**/
    tDLElemPtr tmp;
    while (L->First != NULL) {
        /* Ulo¾ení ukazatele na prvek, který má být smazán */
        tmp = L->First;
        /* Pøesun k dal¹ímu prvku */
        L->First = L->First->rptr;
        /* Uvolnìní starého prvku */
        free(tmp);
    }
    /* Nastavení v¹ech ukazatelù seznamu na NULL (jako pøi inicializaci) */
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr tmp;
	if ((tmp = malloc(sizeof(struct tDLElem))) == NULL) {
        /* Chyba pøi alokaci pamìti */
        DLError();
    } else {
        /* Vlo¾ení dat do nového prvku */
        tmp->data = val;
        /* Jeliko¾ je první, jeho levý ukazatel je NULL */
        tmp->lptr = NULL;
        /* a pravý ukazuje na aktuální First prvek */
        tmp->rptr = L->First;
        
        if (L->First != NULL) {
            /* 
             * Seznam není prazdný - aktuální First prvek bude mít v levém ukazateli 
             * odkaz na novì vkládaný prvek
             */
            L->First->lptr = tmp;
        } else {
            /* Seznam je prázdný - Last bude odkazovat na novì vkládaný prvek */
            L->Last = tmp;
        }
        /* Novì vytvoøený prvek se øadí na 1. místo v seznamu */
        L->First = tmp;
    }
}

void DLInsertLast(tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr tmp;
    if ((tmp = malloc(sizeof(struct tDLElem))) == NULL) {
        DLError();
    } else {
        /* Vlo¾ení dat do nového prvku */
        tmp->data = val;
        /* Jeliko¾ je poslední, jeho pravý ukazatel je NULL */
        tmp->rptr = NULL;
        /* a levý ukazuje na aktuální Last prvek */
        tmp->lptr = L->Last;
        
        if (L->Last != NULL) {
            /*
             * Seznam není prázdný - aktuální Last prvek bude mít v pravém ukazateli
             * odkaz na novì vkládaný prvek
             */
            L->Last->rptr = tmp;
        } else {
            /* Seznam je prázdný - First bude odkazovat na novì vkládaný prvek */
            L->First = tmp;
        }
        /* Novì vytvoøený prvek se øadí na 1. místo v seznamu */
        L->Last = tmp;
    }
}

void DLFirst (tDLList *L)	{
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
    /* Nastavení aktivity na první prvek */
	L->Act = L->First;
}

void DLLast (tDLList *L)	{
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
    /* Nastavení aktivity na poslední prvek */
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL) {
        /* Prázdný seznam */
        DLError();
    } else {
        /* Naètení hodnoty ze seznamu do promìnné */
        *val = L->First->data;
    }
}

void DLCopyLast (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if (L->Last == NULL) {
        /* Prázdný seznam */
        DLError();
    } else {
        /* Naètení hodnoty ze seznamu do promìnné */
        *val = L->Last->data;
    }
}

void DLDeleteFirst (tDLList *L)	{
/*
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
    /* Kontrola prázdného seznamu */
	if (L->First != NULL) {
        if (L->Act == L->First) {
            /* Ztráta aktivity, pokud je aktivní prvek prvním prvkem */
            L->Act = NULL;
        }
        tDLElemPtr tmp;
        /* Ulo¾ení ukazatele na starý prvek */
        tmp = L->First;
        /* Pøesun ukazatele First na následující prvek */
        L->First = L->First->rptr;
        if (L->First == NULL) {
            /* Pokud je seznam prázdný, je tøeba znullovat i ukazatel na Last */
            L->Last = NULL;
        } else {
            /* Pokud prázdný není, musíme znullovat levý ukazatel nového First prvku */
            L->First->lptr = NULL;
        }
        /* Uvolnìní starého prvku */
        free(tmp);
    }
}	

void DLDeleteLast (tDLList *L)	{
/*
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/ 
    /* Kontrola prázdného seznamu */
	if (L->Last != NULL) {
        if (L->Act == L->Last) {
            /* Ztráta aktivity, pokud je aktivní prvek posledním prvkem */
            L->Act = NULL;
        }
        tDLElemPtr tmp;
        /* Ulo¾ení ukazatele na starý prvek */
        tmp = L->Last;
        /* Pøesun ukazatele Last na pøedchozí prvek */
        L->Last = L->Last->lptr;
        if (L->Last == NULL) {
            /* Pokud je seznam prázdný, je tøeba znullovat i ukazatel na First */
            L->First = NULL;
        } else {
            /* Pokud prázdný není, musíme znullovat pravý ukazatel nového Last prvku */
            L->Last->rptr = NULL;
        }
        /* Uvolnìní starého prvku */
        free(tmp);
    }
}

void DLPostDelete (tDLList *L)	{
/*
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
**/
    /* Kontrola prázdnosti seznamu a jestli není aktivní prvek posledním */
	if ((L->Act != NULL) && (L->Act != L->Last)) {
        tDLElemPtr tmp;
        /* Ulo¾ení ukazatele na starý prvek */
        tmp = L->Act->rptr;
        if (tmp != NULL) {
            /* Nastavení nového ukazatele aktivního prvku */
            L->Act->rptr = tmp->rptr;
            if (tmp == L->Last) {
                /* Pokud je starý prvek posledním, je tøeba zmìnit Last ukazatel na Act */
                L->Last = L->Act;
            } else {
                /* Pokud není, je tøeba zmìnit levý ukazatel jeho následníka na Act */
                tmp->rptr->lptr = L->Act;
            }
            /* Uvolnìní starého prvku */
            free(tmp);
        }
    }
}

void DLPreDelete (tDLList *L)	{
/*
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
**/
    /* Kontrola prázdnosti seznamu a jestli není aktivní prvek prvním */
	if ((L->Act != NULL) && (L->Act != L->First)) {
        tDLElemPtr tmp;
        /* Ulo¾ení ukazatele na starý prvek */
        tmp = L->Act->lptr;
        if (tmp != NULL) {
            /* Nastavení nového ukazatele aktivního prvku */
            L->Act->lptr = tmp->lptr;
            if (tmp == L->First) {
                /* Pokud je starý prvek prvním, je tøeba zmìnit First ukazatel na Act */
                L->First = L->Act;
            } else {
                /* Pokud není, je tøeba zmìnit pravý ukazatel jeho pøedchùdce na Act */
                tmp->lptr->rptr = L->Act;
            }
            /* Uvolnìní starého prvku */
            free(tmp);
        }
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
    /* Kontrola aktivity seznamu */
	if (L->Act != NULL) {
        tDLElemPtr tmp;
        if ((tmp = malloc(sizeof(struct tDLElem))) == NULL) {
            /* Chyba pøi alokaci pamìti */
            DLError();
        } else {
            /* Ulo¾ení hodnoty do nového prvku */
            tmp->data = val;
            /* Nastavení adekvátních ukazatelù nového prvku */
            tmp->rptr = L->Act->rptr;
            tmp->lptr = L->Act;
            
            /* Nastavení pravého ukazatele aktivního prvku na nový prvek */
            L->Act->rptr = tmp;
            if (L->Act == L->Last) {
                /* 
                 * Pokud je aktivní prvek posledním, je tøeba zmìnit Last
                 * ukazatel na nový prvek (jeliko¾ ten bude nyní posledním)
                 */
                L->Last = tmp;
            } else {
                /* 
                 * Pokud není aktivní prvek posledním, je tøeba zmìnit ukazatel 
                 * následníka nového prvku na tento nový prvek
                 */
                tmp->rptr->lptr = tmp;
            }
        }
    }
}

void DLPreInsert (tDLList *L, int val)		{
/*
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
    /* Kontrola aktivity seznamu */
	if (L->Act != NULL) {
        tDLElemPtr tmp;
        if ((tmp = malloc(sizeof(struct tDLElem))) == NULL) {
            /* Chyba pøi alokaci pamìti */
            DLError();
        } else {
            /* Ulo¾ení hodnoty do nového prvku */
            tmp->data = val;
            /* Nastavení adekvátních ukazatelù nového prvku */
            tmp->lptr = L->Act->lptr;
            tmp->rptr = L->Act;
            
            /* Nastavení levého ukazatele aktivního prvku na nový prvek */
            L->Act->lptr = tmp;
            if (L->Act == L->First) {
                /* 
                 * Pokud je aktivní prvek prvním, je tøeba zmìnit First
                 * ukazatel na nový prvek (jeliko¾ ten bude nyní prvním)
                 */
                L->First = tmp;
            } else {
                /* 
                 * Pokud není aktivní prvek prvním, je tøeba zmìnit ukazatel 
                 * pøedchùdce nového prvku na tento nový prvek
                 */
                tmp->lptr->rptr = tmp;
            }
        }
    }
}

void DLCopy (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
    /* Kontrola aktivity seznamu */
	if (L->Act == NULL) {
        DLError();
    } else {
        /* Ulo¾ení hodnoty aktivního prvku do promìnné val */
        *val = L->Act->data;
    }
}

void DLActualize (tDLList *L, int val) {
/*
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/
    /* Kontrola aktivity seznamu */
	if (L->Act != NULL) {
        /* Ulo¾ení hodnoty promìnné val do aktuálního prvku seznamu */
        L->Act->data = val;
    }
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/
    /* Kontrola aktivity seznamu */
	if (L->Act != NULL) {
        if (L->Act == L->Last) {
            /* Pokud je aktivní prvek posledním, aktivita se ztrácí */
            L->Act = NULL;
        } else {
            /* Aktivita se pøesune na následníka aktivního prvku */
            L->Act = L->Act->rptr;
        }
    }
}


void DLPred (tDLList *L)	{
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/
    /* Kontrola aktivity seznamu */
	if (L->Act != NULL) {
        if (L->Act == L->First) {
            /* Pokud je aktivní prvek prvním, aktivita se ztrácí */
            L->Act = NULL;
        } else {
            /* Aktivita se pøesune na pøedchùdce aktivního prvku */
            L->Act = L->Act->lptr;
        }
    }
}

int DLActive (tDLList *L) {		
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/
    /* Pokud Act == NULL, seznam aktivní není */
    return (L->Act != NULL) ? TRUE : FALSE;
}

/* Konec c206.c*/
