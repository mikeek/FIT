/* 
 * Project:  Implementácia interpretu imperatívneho jazyka IFJ13
 * File:  builtin.c
 * Authors: 
 *		Zuzana Lietavcová,	xlieta00
 *	    Marek Hurta,		xhurta01
 *	    Michal Kozubík,		xkozub03
 *		Lukáš Huták,		xhutak01
 *		Lukáš Horký,		xhorky21
 *
 * Created on 22.11.2013
 */

#define _GNU_SOURCE
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h> //Kvuli isspace a isdigit

#include "builtin.h"
#include "scanner.h"
#include "ial.h"
#include "str.h"
#include "common.h"
#include "interpret.h"

static int macro_var = ERR_OK; // Pomocna promenna pro makro

#define CHECK_ERR_VAL_DBL(value, operation) \
do { \
	macro_var = value; \
    if (macro_var != ERR_OK) { \
        operation; \
        print_error_msg("internal error"); \
        return macro_var; \
    } \
} while(0)

/** \brief Konvertuje hodnotu na pravdivostnu
 *
 *  \param[in] f ukazatel na zoznam parametrov
 *  \param[out] result bool hodnota
 *  \return chybovy kod
 */
errors boolval(arg_list_t *f, bool_value *result)
{
	errors ret_val = boolval_internal(arg_list_active(f), result);
	arg_list_succ(f);

	return ret_val;
}

/** \brief Konvertuje hodnotu na pravdivostnu(pomocna funkcia)
 *
 *  \param[in] elem ukazatel na prvok v tabulke symbolov,
 *	 ktoreho hodnota sa ma previest
 *  \param[out] result konvertovana bool hodnota
 *  \return chybovy kod
 */
errors boolval_internal(const htable_elem *elem, bool_value *result)
{
	switch (elem->val_type) {
	case T_NULL:
		*result = FALSE;
		break;
	case T_BOOLEAN:
		*result = elem->value.bool_val;
		break;
	case T_INTEGER:
		*result = (elem->value.int_val == 0) ? FALSE : TRUE;
		break;
	case T_DOUBLE:
		*result = (elem->value.double_val == 0.0) ? FALSE : TRUE;
		break;
	case T_STRING:
		*result = (elem->value.str_val.length == 0) ? FALSE : TRUE;
		break;
	}
	return ERR_OK;
}

/** \brief Konvertuje hodnotu na desatinne cislo
 *
 *  \param[in] f ukazatel na zoznam parametrov
 *  \param[out] result konvertovane desatinne cislo
 *  \return chybovy kod
 */
