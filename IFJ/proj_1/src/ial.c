/*
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File: ial.c
 * Authors: 
 *		Michal Kozubík,		xkozub03
 *		Zuzana Lietavcova,	xlieta00
 *	    Marek Hurta,		xhurta01
 * 
 * Created on: 1.10.2013
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "ial.h"
#include "common.h"
#include "builtin.h"
#include "str.h"

/* Makre pro vypocet index rodice a potomku */
#define PARENT(n) ((n - 1) / 2)
#define LEFT(n)   ((n * 2) + 1)
#define RIGHT(n)  ((n * 2) + 2)

#define UNSET (-2)

/* Indexy korenovoho prvku a jeho potomku */
enum {
	ROOT = 0,
	FIRST_LEFT,
	FIRST_RIGHT
};

/**
 *  \brief Prohozeni hodnot dvou ukazatelu
 *
 *  \param[in,out] first
 *  \param[in,out] second
 */
static inline void char_switch(char *first, char *second)
{
	char tmp = *first;
	*first = *second;
	*second = tmp;
}

/**
 *  \brief Razeni stringu pomoci heap sortu
 *
 * \param[in] f Vstupni fronta parametru
 * \param[out] result vystupni serazeny retezec
 * @return chybovy kod
 */
errors sort_string(arg_list_t *f, string *result)
{
	errors ret_val;
	uint32_t tmp, child;
	string str_tmp;

	strInit(&str_tmp);

	ret_val = strval(f, &str_tmp);
	CHECK_AND_FREE(ret_val, &str_tmp);

	if (str_tmp.length == 0) {
		strFree(&str_tmp);
		return ERR_OK;
	}

	char *buff = malloc(str_tmp.length + 1);
	if (buff == NULL) {
		fprintf(stderr, "Error: sort_string: malloc() failed!\n");
		strFree(&str_tmp);
		return ERR_INTERNAL;
	}

	/* Vytvoreni lokalni kopie stringu */
	strncpy(buff, str_tmp.str, str_tmp.length + 1);

	/* Vytvoreni haldy preorganizovanim pole - je to rychlejsi nez vytvareni 
	 * binarniho stromu. Pokud je prvek vetsi nez jeho rodic, jsou prohozeny
	 */
	for (uint32_t i = 1; i < str_tmp.length; i++) {
		tmp = PARENT(i);
		while ((i > 0) && (buff[i] > buff[tmp])) {
			char_switch(buff + i, buff + tmp);
			i = tmp;
			tmp = PARENT(i);
		}
	}

	/* Serazeni haldy - nejvetsi prvek je vzdy na vrcholu (ROOT)
	 * Tento prvek se prohodi s poslednim prvek, ktery se tak stane ROOTem a je
	 * potreba jej dostat na spravne misto ve stromu (znovu sestavit haldu)
	 * Pokud je prvek mensi nez oba jeho potomci, bude prohozen s vetsin z nich.
	 */
	for (uint32_t i = str_tmp.length - 1; i > 0; i--) {
		if (buff[ROOT] == buff[i]) {
			continue;
		}
		char_switch(buff, buff + i);

		/* Nastaveni pocatecnich indexu rodice a potomka */
		tmp = ROOT;
		child = FIRST_LEFT;

		while (child < i) {
			if ((child < i - 1) && (buff[child] < buff[child + 1])) {
				/* Pravy potomek je vetsi nez levy */
				child++;
			}

			/* Prohozeni rodice s adekvatnim potomkem */
			if ((buff[tmp] < buff[child])) {
				char_switch(buff + tmp, buff + child);
				tmp = child;
				child = LEFT(tmp);
			} else {
				break;
			}
		}
	}

	strAddChars(result, buff);
	free(buff);
	strFree(&str_tmp);
	return ERR_OK;
}

/**
 *  \brief Alokuje a inicializuje hashovaciu tabulku
 *
 * \param[in] size velkost tabulky
 * @return pointer na vytvorenu tabulku alebo NULL v pripade zlyhania
 */
htable *hash_init(unsigned int size)
{
	htable *t;
	if ((t = malloc(sizeof (htable) + size * sizeof (htable_elem *))) == NULL) {
		fprintf(stderr, "Error: hash_init: malloc() failde!\n");
		return NULL;
	}

	t->size = size;
	for (int i = 0; i < size; i++) {
		t->ptr[i] = NULL;
	}
	return t;
}

/**
 *  \brief Vyhlada polozku podla retazca(string) v hashovacej tabulke
 *
 * \param[in] t ukazatel na tabulku
 * \param[in] key kluc hladanej polozky
 * @return pointer na najdenu polozku alebo NULL
 */
htable_elem *hash_lookup(htable *t, string *key)
{
	if (t == NULL) {
		return NULL;
	}
	unsigned int index = hash_function(key->str, t->size);

	htable_elem *item = t->ptr[index];

	while (item != NULL && strcmp(item->name.str, key->str)) {
		item = item->next;
	}
	return item;
}

/**
 *  \brief Vyhlada polozku podla retazca(char *) v hashovacej tabulke
 *
 * \param[in] t ukazatel na hashovaciu tabulku
 * \param[in] key kluc polozky
 * @return pointer na najdenu polozku alebo NULL
 */
htable_elem *hash_lookup_char(htable *t, char *key)
{
	if (t == NULL) {
		return NULL;
	}
	unsigned int index = hash_function(key, t->size);

	htable_elem *item = t->ptr[index];

	while (item != NULL && strcmp(item->name.str, key)) {
		item = item->next;
	}
	return item;
}

