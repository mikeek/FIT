/* 
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File:   str.c
 * Authors:
 *		převzato z ukázkového řešení (jednoduchý interpret) k projektu do
 *		předmětu IFJ
 *
 * Jednoduchá knihovna pro práci s nekonečně dlouhými řetězci
 * 
 * Poznámka: 
 * 	- přidána vlastní funkce strDuplicate
 * 	- opravena chyba u strCopyString (na konci nového řetězce chybělo '\0')
 */

#ifndef STR_H
#define	STR_H

typedef struct {
	char* str; /* misto pro dany retezec ukonceny znakem */
	int length; /* skutecna delka retezce */
	int allocSize; /* velikost alokovane pameti */
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strAddChars(string *s1, const char *c);
int strCopyString(string *s1, string *s2);
int strCmpString(const string *s1, const string *s2);
int strCmpConstStr(const string *s1, const char *s2);

char *strGetStr(string *s);
int strGetLength(const string *s);

/* dodatečná funkce */
int strDuplicate(string *dst, const string *src);


#endif