errors doubleval(arg_list_t *f, double *result)
{
	htable_elem *elem = arg_list_active(f);

	switch (elem->val_type) {
	case T_NULL:
		*result = 0.0;
		break;
	case T_BOOLEAN:
		*result = (elem->value.bool_val == TRUE) ? (1.0) : (0.0);
		break;
	case T_INTEGER:
		*result = (double) elem->value.int_val;
		break;
	case T_DOUBLE:
		*result = elem->value.double_val;
		break;
	case T_STRING:
	{

		string converted; // TODO: Kontrolovat stringy!
		if (strInit(&converted) != ERR_OK) {
			print_error_msg("internal error");
			return ERR_INTERNAL;
		}

		char chr = '0';
		int state = 0;
		bool_value brk = FALSE;
		for (int i = 0; i < elem->value.str_val.length; i++) {
			chr = elem->value.str_val.str[i];

			switch (state) {
			case 0:
				// Prvni znak musi byt cislo
				if (isspace(chr)) {
					// Ignoruj mezery ze zacatku
					break;
				}
				if (isdigit(chr)) {
					CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
					state = 1;
				} else {
					strAddChar(&converted, '0');
					brk = TRUE; // Konecny stav - nic jsem nenacetl
				}
				break;
			case 1: // Je konecny stav
				// Cast pred . a pred e
				if (isdigit(chr)) {
					CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
				} else {
					if (chr == '.') {
						CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
						state = 2; // Neni konecny stav!
						break;
					}
					if (chr == 'e' || chr == 'E') {
						CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
						state = 3; // Neni konecny stav!
						break;
					}
					brk = TRUE;
				}
				break;
			case 2: // Neni konecny stav
				// Nacetl jsem tecku, ocekavam cislo
				if (isdigit(chr)) {
					CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
					state = 4;
				} else {
					brk = TRUE;
				}
				break;
			case 3: // Neni konecny stav
				// Nacetl jsem e, ocekavam cislo nebo +/-
				if (isdigit(chr)) {
					CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
					state = 5;
				} else {
					if (chr == '-' || chr == '+') {
						CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
						state = 6; // Neni konecny stav!
					} else {
						brk = TRUE;
					}
				}
				break;
			case 4: // Je konecny stav
				// Nacitam desetinou cast
				if (isdigit(chr)) {
					CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
				} else {
					if (chr == 'e' || chr == 'E') {
						CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
						state = 3; // Neni konecny stav!
					} else {
						brk = TRUE;
					}
				}
				break;
			case 5: // Je konecny stav
				// Nacetl jsem exponent, ted uz jen cisla
				if (isdigit(chr)) {
					CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
				} else {
					brk = TRUE;
				}
				break;
			case 6: // Neni konecny stav
				// Nacetl jsem e+ nebo e-, uz musi byt cislo
				// Pozn.: Oproti stavu 5 se NEJEDNA o konecny stav!
				if (isdigit(chr)) {
					CHECK_ERR_VAL_DBL(strAddChar(&converted, chr), strFree(&converted));
					state = 5;
				} else {
					brk = TRUE;
				}

			}
			if (brk == TRUE) {
				break;
			}
		}
		if (state == 2 || state == 3 || state == 6 || converted.length == 0) {
			// Nejsou konecne stavy, chyba!
			fprintf(stderr, "doubleval() failed!\n");
			strFree(&converted);
			return ERR_SEMAN_DOUBLEVAL;
		} else {
			char *end, *start;
			start = converted.str;
			double tmp = strtod(start, &end);
			*result = tmp;
			strFree(&converted);
		}

		/*char *end, *start;
		start = elem->value.str_val.str;
		double tmp = strtod(start, &end);
		if (end == start || *end == 'e' || *(end - 1) == '.') {
			fprintf(stderr, "doubleval() failed!\n");
			return ERR_SEMAN_DOUBLEVAL;
		}
		 *result = tmp;
		break;*/
	}
	}

	arg_list_succ(f);
	return ERR_OK;
}

/** \brief Konvertuje hodnotu na desatinne cislo
 *
 *  \param[in] f ukazatel na prvok v tabulke symbolov
 *  \param[out] result konvertovany retazec
 *  \return chybovy kod
 */
errors intval(arg_list_t *f, int *result)
{
	htable_elem *elem = arg_list_active(f);

	switch (elem->val_type) {
	case T_NULL:
		*result = 0;
		break;
	case T_BOOLEAN:
		*result = (elem->value.bool_val == TRUE) ? 1 : 0;
		break;
	case T_INTEGER:
		*result = elem->value.int_val;
		break;
	case T_DOUBLE:
		*result = (int) elem->value.double_val;
		break;
	case T_STRING:
	{
		char *end, *start;
		start = elem->value.str_val.str;

		/* posun startu na první nebílý znak */
		while (*start != '\0' && isspace(*start) != 0) {
			start++;
		}

		if (isdigit(*start) == 0) {
			/* první nebílý znak není číslo -> tj. výsledkem je 0 
			   (ani znaky + nebo - nejsou přípustné )*/
			*result = 0;
		} else {
			int tmp = strtol(start, &end, 10);
			*result = (end == start) ? (0) : (tmp);
		}
		break;
	}
	}

	arg_list_succ(f);
	return ERR_OK;
}

/** \brief Konvertuje hodnotu na retazec
 *
 *  \param[in] f ukazatel na zoznam parametrov
 *  \param[out] result konvertovany retazec typu string
 *  \return chybovy kod
 */
errors strval(arg_list_t *f, string *result)
{
	errors ret_val = strval_internal(arg_list_active(f), result);
	arg_list_succ(f);

	return ret_val;
}

/** \brief Konvertuje hodnotu na retazec(pomocna funkcia)
 *
 *  \param[in] elem
 *  \param[out] result konvertovane desatinne cislo
 *  \return chybovy kod
 */
