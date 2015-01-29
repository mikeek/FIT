/* 
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File:   parser.h
 * Authors:
 *		Michal Kozubík,		xkozub03
 *
 * Created on 11.10.2013
 */

#ifndef PARSER_H
#define	PARSER_H

#include "scanner.h"

/* Get token and check error code */
#define GET_TOKEN() if ((ret_val = get_token(&token)) != ERR_OK) return ret_val

/* Break if ret_val is not ERR_OK */
#define CHECK_BREAK() if (ret_val != ERR_OK) break

/* Return if ret_val is not ERR_OK */
#define CHECK_RETURN() if (ret_val != ERR_OK) return ret_val

#define CHECK_RETURN_FREE(str_to_free) \
do {\
	if (ret_val != ERR_OK) {\
			strFree(str_to_free);\
			return NULL;\
	}\
} while (0)

/* If ret_val is not OK, set more to FALSE and break */
#define CHECK_MORE_BREAK(more) if ((ret_val != ERR_OK) && (more = FALSE) == FALSE) break 

/* If poiniter is NULL, return ERR_INTERNAL */
#define CHECK_ERR_INTERNAL(pointer) if (pointer == NULL) return ERR_INTERNAL

/* If pointer is NULL, set ret_val to ERR_INTERNAL, more to FALSE and break */
#define CHECK_ERR_INTERNAL_BREAK(pointer, more) \
if (pointer == NULL && (ret_val = ERR_INTERNAL) == ERR_INTERNAL && (more = FALSE) == FALSE) break

/* Check types for arithmetic operations */
#define CHECK_COMPATIBILITY(left, right, operation) \
do { \
	if ((left->type != T_INTEGER && left->type != T_DOUBLE) || \
		(right->type != T_INTEGER && right->type != T_DOUBLE)) { \
			print_error_msg("incompatible types for %s!", operation); \
			return ERR_SEMAN_COMPATIBILITY; \
	} \
} while(0)

/* Check token type and print error message if it is not what we expected */
#define CHECK_TYPE(token_type, expect, after) \
do { \
	if (token.type != token_type) { \
		print_expected(expect, after, &token); \
		return ERR_SYNAN; \
	} \
} while (0)

/* Check type and subtype, print error message if it is not what we expected */
#define CHECK_TYPE_VAL(token_type, token_subtype, expect, after) \
do { \
	if ((token.type != token_type) || (token.value.int_val != token_subtype)) { \
		print_expected(expect, after, &token); \
		return ERR_SYNAN; \
	} \
} while (0)

#define CHECK_PARENTH(what) \
do {\
	if (last.type != T_BRACKET || last.subtype != B_RIGHT_PAR) {\
		print_parenth(what);\
		return ERR_SYNAN;\
	}\
} while (0)

/* Get value of node */
#define VALUE(node) (\
(node->type == T_INTEGER)?(node->value.int_val):(node->value.double_val))

/* Generate new label */
#define GENERATE_LABEL(new_lbl) (new_lbl = gen_lbl++)

/* Generate new element in actual symbol table */
#define GENERATE_ELEM(elem) \
	snprintf(buff, BUFF_SIZE, "@%d", gen_var++); \
	elem = hash_insert_char(act_table, buff)

/* velikost tabulky symbolů */
#define SYM_TABLE_SIZE 30
#define BUFF_SIZE 255
#define STACK_SIZE 20
#define STACK_EMPTY (-1)
#define UNSET (-2)
#define TABLE_DIMENSION 16
#define ASSIGN_ELEM "@="
#define COMMA_ELEM "@,"
#define LEFT_PAR_ELEM "@("
#define MINUS_ONE_ELEM "@m1"

#define ID "identifier"
#define EXPR "expression"
#define IF_EXPR "if expression"
#define WHILE_EXPR "while expression"

errors init_parser();
void free_all_parser();
errors parse();
errors parse_program();
errors parse_st_list();
errors parse_stat();
errors parse_if();
errors parse_while();
errors parse_return();
errors parse_function();
errors parse_item();
errors parse_item_list();
errors parse_expr(tree_node *act_node);
errors parse_expr_list(tree_node *act_node, int *local_params);
errors parse_op();

typedef struct last_s {
	tokens type;
	int subtype;
} last_t;

typedef enum {
	GLOBAL,
	LOCAL
} scope;

typedef enum table_item_e {
	TI_UMINUS,
	TI_MULTIPLY,
	TI_DIVIDE,
	TI_PLUS,
	TI_MINUS,
	TI_CONCAT,
	TI_LESS,
	TI_LESS_EQ,
	TI_GREATER,
	TI_GREATER_EQ,
	TI_TRIPPLE_EQ,
	TI_NOT_EQ,
	TI_ID,
	TI_NONE,
	TI_LEFT,
	TI_RIGHT,
	TI_FUNCTION,
	TI_UNSET,
} table_item;

typedef enum table_value_e {
	ER,
	LE,
	GR,
	EQ
} table_value;

#endif	/* PARSER_H */

