#include "htable.h"

unsigned int hash_function(const char *str, unsigned htable_size)
{
	unsigned int h = 0;
	unsigned char *p;
	for (p = (unsigned char*) str; *p != '\0'; p++) {
		h = 31 * h + *p;
	}
	return (h % htable_size);
}