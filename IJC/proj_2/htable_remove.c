#include "htable.h"

void htable_remove(htable_t *table, char *key)
{
	if ((table == NULL) || (key == NULL)) {
		return;
	}
	int i = hash_function(key, table->htable_size);
	struct htable_listitem *item;
	struct htable_listitem *lastItem;
	item = (table->h_item)[i];
	lastItem = item;
	while (item != NULL) {
		if (strcmp(key, item->key) == 0) {
			lastItem->next = item->next;
			free(item);
			break;
		}
		lastItem = item;
		item = item->next;
	}
}