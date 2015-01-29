/* 
 * Project:  Implementácia interpretu imperatívneho jazyka IFJ13
 * File:  builtin.h
 * Authors: 
 *		Zuzana Lietavcová,	xlieta00
 *	    Marek Hurta,		xhurta01
 *	    Michal Kozubík,		xkozub03
 *		Lukáš Huták,		xhutak01
 *		Lukáš Horký,		xhorky21
 *
 * Created on 22.11.2013
 */

#ifndef BUILTIN_H
#define	BUILTIN_H

#include "common.h"

#define NOT_OK (-1)

#define CHECK_AND_FREE(retval, str) \
do {\
	if (retval != ERR_OK) {\
		strFree(str);\
		return retval;\
	} \
} while (0)

typedef struct list_item {
	htable_elem *elem;
	unsigned int list_break; /* zarážka odděluje seznam argumentů pro funkce, */
	struct list_item *next; /*  které volají v seznamu argumentů jiné funkce */
	struct list_item *prev; /*  např.: $a = x(5,x(6,7))*/
} list_item_t;

typedef struct arg_list {
	list_item_t *begin;
	list_item_t *end;
	list_item_t *active;
} arg_list_t;

//-------------------------- SEZNAM S ARGUMENTY FUNKCÍ -------------------------
/* inicializace seznamu argumentů */
void arg_list_init(arg_list_t *f);

/* vložení kopie elementu na konec seznamu */
errors arg_list_insert_last(arg_list_t *f, htable_elem *elem);

/* hodnota aktivní položky */
htable_elem *arg_list_active(arg_list_t *f);

/* aktivita se přesuna na následníka */
void arg_list_succ(arg_list_t *f);

/* aktivita se nastaví na první prvek (za zarážkou)*/
void arg_list_first(arg_list_t *f);

/* vložení zarážky za poslední položku */
void arg_list_set_break(arg_list_t *f);

/* smazání části/celého seznamu argumentů */
void arg_list_clear(arg_list_t *f, bool_value to_last_break);

//----------------------------- VESTAVĚNÉ FUNKCE ------------------------------
errors boolval(arg_list_t *f, bool_value *result);
errors boolval_internal(const htable_elem *elem, bool_value *result);
errors intval(arg_list_t *f, int *result);
errors doubleval(arg_list_t *f, double *result);
errors strval(arg_list_t *f, string *result);
errors strval_internal(const htable_elem *elem, string *result);
errors get_string(string *result);
errors put_string(arg_list_t *f, int *result);
errors str_len(arg_list_t *f, int *result);
errors get_substring(arg_list_t *f, string *result);
errors find_string_cover(arg_list_t *f, int *result);
errors sort_string_cover(arg_list_t *f, string *result);

#endif	/* BUILTIN_H */

