/*
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File: ial.h
 * Authors: 
 *		Michal Kozubík,		xkozub03
 *		Zuzana Lietavcova,	xlieta00
 *	    Marek Hurta,		xhurta01
 * 
 * Created on: 1.10.2013
 */


#ifndef IAL_H_
#define IAL_H_

#include "scanner.h"
#include "str.h"
#include "tree.h"

/* Struktura pro hodnoty potřebné u funckí v tabulce symbolů */
struct func_s {
	int num_of_params; /* počet parametrů */
	bool_value is_defined; /* už byla definována? */
	tlist *list; /* tělo funkce */
	struct htable_t *table;
};

/* Prvek tabulky symbolů */
typedef struct htable_elem_t {
	string name;		/* název identifikátoru */
	int type;			/* typ tokenu (T_OPERATOR...) */
	int subtype;		/* subtyp tokenu (OP_PLUS...) */
	int val_type;		/* T_INTEGER, ... */
	value_t value;		/* value union */
	int num_of_params;	/* pocet parametru */
	bool_value is_defined;
	struct htable_t *table;
	struct htable_elem_t *next;
} htable_elem;

/* Tabulka symbolů */
typedef struct htable_t {
	int size;
	htable_elem *ptr[];
} htable;

struct arg_list;

/* \brief Heap sort algoritmus 
 *
 * \param[in] f Ukazatel na frontu 
 * \param[out] result zoradeny retazec 
 * \return chybovy kod
 */
errors sort_string(struct arg_list *f, string *result);

/*
 * \brief Funkcia najde zadany retazec v zdrojovom retazci
 * \param[in] f Ukazatel na frontu kde sa nachadza vzor aj zdrojovy retazec
 * \param[out] result Index zaciatku najdeneho retazca
 * \return chybovy kod
 */
errors find_string(struct arg_list *f, int *result);

#include "builtin.h"

/* Hashovací funkce */
unsigned int hash_function(const char *str, unsigned htable_size);
/* Vytvoření + inicializace tabulky symbolů */
htable *hash_init(unsigned int size);
/* Vložení prvku do tabulky symbolů */
htable_elem *hash_insert(htable *t, string *key); /* vlož */
htable_elem *hash_insert_char(htable *t, char *name);
/* Vyhledání prvku v tabulce symbolů */
htable_elem *hash_lookup(htable *t, string *key); /* najdi */
htable_elem *hash_lookup_char(htable *t, char *key);
/* Zrušení tabulky symbolů */
void hash_free(htable *t);




#endif /* IAL_H_ */
