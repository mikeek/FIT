/*
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File: interpret.c
 * Authors: 
 *		Lukáš Horký,		xhorky21
 *		Lukáš Huták,		xhutak01
 * 
 * Created on: 11.10.2013
 */

#include <stdio.h>
#include <stdlib.h>

#include "interpret.h"
#include "str.h"
#include "generator.h"
#include "scanner.h" 	// boolovské hodnoty TRUE, FALSE
#include "builtin.h"

extern tinst_item **lab_table;

/*
 * Struktura pro uchování další instrukce, která se zavolá po návratu z funkce.
 * Samotný funkce, jenž volání vyvolala je zde také uchován pro uložení
 * návratové hodnoty do těla funkce (do jejího ID)
 */
typedef struct instruct_ptr_s {
	tinst_item *instr; /* další instrukce, která se má vykonat po návratu z funkce*/
	fce_item *func_ptr; /* funkce, do níž se uloží návratová hodnota */
	struct instruct_ptr_s *next; /* další prvek zásobníku */
} instruct_ptr_t;

/*
 * Zásobník obsahující informace o volání funkcí
 * Na vrcholu zásobníku jsou informace o naposledy volané funkci
 */
typedef struct pointer_stack_s {
	instruct_ptr_t *top;
} pointer_stack_t;

/* globální seznam seznamů instrukcí */
extern fce_lists *i_lists;

//-------------------------- ARITMETIKA A OPERÁTOŘI ----------------------------

/** \brief Ověření že hodnoty se rovnají I_EQ (===) 
 *
 * \param[in] first Ukazatel na první operand
 * \param[in] second Ukazatel na druhý operand
 * \return False pokud jsou operandy jiného typu, jinak výsledek porovnání
 */
bool_value compare_EQ(const htable_elem *first, const htable_elem *second)
{

	if (first->val_type != second->val_type)
		return FALSE;

	if (first->val_type == T_BOOLEAN &&
		first->value.bool_val == second->value.bool_val)
		return TRUE;

	if (first->val_type == T_INTEGER &&
		first->value.int_val == second->value.int_val)
		return TRUE;

	if (first->val_type == T_DOUBLE &&
		first->value.double_val == second->value.double_val)
		return TRUE;

	if (first->val_type == T_STRING &&
		strCmpString(&(first->value.str_val), &(second->value.str_val)) == 0)
		return TRUE;

	if (first->val_type == T_NULL)
		return TRUE;

	return FALSE;
}

/** \brief Zpracování operátoru porovnání I_LE (<)
 * Určí pravdivost, že první položka je menší než druhá 
 *
 * \param[in] first Ukazatel na první operand
 * \param[in] second Ukazatel na druhý operand
 * \return Boolovská hodnota potvrzující/vyvracející vztah
 */
bool_value compare_LE(const htable_elem *first, const htable_elem *second)
{

	if (first->val_type == T_BOOLEAN &&
		first->value.bool_val < second->value.bool_val)
		return TRUE;

	if (first->val_type == T_INTEGER &&
		first->value.int_val < second->value.int_val)
		return TRUE;

	if (first->val_type == T_DOUBLE &&
		first->value.double_val < second->value.double_val)
		return TRUE;

	if (first->val_type == T_STRING &&
		strCmpString(&(first->value.str_val), &(second->value.str_val)) < 0)
		return TRUE;

	return FALSE;
}

/** \brief Zpracování operátorů porovnání I_GR (>), I_GRQ(>=), I_LE(<), I_LEQ(<=)
 * Na základě typu operátoru a hodnot operandů vrací boolovskou hodnotu
 *
 * \param[in] first Ukazatel na první operand
 * \param[in] second Ukazatel na druhý operand
 * \param[in] op Typ operátoru
 * \return Boolovská hodnota potvrzující/vyvracející vztah dle užitého operátoru
 */
