/* 
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File:   tree.c
 * Authors:
 *		Michal Kozubík,		xkozub03
 *
 * Created on 11.10.2013
 */


#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "tree.h"

#define STACK_UNDERFLOW (-1)

/**
 * \brief Inicializuje uzel stromu
 * 
 * @param node ukazatel na uzel
 * @param type typ uzlu
 * @return Ukazatel na nový uzel
 */
tree_node *node_init()
{
	tree_node *node = malloc(sizeof (tree_node));
	if (node == NULL) {
		print_error_msg("Error: malloc() failed!");
		return NULL;
	}

	node->left = NULL;
	node->right = NULL;
	node->elem = NULL;

	return node;
}

/**
 * \brief Přidá levého potomka
 * 
 * @param node Rodičovský uzel
 * @param type typ potomka
 * @return Ukazatel na nový uzel
 */
tree_node *add_left_child(tree_node *parent)
{
	if (parent == NULL) {
		print_error_msg("Error: add_left_child(): NULL node accepted!");
		return NULL;
	}

	parent->left = node_init();

	return parent->left;
}

/**
 * \brief Přidá pravého potomka
 * 
 * @param node Rodičovský uzel
 * @param type typ potomka
 * @return Ukazatel na nový uzel
 */
tree_node *add_right_child(tree_node *parent)
{
	if (parent == NULL) {
		print_error_msg("Error: add_right_child(): NULL node accepted!");
		return NULL;
	}

	parent->right = node_init();

	return parent->right;
}

/**
 * \brief Uvolnění stromu z paměti
 * 
 * Rekurzivně prochází strom a postupně uvolňuje od listů až ke kořenu
 * 
 * @param node Uzel stromu
 */
void tree_free(tree_node *node)
{
	if (node == NULL) {
		return;
	}

	if (node->left != NULL) {
		tree_free(node->left);
	}
	if (node->right != NULL) {
		tree_free(node->right);
	}

	free(node);
}

tlist *tlist_init()
{
	tlist *tmp = malloc(sizeof (tlist));
	if (tmp == NULL) {
		print_error_msg("Error: init_tlist malloc() failed!");
		return NULL;
	}
	tmp->first = NULL;
	tmp->last = NULL;
	tmp->act = NULL;
	return tmp;
}

void tlist_free(tlist *list)
{
	if (list == NULL) {
		return;
	}
	tlist_elem *tmp;
	while (list->first != NULL) {
		tmp = list->first;
		list->first = list->first->next;
		tree_free(tmp->tree);
		free(tmp);
	}
	free(list);
}

tlist_elem *tlist_insert_last_node(tlist *list, tree_node *node)
{
	if (list == NULL) {
		print_error_msg("Error: tlist_insert_last(): NULL list accepted!");
		return NULL;
	}
	tlist_elem *tmp = malloc(sizeof (tlist_elem));
	if (tmp == NULL) {
		print_error_msg("Error: tlist_insert_last: malloc() failed!");
		return NULL;
	}
	tmp->tree = node;
	tmp->next = NULL;
	tmp->prev = list->last;
	if (list->last != NULL) {
		list->last->next = tmp;
	} else {
		list->first = tmp;
	}
	list->last = tmp;
	return tmp;
}

tlist_elem *tlist_insert_first_node(tlist *list, tree_node *node)
{
	if (list == NULL) {
		print_error_msg("Error: tlist_insert_first(): NULL list accepter!");
		return NULL;
	}
	tlist_elem *tmp = malloc(sizeof (tlist_elem));
	if (tmp == NULL) {
		print_error_msg("Error: tlist_insert_first: malloc() failed!");
		return NULL;
	}
	tmp->tree = node;
	tmp->prev = NULL;
	tmp->next = list->first;
	if (list->first != NULL) {
		list->first->prev = tmp;
	} else {
		list->last = tmp;
	}
	list->first = tmp;
	return tmp;
}

tlist_elem *tlist_insert_last(tlist *list)
{
	if (list == NULL) {
		print_error_msg("Error: tlist_insert_last(): NULL list accepted!");
		return NULL;
	}
	tlist_elem *tmp = malloc(sizeof (tlist_elem));
	if (tmp == NULL) {
		print_error_msg("Error: tlist_insert_last: malloc() failed!");
		return NULL;
	}
	tmp->next = NULL;
	tmp->prev = list->last;
	if (list->last != NULL) {
		list->last->next = tmp;
	} else {
		list->first = tmp;
	}
	list->last = tmp;
	list->act = tmp;
	return tmp;
}

tlist_elem *tlist_insert_first(tlist *list)
{
	if (list == NULL) {
		print_error_msg("Error: tlist_insert_first(): NULL list accepter!");
		return NULL;
	}
	tlist_elem *tmp = malloc(sizeof (tlist_elem));
	if (tmp == NULL) {
		print_error_msg("Error: tlist_insert_first: malloc() failed!");
		return NULL;
	}
	tmp->prev = NULL;
	tmp->next = list->first;
	if (list->first != NULL) {
		list->first->prev = tmp;
	} else {
		list->last = tmp;
	}
	list->first = tmp;
	return tmp;
}

