/* 
 * Project:  Implemente interpretu imperatívnío jazyka IFJ13
 * File:  common.h
 * Authors: 
 *	    Michal Kozubík,		xkozub03
 *
 * Created on 11.10.2013
 */

#ifndef COMMON_H
#define	COMMON_H

/* includy jsou dole - pralo se to pri prekladu */

/* textova reprezentace pro debug vypisy */
extern const char *tok_str[];	/* typ tokenu */
extern const char *op_str[];	/* typ operatoru */
extern const char *key_str[];	/* klicove slovo */
extern const char *bool_str[];	/* bool hodnota */
extern const char *b_str[];		/* zavorka */
extern const char *ins_str[];	/* instrukce */

/* Chybové kódy, které bude program vracet dle zadání */
typedef enum errors_e {
	/* Vše v pořádku */
	ERR_OK,
	/*
	 * 1: Chyba v programu v rámci lexikální analýzy 
	 *    (chybná struktura aktuálního lexému).
	 */
	ERR_LEXAN,
	/*
	 * 2: Chyba v programu v rámci syntaktické analýzy
	 *    (chybná syntaxe struktury programu).  
	 */
	ERR_SYNAN,
	/*        
	 * 3: Sémantická chyba v programu – nedefinovaná funkce, pokus o redefinici
	 *    funkce.
	 */
	ERR_SEMAN_UNDEF_REDEF,
	/*
	 * 4: Sémantická/běhová chyba v programu – chybějící parametr při volání
	 *    funkce.
	 */
	ERR_SEMAN_MISSING_PARAM,
	/*
	 * 5: Sémantická/běhová chyba v programu – nedeklarovaná proměnná.
	 */
	ERR_SEMAN_UNDECL,
	/*
	 * 10: Sémantická/běhová chyba dělení nulou.
	 */
	ERR_SEMAN_DIV_ZERO = 10,
	/*
	 * 11: Sémantická/běhová chyba při přetypování na číslo (funkce doubleval).
	 */
	ERR_SEMAN_DOUBLEVAL,
	/*
	 * 12: Sémantická/běhová chyba typové kompatibility v aritmetických a 
	 *     relačních výrazech.
	 */
	ERR_SEMAN_COMPATIBILITY,
	/* 
	 * 13: Ostatní sémantické/běhové chyby.
	 */
	ERR_SEMAN_OTHER,
	/* 
	 * 99: Interní chyba interpretu tj. neovlivněná vstupním programem 
	 *     (např. chyba alokace paměti atd.).
	 */
	ERR_INTERNAL = 99
} errors;

struct htable_t;
struct token_s;
struct tree_node_t;
struct tlist_t;

#include "scanner.h"
#include "ial.h"
#include "tree.h"

void print_error_msg(const char *format, ...);
void print_expected(const char *what, const char *after, struct token_s *token);
void print_unexpected(struct token_s *token);
void print_parenth(const char *what);
void print_func_def();
void print_miss_br();
void print_many_br();
void print_few_args(char *name, int given, int required);
void print_few_args_before(char *name, int before, int defined);
void print_redef(char *name);
char *print_token(struct token_s *token);
char *print_elem(struct htable_elem_t *elem);
void print_instruction(int type, int label, struct htable_elem_t *op1, struct htable_elem_t *op2, struct htable_elem_t *result);

void print_table(struct htable_t *table, int indent);
void print_tree(struct tree_node_t *tree, int spaces);
void print_list(struct tlist_t *list);



#endif	/* COMMON_H */