bool_value compare_GR_GRQ_LE_LEQ(const htable_elem *first, const htable_elem
	*second, instructions op)
{
	bool_value negate = FALSE;
	if (op == I_GR) {
		op = I_LEQ;
		negate = TRUE;
	}
	if (op == I_GRQ) {
		op = I_LE;
		negate = TRUE;
	}

	bool_value result = compare_LE(first, second);
	if (result == FALSE && op == I_LEQ) {
		result = compare_EQ(first, second);
	}

	if (negate == TRUE) result = NOT(result);

	return result;
}

/** \brief Výpočet aritmetických operací
 * Zkontroluje zda nad typy operantů lze prováď aritmetické operace a určí typ
 * výsledné hodnoty. Následně vypočítá a uloží výslednou hodnotu
 *
 * Přípusné aritmetické operace: I_ADD, I_SUB, I_MUL, I_DIV
 *
 * \param[out] result Ukazatel na položku, do které se uloží výsledek
 * \param[in]  first Ukazetel na první operand
 * \param[in]  second Ukazatel na druhý operand
 * \param[in]  op Typ aritmetické operace
 * \return     Pokud vše proběhlo správně vrací ERR_OK, jinak typ chyby
 */
errors arithm(htable_elem *result, htable_elem *first, htable_elem *second,
	instructions op)
{
	/* určení výsledného typu */
	if (first->val_type == T_DOUBLE || second->val_type == T_DOUBLE) {
		if ((first->val_type == second->val_type) ||
			(first->val_type == T_INTEGER || second->val_type == T_INTEGER)) {
			// operandy jsou oba typu double nebo jeden typu double a druhy int
			result->val_type = T_DOUBLE;
		} else {
			/* druhy operand není není integer */
			return ERR_SEMAN_COMPATIBILITY;
		}
	} else if (first->val_type == T_INTEGER && second->val_type == T_INTEGER) {
		/* oba parametry jsou typu integer */
		if (op != I_DIV) {
			result->val_type = T_INTEGER;
		} else {
			/* při dělení je vždy výsledek double */
			result->val_type = T_DOUBLE;
		}
	} else {
		/* operandy nejsou ani typu integer ani typu double */
		return ERR_SEMAN_COMPATIBILITY;
	}

	/* provedení příslušné aritmetické operace */
	switch (op) {
	case I_ADD:
		if (result->val_type == T_DOUBLE) {
			CHANGE_VAL_TYPE(result, T_DOUBLE);
			result->value.double_val = (double) ARIT_VALUE(first) +
				(double) ARIT_VALUE(second);
		} else {
			CHANGE_VAL_TYPE(result, T_INTEGER);
			result->value.int_val = first->value.int_val + second->value.int_val;
		}
		break;
	case I_SUB:
		if (result->val_type == T_DOUBLE) {
			CHANGE_VAL_TYPE(result, T_DOUBLE);
			result->value.double_val = (double) ARIT_VALUE(first) -
				(double) ARIT_VALUE(second);
		} else {
			CHANGE_VAL_TYPE(result, T_INTEGER);
			result->value.int_val = first->value.int_val - second->value.int_val;
		}
		break;
	case I_MUL:
		if (result->val_type == T_DOUBLE) {
			CHANGE_VAL_TYPE(result, T_DOUBLE);
			result->value.double_val = (double) ARIT_VALUE(first) *
				(double) ARIT_VALUE(second);
		} else {
			CHANGE_VAL_TYPE(result, T_INTEGER);
			result->value.int_val = first->value.int_val * second->value.int_val;
		}
		break;
	case I_DIV:
		if ((double) ARIT_VALUE(second) == 0.0) {
			return ERR_SEMAN_DIV_ZERO;
		}

		CHANGE_VAL_TYPE(result, T_DOUBLE);
		result->value.double_val = (double) ARIT_VALUE(first) /
			(double) ARIT_VALUE(second);
		break;
	default:
		return ERR_INTERNAL; // nikdy by neměla správně nastat
		break;
	}

	return ERR_OK;
}

//------------------ MANIPULACE SE ZÁLOHAMI TABULEK SYMBOLŮ --------------------

/** \brief Kontrola nepřítomnosti žádné zálohy tabulky symbolů funkce
 * Zkontroluje, zda existuje nějaká záloha k dané tabulce symbolů
 *
 * \param[in] function Ukazatel na položku funkce
 * \return Pokud neexistuje záloha TRUE, jinak FALSE
 */
