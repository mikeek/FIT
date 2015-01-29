
#include "io.h"

int fgetword(char *s, int max, FILE *f)
{
	int c;
	int len = 0;
	int end = 0;
	if (f == NULL) {
		return len;
	}
	while (((c = fgetc(f)) != EOF) && (isspace(c))) {
		continue;
	}
	while ((c != EOF) && (!isspace(c))) {
		if (len < max) {
			s[len] = c;
			len++;
			end++;
		} else {
			len++;
		}
		c = fgetc(f);
	}
	if (end > 0) {
		s[end] = '\0';
	}
	if (c == EOF) {
		return EOF;
	}
	return len;
}