errors strval_internal(const htable_elem *elem, string *result)
{
	char *buff = NULL;
	int ret = NOT_OK;
	switch (elem->val_type) {
	case T_NULL:
		ret = asprintf(&buff, "%c", '\0');
		break;
	case T_BOOLEAN:
		if (elem->value.int_val == TRUE) {
			ret = asprintf(&buff, "1");
		} else {
			ret = asprintf(&buff, "%c", '\0');
		}
		break;
	case T_INTEGER:
		ret = asprintf(&buff, "%d", elem->value.int_val);
		break;
	case T_DOUBLE:
		ret = asprintf(&buff, "%g", elem->value.double_val);
		break;
	case T_STRING:
		ret = asprintf(&buff, "%s", elem->value.str_val.str);
		break;
	}
	if (ret == NOT_OK) {
		fprintf(stderr, "strval: asprintf failed!\n");
		return ERR_INTERNAL;
	}

	strAddChars(result, buff);

	free(buff);
	return ERR_OK;
}

/** \brief Zisti dlzku (pocet znakov) retazca
 *
 *  \param[in] f ukazatel na zoznam parametrov
 *  \param[out] result dlzka retazca
 *  \return chybovy kod
 */
errors str_len(arg_list_t *f, int *result)
{
	string buff;
	strInit(&buff);

	errors ret_val = strval_internal(arg_list_active(f), &buff);

	if (ret_val == ERR_OK) {
		*result = buff.length;
	}

	strFree(&buff);
	arg_list_succ(f);
	return ret_val;
}

/** \brief Nacita jeden riadok zo standardneho vstupu a	
 * 	vrati ho ako hodnotu typu string
 *
 *  \param[out] result retazec typu string
 *  \return chybovy kod
 */
errors get_string(string *result)
{
	char *buff = NULL;
	size_t size = 0;

	int ret = getline(&buff, &size, stdin);

	if (ret == NOT_OK) {
		fprintf(stderr, "get_string: getline() failed!\n");
		return ERR_INTERNAL;
	}
	if (buff[ret - 1] == '\n' || buff[ret - 1] == EOF) {
		buff[ret - 1] = '\0';
	}

	strAddChars(result, buff);
	free(buff);

	return ERR_OK;
}

/** \brief Vypise hodnoty parametrov na standardny vystup
 *
 *  \param[in] f ukazatel na zoznam parametrov
 *  \param[out] pocet obdrzanych/vypisanych parametrov
 *  \return chybovy kod
 */
errors put_string(arg_list_t *f, int *result)
{
	*result = 0;
	htable_elem *elem;
	errors ret_val = ERR_OK;
	string tmp;
	strInit(&tmp);

	/* Postupne tlaci jednotlive parametre */
	while ((elem = arg_list_active(f)) != NULL) {
		if (elem->val_type != T_STRING) {
			strClear(&tmp);

			ret_val = strval_internal(elem, &tmp);
			CHECK_AND_FREE(ret_val, &tmp);

			printf("%s", tmp.str);
		} else {
			printf("%s", elem->value.str_val.str);
		}
		arg_list_succ(f);
		(*result)++;
	}

	strFree(&tmp);
	return ret_val;
}

/** \brief Vrati podretazec zadaneho retazca, urceny zaciatocnym a koncovym indexom
 *
 *  \param[in] f ukazatel na zoznam parametrov
 *  \param[out] result podretazec
 *  \return chybovy kod
 */
errors get_substring(arg_list_t *f, string *result)
{
	errors ret_val;
	string source;
	int start = -1;
	int end = -1;
	strInit(&source);

	ret_val = strval(f, &source);
	CHECK_AND_FREE(ret_val, &source);
	/* arg_list_succ(f) zavolal strval */
	ret_val = intval(f, &start);
	CHECK_AND_FREE(ret_val, &source);
	/* arg_list_succ(f) zavolal int */
	ret_val = intval(f, &end);
	CHECK_AND_FREE(ret_val, &source);

	if (start < 0 || end < 0 || start > end || start >= source.length || end > source.length) {
		strFree(&source);
		return ERR_SEMAN_OTHER;
	}

	while (start < end) {
		strAddChar(result, source.str[start++]);
	}

	strFree(&source);
	return ERR_OK;
}

//-----------------------------------------------------------------------------
// Implementace seznamu parametrů

/** \brief Inicializace seznamu argumentů
 *  \param[out] f Ukazatel na seznam argumentů k inicializaci
 */
void arg_list_init(arg_list_t *f)
{
	f->begin = NULL;
	f->end = NULL;
	f->active = NULL;
}

/** \brief Vložení kopie elementu na konec seznamu argumentů (pro I_PUSH)
 * \param[in,out] f Ukazatel na seznam argumentů, do něhož se kopie uloží
 * \param[in] elem Ukazatel na element, který se zkopíruje
 * \return Povede-li se vrací ERR_OK, jinak kód chyby
 */