bool_value backup_empty(const fce_item *function)
{
	if (function->backup.last == NULL) {
		return TRUE;
	}
	return FALSE;
}

/** \brief Tvorba nové zálohy tabulky symbolů funkce
 * Vytváří vždy kopii aktuální tabulky symbolů a uloží ji na konec seznamu 
 * záloh, které si uchovává každá funkce zvlášť.
 * 
 * \param[in,out] function Ukazatel na položku funkce, která se bude zálohovat
 * \return Provedla-li se záloha vrací ERR_OK, jinak kód chyby
 */
errors backup_create_new(fce_item *function)
{
	htable * table_original = function->id->table;
	htable_backup_t * table_backup = NULL;
	if ((table_backup = malloc(sizeof (htable_backup_t) +
		(table_original->size) * sizeof (htable_elem_backup_t *))) == NULL) {
		return ERR_INTERNAL;
	}
	table_backup->size = table_original->size;
	table_backup->next = NULL;

	/* alokace tabulky dokončena, začátek kopírování položek */
	for (int i = 0; i < table_original->size; i++) {
		htable_elem * pointer = table_original->ptr[i];
		htable_elem_backup_t * last_elem = NULL;
		table_backup->ptr[i] = NULL;
		while (pointer != NULL) {
			htable_elem_backup_t * el;
			if ((el = malloc(sizeof (htable_elem_backup_t))) == NULL) {
				return ERR_INTERNAL;
			} else {
				/* položka naalokována, kopirovaní hodnot */
				el->val_type = pointer->val_type;
				if (el->val_type != T_STRING) {
					el->value = pointer->value;
				} else {
					string val;
					if (strInit(&val) != ERR_OK) {
						return ERR_INTERNAL;
					}
					if (strCopyString(&val, &pointer->value.str_val) != ERR_OK) {
						return ERR_INTERNAL;
					}
					el->value.str_val = val;
				}
				el->is_defined = pointer->is_defined;
				el->next = NULL;

				if (last_elem != NULL) {
					last_elem->next = el;
				} else {
					table_backup->ptr[i] = el;
				}
				last_elem = el;
				pointer = pointer->next;
			}
		}
	}

	/* přidání na seznam záloh */
	if (backup_empty(function)) {
		table_backup->prev = NULL;
		function->backup.first = table_backup;
		function->backup.last = table_backup;
	} else {
		table_backup->prev = function->backup.last;
		function->backup.last->next = table_backup;
		function->backup.last = table_backup;
	}

	return ERR_OK;
}

/** \brief Nahrazení aktuální tabulky symbolů (TS) funkce dřívější zálohou
 * Funkce očekává, že záloha určitě existuje. 
 *
 * \param[in,out] function Ukazatel na funkci, jejiž TS se obnoví ze zálohy
 * \param[in] from_first Pokud je FALSE obnoví se z poslední zálohy. Jinak se 
 * 		obnoví z první existující zálohy.
 * \return Provede-li se obnova ERR_OK, jinak kód chyby
 */
errors backup_apply(fce_item *function, bool_value from_first)
{
	htable * table_original = function->id->table;
	htable_backup_t * table_backup = function->backup.first;
	if (from_first == FALSE) {
		table_backup = function->backup.last;
	}
	for (int i = 0; i < table_original->size; i++) {
		htable_elem * pointer_orig = table_original->ptr[i];
		htable_elem_backup_t * pointer_back = table_backup->ptr[i];
		while (pointer_orig != NULL) {
			CHANGE_VAL_TYPE(pointer_orig, pointer_back->val_type);
			if (pointer_back->val_type == T_STRING) {
				if (strCopyString(&pointer_orig->value.str_val, &pointer_back->value.str_val)) {
					return ERR_INTERNAL;
				}
			} else {
				pointer_orig->value = pointer_back->value;
			}
			pointer_orig->is_defined = pointer_back->is_defined;

			pointer_orig = pointer_orig->next;
			pointer_back = pointer_back->next;
		}
	}

	return ERR_OK; // K chybe muze dojit v makru CHANGE_VAL_TYPE
}

