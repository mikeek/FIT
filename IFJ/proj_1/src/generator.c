/*
 * Project:  Implementacia interpretu imperativneho jazyka IFJ13
 * File: generator.c
 * Authors: 
 *		Zuzana Lietavcova,	xlieta00
 *	    Marek Hurta,		xhurta01
 * 
 * Created on: 30.10.2013
 */

#include <stdio.h>
#include <stdlib.h>

#include "generator.h"
#include "ial.h"
#include "str.h"
#include "common.h"

#define LABEL 10

tinst_item **lab_table = NULL;
fce_lists *i_lists = NULL;
int lab_table_size = LABEL;

/* \brief Inicializuje zoznam labelov, pointer ulozi do globalnej premennej
 * 	    Vytvorenie hlavnej struktury v kotrej su ulozene ukazatele na
 * 		jednotlive instrukcne sady, prvy ukazatel ukazuje vzdy na 
 * 		instrukcnu sadu hlavneho programu, dalsie polozky su 
 * 		pridavane pri vyskyte funkcie a obsahuju ukazatel 
 * 		na instrukcnu sadu specificku pre kazdu fce.	
 * \return chybovy kod
 */
errors init_generator(void)
{
	/* Vytvorenie hlavnej struktury pre ukazatele na instrukcne sady */
	if ((i_lists = malloc(sizeof (fce_lists))) == NULL) {
		return ERR_INTERNAL;
	}

	/* Vytvori hlavny zoznam instrukcii */
	if ((i_lists->first = malloc(sizeof (fce_item))) == NULL) {
		return ERR_INTERNAL;
	}

	i_lists->act = i_lists->first;
	i_lists->last = i_lists->first;

	/* Inicializovanie hl. zoznamu, pre prehladnost nastaveny na NULL */
	i_lists->first->id = NULL;

	/* Vytvori instrukcnu pasku hlavneho zoznamu */
	if ((i_lists->first->list = list_init()) == NULL) {
		return ERR_INTERNAL;
	}
	i_lists->first->next = NULL;

	/*Label table*/
	if ((lab_table = malloc(LABEL * sizeof (tinst_item*))) == NULL) {
		return ERR_INTERNAL;
	}

	return ERR_OK;
}

/* \brief Uvolni vsetky zdroje, detail struktury je uvedeny v .h subore
 * 		
 */
void free_all_generator(void)
{
	fce_item *tmp;
	tinst_item *tmp2;

	/* Ak je zoznam prazdny nic neuvolnuj */
	if (i_lists == NULL) {
		return;
	}

	while (i_lists->first != NULL) {
		/* Ulozenie nasledujucej instrukcnej sady */
		tmp = i_lists->first->next;

		/* Cyklus prechadza urcenu instrukcnu pasku unikatnu pre kazdu fce 
		 * jedna sa o odstranovanie na najnizsej urovni, cize sa priamo 
		 * uvolnuju jednotlive instrukcie 
		 * Do tmp2 sa ulozi instrukcia pred instrukciou ulozenou vo first
		 * Po uvolneni instrukcie vo first sa do first ulozi tmp2 cim je 
		 * zabezpecene previazanie 
		 */
		while (i_lists->first->list->first != i_lists->first->list->last) {
			tmp2 = i_lists->first->list->first->next;
			free(i_lists->first->list->first);
			i_lists->first->list->first = tmp2;
		}

		/* Uvolnenie prazdnej instrukcnej pasky a nastavenie na NULL */
		free(i_lists->first->list->first);
		i_lists->first->list->first = NULL;
		/* Uvolnenie struktury instrukcnej pasky a nastavenie na NULL */
		free(i_lists->first->list);
		i_lists->first->list = NULL;
		/* Uvolnenie pamati kde bola ulozena struktura instrukcnej sady */
		free(i_lists->first);
		/* Nastavenie nasledujucej struktury instrukcnej sady do first */
		i_lists->first = tmp;
	}
	/* Uvolennie poslednej struktury instrukcnej sady a nastavenie na NULL */
	free(i_lists->first);
	i_lists->first = NULL;
	/* Uvolnenie najvyssej urovne */
	free(i_lists);
	i_lists = NULL;

	/* Uvolnenie tabulky labelov a nastavenie na NULL */
	free(lab_table);
	lab_table = NULL;
}

/* \brief Vyhlada polozku pre funkciu v i_lists
 * 	 	
 * \return pointer na polozku - ak sa nasla
 */
fce_item *search_fce_item(htable_elem *id)
{
	fce_item *tmp = i_lists->first->next;

	while (tmp != NULL) {
		if (tmp->id == id) break;
		tmp = tmp->next;
	}

	return tmp;
}

