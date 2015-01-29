#include "htable.h"

htable_t *htable_init(int size)
{
	htable_t *table;
	table = malloc(sizeof (htable_t));
	if (table == NULL) {
		return NULL;
	}
	table->htable_size = size;
	table->h_item = calloc(size, sizeof (struct htable_listitem *));
	if (table->h_item == NULL) {
		free(table);
		return NULL;
	}
	return table;
}