/** \brief Odstraní poslední zálohu
 * Funkce očekává, že alespoň jedna záloha existuje. Provede uvolnění všech 
 * položek zálohy a upraví seznam záloh.
 *
 * \param[in,out] function Ukazatel na funci z niž se odebere poslední záloha
 */
void backup_delete_last(fce_item *function)
{
	htable_backup_t * table_backup = function->backup.last;
	for (int i = 0; i < table_backup->size; i++) {
		htable_elem_backup_t * pointer = table_backup->ptr[i];
		while (pointer != NULL) {
			if (pointer->val_type == T_STRING) {
				strFree(&pointer->value.str_val);
			}
			htable_elem_backup_t * next = pointer->next;
			free(pointer);
			pointer = next;
		}
	}
	function->backup.last = table_backup->prev;
	if (function->backup.last == NULL) {
		function->backup.first = NULL;
	}
	free(table_backup);
}

//------------------------------------ JINÉ ------------------------------------

/** \brief Volání vnitřních funkcí 
 *
 * \param[in,out] oper Ukazatel na funkci v tabulce symbolů
 * \param[in] que Seznam s parametry funkcí
 */
errors inner_functions(htable_elem * oper, arg_list_t * list)
{
	errors r = ERR_OK;
	switch (oper->subtype) {
	case KEY_BOOLVAL:
		CHANGE_VAL_TYPE(oper, T_BOOLEAN);
		r = boolval(list, &oper->value.bool_val);
		break;
	case KEY_DOUBLEVAL:
		CHANGE_VAL_TYPE(oper, T_DOUBLE);
		r = doubleval(list, &oper->value.double_val);
		break;
	case KEY_INTVAL:
		CHANGE_VAL_TYPE(oper, T_INTEGER);
		r = intval(list, &oper->value.int_val);
		break;
	case KEY_STRVAL:
		CHANGE_VAL_TYPE(oper, T_STRING);
		r = strval(list, &oper->value.str_val);
		break;
	case KEY_GET_STRING:
		CHANGE_VAL_TYPE(oper, T_STRING);
		r = get_string(&oper->value.str_val);
		break;
	case KEY_PUT_STRING:
		CHANGE_VAL_TYPE(oper, T_INTEGER);
		r = put_string(list, &oper->value.int_val);
		break;
	case KEY_STRLEN:
		CHANGE_VAL_TYPE(oper, T_INTEGER);
		r = str_len(list, &oper->value.int_val);
		break;
	case KEY_GET_SUBSTRING:
		CHANGE_VAL_TYPE(oper, T_STRING);
		r = get_substring(list, &oper->value.str_val);
		break;
	case KEY_FIND_STRING:
		CHANGE_VAL_TYPE(oper, T_INTEGER);
		r = find_string(list, &oper->value.int_val);
		break;
	case KEY_SORT_STRING:
		CHANGE_VAL_TYPE(oper, T_STRING);
		r = sort_string(list, &oper->value.str_val);
		break;
	default:
		fprintf(stderr, "DOSUD NEIMPLEMENTOVANÁ VNITŘNÍ FUNKCE BYLA ZAVOLÁNA\n");
		break;
	}

	/* pročištění seznamu parametrů po poslední zarážku*/
	arg_list_clear(list, TRUE);

	return r;
}

//-------------------------- ZÁSOBNÍK VOLÁNÍ FUNKCÍ ----------------------------

/** \brief Inicializace zásobníku obsahujícího informace o volaných funkcich
 * \param[out] stack Zásobník
 */
void instr_stack_init(pointer_stack_t *stack)
{
	stack->top = NULL;
}

/** \brief Vložení položky na vrchol zásobníku s informacemi o volaných funkcích
 * \param[in,out] stack Zásobník na jehož vrchol se bude ukládat
 * \param[in] instr Ukazatel instrukce, která se má jako další po návratu z funkce vykonat
 * \param[in] fce_ptr Pointer na funkci, která byla volána
 * \return Chybový kód, ERR_OK = žádná chyba
 */
