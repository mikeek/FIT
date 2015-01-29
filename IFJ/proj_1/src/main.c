/*
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File: main.c
 * Authors: 
 *		Lukáš Huták,		xhutak01
 *		Michal Kozubík,		xkozub03
 * 
 * Created on: 11.10.2013
 */


#include <stdio.h>

#include "parser.h"
#include "scanner.h"
#include "common.h"
#include "generator.h"

/* Hlavní tělo programu */
int main(int argc, char *argv[])
{
	if (argc > 2) {
		fprintf(stderr, "Příliš mnoho parametrů (%d), vyžadován 1!\n", argc - 1);
		return ERR_INTERNAL;
	}
	if (argc < 2) {
		fprintf(stderr, "Nezadána cesta ke zdrojovému souboru!\n");
		return ERR_INTERNAL;
	}

	int ret_val;
	FILE *source;

	/* otevření souboru */
	if ((source = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Nepovedlo se otevření souboru '%s'\n", argv[1]);
		return ERR_INTERNAL;
	}

	/* nastavení otevřeného souboru */
	set_source(source);

	/* inicializace parseru */
	ret_val = init_parser();

	/* inicializace generátoru */
	if (ret_val == ERR_OK) {
		ret_val = init_generator();
	}

	/* spuštění parseru */
	if (ret_val == ERR_OK) {
		ret_val = parse();
	}

	/* spuštění interpretu */
	if (ret_val == ERR_OK) {
		ret_val = interpret_run();
	}

	/* provedení úklidu */
	free_all_generator();
	free_all_parser();
	fclose(source);
	return ret_val;
}

