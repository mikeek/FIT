#include "htable.h"

void htable_statistics(htable_t *table)
{
	if (table == NULL) {
		return;
	}
	int aver = 0;
	int min = -1;
	int max = -1;
	int local = 0;
	struct htable_listitem *item;
	for (int i = 0; i < table->htable_size; i++) {
		for (item = (table->h_item)[i]; item != NULL; item = item->next) {
			aver++;
			local++;
		}
		if (local > max) {
			max = local;
		}
		if ((min == -1) || (local < min)) {
			min = local;
		}
		local = 0;
	}
	if (table->htable_size > 0) {
		aver /= table->htable_size;
		printf("Prumerna delka zaznamu: %d\n", aver);
	}
	printf("Minimalni delka zaznamu: %d\n", min);
	printf("Maximalni delka zaznamu: %d\n", max);
	return;
}