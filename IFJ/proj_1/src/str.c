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

#include <string.h>
#include <malloc.h>

#include "str.h"
#include "scanner.h"

/*
 * udává počet bytů počáteční alokace paměti
 * realokace probíhá na násobky této konstanty
 */
#define STR_LEN_INC 8

/**
 * \brief Vytvoří nový řetězec
 * 
 * \param s[out] string řetězec
 * \return ERR_OK pokud je vše v pořádku
 */
int strInit(string *s)
{
	if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL) {
		return ERR_INTERNAL;
	}
	s->str[0] = '\0';
	s->length = 0;
	s->allocSize = STR_LEN_INC;

	return ERR_OK;
}

/**
 * \brief Uvolní řetězec z paměti
 * 
 * \param[out] s uvolňovaný string řetězec
 */
void strFree(string *s)
{
	free(s->str);
}

/**
 * \brief Vymaže obsah řetězce
 * 
 * \param[out] s string řetězec
 */
void strClear(string *s)
{
	s->str[0] = '\0';
	s->length = 0;
}

/**
 * \brief Přidá na konec řetězce jeden znak
 * 
 * Pokud je to nutné, naalokuje větší prostor pro řetězec
 * 
 * \param[out] s1 cílový string řetězec
 * \param[in] c zdrojový znak
 * \return ERR_OK pokud je vše v pořádku
 */
int strAddChar(string *s1, char c)
{
	if (s1->length + 1 >= s1->allocSize) {
		/* pamet nestaci, je potreba provest realokaci */
		if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL) {
			return ERR_INTERNAL;
		}
		s1->allocSize = s1->length + STR_LEN_INC;
	}
	s1->str[s1->length] = c;
	s1->length++;
	s1->str[s1->length] = '\0';

	return ERR_OK;
}

int strAddChars(string *s1, const char *c)
{
	int len = strlen(c);
	if (s1->length + len >= s1->allocSize) {
		if ((s1->str = (char*) realloc(s1->str, s1->length + len + 1)) == NULL) {
			return ERR_INTERNAL;
		}
		s1->allocSize = s1->length + len;
	}

	strncpy((s1->str + s1->length), c, len);
	s1->length += len;
	s1->str[s1->length] = '\0';

	return ERR_OK;
}

/**
 * \brief Překopíruje řetězec s2 do s1
 * 
 * \param[out] s1 řetězec do kterého se bude kopírovat
 * \param[in] s2 zdrojový řetězec
 * \return ERR_OK pokud je vše v pořádku
 */
int strCopyString(string *s1, string *s2)
{
	int newLength = s2->length;
	if (newLength >= s1->allocSize) {
		// pamet nestaci, je potreba provest realokaci
		if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL) {
			return ERR_INTERNAL;
		}
		s1->allocSize = newLength + 1;
	}
	strncpy(s1->str, s2->str, newLength);
	s1->str[newLength] = '\0';
	s1->length = newLength;

	return ERR_OK;
}

/** 
 * \brief Porovná oba řetězce a vrátí výsledek porovnání
 * 
 * \param[in] s1 string řetězec
 * \param[in] s2 string řetězec
 * \return výsledek porovnání
 */
int strCmpString(const string *s1, const string *s2)
{
	return strcmp(s1->str, s2->str);
}

/**
 * \brief Porovná řetězec s konstantním řetězcem
 * 
 * \param[in] s1 string řetězec
 * \param[in] s2 char * řetězec
 * \return výsledek porovnání
 */
int strCmpConstStr(const string *s1, const char *s2)
{
	return strcmp(s1->str, s2);
}

/**
 * \brief Vrátí textovou část řetězce
 * 
 * \param[in] s string řetězec
 * \return textovou část řetězce
 */
char *strGetStr(string *s)
{
	return s->str;
}

/**
 * \brief Vrátí délku zadaného řetězce
 * 
 * \param[in] s string řetězec
 * \return délka řetězce
 */
int strGetLength(const string *s)
{
	return s->length;
}

/* */

/** \brief Inicializuje a zkopíruje obsah řetězce
 * Varování - pokud byl cílový řetězec již inicializovaný, dojde k úniku paměti!
 *
 * \param[out] dst Cílový řetězec
 * \param[in] src Zdrojový řetězec
 * \return ERR_OK pokud je vše v pořádku
 */
int strDuplicate(string *dst, const string *src)
{
	if ((dst->str = (char*) malloc(src->allocSize)) == NULL) {
		return ERR_INTERNAL;
	}

	strncpy(dst->str, src->str, src->length + 1);
	dst->length = src->length;
	dst->allocSize = src->allocSize;

	return ERR_OK;
}