/**
 *  \brief Vlozi novu polozku na zaciatok zoznamu
 *
 * \param[in] t ukazatel na hashovaciu tabulku
 * \param[in] name kluc polozky
 * @return pointer na polozku alebo NULL
 */
htable_elem *hash_insert(htable *t, string *name)
{
	unsigned int index = hash_function(name->str, t->size);
	htable_elem *item;

	/* Alokacia miesta pre polozku */
	if ((item = malloc(sizeof (htable_elem))) == NULL) {
		fprintf(stderr, "Error: hash_insert: malloc() failed!\n");
		return NULL;
	}

	/* Inicializacia */
	strInit(&(item->name));
	strCopyString(&(item->name), name);
	item->is_defined = FALSE;
	item->num_of_params = UNSET;
	item->type = T_UNSET;
	item->subtype = T_UNSET;
	item->val_type = T_UNSET;
	item->table = NULL;
	item->next = t->ptr[index];
	/* Vlozenie polozky do zoznamu */
	t->ptr[index] = item;

	return item;
}

/**
 *  \brief Vlozi novu polozku na zaciatok zoznamu
 *
 * \param[in] t ukazatel na hashovaciu tabulku
 * \param[in] name kluc polozky
 * @return pointer na polozku alebo NULL
 */
htable_elem *hash_insert_char(htable *t, char *name)
{
	unsigned int index = hash_function(name, t->size);
	htable_elem *item;

	/* Alokacia miesta pre polozku */
	if ((item = malloc(sizeof (htable_elem))) == NULL) {
		fprintf(stderr, "Error: hash_insert: malloc() failed!\n");
		return NULL;
	}

	/* Inicializacia */
	strInit(&(item->name));
	strAddChars(&(item->name), name);
	item->is_defined = FALSE;
	item->num_of_params = UNSET;
	item->type = T_UNSET;
	item->subtype = T_UNSET;
	item->val_type = T_UNSET;
	item->table = NULL;
	item->next = t->ptr[index];
	/* Vlozenie polozky do zoznamu */
	t->ptr[index] = item;

	return item;
}

/**
 *  \brief Zrusi tabulku, vsetky zoznamy a polozky v nich
 *
 * \param[in] t ukazatel na hashovaciu tabulku
 */
void hash_free(htable *t)
{
	if (t == NULL) {
		return;
	}
	int size = t->size;
	htable_elem *tmp = NULL;

	for (int i = 0; i < size; i++) {
		tmp = t->ptr[i];
		/* Rusenie poloziek v zozname */
		while (tmp != NULL) {
			t->ptr[i] = tmp->next;

			if (tmp->type == T_FUNCTION) {
				hash_free(tmp->table);
			}

			if (tmp->val_type == T_STRING) {
				strFree(&(tmp->value.str_val));
			}

			strFree(&(tmp->name));
			free(tmp);
			tmp = t->ptr[i];
		}
	}

	free(t);
}

/**
 *  \brief Hashovacia funkcia
 *
 * \param[in] str kluc polozky
 * \param[in] htable_size velkost hashovacej tabulky
 * @return index do hashovacej tabulky
 */
unsigned int hash_function(const char *str, unsigned htable_size)
{
	unsigned int h = 0;
	unsigned char *p;

	for (p = (unsigned char*) str; *p != '\0'; p++) {
		h = 31 * h + *p;
	}
	return h % htable_size;
}

/**
 * \brief Funkce vyhleda podretezec v retezci
 * 
 * \param[in] f fronta argumentu
 * \param[out] result pozice podretezce
 * @return chybovy kod
 */
errors find_string(arg_list_t *f, int *result)
{
	errors ret_val;
	/* Dlzky vstupnych retazcov */
	int src_len;
	int word_len;
	/* Indexy do retazcov */
	int word_index = 0;
	int src_index = 0;
	int *table;
	int length = 0;
	int i = 1;

	string src, word;
	strInit(&src);
	strInit(&word);

	ret_val = strval(f, &src);

	if (ret_val != ERR_OK) {
		strFree(&src);
		strFree(&word);
		return ret_val;
	}
	/* arg_list_succ(f) zavoval predchozi strval */
	ret_val = strval(f, &word);

	if (ret_val != ERR_OK) {
		strFree(&src);
		strFree(&word);
		return ret_val;
	}

	src_len = src.length;
	word_len = word.length;

	if (word_len == 0) {
		*result = 0;
		strFree(&src);
		strFree(&word);
		return ERR_OK;
	}

	/* Vytvorenie fail vektora */
	if ((table = (int *) malloc(sizeof (int) * word_len)) == NULL) {
		strFree(&src);
		strFree(&word);
		return ERR_INTERNAL;
	}

	table[0] = 0;

	/* Vypocet fail vektora */
	while (i < word_len) {
		if (word.str[i] == word.str[length]) {
			length++;
			table[i] = length;
			i++;
		} else {
			if (length != 0) {
				length = table[length - 1];
			} else {
				table[i] = 0;
				i++;
			}
		}
	}

	/* Cyklus kde sa postupne porovnava zdrojovy retazec s hladanym slovom */
	while (src_index < src_len) {
		if (src.str[src_index] == word.str[word_index]) {
			src_index++;
			word_index++;
		}
		/* Ak dojde k zhode, vrati index kde zhoda zacina  */
		if (word_index == word_len) {
			strFree(&src);
			strFree(&word);
			free(table);
			*result = (src_index - word_index);
			return ERR_OK;
		} else if (word.str[word_index] != src.str[src_index]) {
			if (word_index != 0) {
				word_index = table[word_index - 1];
			} else {
				src_index++;
			}
		}
	}

	*result = -1;

	free(table);
	strFree(&src);
	strFree(&word);
	return ERR_OK;
}
