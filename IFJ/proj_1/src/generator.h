/*
 * Project:  Implementacia interpretu imperativneho jazyka IFJ13
 * File: generator.h
 * Authors: 
 *		Zuzana Lietavcova,	xlieta00
 *	    Marek Hurta,		xhurta01
 * 
 * Created on: 30.10.2013
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "ial.h"
#include "interpret.h"

/* Instrukcie */
typedef enum {
	I_MUL, /* * */
	I_DIV, /* / */
	I_ADD, /* + */
	I_SUB, /* - */
	I_CON, /* . */
	I_LE, /* < */
	I_LEQ, /* <= */
	I_GR, /* > */
	I_GRQ, /* >= */
	I_EQ, /* === */
	I_NEQ, /* !== */
	I_ASGN, /* = */
	I_JMP, /* nepodmieny skok */
	I_TJMP, /* true skok */
	I_FJMP, /* false skok */
	I_FCEJMP, /* function jump */
	I_BACK, /* návrat z f-ce/programu */
	I_PUSH, /* je treba pushovat na zasobnik parametry f-ce pred skokem do fce */
	I_POP, /* a taky popovat primo ve funkci */
	I_LBL, /* navesti */
	I_FCELBL, /* navesti funkce - treba bude jednodussi to rozlisit */
	I_RET, /* navraceni hodnoty funkce/programu */
	I_CALL, /* detekce volani funkce */
	I_END
} instructions;

/* Polozka ktora bude ulozena v instrukcnom zozname */
typedef union label_t {
	int label;
	struct fce_item_t *fce_item;
} label_u;

typedef struct tinst_item_t {
	instructions type;
	label_u label; /* union obsahuje: int label, adresa instrukcie, uk na konkretny zoznam inst */
	htable_elem *op1;
	htable_elem *op2;
	htable_elem *result;
	struct tinst_item_t *next;
} tinst_item;

/* Linearny zoznam instrukcii */
typedef struct tinst_list_t {
	tinst_item *first;
	tinst_item *last;
} tinst_list;

/* Polozka fcelist */ /* Predavame ukazatel na nu pri FCEJMP */

typedef struct fce_item_t {
	htable_elem *id; /* Hlavny zoznam bude na zaciatku a id = NULL*/
	backup_t backup; /* Zálohy tabulky symbolů */
	tinst_list *list;
	struct fce_item_t *next;
} fce_item;

/* Zoznam fcelists */
typedef struct fce_lists_t {
	fce_item *first;
	fce_item *act;
	fce_item *last;
} fce_lists;


/* Vytvori a vlozi na koniec zoznamu novu polozku */
tinst_item *list_insert(tinst_list *list, instructions type, int label, htable_elem *op1, htable_elem *op2, htable_elem *result);
/* Vytvori a inicializuje zoznam instrukcii */
tinst_list *list_init(void);
/* Vyhlada zoznam instrukcii patriaci k funkcii*/
fce_item *search_fce_item(htable_elem *id);
/* Vytvori a incializuje polozku pre novu funkciu v i_lists */
fce_item *new_fce(htable_elem *id);
/* Prida novu polozku do tabulky labelov */
tinst_item *label_insert(int index);
/* Zvacsi velkost tabulky labelov o konstantu LABEL */
tinst_item **label_realloc(void);

/* Vytvori a inicializuje tabulku labelov a zoznam fcelists v maine */
errors init_generator();
/* Uvolni zdroje */
void free_all_generator();

/* Vyvtvori instrukciu a pripoji ju do zoznamu */
errors store_instruction(instructions type, int label, htable_elem *op1, htable_elem *op2, htable_elem *result);

/*TEST*/
void testovaci_vypis_generatoru(void);

#endif /* INSTRUCT_H */