errors instr_stack_push(pointer_stack_t *stack, tinst_item *instr,
	fce_item *fce_ptr)
{
	instruct_ptr_t *temp;
	if ((temp = malloc(sizeof (instruct_ptr_t))) == NULL) {
		return ERR_INTERNAL;
	}

	temp->instr = instr;
	temp->func_ptr = fce_ptr;
	temp->next = stack->top;
	stack->top = temp;

	return ERR_OK;
}

/** \brief Odebrání položky z vrcholu zásobníku s informacemi o volaných funkcích
 * Pokud se na vrcholu zásobníku nic nenachází, nic se neprovede.
 * \param[in,out] stack Zásobník, ze kterého se odebírá
 */
void instr_stack_pop(pointer_stack_t *stack)
{
	if (stack->top != NULL) {
		instruct_ptr_t *temp = stack->top;
		stack->top = stack->top->next;
		free(temp);
	}
}

/* Makro pro přístup na vrchol zásobníku s informacemi o volaných funkcích */
#define INSTR_TOP(stack) (stack).top

/** \brief Test prázdnost zásobníku s informacemi o volaných funkcích
 * \param[out] stack Zásobník, který se ověřuje
 * \return Pokud je prázdný vrací TRUE, jinak FALSE
 */
bool_value instr_stack_empty(const pointer_stack_t *stack)
{
	return stack->top == NULL ? TRUE : FALSE;
}

//--------------------------------- INTERPRET ----------------------------------

/** \brief Spustí interpretaci instrukcí
 * Využívá globální proměnnou i_lists obsahující seznam seznamů instrukcí
 *
 * \return Pokud se program bezchybně vykonal vrací ERR_OK, jinak typ chyby
 */