tlist_elem *tlist_first(tlist *list)
{
	list->act = list->first;
	return list->act;
}

tlist_elem *tlist_last(tlist *list)
{
	list->act = list->last;
	return list->act;
}

tlist_elem *tlist_succ(tlist *list)
{
	if (list->act != NULL) {
		list->act = list->act->next;
	}
	return list->act;
}

tlist_elem *tlist_prev(tlist *list)
{
	if (list->act != NULL) {
		list->act = list->act->prev;
	}
	return list->act;
}

stack *stack_init(int size)
{
	if (size < 0) {
		print_error_msg("Error: stack_init: negative size!");
		return NULL;
	}

	stack *tmp = malloc(sizeof (stack));
	if (tmp == NULL) {
		print_error_msg("Error: stack_init: malloc() failed!");
		return NULL;
	}

	tmp->nodes = calloc(size + 1, sizeof (tree_node *));

	if (tmp->nodes == NULL) {
		print_error_msg("Error: stack_init: calloc() failed!");
		free(tmp);
		return NULL;
	}

	tmp->max = size;
	tmp->top = -1;
	return tmp;
}

tree_node *stack_top(stack *s)
{
	if (s == NULL) {
		return NULL;
	}
	if (s->top >= 0) {
		return s->nodes[s->top];
	}
	return NULL;
}

tree_node *stack_pop(stack *s)
{
	if (s == NULL) {
		return NULL;
	}
	if (s->top >= 0) {
		return s->nodes[(s->top)--];
	}
	return NULL;
}

int stack_push(stack *s, tree_node *node)
{
	if (s == NULL) {
		print_error_msg("Error: stack_push: NULL stack pointer!");
		return ERR_INTERNAL;
	}

	if (s->top == s->max - 1) {
		s->max += 10;
		s->nodes = realloc(s->nodes, (s->max + 1) * (sizeof (tree_node *)));
		if (s->nodes == NULL) {
			print_error_msg("Error: stack_push: realloc() failed!");
			return ERR_INTERNAL;
		}
	}
	s->nodes[++(s->top)] = node;
	return ERR_OK;
}

void stack_clear(stack *s)
{
	if (s == NULL) {
		return;
	}
	if (s->nodes == NULL) {
		return;
	}
	for (int i = 0; i <= s->top; i++) {
		tree_free(s->nodes[i]);
	}
	s->top = -1;
}

void stack_free(stack *s)
{
	if (s != NULL) {
		stack_clear(s);
		free(s->nodes);
		free(s);
	}
}

stack_i *stack_i_init(int size)
{
	if (size < 0) {
		print_error_msg("Error: stack_init: negative size!");
		return NULL;
	}

	stack_i *tmp = malloc(sizeof (stack_i));
	if (tmp == NULL) {
		print_error_msg("Error: stack_i_init: malloc() failed!");
		return NULL;
	}

	tmp->indexes = calloc(size + 1, sizeof (int));

	if (tmp->indexes == NULL) {
		print_error_msg("Error: stack_i_init: calloc() failed!");
		free(tmp);
		return NULL;
	}

	tmp->max = size;
	tmp->top = -1;
	return tmp;
}

int stack_i_top(stack_i *s)
{
	if (s == NULL) {
		return STACK_UNDERFLOW;
	}
	if (s->top >= 0) {
		return s->indexes[s->top];
	}
	return STACK_UNDERFLOW;
}

int stack_i_pop(stack_i *s)
{
	if (s == NULL) {
		return STACK_UNDERFLOW;
	}
	if (s->top >= 0) {
		return s->indexes[(s->top)--];
	}
	return STACK_UNDERFLOW;
}

int stack_i_push(stack_i *s, int index)
{
	if (s == NULL) {
		print_error_msg("Error: stack_i_push: NULL stack pointer!");
		return ERR_INTERNAL;
	}

	if (s->top == s->max - 1) {
		s->max += 10;
		s->indexes = realloc(s->indexes, (s->max + 1) * (sizeof (int)));
		if (s->indexes == NULL) {
			print_error_msg("Error: stack_i_push: realloc() failed!");
			return ERR_INTERNAL;
		}
	}
	s->indexes[++(s->top)] = index;
	return ERR_OK;
}

void stack_i_clear(stack_i *s)
{
	if (s == NULL) {
		return;
	}
	if (s->indexes == NULL) {
		return;
	}

	s->top = -1;
}

void stack_i_free(stack_i *s)
{
	if (s != NULL) {
		stack_i_clear(s);
		free(s->indexes);
		free(s);
	}
}