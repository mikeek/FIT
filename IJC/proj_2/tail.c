/* tail.c
 * Reseni IJC-DU2, priklad a) 26.3.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * Program vypise ze zadaneho vstupniho souboru n radku. Pri nezadani souboru
 * cte ze stdin. Pokud je zadan parametr -n +X tak vypisuje od radku X az
 * do konce souboru. Pri nezadani poctu radku je implicitne nastaveno 10
 */

#include <stdio.h>
#include <string.h>
#include "error.h"

#define LINE_LEN 1024
#define DEF_LINES 10

enum ERR {
	NOT_SET = -1, EOK, BAD_ARG
};

int skip(FILE *f, char end);
void printFrom(FILE *f, int fromline);
void printLines(FILE *f, int lines);

int skip(FILE *f, char end)
{
	int c;
	while (((c = fgetc(f)) != end) && (c != EOF)) {
		continue;
	}
	if (c == EOF) return 1;
	return 0;
}

void printFrom(FILE *f, int fromline)
{
	int len;
	int cnt = 0;
	int skipped = 0;
	char buff[LINE_LEN];
	strncpy(buff, "", LINE_LEN);
	while (fgets(buff, sizeof (buff), f) != NULL) {
		cnt++;
		len = strlen(buff);
		if (buff[len - 1] != '\n') {
			if (skip(f, '\n') == 0) {
				buff[len - 1] = '\n';
				if (skipped == 0) {
					skipped = 1;
					fprintf(stderr, "Prilis velka delka radku!\n");
				}
			}
		}
		if (cnt >= fromline)
			printf("%s", buff);
	}
}

void printLines(FILE *f, int lines)
{
	char buff[lines][LINE_LEN];
	int len;
	int skipped = NOT_SET;
	int last = 0;
	int more = 0;
	for (int i = 0; i < lines; i++)
		strncpy(buff[i], "", LINE_LEN);
	while (fgets(buff[last], LINE_LEN, f) != NULL) {
		len = strlen(buff[last]);
		if (buff[last][len - 1] != '\n') {
			if (skip(f, '\n') == 0) {
				buff[last][len - 1] = '\n';
				if (skipped == NOT_SET) {
					skipped = 1;
				}
			}
		}
		last++;
		if (last == lines) {
			last = 0;
			more = 1;
		}
	}
	if (skipped != NOT_SET)
		fprintf(stderr, "Prilist velka delka radku!\n");
	if (more == 0) {
		for (int i = 0; i <= last; i++)
			printf("%s", buff[i]);
	} else {
		for (int i = last; i < lines; i++)
			printf("%s", buff[i]);
		for (int i = 0; i < last; i++)
			printf("%s", buff[i]);
	}
}

int main(int argc, char *argv[])
{
	char *filename = NULL;
	char *endptr;
	int fromline = NOT_SET;
	int lines = NOT_SET;
	//     Zpracovani argumentu programu (getopts() zakazano)
	switch (argc) {
	case 1:
		lines = DEF_LINES;
		break;
	case 2:
		filename = argv[1];
		break;
	case 3:
		if (strcmp(argv[1], "-n") != 0) {
			FatalError("spatne zadane parametry!");
		}
		if (argv[2][0] == '+') {
			fromline = strtol(argv[2] + 1, &endptr, 10);
		} else {
			lines = strtol(argv[2], &endptr, 10);
		}
		if ((*endptr != '\0') || ((lines < 0) && (fromline < 0))) {
			FatalError("zadana spatna hodnota (%s)!", argv[2]);
		}
		break;
	case 4:
		if (strcmp(argv[1], "-n") != 0) {
			if (strcmp(argv[2], "-n") != 0) {
				FatalError("spatne zadane parametry!");
			}
			filename = argv[1];
			if (argv[3][0] == '+') {
				fromline = strtol(argv[3] + 1, &endptr, 10);
			} else {
				lines = strtol(argv[3], &endptr, 10);
			}
		} else {
			filename = argv[3];
			if (argv[2][0] == '+') {
				fromline = strtol(argv[2] + 1, &endptr, 10);
			} else {
				lines = strtol(argv[2], &endptr, 10);
			}
		}
		if ((*endptr != '\0') || ((lines < 0) && (fromline < 0))) {
			FatalError("zadana spatna hodnota!");
		}
		break;
	default:
		FatalError("prilis mnoho parametru (%d)!", argc - 1);
	}
	//     Otevreni vstupniho souboru (pripadne stdin)
	FILE *f;
	if (filename != NULL) {
		f = fopen(filename, "r");
		if (f == NULL) {
			FatalError("chyba pri otevirani souboru \"%s\"!", filename);
		}
	} else {
		f = stdin;
	}
	//     Zjisteni pozadovane akce
	if (fromline != NOT_SET) {
		printFrom(f, fromline);
	} else {
		if (lines == NOT_SET) {
			lines = DEF_LINES;
		}
		if (lines > 0) {
			printLines(f, lines);
		}
	}
	fclose(f);
	return 0;
}