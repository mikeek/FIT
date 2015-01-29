#include "htable.h"

void htable_foreach(htable_t *table, void (*function)(struct htable_listitem *))
{
	if (table == NULL) {
		return;
	}
	struct htable_listitem *item;
	for (int i = 0; i < table->htable_size; i++) {
		for (item = (table->h_item)[i]; item != NULL; item = item->next) {
			(*function)(item);
		}
	}
}