errors arg_list_insert_last(arg_list_t *f, htable_elem *elem)
{
	/* alokace položky seznamu */
	list_item_t *item;
	if ((item = malloc(sizeof (list_item_t))) == NULL) {
		return ERR_INTERNAL;
	}

	/* alokace elementu */
	if ((item->elem = malloc(sizeof (htable_elem))) == NULL) {
		free(item);
		return ERR_INTERNAL;
	}

	/* kopie elementu */
	item->elem->val_type = T_UNSET;
	CHANGE_VAL_TYPE(item->elem, elem->val_type);
	item->elem->is_defined = elem->is_defined;
	if (item->elem->val_type == T_STRING) {
		strCopyString(&item->elem->value.str_val, &elem->value.str_val);
	} else {
		item->elem->value = elem->value;
	}

	/* vložený prvek není zarážkový */
	item->list_break = 0;

	/* napojední do seznamu argumentů */
	item->prev = f->end;
	item->next = NULL;
	if (f->end == NULL) {
		f->begin = item;
	} else {
		f->end->next = item;
	}

	f->end = item;
	return ERR_OK;
}

/** \brief Hodnota aktivní položky seznamu argumentů
 * \param[in] f Seznam argumentů s aktivní položkou
 * \return Vrací odkaz element aktivní položky, pokud není aktivní vrací NULL
 */
htable_elem *arg_list_active(arg_list_t *f)
{
	return f->active != NULL ? f->active->elem : NULL;
}

/** \brief Nastavení aktivní položky seznamu argumentů na následníka
 * \param[in,out] f Seznam argumentů
 */
void arg_list_succ(arg_list_t *f)
{
	if (f->active == NULL) {
		return;
	}

	if (f->active != f->end) {
		f->active = f->active->next;
	} else {
		f->active = NULL;
	}
}

/** \brief Nastavení aktivní položky seznamu argumentů na první položku (za zarážku)
 * Najde první prvek seznamu argumentů, který se nachází za poslední zarážkou
 * \param[in,out] f Seznam argumentů
 */
void arg_list_first(arg_list_t *f)
{
	if (f->end == NULL) {
		f->active = NULL;
		return;
	}

	f->active = f->end;
	while (f->active->list_break == 0 && f->active->prev != NULL) {
		f->active = f->active->prev;
	}

	if (f->active->list_break > 0) {
		f->active = f->active->next;
	}
}

/** \brief Vložení zarážky oddělující seznamy argumentů různých funkcí
 * Vloží zarážku za poslední prvek v seznamu. Pokud neexistuje žádný prvek,
 * žádná zarážka se nevloží.
 * Příklad problému, kde se užívá: $a = x(5, x(6, 7)); 	(x je uživ. funkce)
 * \param[in,out] f Seznam argumentu, kam se má zařážku umístnit
 */
void arg_list_set_break(arg_list_t *f)
{
	if (f->end != NULL) {
		f->end->list_break++;
	}
}

/** \brief Vymazání část/celý seznamu argumentů 
 * Pokud je druhým parametrem nastaveno mazání do poslední zařážky (TRUE), jsou 
 * ostraněny položky od konce až po nalezení zarážky nebo smazání všech položek.
 * Je-li druhý parametr jiný, maže se vše bez ohledu na zarážky.
 * \param[in,out] f Seznam argumentů, který se (částečně) smaže
 * \param[in] to_last_break Určení pokud se má mazat
 */
void arg_list_clear(arg_list_t *f, bool_value to_last_break)
{
	while (f->end != NULL) {
		list_item_t *item = f->end;

		if (to_last_break == TRUE && item->list_break > 0) {
			/* položka na konci seznamu je zarážka -> odebrat zarážku a konec */
			item->list_break--;
			break;
		}

		if (f->begin == f->end) {
			/* poslední prvek je zároveň první */
			f->begin = NULL;
			f->end = NULL;
			f->active = NULL;
		} else {
			/* vyřadí se poslední položka v seznamu */
			if (f->end == f->active) {
				f->active = NULL;
			}
			f->end = item->prev;
			f->end->next = NULL;
		}

		/* uvolnění paměti */
		if (item->elem->val_type == T_STRING) {
			strFree(&item->elem->value.str_val);
		}

		free(item->elem);
		free(item);
	}
}
