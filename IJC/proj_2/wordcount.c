/* wordcount.c
 * Reseni IJC-DU2, priklad b) 6.4.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Program cte ze stdin a po ukonceni vstupu vypise vsechna nalezena slova,
 * pocet jejich vyskytu a nakonec statistiky o hashovaci tabulce
 */

#include <stdio.h>
#include <ctype.h>
#include "io.h"
#include "error.h"
#include <string.h>
#include "htable.h"

#define IN   0
#define OUT  1
#define SKIP 2

#define MAX_LEN 255
/* Velikost hashovaci tabulky. Cim mene prvku je v jednotlivych seznamech,
 * tim rychleji se k nim da pristupovat. Z toho plyne, ze naprosto idealni
 * tabulka by mohla vypadat tak, ze v kazdem seznamu (na kazdem indexu tabulky)
 * je jen 1 slovo. Na druhou stranu je zbytecne vytvaret prilis velkou tabulku,
 * jelikoz vsupem muze byt soubor s malo slovy a tudiz by tabulka byla zbytecne
 * velka (= nevyhoda staticke velikosti tabulky). Problem by resila dynamicka 
 * velikost tabulky s predem odhadnutym (nebo presne zjistenym) poctem slov ve
 * vstupnim souboru
 */
#define HT_SIZE 1000

void printItem(struct htable_listitem *item)
{
	if (item != NULL) {
		printf("%s\t%d\n", item->key, item->data);
	}
}

int main(void)
{
	htable_t *table = htable_init(HT_SIZE);
	if (table == NULL) {
		Error("Chyba pri alokaci pameti pro tabulku!");
		return -1;
	}
	struct htable_listitem *item;
	char word[MAX_LEN + 1];
	int len;
	int skipped = 0;
	while ((len = fgetword(word, MAX_LEN, stdin)) != EOF) {
		if ((len > strlen(word)) && (!skipped)) {
			Error("Nektera slova byla prilis dlouha a doslo k jejich orezani!");
			skipped = 1;
		}
		item = htable_lookup(table, word);
		if (item == NULL) {
			Error("Chyba pri alokaci pameti pro nove slovo!");
			free(table);
			return -1;
		}
		(item->data)++;
	}
	htable_foreach(table, &printItem);
	//     htable_statistics(table);
	htable_free(table);
	return 0;
}