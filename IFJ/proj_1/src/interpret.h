/*
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File: interpret.h
 * Authors: 
 *		Lukáš Horký,		xhorky21
 *		Lukáš Huták,		xhutak01
 * 
 * Created on: 11.10.2013
 */


#ifndef INTERPRET_H
#define	INTERPRET_H

#include "common.h"

/* Struktura pro uchování zálohy jedné položky v tabulce symbolů funkce. Není 
 * třeba uchovávat všechny položky. Zálohují se jen položky nutné pro interpret.
 */
typedef struct htable_elem_backup_s {
	int val_type;
	value_t value;
	bool_value is_defined;
	struct htable_elem_backup_s *next;
} htable_elem_backup_t;

/* Položka v obousměrném seznamu záloh funkce. Obsahuje zálohovanou tabulku. */
typedef struct htable_backup_s {
	int size; /* Velikost zálohované tabulky */
	struct htable_backup_s *next;
	struct htable_backup_s *prev;
	htable_elem_backup_t *ptr[]; /* Zálohovaná tabulka */
} htable_backup_t;

/* Struktura seznamu záloh jedné funkce */
typedef struct backup_s {
	htable_backup_t *first;
	htable_backup_t *last;
} backup_t;

/* Spuštení interpretace funkcí */
errors interpret_run();


/* Zkontroluje, že operátor je definovaný */
#define CHECK_FIRST_OP_VALIDITY(stop, ret) \
if (instr->op1->is_defined != TRUE) \
{ \
	fprintf(stderr, "Runtime error: undefined value!\n"); \
	stop = 1; \
	ret = ERR_SEMAN_UNDECL; \
	break; \
}

/* Zkontroluje, že oba operátoři jsou definovaní */
#define CHECK_BOTH_OP_VALIDITY(stop, ret) \
if (instr->op1->is_defined != TRUE || instr->op2->is_defined != TRUE) \
{ \
	fprintf(stderr, "Runtime error: undefined value!\n"); \
	stop = 1; \
	ret = ERR_SEMAN_UNDECL; \
	break; \
}

/* Nastav výstupní hodnotu jako definovanou */
#define DEFINE_RESULT(result) result->is_defined = TRUE

/* Negace Boolovské hodnoty */
#define NOT(boolvalue) (1 - (boolvalue))

/* Get value of node */
#define ARIT_VALUE(node) (\
(node->val_type == T_INTEGER)?(node->value.int_val):(node->value.double_val))

/* Prenastaveni typu hodnoty elementu */
#define CHANGE_VAL_TYPE(element, newtype) \
do { \
	if (element->val_type == T_STRING && newtype == T_STRING) { \
		strClear(&element->value.str_val); \
	} else { \
		if (element->val_type == T_STRING) { \
			strFree(&element->value.str_val); \
		} \
		if (newtype == T_STRING) { \
			if (strInit(&element->value.str_val) != ERR_OK) \
				return ERR_INTERNAL; \
		} \
	} \
	element->val_type = newtype; \
} while(0)

/* Kontrola chybových zpráv */
#define CHECK_ERR_VAL(value, message) \
do { \
	if (value != ERR_OK) { \
		fprintf(stderr, message"\n"); \
		stop = 1; \
		break; \
	} \
} while(0)

#define CHECK_ERR_VAL_ADV(value, message, operation) \
do { \
	if (value != ERR_OK) { \
		fprintf(stderr, message"\n"); \
		stop = 1; \
		operation; \
		break; \
	} \
} while(0)


#endif	/* INTERPRET_H */

