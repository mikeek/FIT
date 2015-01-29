#include "htable.h"

struct htable_listitem *htable_lookup(htable_t *table, char *key)
{
	if ((table == NULL) || (key == NULL)) {
		return NULL;
	}
	int i = hash_function(key, table->htable_size);
	struct htable_listitem *item;
	item = (table->h_item)[i];
	while (item != NULL) {
		if (strcmp(key, item->key) == 0) {
			return item;
		}
		item = item->next;
	}
	item = malloc(sizeof (struct htable_listitem) +strlen(key) + 1);
	if (item == NULL) {
		return NULL;
	}
	strncpy(item->key, key, strlen(key) + 1);
	item->data = 0;
	item->next = (table->h_item)[i];
	(table->h_item)[i] = item;
	return item;
}