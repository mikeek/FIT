#include "htable.h"

void htable_free(htable_t *table)
{
	if (table == NULL) {
		return;
	}
	htable_clear(table);
	free(table);
}