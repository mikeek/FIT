#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct htable_listitem {
	int data;
	struct htable_listitem *next;
	char key[];
};

typedef struct THtable {
	int htable_size;
	struct htable_listitem **h_item;
} htable_t;

unsigned int hash_function(const char *str, unsigned htable_size);

htable_t *htable_init(int size);

struct htable_listitem *htable_lookup(htable_t *table, char *key);

void htable_foreach(htable_t *table, void (*function)(struct htable_listitem *));

void htable_remove(htable_t *table, char *key);

void htable_clear(htable_t *table);

void htable_free(htable_t *table);

void htable_statistics(htable_t *table);


#endif