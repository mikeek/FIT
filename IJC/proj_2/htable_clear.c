#include "htable.h"

void htable_clear(htable_t *table)
{
	if (table == NULL) {
		return;
	}
	struct htable_listitem *item;
	for (int i = 0; i < table->htable_size; i++) {
		item = (table->h_item)[i];
		while (item != NULL) {
			(table->h_item)[i] = item->next;
			free(item);
			item = (table->h_item)[i];
		}
	}
	free(table->h_item);
}