errors interpret_run()
{
	/* Nastavení první instrukce */
	fce_item *fce = i_lists->first;
	tinst_item *instr = fce->list->first;

	int stop = 0;
	int ret_value = ERR_OK;
	bool_value jump = FALSE;

	pointer_stack_t stack;
	instr_stack_init(&stack);

	// TODO: Nemeli by funkce pro frontu vracet errors?
	arg_list_t func_arg_list;
	arg_list_init(&func_arg_list);
	htable_elem *pop_temp = NULL; // Pouziva se pri POPovani z fronty
	bool_value bool_temp = FALSE;

	/* Zahájení interpretace */
	while (!stop) {
		switch (instr->type) {
		case I_ASGN:
			/* Přiřazení |  op2 = op1 */
			CHECK_FIRST_OP_VALIDITY(stop, ret_value);
			CHANGE_VAL_TYPE(instr->op2, instr->op1->val_type);
			if (instr->op2->val_type == T_STRING) {
				ret_value = strCopyString(&instr->op2->value.str_val, &instr->op1->value.str_val);
				CHECK_ERR_VAL(ret_value, "Runtime error: not enough memory");
			} else {
				instr->op2->value = instr->op1->value;
			}
			DEFINE_RESULT(instr->op2);
			break;

		case I_ADD:
		case I_SUB:
		case I_MUL:
		case I_DIV:
			/* Sčítání, odečítání, násobení, dělení |  result = op1 (+|-|*|/) op2 */
			CHECK_BOTH_OP_VALIDITY(stop, ret_value);
			ret_value = arithm(instr->result, instr->op1, instr->op2, instr->type);
			CHECK_ERR_VAL(ret_value, "Runtime error: arithmetic operation failed");
			DEFINE_RESULT(instr->result);
			break;

		case I_CON:
			/* Konkatenace dvou řetězců |  result = op1 . op2 */
			CHECK_BOTH_OP_VALIDITY(stop, ret_value);

			if (instr->op1->val_type != T_STRING) {
				fprintf(stderr, "Runtime error: concatenation: the left operand is not a string\n");
				return ERR_SEMAN_COMPATIBILITY;
			}

			CHANGE_VAL_TYPE(instr->result, T_STRING);
			ret_value = strCopyString(&instr->result->value.str_val, &instr->op1->value.str_val);
			CHECK_ERR_VAL(ret_value, "Runtime error: not enough memory");

			string secondString;
			ret_value = strInit(&secondString);
			CHECK_ERR_VAL(ret_value, "Runtime error: not enough memory");
			ret_value = strval_internal(instr->op2, &secondString);
			CHECK_ERR_VAL_ADV(ret_value, "Runtime error: type conversion failed", strFree(&secondString));
			ret_value = strAddChars(&instr->result->value.str_val, secondString.str);
			CHECK_ERR_VAL_ADV(ret_value, "Runtime error: not enough memory", strFree(&secondString));
			strFree(&secondString);

			DEFINE_RESULT(instr->result);
			break;

		case I_EQ:
			/* Test ekvivalence |  result = (op1 === op2) */
			CHECK_BOTH_OP_VALIDITY(stop, ret_value);
			CHANGE_VAL_TYPE(instr->result, T_BOOLEAN);
			instr->result->value.bool_val = compare_EQ(instr->op1, instr->op2);
			DEFINE_RESULT(instr->result);
			break;

		case I_NEQ:
			/* Test non-ekvivalence |  result = (op1 !== op2) */
			CHECK_BOTH_OP_VALIDITY(stop, ret_value);
			CHANGE_VAL_TYPE(instr->result, T_BOOLEAN);
			instr->result->value.bool_val = NOT(compare_EQ(instr->op1, instr->op2));
			DEFINE_RESULT(instr->result);
			break;

		case I_LE:
		case I_GR:
		case I_LEQ:
		case I_GRQ:
			/* Operator porovnání |  result = op1 (<|>|<=|>=) op2 */
			CHECK_BOTH_OP_VALIDITY(stop, ret_value);
			if (instr->op1->val_type != instr->op2->val_type) {
				fprintf(stderr, "Runtime error: comparing elements of different types\n");
				return ERR_SEMAN_COMPATIBILITY;
			}

			CHANGE_VAL_TYPE(instr->result, T_BOOLEAN);
			instr->result->value.bool_val = compare_GR_GRQ_LE_LEQ(instr->op1,
				instr->op2, instr->type);
			DEFINE_RESULT(instr->result);
			break;

		case I_JMP:
			/* Nepodmíněný skok na lokální instrukci */
			instr = lab_table[instr->label.label];
			//			instr = *(instr->label.addr);
			jump = TRUE;
			break;

		case I_TJMP:
		case I_FJMP:
			/* Podmíněný skok na lokální instrukci */
			CHECK_FIRST_OP_VALIDITY(stop, ret_value);
			ret_value = boolval_internal(instr->op1, &bool_temp);
			CHECK_ERR_VAL(ret_value, "Runtime error: conversion error");

			if (bool_temp == (instr->type == I_TJMP) ? TRUE : FALSE) {
				instr = lab_table[instr->label.label];
			} else {
				instr = instr->next;
			}
			jump = TRUE;
			break;

		case I_LBL:
		case I_FCELBL:
			/* Návěstí - prázdná instrukce */
			;
			break;

		case I_FCEJMP:
			/* Skok do funkce */
			/* Nastavení aktuální položky v seznamu argumentů na první */
			arg_list_first(&func_arg_list);

			/* Ověření, zda se nejedná o vestavěnou funkci. */
			if (instr->op1->subtype >= KEY_BOOLVAL) {
				ret_value = inner_functions(instr->op1, &func_arg_list);
				CHECK_ERR_VAL(ret_value, "Runtime error: builtin function failed!");
				break;
			}

			/* uložení následující instrukce a ukazatele na volanou funkci na zásobník */
			ret_value = instr_stack_push(&stack, instr->next, instr->label.fce_item);
			CHECK_ERR_VAL(ret_value, "Runtime error: stack failed");

			/* vytvoření zálohy tabulky a případná obnova výchozích hodnot */
			if (backup_empty(instr->label.fce_item) == TRUE) {
				ret_value = backup_create_new(instr->label.fce_item);
				CHECK_ERR_VAL(ret_value, "Runtime error: backup failed");
			} else {
				ret_value = backup_create_new(instr->label.fce_item);
				CHECK_ERR_VAL(ret_value, "Runtime error: backup failed");
				ret_value = backup_apply(instr->label.fce_item, TRUE);
				CHECK_ERR_VAL(ret_value, "Runtime error: backup application failed");
			}

			/* nastavení nové instrukce */
			instr = instr->label.fce_item->list->first;
			jump = TRUE;
			break;

		case I_RET:
		case I_BACK:
			/* Skok z funkce bez/s návratové hodnoty */
			/* nastavení návratové hodnoty */
			if (instr->type == I_RET) {
				if (instr_stack_empty(&stack) == TRUE) {
					/* volání return na konci programu */
					stop = 1;
					break;
				}

				/* nastaví se výstupní hodnota udaná dle I_RET */
				CHANGE_VAL_TYPE(INSTR_TOP(stack)->func_ptr->id,
					instr->op1->val_type);

				if (INSTR_TOP(stack)->func_ptr->id->val_type == T_STRING) {
					ret_value = strCopyString(&(INSTR_TOP(stack)->func_ptr->id->value.str_val),
						&instr->op1->value.str_val);
					CHECK_ERR_VAL(ret_value, "Runtime error: not enough memory");
				} else {
					INSTR_TOP(stack)->func_ptr->id->value = instr->op1->value;
				}
			} else {
				/* nastaví se null */
				INSTR_TOP(stack)->func_ptr->id->val_type = T_NULL;
			}

			/* návratová hodnota je definována */
			DEFINE_RESULT(INSTR_TOP(stack)->func_ptr->id);

			/* obnovení původní podoby tabulky */
			ret_value = backup_apply(INSTR_TOP(stack)->func_ptr, FALSE);
			CHECK_ERR_VAL(ret_value, "Runtime error: backup application failed");
			backup_delete_last(INSTR_TOP(stack)->func_ptr);

			/* nastavení nové instrukce */
			instr = INSTR_TOP(stack)->instr;
			jump = TRUE;
			instr_stack_pop(&stack);
			break;

		case I_PUSH:
			/* Pushovani argumentu funkci do fronty */
			CHECK_FIRST_OP_VALIDITY(stop, ret_value);
			ret_value = arg_list_insert_last(&func_arg_list, instr->op1);
			CHECK_ERR_VAL(ret_value, "Runtime error: arguments stack failed");
			break;

		case I_POP:
			/* Pushovani argumentu funkci do fronty */
			pop_temp = arg_list_active(&func_arg_list);
			instr->op1->is_defined = pop_temp->is_defined;
			CHANGE_VAL_TYPE(instr->op1, pop_temp->val_type);
			if (instr->op1->val_type == T_STRING) {
				ret_value = strCopyString(&instr->op1->value.str_val, &pop_temp->value.str_val);
				CHECK_ERR_VAL(ret_value, "Runtime error: not enough memory");
			} else {
				instr->op1->value = pop_temp->value;
			}
			arg_list_succ(&func_arg_list);
			break;

		case I_END:
			/* Konec programu */
			stop = 1;
			break;
		case I_CALL:
			/* Začíná vkládání parametrů funkce, která se bude volat */
			arg_list_set_break(&func_arg_list);
			break;
		default:
			// fprintf(stderr, "NEZNÁMÁ INSTRUKCE - STOP \n");
			stop = 1;
			break;
		}

		// nastavení další instrukce
		if (jump == FALSE) {
			if (instr->type == I_POP && instr->next->type != I_POP) {
				arg_list_clear(&func_arg_list, TRUE);
			}
			instr = instr->next;
		} else {
			jump = FALSE;
		}
	}

	/* Konec interpretace -> provedení úklidu */
	arg_list_clear(&func_arg_list, FALSE);

	/* Čistění seznamu volaných funkcí a jejich záloh tabulek symbolů funkcí
	 * v případě, že nastala chyba.
	 */
	while (instr_stack_empty(&stack) != TRUE) {
		while (backup_empty(INSTR_TOP(stack)->func_ptr) != TRUE) {
			backup_delete_last(INSTR_TOP(stack)->func_ptr);
		}
		instr_stack_pop(&stack);
	}

	return ret_value;
}