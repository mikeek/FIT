/* 
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File:   tree.h
 * Authors:
 *		Michal Kozubík,		xkozub03
 *
 * Created on 11.10.2013
 */

#ifndef TREE_H
#define	TREE_H

#include "scanner.h"
#include "ial.h"

struct htable_elem_t;

/* 
 * struktura pro derivační strom - pravděpodobně bude stejná jako AST, takže by
 * se později mohla pojmenovat nějak lépe a přidat do ní potřebná data
 */
typedef struct tree_node_t {
	struct tree_node_t *left, *right;
	struct htable_elem_t *elem;
} tree_node;

typedef struct tlist_elem_t {
	struct tlist_elem_t *prev, *next;
	tree_node *tree;
} tlist_elem;

typedef struct tlist_t {
	tlist_elem *first, *last, *act;
} tlist;

typedef struct stack_t {
	tree_node **nodes;
	int top, max;
} stack;

typedef struct stack_i_t {
	int *indexes;
	int top, max;
} stack_i;


tree_node *node_init();
tree_node *add_left_child(tree_node *parent);
tree_node *add_right_child(tree_node *parent);
void tree_free(tree_node *node);

tlist *tlist_init();
tlist_elem *tlist_insert_last_node(tlist *list, tree_node *node);
tlist_elem *tlist_insert_first_node(tlist *list, tree_node *node);
tlist_elem *tlist_insert_last(tlist *list);
tlist_elem *tlist_insert_first(tlist *list);
tlist_elem *tlist_first(tlist *list);
tlist_elem *tlist_last(tlist *list);
tlist_elem *tlist_succ(tlist *list);
tlist_elem *tlist_prev(tlist *list);
void tlist_free(tlist *list);

stack *stack_init(int size);
tree_node *stack_pop(stack *s);
tree_node *stack_top(stack *s);
int stack_push(stack *s, tree_node *node);
void stack_clear(stack *s);
void stack_free(stack *s);

stack_i *stack_i_init(int size);
int stack_i_pop(stack_i *s);
int stack_i_top(stack_i *s);
int stack_i_push(stack_i *s, int index);
void stack_i_clear(stack_i *s);
void stack_i_free(stack_i *s);


#endif	/* TREE_H */

