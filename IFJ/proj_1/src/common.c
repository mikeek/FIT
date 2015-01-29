/* 
 * Project:  Implemente interpretu imperatívnío jazyka IFJ13
 * File:  common.c
 * Authors: 
 *	    Michal Kozubík,		xkozub03
 *
 * Created on 11.10.2013
 */

#include <stdio.h>
#include <stdarg.h>

#include "ial.h"
#include "common.h"
#include "scanner.h"
#include "tree.h"
#include "generator.h"

#define BUFF_LEN 255
#define INDENT 4

const char *tok_str[] = {
	"<?php", "EOF", "variable", "function", "integer", "double", "string",
	"boolean", "null", "operator", ";", ",", "bracket", "keyword", "expr",
	"unset"
};

const char *op_str[] = {
	"u-", "*", "/", "+", "-", ".", "<", ">", "<=", ">=", "===", "!==", "="
};

const char *key_str[] = {
	"if", "else", "elseif", "return", "while", "function", "true", "false",
	"boolval", "doubleval", "intval", "strval", "get_string", "put_string",
	"strlen", "get_substring", "find_string", "sort_string"
};

const char *bool_str[] = {
	"false", "true"
};

const char *b_str[] = {
	"(", ")", "{", "}"
};

const char *ins_str[] = {
	"I_MUL", "I_DIV", "I_ADD", "I_SUB", "I_CON", "I_LE", "I_LEQ", "I_GR",
	"I_GRQ", "I_EQ", "I_NEQ", "I_ASGN", "I_JMP", "I_TJMP", "I_FJMP", "I_FCEJMP",
	"I_BACK", "I_PUSH", "I_POP", "I_LBL", "I_FCELBL", "I_RET", "I_CALL", "I_END"
};

static int row, col;

static inline void print_spaces(int spaces)
{
	for (int i = 0; i < spaces; i++) {
		printf(" ");
	}
}