/* \brief Vytvori strukturu do ktorej sa vlozi instrukcna sada novej funkcie
 * 	 	
 * \return pointer na strukturu
 */
fce_item *new_fce(htable_elem *id)
{
	fce_item *item = NULL;

	/* Alokacia pamate pre strukturu */
	if ((item = malloc(sizeof (fce_item))) == NULL) {
		return NULL;
	}

	/* Nastavenie id priznaku podla id ktore je ulozene v tabulke symbolov */
	item->id = id;

	/* Inicializacna rutina, ktora uz nainicializuje primo instrukcnu sadu */
	if ((item->list = list_init()) == NULL) {
		return NULL;
	}
	item->next = NULL;

	item->backup.first = NULL;
	item->backup.last = NULL;

	/* Previazanie ukazatelov v hlavnej strukture */
	i_lists->last->next = item;
	i_lists->last = item;

	return item;
}

/* \brief Inicializuje instrukcnu sadu 
 *
 * \return pointer na zoznam alebo NULL
 */
tinst_list *list_init(void)
{
	tinst_list *list;

	/* Alokacia pamate priamo pre instrukcnu sadu */
	if ((list = malloc(sizeof (tinst_list))) == NULL) {
		return NULL;
	}

	/* Uprava ukazatelov */
	list->first = NULL;
	list->last = NULL;

	return list;
}

/* \brief Vlozi novu instrukciu do instrukcnej sady, spravna instrukcna sada
 * 		je definovana parametrom *list
 *
 * \param[in] list Instrukcna sada unikatna pre kazdu funkciu
 * \param[in] type Typ instrukcie
 * \param[in] op1  Adresa prveho operandu 
 * \param[in] op2  Adresa druheho operandu
 * \param[in] result  Adresa vysledku 
 *
 * \return pointer na polozku alebo NULL
 */
tinst_item *list_insert(tinst_list *list, instructions type, int label, htable_elem *op1, htable_elem *op2, htable_elem *result)
{
	tinst_item *item;

	/* Alokacia pamate pre jednu instrukciu */
	if ((item = malloc(sizeof (tinst_item))) == NULL) {
		return NULL;
	}

	/* Nastavenie obsahu instrukcie
	 * Polozka label je union a obsahuje cislo ktorym je mozne identifikovat
	 * konkretny label, adresu instrukcie a ukazatel na strukturu (fce_item) 
	 * ktora obsahuje <*id,backup,*list>
	 */
	item->type = type;
	item->label.label = label;
	item->op1 = op1;
	item->op2 = op2;
	item->result = result;
	item->next = NULL;

	/* Korekcia ukazatelov */
	if (list->last != NULL) {
		list->last->next = item;
	} else {
		list->first = item;
	}
	list->last = item;

	return item;
}

/* \brief Vlozi do tabulky novy label ktory je identifikovany cislom vola 
 * 		sa pri vyskyte instrukcie I_LBL
 *
 * \param[in] index do tabuky labelov
 * \return pointer na zoznam alebo NULL
 */

tinst_item *label_insert(int index)
{
	/* Realokacia pamate pri presiahnuti aktualnej velkosti tabulky */
	while (index >= lab_table_size) {
		if (label_realloc() == NULL) {
			return NULL;
		}
	}

	/* Vlozenie ukazatela na poslednu instrukciu z instrukcnej sady, 
	 * ktora je aktivna 
	 */
	lab_table[index] = i_lists->act->list->last;

	return lab_table[index];
}

/* \brief Zvacsi velkost tabulky labelov o konstantu LABEL
 *
 * \return pointer na tabulku alebo NULL pri neuspechu
 */
tinst_item **label_realloc(void)
{
	lab_table_size += LABEL;
	lab_table = realloc(lab_table, (lab_table_size + 1) * sizeof (tinst_item *));

	return lab_table;
}

/* \brief Riadiaca funkcia ktora vykonava akcie na zaklade prichadzajucich 
 * 		instrukcii
 * 
 * \return Pri vnutornej chybe v niektorych z pouzivanych fce(alokacia pamate) 
 * 		vracia ERR_INTERNAL, inak vracia ERR_OK
 */
