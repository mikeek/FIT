/* 
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File:   parser.c
 * Authors:
 *		Michal Kozubík,		xkozub03
 *
 * Created on 11.10.2013
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "ial.h"
#include "common.h"
#include "parser.h"
#include "scanner.h"
#include "tree.h"
#include "str.h"
#include "generator.h"

static errors ret_val = ERR_OK;
static token_t token;
static last_t last;
static htable *global_table = NULL, *act_table = NULL;
static htable_elem *global_elem = NULL, *minus_one_elem = NULL;
static tlist *list = NULL;
static tree_node *global_node = NULL;
static scope act_scope = GLOBAL;
static char buff[BUFF_SIZE] = {0};

static int global_params = 0, gen_lbl = 0, gen_var = 0;

/* hloubka zanoření globální a lokální úrovně */
static int depth[] = {0, 0};

/* počet parametrů vestavěných funkcí */
/* -1 = dynamický počet */
static int built_in_params[] = {
	1, 1, 1, 1, 0, -1, 1, 3, 2, 1
};

/* Tabulka pro precedenční analýzu výrazů */
static int prec_table[TABLE_DIMENSION][TABLE_DIMENSION] = {
/*|         | u-| * | / | + | - | . | < | > |<= |>= |===|!==|id | $ | ( | ) | */
/*|  u- |*/ {LE, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|   * |*/ {LE, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|   / |*/ {LE, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|   + |*/ {LE, LE, LE, GR, GR, GR, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|   - |*/ {LE, LE, LE, GR, GR, GR, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|   . |*/ {LE, LE, LE, GR, GR, GR, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|   < |*/ {LE, LE, LE, LE, LE, LE, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|  <= |*/ {LE, LE, LE, LE, LE, LE, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|   > |*/ {LE, LE, LE, LE, LE, LE, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*|  >= |*/ {LE, LE, LE, LE, LE, LE, GR, GR, GR, GR, GR, GR, LE, GR, LE, GR},
/*| === |*/ {LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, GR, GR, LE, GR, LE, GR},
/*| !== |*/ {LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, GR, GR, LE, GR, LE, GR},
/*|  id |*/ {GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, ER, GR, ER, GR},
/*|   $ |*/ {LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, ER, LE, ER},
/*|   ( |*/ {LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, EQ},
/*|   ) |*/ {GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, ER, GR, ER, GR},
};

/**
 * \brief Uvolnění všech alokovaných zdrojů
 */
void free_all_parser()
{
	if (global_table != NULL) {
		hash_free(global_table);
	}
	if (list != NULL) {
		tlist_free(list);
	}

	if ((token.type == T_VARIABLE || token.type == T_FUNCTION || token.type == T_STRING)
		&& ret_val > ERR_LEXAN) {
		strFree(&token.value.str_val);
	}
	global_node = NULL;
	global_table = NULL;
	act_table = NULL;
	list = NULL;
}

/**
 * \brief Vložení vestavěných funkcí, některých konstant atp. do tabulky 
 * 
 * @return error code
 */
static errors insert_built_in()
{
	/* vložení vestavěných funkcí do tabulky symbolů */
	for (keywords i = KEY_BOOLVAL; i <= KEY_SORT_STRING; i++) {
		global_elem = hash_insert_char(global_table, (char *) key_str[i]);
		CHECK_ERR_INTERNAL(global_elem);
		global_elem->type = T_FUNCTION;
		global_elem->is_defined = TRUE;
		global_elem->num_of_params = built_in_params[i - KEY_BOOLVAL];
		global_elem->subtype = i;
	}

	/* vložení TRUE a FALSE do TS */
	for (bool_value i = FALSE; i <= TRUE; i++) {
		global_elem = hash_insert_char(global_table, (char *) bool_str[i]);
		CHECK_ERR_INTERNAL(global_elem);
		global_elem->type = T_BOOLEAN;
		global_elem->val_type = T_BOOLEAN;
		global_elem->value.bool_val = i;
		global_elem->is_defined = TRUE;
	}

	/* vložení NULL do TS */
	global_elem = hash_insert_char(global_table, (char *) tok_str[T_NULL]);
	CHECK_ERR_INTERNAL(global_elem);
	global_elem->type = T_NULL;
	global_elem->val_type = T_NULL;
	global_elem->value.int_val = 0;
	global_elem->is_defined = TRUE;

	/* vložení "=" do TS */
	global_elem = hash_insert_char(global_table, ASSIGN_ELEM);
	CHECK_ERR_INTERNAL(global_elem);
	global_elem->type = T_OPERATOR;
	global_elem->subtype = OP_ASSIGN;
	global_elem->val_type = T_UNSET;
	global_elem->is_defined = TRUE;

	/* vložení "," do TS */
	global_elem = hash_insert_char(global_table, COMMA_ELEM);
	CHECK_ERR_INTERNAL(global_elem);
	global_elem->type = T_COMMA;
	global_elem->is_defined = TRUE;

	/* vložení -1 do TS (pro operace s operátorem unární mínus) */
	minus_one_elem = hash_insert_char(global_table, MINUS_ONE_ELEM);
	CHECK_ERR_INTERNAL(minus_one_elem);
	minus_one_elem->type = T_INTEGER;
	minus_one_elem->val_type = T_INTEGER;
	minus_one_elem->is_defined = TRUE;
	minus_one_elem->value.int_val = -1;

	return ERR_OK;
}

/**
 * \brief Inicializace dynamických proměnných atp.
 * 
 * @return error code
 */
errors init_parser()
{
	global_table = hash_init(SYM_TABLE_SIZE);
	list = tlist_init();

	CHECK_ERR_INTERNAL(global_table);
	CHECK_ERR_INTERNAL(list);
	CHECK_ERR_INTERNAL(tlist_insert_first(list));

	list->first->tree = node_init();
	CHECK_ERR_INTERNAL(list->first->tree);

	list->act = list->first;
	token.type = T_UNSET;
	act_table = global_table;
	global_node = list->act->tree;

	ret_val = insert_built_in();
	return ret_val;
}

/**
 * \brief Posunutí se v seznamu stromů
 * 
 * @return odkaz na nový strom
 */
static errors new_tree()
{
	if (list->act->tree->elem == NULL && list->act->tree->left == NULL &&
		list->act->tree->right == NULL) {
		global_node = list->act->tree;
		return ERR_OK;
	}

	CHECK_ERR_INTERNAL(tlist_insert_last(list));

	list->act->tree = node_init();
	global_node = list->act->tree;

	CHECK_ERR_INTERNAL(global_node);

	return ERR_OK;
}

/**
 * \brief Kontrola nedefinovaných funkcí
 * 
 * @return error code
 */
static errors check_undefined()
{
	for (int i = 0; i < global_table->size; i++) {
		global_elem = global_table->ptr[i];
		while (global_elem != NULL) {
			if (global_elem->type == T_FUNCTION && global_elem->is_defined == FALSE) {
				fprintf(stderr, "Error: undefined function '%s'!\n", global_elem->name.str);
				return ERR_SEMAN_UNDEF_REDEF;
			}
			global_elem = global_elem->next;
		}
	}
	return ERR_OK;
}

/**
 * \brief Najde prvek v tabulce symbolů - pokud ne, tak ho do ní vloží
 * 
 * @return odkaz na vložený prvek
 */
static htable_elem *look_or_insert()
{
	global_elem = hash_lookup(act_table, &token.value.str_val);
	if (global_elem == NULL) {
		global_elem = hash_insert(act_table, &token.value.str_val);
		if (global_elem != NULL) {
			global_elem->type = token.type;
		}
	}
	return global_elem;
}

/**
 * \brief Najde konstantu v tabulce symbolů - pokud ne, tak ji do ní vloží
 * 
 * @return odkaz na vložený prvek
 */
static htable_elem *look_or_insert_const()
{
	string buff_str;

	switch (token.type) {
	case T_BOOLEAN:
		global_elem = hash_lookup_char(global_table, (char *) bool_str[token.value.bool_val]);
		break;
	case T_NULL:
		global_elem = hash_lookup_char(global_table, (char *) tok_str[token.type]);
		break;
	case T_INTEGER:
	case T_DOUBLE:
	case T_STRING:
		ret_val = strInit(&buff_str);
		if (ret_val != ERR_OK) {
			return NULL;
		}
		if (token.type == T_INTEGER) {
			snprintf(buff, BUFF_SIZE, "@int_%d", token.value.int_val);
			ret_val = strAddChars(&buff_str, buff);
			CHECK_RETURN_FREE(&buff_str);
		} else if (token.type == T_DOUBLE) {
			snprintf(buff, BUFF_SIZE, "@dbl_%g", token.value.double_val);
			ret_val = strAddChars(&buff_str, buff);
			CHECK_RETURN_FREE(&buff_str);
		} else {
			ret_val = strAddChars(&buff_str, "@str_");
			CHECK_RETURN_FREE(&buff_str);
			ret_val = strAddChars(&buff_str, token.value.str_val.str);
			CHECK_RETURN_FREE(&buff_str);
		}
		global_elem = hash_lookup(global_table, &buff_str);
		if (global_elem == NULL) {
			global_elem = hash_insert(global_table, &buff_str);
			if (global_elem != NULL) {
				global_elem->type = token.type;
				if (token.type == T_STRING) {
					ret_val = strDuplicate(&global_elem->value.str_val, &token.value.str_val);
					CHECK_RETURN_FREE(&buff_str);
				} else {
					global_elem->value = token.value;
				}
				global_elem->val_type = token.type;
				global_elem->is_defined = TRUE;
			}
		}
		strFree(&buff_str);
		break;
	default:
		global_elem = NULL;
		break;
	}
	return global_elem;
}

/**
 * \brief Optimalizuje uzel stromu 
 * 
 * Pokud má např. uzel "+" oba potomky celočíselné konstany, zde se sečtou a 
 * nová hodnota se uloží do tohoto uzlu - zmenší se počet instrukcí
 * 
 * @param node[in, out] uzel stromu
 * @return error code
 */
static errors optimize(tree_node *node)
{
	htable_elem *left, *right;
	bool_value neg = FALSE, result;

	if (node == NULL || node->left == NULL || node->right == NULL ||
		node->left->elem == NULL || node->right->elem == NULL ||
		node->elem->type != T_OPERATOR) {
		return ret_val;
	}

	left = node->left->elem;
	right = node->right->elem;

	if (left->type == T_VARIABLE || right->type == T_VARIABLE ||
		left->type == T_OPERATOR || right->type == T_OPERATOR ||
		left->type == T_FUNCTION || right->type == T_FUNCTION) {
		return ret_val;
	}

	switch (node->elem->subtype) {
	case OP_DIVIDE:
		/* Kontrola dělení nulou */
		if ((right->type == T_INTEGER && right->value.int_val == 0) ||
			(right->type == T_DOUBLE && right->value.double_val == 0.0)) {
			print_error_msg("division by zero!");
			return ERR_SEMAN_DIV_ZERO;
		}
		/* Kontrola typů pro dělení */
		CHECK_COMPATIBILITY(left, right, "division");

		node->elem->type = T_DOUBLE;
		node->elem->value.double_val = (double) VALUE(left) / VALUE(right);
		break;
	case OP_MULTIPLY:
		CHECK_COMPATIBILITY(left, right, "multiplication");

		if (left->type == T_INTEGER && right->type == T_INTEGER) {
			node->elem->type = T_INTEGER;
			node->elem->value.int_val = left->value.int_val * right->value.int_val;
		} else {
			node->elem->type = T_DOUBLE;
			node->elem->value.double_val = VALUE(left) * VALUE(right);
		}
		break;
	case OP_PLUS:
		CHECK_COMPATIBILITY(left, right, "addition");

		if (left->type == T_INTEGER && right->type == T_INTEGER) {
			node->elem->type = T_INTEGER;
			node->elem->value.int_val = left->value.int_val + right->value.int_val;
		} else {
			node->elem->type = T_DOUBLE;
			node->elem->value.double_val = VALUE(left) + VALUE(right);
		}
		break;
	case OP_MINUS:
		CHECK_COMPATIBILITY(left, right, "subtraction");

		if (left->type == T_INTEGER && right->type == T_INTEGER) {
			node->elem->type = T_INTEGER;
			node->elem->value.int_val = left->value.int_val - right->value.int_val;
		} else {
			node->elem->type = T_DOUBLE;
			node->elem->value.double_val = VALUE(left) - VALUE(right);
		}
		break;
	case OP_CONCAT:
		/* První musí být string */
		if (left->type != T_STRING) {
			print_error_msg("incompatible types for concatenation!");
			return ERR_SEMAN_COMPATIBILITY;
		}
		if (left->type == T_STRING && right->type == T_STRING) {
			if (node->elem->val_type == T_STRING) {
				strClear(&node->elem->value.str_val);
			} else {
				node->elem->type = T_STRING;
				ret_val = strInit(&node->elem->value.str_val);
				CHECK_RETURN();
			}
			ret_val = strAddChars(&node->elem->value.str_val, left->value.str_val.str);
			CHECK_RETURN();
			ret_val = strAddChars(&node->elem->value.str_val, right->value.str_val.str);
			CHECK_RETURN();
		} else {
			/* TODO */
			return ERR_OK;
		}
		break;
	case OP_NOT_EQ:
		neg = TRUE;
	case OP_TRIPLE_EQ:
		node->elem->type = T_BOOLEAN;

		if (left->type == right->type && (
			(left->type == T_INTEGER && left->value.int_val == right->value.int_val) ||
			(left->type == T_DOUBLE && left->value.double_val == right->value.double_val) ||
			(left->type == T_BOOLEAN && left->value.bool_val == right->value.bool_val) ||
			(left->type == T_STRING && (strCmpString(&left->value.str_val, &right->value.str_val) == 0)) ||
			(left->type == T_KEYWORD && left->value.int_val == right->value.int_val) ||
			(left->type == T_NULL && right->type == T_NULL))) {
			result = (neg == TRUE) ? FALSE : TRUE;
		} else {
			result = (neg == TRUE) ? TRUE : FALSE;
		}
		node->elem = hash_lookup_char(global_table, (char *) bool_str[result]);
		break;
	case OP_LESS:
	case OP_LESS_EQ:
	case OP_GREATER:
	case OP_GREATER_EQ:
		if (left->type != right->type) {
			print_error_msg("incompatible types for comparison!");
			return ERR_SEMAN_COMPATIBILITY;
		}
	default:
		return ERR_OK;
	}
	/* potomky už nepotřebujeme */
	node->elem->val_type = node->elem->type;
	node->elem->is_defined = TRUE;
	tree_free(node->left);
	tree_free(node->right);
	node->left = NULL;
	node->right = NULL;
	return ret_val;
}

/**
 * \brief Zpracuje seznam parametrů (při definici funkce)
 * 
 * <item-list> -----> ,<item><item-list>
 * <item-list> -----> )
 * 
 * @return error code
 */
errors parse_item_list()
{
	/* , */
	if (token.type == T_COMMA) {
		global_node = add_right_child(global_node);
		global_node->elem = hash_lookup_char(global_table, COMMA_ELEM);

		GET_TOKEN();

		/* ,<item> */
		ret_val = parse_item();
		CHECK_RETURN();

		/* ,<item><item-list> */
		ret_val = parse_item_list();
		/* ) */
	} else {
		CHECK_TYPE_VAL(T_BRACKET, B_RIGHT_PAR, ",' or ')", ID);
	}

	global_params++;
	return ret_val;
}

/**
 * \brief Zpracuje parametr funkce (při definici)
 * 
 * <item> -----> T_VARIABLE
 * 
 * @return error code
 */
errors parse_item()
{
	CHECK_TYPE(T_VARIABLE, tok_str[T_VARIABLE], "(' or ','");

	add_left_child(global_node);

	global_node->left->elem = hash_lookup(act_table, &token.value.str_val);
	if (global_node->left->elem != NULL) {
		print_error_msg("redefinition of function argument!");
		return ERR_SEMAN_OTHER;
	}

	global_node->left->elem = hash_insert(act_table, &token.value.str_val);
	CHECK_ERR_INTERNAL(global_node->left->elem);

	global_node->left->elem->type = token.type;
	/* generování pop instrukce */
	ret_val = store_instruction(I_POP, 0, global_node->left->elem, NULL, NULL);
	CHECK_RETURN();

	GET_TOKEN();

	return ret_val;
}

/**
 * \brief Zpracování volání funkce
 * 
 * <function-call> -----> call (<expr><expr-list>)
 * 
 * @param[in] act_node aktuální uzel stromu
 * @return error code
 */
errors parse_function_call(tree_node *act_node)
{
	char *name;
	int required, local_params = 0;
	htable_elem *elem;

	if (token.type == T_KEYWORD) {
		name = (char *) key_str[token.value.int_val];
	} else {
		name = token.value.str_val.str;
	}

	elem = hash_lookup_char(global_table, name);
	if (elem == NULL) {
		elem = hash_insert_char(global_table, name);
		CHECK_ERR_INTERNAL(elem);

		elem->type = T_FUNCTION;
		elem->subtype = KEY_FALSE;
	}

	act_node->elem = elem;
	required = elem->num_of_params;

	/* Informovani interpretu o zacatku volani funkce */
	ret_val = store_instruction(I_CALL, 0, NULL, NULL, NULL);
	CHECK_RETURN();

	/* call ( */
	GET_TOKEN();
	CHECK_TYPE_VAL(T_BRACKET, B_LEFT_PAR, b_str[B_LEFT_PAR], ID);

	GET_TOKEN();

	if ((token.type != T_BRACKET) || (token.value.int_val != B_RIGHT_PAR)) {
		/* call (<expr> */
		ret_val = parse_expr(act_node);
		CHECK_RETURN();

		/* call (<expr><expr-list>) */
		ret_val = parse_expr_list(act_node, &local_params);
		CHECK_RETURN();
	}

	if (required == UNSET) {
		elem->num_of_params = local_params;
	} else if (local_params < required) {
		if (elem->is_defined == TRUE) {
			print_few_args(elem->name.str, local_params, required);
			return ERR_SEMAN_MISSING_PARAM;
		}
		elem->num_of_params = local_params;
	}

	/* generovani goto */
	ret_val = store_instruction(I_FCEJMP, 0, elem, NULL, NULL);
	CHECK_RETURN();

	return ret_val;
}

/**
 * \brief Zpracování definice funkce
 * 
 * function id (<item><item-list>) { <st-list> }
 * 
 * @return error code
 */
errors parse_function()
{
	htable_elem *elem;
	int params_before;

	/* function id */
	if ((token.type == T_KEYWORD) && (token.value.int_val >= KEY_BOOLVAL)) {
		print_error_msg("'%s' is keyword!", key_str[token.value.int_val]);
		return ERR_SEMAN_UNDEF_REDEF;
	}

	CHECK_TYPE(T_FUNCTION, ID, key_str[KEY_FUNCTION]);

	global_node = add_left_child(global_node);

	elem = hash_lookup(global_table, &token.value.str_val);
	if (elem == NULL) {
		elem = hash_insert(global_table, &token.value.str_val);
		CHECK_ERR_INTERNAL(elem);
		elem->type = T_FUNCTION;
		elem->subtype = KEY_FALSE;
	} else {
		if (elem->is_defined == TRUE) {
			print_redef(token.value.str_val.str);
			return ERR_SEMAN_UNDEF_REDEF;
		}
	}

	/* generate jmp */
	ret_val = store_instruction(I_FCELBL, 0, elem, NULL, NULL);
	CHECK_RETURN();

	global_node->elem = elem;

	elem->table = hash_init(SYM_TABLE_SIZE);
	act_table = elem->table;
	CHECK_ERR_INTERNAL(act_table);

	elem->is_defined = TRUE;
	params_before = elem->num_of_params;

	global_params = 0;

	/* function id (*/
	GET_TOKEN();
	CHECK_TYPE_VAL(T_BRACKET, B_LEFT_PAR, b_str[B_LEFT_PAR], ID);

	/* function id (<item><item-list> */
	GET_TOKEN();

	if (token.type != T_BRACKET || token.value.int_val != B_RIGHT_PAR) {
		/* function id (<item> */
		ret_val = parse_item();
		CHECK_RETURN();

		/* function id (<item><item-list>) */
		ret_val = parse_item_list();
		CHECK_RETURN();
	} /* else -> function id () */

	/* pravou zavorku jsme nacetli bud v minulem get_token nebo v parse_item()*/

	/* function id (<item><item-list>) { */
	GET_TOKEN();
	CHECK_TYPE_VAL(T_BRACKET, B_LEFT_BRACE, b_str[B_LEFT_BRACE], b_str[B_RIGHT_PAR]);

	if (params_before == UNSET) {
		elem->num_of_params = global_params;
	} else if (global_params > params_before) {
		print_few_args_before(elem->name.str, params_before, global_params);
		return ERR_SEMAN_MISSING_PARAM;
	}

	/* function id (<item><item-list>) { <st-list> } */
	GET_TOKEN();

	/* TĚLO FUNKCE */
	depth[act_scope]++;

	ret_val = parse_st_list();
	CHECK_RETURN();

	depth[act_scope]--;
	/* KONEC TĚLA FUNKCE */

	if (depth[LOCAL] > 0) {
		print_miss_br();
		return ERR_SYNAN;
	} else if (depth[LOCAL] < 0) {
		print_many_br();
		return ERR_SYNAN;
	}

	act_table = global_table;

	/* návratová instrukce */
	ret_val = store_instruction(I_BACK, 0, elem, NULL, NULL);
	return ret_val;
}

/**
 * \brief Zpracování seznamu parametrů při volání funkce
 * 
 * <expr-list> -----> ,<expr><expr-list>
 * <expr-list> -----> )
 * 
 * @param[in] act_node aktuální uzel stromu
 * @param[out] local_params počet načtených parametrů
 * @return error code
 */
errors parse_expr_list(tree_node *act_node, int *local_params)
{
	/* generovani push instruce */
	ret_val = store_instruction(I_PUSH, 0, act_node->left->elem, NULL, NULL);
	CHECK_RETURN();

	/* , */
	if (token.type == T_COMMA) {
		act_node = add_right_child(act_node);
		act_node->elem = hash_lookup_char(global_table, COMMA_ELEM);

		GET_TOKEN();

		/* ,<expr> */
		ret_val = parse_expr(act_node);
		CHECK_RETURN();

		/* ,<expr><expr-list> */
		ret_val = parse_expr_list(act_node, local_params);
		/* ) */
	} else {
		CHECK_TYPE_VAL(T_BRACKET, B_RIGHT_PAR, ",' or ')", ID);
	}

	(*local_params)++;
	return ret_val;
}

/**
 * \brief Funkce pro redukování načteného výrazu
 * 
 * @param nodes zásobník s operandy
 * @param ops zásobník s operátory
 * @return error code
 */
static errors reduce(stack *nodes, stack_i *ops)
{
	int top = stack_i_pop(ops);

	if (top == STACK_EMPTY || top == TI_ID || top == TI_LEFT || top == TI_RIGHT || top == TI_NONE) {
		return ERR_OK;
	}

	tree_node *node;
	node = node_init();
	CHECK_ERR_INTERNAL(node);

	GENERATE_ELEM(node->elem);
	node->elem->type = T_OPERATOR;
	node->elem->val_type = T_UNSET;
	node->elem->subtype = top;

	node->right = stack_pop(nodes);

	if (top == TI_UMINUS) {
		node->elem->subtype = OP_MULTIPLY;
		node->left = node_init();
		CHECK_ERR_INTERNAL(node->left);
		node->left->elem = minus_one_elem;
	} else {
		node->left = stack_pop(nodes);
	}

	ret_val = stack_push(nodes, node);
	CHECK_RETURN();

	if (node->right == NULL || node->left == NULL) {
		print_error_msg("missing operand!");
		return ERR_SYNAN;
	}

	ret_val = optimize(node);
	CHECK_RETURN();

	if (node->left != NULL || node->right != NULL) {
		ret_val = store_instruction(node->elem->subtype - 1, 0, node->left->elem, node->right->elem, node->elem);
		CHECK_RETURN();
	}

	return ret_val;
}

/**
 * \brief Precedenční syntaktický analyzátor pro zpracování výrazů
 * 
 * Pozn.: volání funkcí je zpracováváno zvlášť (rekurzivním sestupen a příp.
 *	další precedenční analýzou při čtení parametrů)
 * 
 * @param act_node aktuální uzel stromu
 * @return error code
 */
errors parse_expr(tree_node *act_node)
{
	int col, row = TI_NONE, state, pars = 0;
	last_t last_expr;
	bool_value more = TRUE;
	tree_node *node;
	stack *nodes = stack_init(STACK_SIZE);
	CHECK_ERR_INTERNAL(nodes);
	stack_i *ops = stack_i_init(STACK_SIZE);

	if (ops == NULL) {
		stack_free(nodes);
		return ERR_INTERNAL;
	}


	last_expr.type = T_UNSET;
	ret_val = stack_i_push(ops, TI_NONE);
	CHECK_RETURN();

	while (ret_val == ERR_OK) {
		switch (token.type) {
		case T_OPERATOR:
			if (token.value.int_val == OP_ASSIGN) {
				print_error_msg("parse_expr: assign operator not "\
						"supported in expression!");
				ret_val = ERR_SYNAN;
				more = FALSE;
				break;
			}
			/* Zpracování unárního + a - */
			if (token.value.int_val == OP_MINUS && stack_i_top(ops) != TI_ID) {
				token.value.int_val = OP_UMINUS;
			} else if (token.value.int_val == OP_PLUS && stack_i_top(ops) != TI_ID) {
				ret_val = get_token(&token);
				continue;
			}

			col = token.value.int_val;
			last_expr.subtype = token.value.int_val;
			break;
		case T_FUNCTION:
			node = node_init();
			ret_val = stack_push(nodes, node);
			CHECK_ERR_INTERNAL_BREAK(node, more);
			CHECK_MORE_BREAK(more);

			ret_val = parse_function_call(node);
			CHECK_MORE_BREAK(more);

			col = TI_ID;
			break;
		case T_VARIABLE:
			node = node_init();
			ret_val = stack_push(nodes, node);
			CHECK_ERR_INTERNAL_BREAK(node, more);
			CHECK_MORE_BREAK(more);

			node->elem = look_or_insert();
			CHECK_ERR_INTERNAL(node->elem);

			col = TI_ID;
			break;
		case T_INTEGER:
		case T_DOUBLE:
		case T_BOOLEAN:
		case T_STRING:
		case T_NULL:
			node = node_init();
			ret_val = stack_push(nodes, node);
			CHECK_ERR_INTERNAL_BREAK(node, more);
			CHECK_MORE_BREAK(more);

			node->elem = look_or_insert_const();
			col = TI_ID;
			break;
		case T_BRACKET:
			if (token.value.int_val == B_LEFT_PAR) {
				pars++;
				col = TI_LEFT;
			} else if (token.value.int_val == B_RIGHT_PAR) {
				pars--;
				if (pars < 0) {
					col = TI_NONE;
				} else {
					col = TI_RIGHT;
				}
			} else {
				col = TI_NONE;
			}
			last_expr.subtype = token.value.int_val;
			break;
		case T_KEYWORD:
			if (token.value.int_val < KEY_BOOLVAL) {
				print_unexpected(&token);
				ret_val = ERR_SYNAN;
				more = FALSE;
				break;
			}

			node = node_init();
			ret_val = stack_push(nodes, node);
			CHECK_ERR_INTERNAL_BREAK(node, more);
			CHECK_MORE_BREAK(more);

			ret_val = parse_function_call(node);
			CHECK_MORE_BREAK(more);

			col = TI_ID;
			last_expr.subtype = token.value.int_val;
			break;
		default:
			col = TI_NONE;
			break;
		}
		last_expr.type = token.type;

		if (more == FALSE) {
			break;
		}
		do {
			row = stack_i_top(ops);
			state = prec_table[row][col];
			switch (state) {
			case LE:
				ret_val = stack_i_push(ops, col);
				CHECK_MORE_BREAK(more);
				break;
			case GR:
				ret_val = reduce(nodes, ops);
				CHECK_MORE_BREAK(more);
				break;
			case ER:
				col = TI_NONE;
				break;
			case EQ:
				stack_i_pop(ops);
				ret_val = stack_i_push(ops, TI_ID);
				CHECK_MORE_BREAK(more);
				break;
			}
		} while (state == GR && more == TRUE);

		if (ret_val != ERR_OK) {
			break;
		}

		if (col == TI_NONE) {
			if (nodes->top == STACK_EMPTY) {
				print_error_msg("missing operands!");
				ret_val = ERR_SYNAN;
			}
			if (act_node->left == NULL) {
				act_node->left = stack_pop(nodes);
			} else {
				act_node->right = stack_pop(nodes);
			}
			if (nodes->top != STACK_EMPTY) {
				print_error_msg("missing operator(s)!");
				ret_val = ERR_SYNAN;
			}
			break;
		}

		if (ret_val != ERR_OK) {
			break;
		}

		last = last_expr;
		ret_val = get_token(&token);
	}

	stack_free(nodes);
	stack_i_free(ops);
	return ret_val;
}

/**
 * \brief Zpracování cyklu while
 * 
 * while (<expr>) { <st-list> }
 * 
 * @return error code
 */
errors parse_while()
{
	int while_label, after_label;
	GENERATE_LABEL(while_label);
	GENERATE_LABEL(after_label);

	/* while ( */
	CHECK_TYPE_VAL(T_BRACKET, B_LEFT_PAR, b_str[B_LEFT_PAR], key_str[KEY_WHILE]);

	ret_val = store_instruction(I_LBL, while_label, NULL, NULL, NULL);
	CHECK_RETURN();

	/* while (<expr> */
	ret_val = parse_expr(global_node);
	CHECK_RETURN();

	ret_val = store_instruction(I_FJMP, after_label, global_node->left->elem, NULL, NULL);
	CHECK_RETURN();

	/* while (<expr>) */
	CHECK_PARENTH(WHILE_EXPR);

	/* while (<expr>) { */
	CHECK_TYPE_VAL(T_BRACKET, B_LEFT_BRACE, b_str[B_LEFT_BRACE], b_str[B_RIGHT_PAR]);

	/* while (<expr>) { <st-list> } */
	GET_TOKEN();

	depth[act_scope]++;
	ret_val = parse_st_list();
	depth[act_scope]--;

	CHECK_RETURN();

	ret_val = store_instruction(I_JMP, while_label, NULL, NULL, NULL);
	CHECK_RETURN();

	ret_val = store_instruction(I_LBL, after_label, NULL, NULL, NULL);
	return ret_val;
}

/**
 * \brief Zpracování else větve podmínky 
 * 
 * ... else { <st-list> }
 * 
 * @return error code
 */
errors parse_else()
{
	/* else */
	//	CHECK_TYPE_VAL(T_KEYWORD, KEY_ELSE, key_str[KEY_ELSE], b_str[B_RIGHT_BRACE]);

	/* else { */
	GET_TOKEN();
	CHECK_TYPE_VAL(T_BRACKET, B_LEFT_BRACE, b_str[B_LEFT_BRACE], key_str[KEY_ELSE]);

	/* else { <st-list> } */
	GET_TOKEN();

	depth[act_scope]++;

	ret_val = parse_st_list();

	depth[act_scope]--;

	return ret_val;
}

/**
 * \brief Zpracování if podmínky
 * 
 * (else)if (<expr>) { <st-list> } [elseif (<expr>) { <st-list> }]* [else {<st-list>}]
 * 
 * Po zpracování if (<expr>) { <st-list> } se zkontroluje další token - pokud je
 * to KEY_ELSE, provede se zpracování ELSE a podmíněný příkaz končí. Pokud je to
 * KEY_ELSEIF, zavolá se znovu parse_if a postup se opakuje
 * 
 * @param[in] after_label navesti kodu za celym podminenym prikazem
 * @return error code
 */
errors parse_if(int after_label)
{
	/* generovat skok na else/elseif větev */
	int else_label;
	GENERATE_LABEL(else_label);

	/* (else)if ( */
	CHECK_TYPE_VAL(T_BRACKET, B_LEFT_PAR, b_str[B_LEFT_PAR], key_str[KEY_IF]);

	/* (else)if (<expr> */
	ret_val = parse_expr(global_node);
	CHECK_RETURN();

	ret_val = store_instruction(I_FJMP, else_label, global_node->left->elem, NULL, NULL);
	CHECK_RETURN();

	/* (else)if (<expr>) */
	CHECK_PARENTH(IF_EXPR);

	/* (else)if (<expr>) { */
	CHECK_TYPE_VAL(T_BRACKET, B_LEFT_BRACE, b_str[B_LEFT_BRACE], b_str[B_RIGHT_PAR]);

	/* (else)if (<expr>) { <st-list> } */
	GET_TOKEN();

	depth[act_scope]++;
	ret_val = parse_st_list();
	depth[act_scope]--;

	CHECK_RETURN();

	ret_val = store_instruction(I_JMP, after_label, NULL, NULL, NULL);
	CHECK_RETURN();

	ret_val = store_instruction(I_LBL, else_label, NULL, NULL, NULL);
	CHECK_RETURN();

	/* (else)if (<expr>) { <st-list> } [ else(if) ] */
	GET_TOKEN();

	ret_val = new_tree();
	CHECK_RETURN();

	if (token.type == T_KEYWORD && token.value.int_val == KEY_ELSE) {
		ret_val = parse_else();
		CHECK_RETURN();
		GET_TOKEN();
	} else if (token.type == T_KEYWORD && token.value.int_val == KEY_ELSEIF) {
		GET_TOKEN();
		ret_val = parse_if(after_label);
		CHECK_RETURN();
	}

	return ret_val;
}

/**
 * \brief Základní funkce pro zpracování příkazů
 * 
 * <stat> -----> $var = <expr>;
 * <stat> -----> if (<expr>) { <st-list> } else { <st-list> }
 * <stat> -----> while (<expr>) { <st-list> }
 * <stat> -----> return <expr>;
 * <stat> -----> function id (<item> <item-list>) { <st-list> }
 * <stat> -----> { <st-list> } 
 * 
 * @return error code
 */
errors parse_stat()
{
	tree_node *act_node;

	ret_val = new_tree();
	CHECK_RETURN();

	act_node = global_node;

	switch (token.type) {
		/* $var */
	case T_VARIABLE:
		add_left_child(act_node);
		act_node->left->elem = look_or_insert();
		CHECK_ERR_INTERNAL(act_node->left->elem);

		/* $var = */
		GET_TOKEN();
		CHECK_TYPE_VAL(T_OPERATOR, OP_ASSIGN, "=", "variable");

		act_node->elem = hash_lookup_char(global_table, ASSIGN_ELEM);

		/* $var = <expr> */
		GET_TOKEN();

		ret_val = parse_expr(act_node);
		CHECK_RETURN();

		/* $var = <expr>; */
		CHECK_TYPE(T_SEMICOL, tok_str[T_SEMICOL], EXPR);

		ret_val = store_instruction(I_ASGN, 0, act_node->right->elem, act_node->left->elem, NULL);
		CHECK_RETURN();
		GET_TOKEN();
		break;
	case T_KEYWORD:
		switch (token.value.int_val) {
		case KEY_ELSE:
			print_unexpected(&token);
			return ERR_SYNAN;
		case KEY_IF:
		{
			GET_TOKEN();
			int after_label;
			GENERATE_LABEL(after_label);
			ret_val = parse_if(after_label);
			CHECK_RETURN();
			ret_val = store_instruction(I_LBL, after_label, NULL, NULL, NULL);
			CHECK_RETURN();
			break;
		}
		case KEY_WHILE:
			GET_TOKEN();
			ret_val = parse_while();
			CHECK_RETURN();
			GET_TOKEN();
			break;
		case KEY_RETURN:
			GET_TOKEN();
			/* return <expr> */
			ret_val = parse_expr(global_node);
			CHECK_RETURN();
			/* return <expr> ;*/
			CHECK_TYPE(T_SEMICOL, tok_str[T_SEMICOL], EXPR);
			ret_val = store_instruction(I_RET, 0, global_node->left->elem, NULL, NULL);
			CHECK_RETURN();
			GET_TOKEN();
			break;
		case KEY_FUNCTION:
			if ((act_scope != GLOBAL) || (depth[GLOBAL] != 0)) {
				print_func_def();
				return ERR_SYNAN;
			}

			GET_TOKEN();

			act_scope = LOCAL;
			ret_val = parse_function();
			act_scope = GLOBAL;
			CHECK_RETURN();
			GET_TOKEN();
			break;
		default:
			print_unexpected(&token);
			ret_val = ERR_SYNAN;
			break;
		}
		break;
		/* { */
	case T_BRACKET:
		if (token.value.int_val != B_LEFT_BRACE) {
			print_unexpected(&token);
			ret_val = ERR_SYNAN;
			break;
		}
		/* { <st-list> } */
		GET_TOKEN();
		depth[act_scope]++;

		ret_val = parse_st_list();

		depth[act_scope]--;
		CHECK_RETURN();
		GET_TOKEN();
		break;
	default:
		print_unexpected(&token);
		ret_val = ERR_SYNAN;
		break;
	}

	return ret_val;
}

/**
 * \brief Zpracování seznamu příkazů
 * 
 * <st-list> -----> <stat> <st-list>
 * <st-list> -----> }
 * <st-list> -----> T_END
 * 
 * @return error code
 */
errors parse_st_list()
{
	switch (token.type) {
		/* T_END */
	case T_END:
		if ((act_scope != GLOBAL) || (depth[GLOBAL] != 0)) {
			print_miss_br();
			return ERR_SYNAN;
		}

		ret_val = store_instruction(I_END, 0, NULL, NULL, NULL);
		return ret_val;
		/* } */
	case T_BRACKET:
		if (token.value.int_val == B_RIGHT_BRACE) {
			if ((depth[GLOBAL] == 0) && (depth[LOCAL] == 0)) {
				print_many_br();
				return ERR_SYNAN;
			}
			return ERR_OK;
		}
		break;
	default:
		break;
	}

	/* <stat> */
	ret_val = parse_stat();
	CHECK_RETURN();

	/* <stat><st-list> */
	return parse_st_list();
}

/**
 * \brief Zpracování programu
 * 
 * <program> -----> T_START <st-list>
 * 
 * @return error code
 */
errors parse_program()
{
	if (token.type != T_START) {
		print_error_msg("Error: program not starting with '<?php'!");
		return ERR_SYNAN;
	}

	GET_TOKEN();

	ret_val = parse_st_list();
	CHECK_RETURN();

	ret_val = check_undefined();
	return ret_val;
}

/**
 * \brief Main funkce parseru
 * 
 * @return error code 
 */
errors parse()
{
	GET_TOKEN();

	ret_val = parse_program();
	return ret_val;
}