void print_error_msg(const char *format, ...)
{
	va_list ap;

	get_last_token_position(&row, &col);
	fprintf(stderr, "Error: %d:%d: ", row, col);

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

char *print_token(token_t *token)
{
	static char buff[BUFF_LEN];
	switch (token->type) {
	case T_VARIABLE:
	case T_FUNCTION:
	case T_STRING:
		snprintf(buff, BUFF_LEN, "%s", token->value.str_val.str);
		break;
	case T_INTEGER:
		snprintf(buff, BUFF_LEN, "%d", token->value.int_val);
		break;
	case T_DOUBLE:
		snprintf(buff, BUFF_LEN, "%g", token->value.double_val);
		break;
	case T_KEYWORD:
		snprintf(buff, BUFF_LEN, "%s", key_str[token->value.int_val]);
		break;
	case T_OPERATOR:
		snprintf(buff, BUFF_LEN, "%s", op_str[token->value.int_val]);
		break;
	case T_BOOLEAN:
		snprintf(buff, BUFF_LEN, "%s", bool_str[token->value.int_val]);
		break;
	case T_BRACKET:
		snprintf(buff, BUFF_LEN, "%s", b_str[token->value.int_val]);
		break;
	default:
		snprintf(buff, BUFF_LEN, "%s", tok_str[token->type]);
		break;
	}
	return buff;
}

void print_expected(const char *what, const char *after, token_t *token)
{
	print_error_msg("'%s' expected after '%s' but '%s' found!",\
		what, after, print_token(token));
}

void print_unexpected(token_t *token)
{
	print_error_msg("unexpected token '%s'!", print_token(token));
}

void print_parenth(const char *what)
{
	print_error_msg("%s must be in parentheses!", what);
}

void print_func_def()
{
	print_error_msg("function definition allowed only at global scope!");
}

void print_miss_br()
{
	print_error_msg("missing closing bracket!");
}

void print_many_br()
{
	print_error_msg("too many closing brackets!");
}

void print_few_args(char *name, int given, int required)
{
	print_error_msg("too few arguments for function '%s' (given %d, required "\
		"%d)!", name, given, required);
}

void print_few_args_before(char *name, int before, int defined)
{
	print_error_msg("function '%s' was previously called with less "\
			"arguments that it is defined here (previous %d, defined %d)!",\
			name, before, defined);
}

void print_redef(char *name)
{
	print_error_msg("redefinition of function '%s'!", name);
}

void print_table(struct htable_t *table, int indent)
{
	if (table == NULL) {
		printf(" is NULL\n");
		return;
	}
	printf("\n");
	htable_elem *tmp;
	for (int i = 0; i < table->size; i++) {
		tmp = table->ptr[i];
		if (tmp != NULL) {
			printf("\n");
		}
		while (tmp != NULL) {
			print_spaces(indent);

			printf("[%d] %s: %s", i, tok_str[tmp->type], tmp->name.str);

			if (tmp->type == T_FUNCTION) {
				printf(" (params = %d, defined = %s)", tmp->num_of_params,\
					bool_str[tmp->is_defined]);
				if (tmp->is_defined == TRUE) {
					printf("\n\n");
					print_spaces(indent);
					printf("LOCAL TABLE");
					print_table(tmp->table, indent + INDENT);
				}
			}

			printf("\n");
			tmp = tmp->next;
		}
	}
	printf("\n");
}

void print_tree(struct tree_node_t *tree, int spaces)
{
	if (tree == NULL) {
		return;
	}

	const char *name, *ch_name;
	if (tree->elem == NULL) {
		name = tok_str[T_NULL];
	} else if (tree->elem->type == T_OPERATOR) {
		name = op_str[tree->elem->subtype];
	} else if (tree->elem->type == T_KEYWORD) {
		name = key_str[tree->elem->subtype];
	} else {
		name = tok_str[tree->elem->type];
	}

	/* print left child */
	if (tree->left != NULL) {
		if (tree->left->elem == NULL) {
			ch_name = tok_str[T_NULL];
		} else if (tree->left->elem->type == T_OPERATOR) {
			ch_name = op_str[tree->left->elem->subtype];
		} else if (tree->left->elem->type == T_KEYWORD) {
			ch_name = key_str[tree->left->elem->subtype];
		} else {
			ch_name = tok_str[tree->left->elem->type];
		}
		print_spaces(spaces);
		if ((tree->elem != NULL) &&
			((tree->elem->type == T_VARIABLE) || (tree->elem->type == T_FUNCTION))) {
			printf("%s (%s) left child is %s", name, tree->elem->name.str, ch_name);
		} else {
			printf("%s left child is %s", name, ch_name);
		}
		if (tree->left->elem != NULL) {
			switch (tree->left->elem->type) {
			case T_INTEGER:
				printf(" (%d)", tree->left->elem->value.int_val);
				break;
			case T_DOUBLE:
				printf(" (%g)", tree->left->elem->value.double_val);
				break;
			case T_STRING:
				printf(" (%s)", tree->left->elem->value.str_val.str);
				break;
			case T_BOOLEAN:
				printf(" (%s)", bool_str[tree->left->elem->value.bool_val]);
				break;
			case T_NULL:
				printf(" (null)");
				break;
			case T_VARIABLE:
			case T_FUNCTION:
				printf(" (%s)", tree->left->elem->name.str);
				break;
			default:
				break;
			}
		}
		printf("\n");
		print_tree(tree->left, spaces + INDENT);
	}

	/* print right child */
	if (tree->right != NULL) {
		if (tree->right->elem == NULL) {
			ch_name = tok_str[T_NULL];
		} else if (tree->right->elem->type == T_OPERATOR) {
			ch_name = op_str[tree->right->elem->subtype];
		} else if (tree->right->elem->type == T_KEYWORD) {
			ch_name = key_str[tree->right->elem->subtype];
		} else {
			ch_name = tok_str[tree->right->elem->type];
		}
		print_spaces(spaces);
		if ((tree->elem != NULL) &&
			((tree->elem->type == T_VARIABLE) || (tree->elem->type == T_FUNCTION))) {
			printf("%s (%s) right child is %s", name, tree->elem->name.str, ch_name);
		} else {
			printf("%s right child is %s", name, ch_name);
		}
		if (tree->right->elem != NULL) {
			switch (tree->right->elem->type) {
			case T_INTEGER:
				printf(" (%d)", tree->right->elem->value.int_val);
				break;
			case T_DOUBLE:
				printf(" (%f)", tree->right->elem->value.double_val);
				break;
			case T_STRING:
				printf(" (%s)", tree->right->elem->value.str_val.str);
				break;
			case T_BOOLEAN:
				printf(" (%s)", bool_str[tree->right->elem->value.bool_val]);
				break;
			case T_NULL:
				printf(" (null)");
				break;
			case T_VARIABLE:
			case T_FUNCTION:
				printf(" (%s)", tree->right->elem->name.str);
				break;
			default:
				break;
			}
		}
		printf("\n");
		print_tree(tree->right, spaces + INDENT);
	}

	if ((tree->left == NULL) && (tree->right == NULL)) {
		print_spaces(spaces);
		if ((tree->elem->type == T_VARIABLE) || (tree->elem->type == T_FUNCTION)) {
			printf("%s (%s) is childless\n", name, tree->elem->name.str);
		} else {
			printf("%s is childless\n", name);
		}
	}
}

/**
 * \brief Tisk zřetězeného seznamu
 */
void print_list(struct tlist_t *list)
{
	if (list == NULL) {
		printf("list is NULL\n");
		return;
	}

	int i = 1;
	tlist_first(list);
	while (list->act != NULL) {
		printf("\nList element: %d\n", i++);
		print_tree(list->act->tree, 0);
		tlist_succ(list);
	}
	printf("\n");
}

char *print_elem(struct htable_elem_t *elem)
{
	static char buff[BUFF_LEN];
	switch (elem->type) {
	case T_VARIABLE:
	case T_FUNCTION:
		snprintf(buff, BUFF_LEN, "%s", elem->name.str);
		break;
	case T_STRING:
		snprintf(buff, BUFF_LEN, "%s (%s: %s)", elem->name.str, tok_str[elem->type], elem->value.str_val.str);
		break;
	case T_INTEGER:
		snprintf(buff, BUFF_LEN, "%s (%s: %d)", elem->name.str, tok_str[elem->type], elem->value.int_val);
		break;
	case T_DOUBLE:
		snprintf(buff, BUFF_LEN, "%s (%s: %g)", elem->name.str, tok_str[elem->type], elem->value.double_val);
		break;
	case T_KEYWORD:
		snprintf(buff, BUFF_LEN, "%s (%s: %s)", elem->name.str, tok_str[elem->type], key_str[elem->subtype]);
		break;
	case T_OPERATOR:
		snprintf(buff, BUFF_LEN, "%s (%s: %s)", elem->name.str, tok_str[elem->type], op_str[elem->subtype]);
		break;
	case T_BOOLEAN:
		snprintf(buff, BUFF_LEN, "%s (%s: %s)", elem->name.str, tok_str[elem->type], bool_str[elem->value.int_val]);
		break;
	case T_BRACKET:
		snprintf(buff, BUFF_LEN, "%s (%s: %s)", elem->name.str, tok_str[elem->type], b_str[elem->subtype]);
		break;
	default:
		snprintf(buff, BUFF_LEN, "%s (%s)", elem->name.str, tok_str[elem->type]);
		break;
	}
	return buff;
}

void print_instruction(int type, int label, htable_elem *op1, htable_elem *op2, htable_elem *result)
{
	printf("%s ", ins_str[type]);
	switch (type) {
	case I_ASGN:
		printf("%s ", print_elem(op1));
		printf("%s\n", print_elem(op2));
		break;
	case I_POP:
	case I_PUSH:
	case I_FCEJMP:
	case I_FCELBL:
	case I_BACK:
	case I_RET:
		printf("%s\n", print_elem(op1));
		break;
	case I_LBL:
	case I_JMP:
		printf("%d\n", label);
		break;
	case I_FJMP:
	case I_TJMP:
		printf("%s ", print_elem(op1));
		printf("%d\n", label);
		break;
	case I_END:
		printf("\n");
		break;
	case I_CALL:
		printf("\n");
		break;
	default:
		printf("%s ", print_elem(op1));
		printf("%s ", print_elem(op2));
		printf("%s\n", print_elem(result));
		break;
	}
}