errors store_instruction(instructions type, int label, htable_elem *op1, htable_elem *op2, htable_elem *result)
{
	/* TEST neskor zakomentovat */
	////////////////////////////////////////////////
	//	print_instruction(type, label, op1, op2, result);
	/////////////////////////////////////////////////


	/* Ulozenie prvej struktrury(hlavna instrukcna sada) */
	fce_item *head_list = i_lists->first;

	tinst_item *item = NULL;
	fce_item *fce_item_tmp = NULL;

	/* Pri prichode novej instrukcie okrem I_FCELBL sa do polozky item vlozi 
	 * nova, vytvorena instrukcia, ktorej obsah je definovany parametrami 
	 * tejto funkcie
	 */
	if (type != I_FCELBL) {
		if ((item = list_insert(i_lists->act->list, type, label, op1, op2, result)) == NULL) {
			return ERR_INTERNAL;
		}
	}

	/* Vykonavanie akcii na zaklade typu instrukcie */
	switch (type) {
	case I_LBL:
		/* Vytvori polozku v tabulke labelov, indexom do tejto tabulky je 
		 * cislo labelu 
		 */
		if (label_insert(label) == NULL) {
			return ERR_INTERNAL;
		}
		break;
	case I_FCELBL:
		/* Skontroluje ci uz existuje instrukcna sada pre danu funkciu
		 * ak nie, tak ho vytvori, pri prichode tejto instrukcie 
		 * sa prepina medzi instrukcnymi sadami kedze sa v programe
		 * vyskytla funckia musi sa pracovat s jej instrukcnou sadou
		 */
		if ((fce_item_tmp = search_fce_item(op1)) == NULL) {
			if ((fce_item_tmp = new_fce(op1)) == NULL) {
				return ERR_INTERNAL;
			}
		}

		/* Prepnutie medzi instrukcnymi sadami, aktivna bude prave najdena 
		 * alebo vytvorena instrukcna sada
		 */
		i_lists->act = fce_item_tmp;
		break;
	case I_TJMP:
	case I_FJMP:
	case I_JMP:
		/* Do instrukcie sa vlozi pointer na polozku s isntrukciou, na ktoru sa ma skocit */
		//			tmp = item->label.label;
		//			/* Kontrola medzi - ak label, na ktory sa ma skocit 
		//			 * je za hranicou aktualnej velkosti tabulky labelov(lab_table_size),
		//			 * tabulka sa zvacsi
		//			 */
		//			while (tmp >= lab_table_size) {
		//				if (label_realloc() == NULL) {
		//					return ERR_INTERNAL;
		//				}
		//			}
		//			item->label.addr = &lab_table[tmp]; 
		break;
	case I_FCEJMP:
		/* Zisti ci uz bola funkcia definovana
		 *	ak ano: vlozi do instrukcie adresu jej instrukcnej sady
		 *	ak nie: vytvori novu sadu instrukcii a vlozi odkaz nanu do instrukcie
		 *	Pokial ide o vstavanu funkciu, nevytvara sa novy zoznam
		 */
		if (op1->subtype == KEY_FALSE) {
			if ((fce_item_tmp = search_fce_item(op1)) == NULL) {
				if ((fce_item_tmp = new_fce(op1)) == NULL) {
					return ERR_INTERNAL;
				}
			}
			item->label.fce_item = fce_item_tmp;
		}
		break;
	case I_BACK:
		/* Nastavi aktualny zoznam instrukcii na hlavny zoznam */
		i_lists->act = head_list;
		break;
	default:
		break;
	}
	//	if (type == I_LBL || type == I_JMP || type == I_EQ || type == I_NEQ || type == I_FJMP) {
	//		for (int i = 0; i <= lab_table_size; i++) {
	//			printf("%d) %s\n", i, (lab_table[i]==NULL)?"NULL":"NEnull");
	//		}
	//	}
	return ERR_OK;
}


/********** TESTOVACI VYPIS ****************/
/*
void testovaci_vypis_generatoru(void)
{
	fce_item *fce = i_lists->first;
	tinst_item *tmp;

	printf("+----------------------------+\n");
	printf("| START OF GENERATOR OUTPUT  |\n");
	printf("+----------------------------+\n");

	printf("Hlavny zoznam instrukcii\n");
	printf("----------------------------\n");
	if (i_lists->first == NULL) {
		return;
	}

	if (i_lists->first == i_lists->last){
		printf("Je len 1 zoznam instrukcii\n");
	}

	while (fce != NULL) {
		if (fce->id != NULL) {
			printf("----------------------------\n");
			printf("Funkcia s menom %s\n", fce->id->name.str);
			printf("----------------------------\n");
		}
		//printf("%d\n",fce->list->first->label.label);
		tmp = fce->list->first;
		while (tmp != NULL) {
			printf("%s\n", ins_str[tmp->type]);
			tmp = tmp->next;
		}
		fce = fce->next;
	}

	printf("+--------------------------+\n");
	printf("| END OF GENERATOR OUTPUT  |\n");
	printf("+--------------------------+\n");
}
 */
