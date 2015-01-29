/* 
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File:   scanner.c
 * Authors:
 *		Lukáš Horký,		xhorky21
 *		Lukáš Huták,		xhutak01
 *
 * Created on 11.10.2013
 */


#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

/* Globální proměnné */
static inner_states scanner_state;	/* vnitrni stav scanneru */
static FILE *source_file;			/* ukazatel na otevřený soubor */
static int current_row = 1;			/* řádek se startovní pozicí posledního tokenu */
static int current_col = 1;			/* sloupec se startovní pozicí posledního tokenu */
static int row_add = 0;				/* délka posledního tokenu (rozdíl řádků) */
static int col_add = 0;				/* délka posledního tokenu (rozdíl sloupců) */
static bool_value first_token = TRUE; /* urcuje, zda ocekavat <?php */
static int macro_var = ERR_OK;		/* Pomocna promenna pro makro */

/* Ovladani vnitrnich chyb */
#define CHECK_ERR_VAL_INN(value, operation) \
do { \
	macro_var = value; \
    if (macro_var != ERR_OK) { \
        operation; \
        print_error_msg("internal error"); \
        return macro_var; \
    } \
} while(0)

/**
 * \brief Nastavení zdroj čtení tokenů
 * Nedetekuje pokud soubor není otevřený.
 * 
 * \param[in,out] source Ukazatel na již otevřený soubor
 */
void set_source(FILE *source)
{
	source_file = source;
}

/** \brief Pozice posledního tokenu
 * Tato funcke vrací pozici odkud se začal číst poslední token.
 * V případě lexikální chyby (ERR_LEXAN) odkazuje na začátek prvku, který tuto
 * chybu způsobil
 *
 * \param[out] row Řádek, na kterém se nachází token
 * \param[out] col Sloupec, na kterém začíná token
 */
void get_last_token_position(int *row, int *col)
{
	*row = current_row;
	*col = current_col;
}

/** \brief Vracení znaku na vstupní stream
 * Upravuje informace o pozici dalšího čteného tokenu
 *
 * \param[in] ch Vracený znak
 */
void token_put_char(char ch)
{
	/* úprava informace o aktuálně čtené pozici (zmenšení) */
	// TODO: ověřit zda se může vrátit znak konce řádku v souboru
	if (ch == '\n') {
		row_add--;
	} else {
		col_add--;
	}

	ungetc(ch, source_file);
}

/** \brief Přečtení jednoho znaku ze vstupního streamu
 * Upravuje informace o pozici dalšího čteného tokenu
 *
 * \return Znak přečtený ze streamu
 */
char token_get_char()
{
	/* úprava informace o aktuálně čtené pozici (zvětšení) */
	char temp = getc(source_file);
	if ((scanner_state == IN_DEFAULT && isspace(temp)) ||
		scanner_state == IN_COMMENT_LINE ||
		scanner_state == IN_COMMENT_BLOCK ||
		scanner_state == IN_COMMENT_BLOCK_ST) {
		if (temp == '\n') {
			current_row++;
			current_col = 1;
		} else {
			current_col++;
		}
		row_add = 0;
		col_add = 0;
	} else {
		if (temp == '\n') {
			row_add++;
			col_add = 0;
		} else {
			col_add++;
		}
	}

	return temp;
}

/** \brief Specifická detekce tokenu na základě řetězce
 * Rozpozná typ NULL, BOOL(tj. TRUE, FALSE), klíčová slova (else, if,...) a 
 * vestavěné funkce (get_string, get_substring,...) v předaném řetězci.
 * Pokud se nejedná ani o jeden jmenovaný prvek, token je vyplněn 
 * jako identifikátor funkce.
 * Pokud se nejedná o identifikátor funkce je paměť předaného parametru str
 * uvolněna.
 *
 * \param[in] str Ukazatel na řetězec, který bude prověřován
 * \param[out] token Vyplněný token s příslušnou specifikací
 */
void set_token_from_str(string *str, token_t * token)
{
	/* neni mozne pouzit switch, jelikoz se stringy nedaji switchovat
	   (neni to hodnota kterou by slo porovnavat normalne) */

	if (strCmpConstStr(str, "null") == 0) {
		token->type = T_NULL;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "true") == 0) {
		token->type = T_BOOLEAN;
		token->value.bool_val = TRUE;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "false") == 0) {
		token->type = T_BOOLEAN;
		token->value.bool_val = FALSE;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "if") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_IF;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "else") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_ELSE;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "elseif") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_ELSEIF;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "return") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_RETURN;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "while") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_WHILE;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "function") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_FUNCTION;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "boolval") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_BOOLVAL;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "doubleval") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_DOUBLEVAL;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "intval") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_INTVAL;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "strval") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_STRVAL;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "get_string") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_GET_STRING;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "put_string") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_PUT_STRING;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "strlen") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_STRLEN;
		strFree(str);
		return;
	}

	if (strCmpConstStr(str, "get_substring") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_GET_SUBSTRING;
		strFree(str);
		return;
	}
	if (strCmpConstStr(str, "find_string") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_FIND_STRING;
		strFree(str);
		return;
	}
	if (strCmpConstStr(str, "sort_string") == 0) {
		token->type = T_KEYWORD;
		token->value.int_val = KEY_SORT_STRING;
		strFree(str);
		return;
	}

	token->type = T_FUNCTION;
	token->value.str_val = *str;
}

/** \brief Získání dalšího tokenu
 * Z definovaného vstupního souboru čte jednotlivé znaky a z nich vyskládává
 * token. Jakmile je token určen, vyplní se předaná položka a funkce končí. 
 * Typy a "podtypy" tokenů viz hlavičkový soubor scanner.h
 *
 * \param[out] token Ukazatel, do nehož se zaznamenají informace o typu tokenu
 * \return Chybový kód dle errors. Pokud je vše v pořádku vrací E_OK
 */
errors get_token(token_t * token)
{
	if (token->type == T_STRING || token->type == T_VARIABLE || token->type == T_FUNCTION) {
		strFree(&token->value.str_val);
	}
	/* nastavit scanner na vychozi stav */
	scanner_state = IN_DEFAULT;

	/* aktualizace informace o pozici začátku čteného tokenu */
	current_row += row_add;
	current_col += col_add;
	row_add = 0;
	col_add = 0;

	/* pomocná proměnná */
	string str_temp;

	/* individualni zpracovani prvniho tokenu */
	if (first_token) {

		CHECK_ERR_VAL_INN(strInit(&str_temp),
		{});
		for (int i = 0; i < 6; i++) {
			CHECK_ERR_VAL_INN(strAddChar(&str_temp, token_get_char()), strFree(&str_temp));
		}
		if (strncmp(str_temp.str, "<?php", 5) == 0 &&
			isspace(str_temp.str[5]) != 0) { //srovnáme prvních 5 znaků
			token->type = T_START;
			first_token = FALSE;
			strFree(&str_temp);
			return ERR_OK;
		} else {
			strFree(&str_temp);
			print_error_msg("program not starting with '<?php'!");
			return ERR_SYNAN;
		}
	}


	while (1) {
		/* Načtení dalšího znaku */
		int next_char = token_get_char();

		switch (scanner_state) {
		case (IN_DEFAULT):
			/* Vychozi stav */

			/* Přeskočení všech bílých znaků a úprava info o začátku tokenu*/
			if (isspace(next_char)) {
				continue;
			}

			switch (next_char) {
			case (EOF):
				token->type = T_END;
				return ERR_OK;
			case (';'):
				/* vysledny stav: strednik */
				token->type = T_SEMICOL;
				return ERR_OK;
			case (','):
				/* vysledny stav: carka */
				token->type = T_COMMA;
				return ERR_OK;
			case ('+'):
				/* vysledny stav: plus */
				token->type = T_OPERATOR;
				token->value.int_val = OP_PLUS;
				return ERR_OK;
			case ('-'):
				/* vysledny stav: minus */
				token->type = T_OPERATOR;
				token->value.int_val = OP_MINUS;
				return ERR_OK;
			case ('*'):
				/* vysledny stav: nasobeni */
				token->type = T_OPERATOR;
				token->value.int_val = OP_MULTIPLY;
				return ERR_OK;
			case ('('):
				/* vysledny stav: leva kulata zavorka */
				token->type = T_BRACKET;
				token->value.int_val = B_LEFT_PAR;
				return ERR_OK;
			case (')'):
				/* vysledny stav: prava kulata zavorka */
				token->type = T_BRACKET;
				token->value.int_val = B_RIGHT_PAR;
				return ERR_OK;
			case ('{'):
				/* vysledny stav: leva slozena zavorka */
				token->type = T_BRACKET;
				token->value.int_val = B_LEFT_BRACE;
				return ERR_OK;
			case ('}'):
				/* vysledny stav: leva slozena zavorka */
				token->type = T_BRACKET;
				token->value.int_val = B_RIGHT_BRACE;
				return ERR_OK;
			case ('.'):
				/* vysledny stav: konkatenace */
				token->type = T_OPERATOR;
				token->value.int_val = OP_CONCAT;
				return ERR_OK;
				/* scanner_state = IN_DOT; */ //zaloha
				break;
			case ('='):
				scanner_state = IN_ONE_EQ;
				break;
			case ('/'):
				scanner_state = IN_SLASH;
				break;

			case ('"'):
				CHECK_ERR_VAL_INN(strInit(&str_temp),
				{});
				scanner_state = IN_STRING;
				break;
			case ('<'):
				scanner_state = IN_LESS;
				break;
			case ('>'):
				scanner_state = IN_GREATER;
				break;
			case ('!'):
				scanner_state = IN_EXCLAM;
				break;

			case ('$'):
				CHECK_ERR_VAL_INN(strInit(&str_temp),
				{});
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, '$'), strFree(&str_temp));
				scanner_state = IN_VARIABLE;
				break;
			default:
				if (isalpha(next_char) || next_char == '_') {

					CHECK_ERR_VAL_INN(strInit(&str_temp),
					{});
					CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
					scanner_state = IN_KEY_OR_ID;
				} else if (isdigit(next_char)) {

					CHECK_ERR_VAL_INN(strInit(&str_temp),
					{});
					CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
					scanner_state = IN_INTEGER;
				}
				else {
					print_error_msg("unknown token!");
					return ERR_LEXAN;
				}
				break;
			}
			break;
		case (IN_ONE_EQ):
			/* stav = */
			if (next_char == '=') {
				scanner_state = IN_DOUBLE_EQ;
			} else {
				/* vysledny stav: jedno rovna se */
				token->type = T_OPERATOR;
				token->value.int_val = OP_ASSIGN;
				token_put_char(next_char);
				return ERR_OK;
			}
			break;
		case (IN_DOUBLE_EQ):
			/* stav == */
			if (next_char == '=') {
				/* vysledny stav: tri rovna se */
				token->type = T_OPERATOR;
				token->value.int_val = OP_TRIPLE_EQ;
				return ERR_OK;
			} else {
				print_error_msg("unknown token!");
				return ERR_LEXAN;
			}
			break;
		case (IN_SLASH):
			/* stav / */
			switch (next_char) {
			case ('/'):
				scanner_state = IN_COMMENT_LINE;
				break;
			case('*'):
				current_col += 2; /* nutno započítat předchozí znaky */
				scanner_state = IN_COMMENT_BLOCK;
				break;
			default:
				/* vysledny stav: deleni */
				token->type = T_OPERATOR;
				token->value.int_val = OP_DIVIDE;
				token_put_char(next_char);
				return ERR_OK;
			}
			break;
		case (IN_STRING):
			/* stav uvnitr " */
			if (next_char == EOF) {
				strFree(&str_temp);
				print_error_msg("unexpected end of file!");
				return ERR_LEXAN;
			}

			if (next_char <= 31) {
				strFree(&str_temp);
				print_error_msg("invalid symbol in a string");
				return ERR_LEXAN;
			}

			switch (next_char) {
			case ('"'):
				/* vysledny stav: string */
				token->type = T_STRING;
				token->value.str_val = str_temp;
				if (str_escape_sequences(&token->value.str_val) != ERR_OK) {
					strFree(&str_temp);
					return ERR_LEXAN;
				}
				return ERR_OK;
			case ('\\'):
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
				scanner_state = IN_STRING_SLASH;
				break;
			default:
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
				break;
			}
			break;
		case (IN_STRING_SLASH):
			/* stav uvnitr " a posledni bylo \ */
			if (next_char <= 31) {
				strFree(&str_temp);
				print_error_msg("invalid symbol in a string");
				return ERR_LEXAN;
			}

			CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
			scanner_state = IN_STRING;

			/* v podstate me nezajima, co za tim \ bylo,
			   proste to jen pridam do stringu a vratim se
			   do stavu bez \ */
			break;
		case (IN_LESS):
			/* stav s nactenym < */
			if (next_char == '=') {
				/* vysledny stav: mensi nebo rovno */
				token->type = T_OPERATOR;
				token->value.int_val = OP_LESS_EQ;
				return ERR_OK;
			} else {
				/* vysledny stav: je mensi */
				token->type = T_OPERATOR;
				token->value.int_val = OP_LESS;
				token_put_char(next_char);
				return ERR_OK;
			}
			break;
		case (IN_GREATER):
			/* stav s nactenym > */
			if (next_char == '=') {
				/* vysledny stav: vetsi nebo rovno */
				token->type = T_OPERATOR;
				token->value.int_val = OP_GREATER_EQ;
				return ERR_OK;
			} else {
				/* vysledny stav: je mensi */
				token->type = T_OPERATOR;
				token->value.int_val = OP_GREATER;
				token_put_char(next_char);
				return ERR_OK;
			}
			break;
		case (IN_EXCLAM):
			/* stav s nactenym ! */
			if (next_char == '=') {
				scanner_state = IN_EXCLAM_EQ;
			} else {
				print_error_msg("unknown token!");
				return ERR_LEXAN;
			}
			break;
		case (IN_EXCLAM_EQ):
			/* stav s nactenym != */
			if (next_char == '=') {
				token->type = T_OPERATOR;
				token->value.int_val = OP_NOT_EQ;
				return ERR_OK;
			} else {
				print_error_msg("unknown token!");
				return ERR_LEXAN;
			}
			break;
		case (IN_COMMENT_LINE):
			/* stav za // */
			if (next_char == '\n') {
				scanner_state = IN_DEFAULT;
			} else if (next_char == EOF) {
				token->type = T_END;
				return ERR_OK;
			}
			break;
		case (IN_COMMENT_BLOCK):
			/* stav uvnitr "/ *" */
			if (next_char == '*') {
				scanner_state = IN_COMMENT_BLOCK_ST;
			} else if (next_char == EOF) {
				// komentář může být omylem neukončen na konci souboru
				print_error_msg("block comment is not ended");
				return ERR_LEXAN;
			}
			break;
		case (IN_COMMENT_BLOCK_ST):
			/* stav uvnitr "/ *" s nactenim "*" */
			if (next_char == '/') {
				scanner_state = IN_DEFAULT;
			} else if (next_char != '*') {
				scanner_state = IN_COMMENT_BLOCK;
			}
			break;
		case (IN_INTEGER):
			/* stav s nactenim celych cisel */
			if (isdigit(next_char)) {
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
			} else if (next_char == '.') {
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
				scanner_state = IN_DOUBLE;
			} else if (next_char == 'e' || next_char == 'E') {
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, 'e'), strFree(&str_temp));
				scanner_state = IN_EXPONENCIAL;
			} else {
				/* vysledny stav: cele cislo */
				token->type = T_INTEGER;
				token->value.int_val = atoi(str_temp.str);
				token_put_char(next_char);
				strFree(&str_temp);
				return ERR_OK;
			}
			break;
		case (IN_DOUBLE):
			/* stav po te, co jsem nacetl desetinnou tecku */
			if (isdigit(next_char)) {
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
			} else if ((str_temp.str[str_temp.length - 1] != '.') &&
				(next_char == 'e' || next_char == 'E')) {
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, 'e'), strFree(&str_temp));
				scanner_state = IN_EXPONENCIAL;
			} else {
				/* vysledny stav: cislo double bez e */

				/* osetreni, zda cislo nekonci na . */
				if (str_temp.str[str_temp.length - 1] == '.') {
					strFree(&str_temp);
					print_error_msg("invalid double format!");
					return ERR_LEXAN;
				}
				token->type = T_DOUBLE;
				token->value.double_val = atof(str_temp.str);
				token_put_char(next_char);
				strFree(&str_temp);
				return ERR_OK;
			}
			break;
		case (IN_EXPONENCIAL):
			/* stav po té, co jsem načetl "e"/"E" v čísle - vždy se vkládá pouze
			   malé 'e', tudíž v podmínkách není velké 'E' uvedeno */
			if (isdigit(next_char) ||
				(str_temp.str[str_temp.length - 1] == 'e' &&
				(next_char == '+' || next_char == '-'))) {
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
			} else {
				/* vysledny stav: cislo double s e */

				/* je treba osetrit vyjimku, kdy znak na konci je e, + nebo - */
				if (str_temp.str[str_temp.length - 1] == 'e' ||
					str_temp.str[str_temp.length - 1] == '+' ||
					str_temp.str[str_temp.length - 1] == '-') {
					strFree(&str_temp);
					print_error_msg("invalid double format!");
					return ERR_LEXAN;
				}

				token->type = T_DOUBLE;
				token->value.double_val = atof(str_temp.str);
				token_put_char(next_char);
				strFree(&str_temp);
				return ERR_OK;
			}
			break;
		case (IN_VARIABLE):
			/* stav po te, co jsem nacetl $ a ctu promennou */
			if (isalpha(next_char) || next_char == '_' ||
				(strGetLength(&str_temp) > 1 && isdigit(next_char))) {
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
			} else {
				/* vysledny stav: promenna */
				if (strGetLength(&str_temp) == 1) {
					strFree(&str_temp);
					print_error_msg("invalid variable name!");
					return ERR_LEXAN;
				}

				token->type = T_VARIABLE;
				token->value.str_val = str_temp;
				token_put_char(next_char);
				return ERR_OK;
			}
			break;
		case (IN_KEY_OR_ID):
			/* stav identifikatoru ci klicoveho slova */
			if (isalnum(next_char) || next_char == '_') {
				CHECK_ERR_VAL_INN(strAddChar(&str_temp, next_char), strFree(&str_temp));
			} else {
				/* vysledny stav: identifikator ci klicove slovo */
				set_token_from_str(&str_temp, token);
				token_put_char(next_char);
				return ERR_OK;
			}
			break;
		default:

			break;
		}

	}

}

int hexa_to_int(char c)
{
	if (c >= 48 && c <= 57) {
		return (c - 48);
	}
	if (c >= 65 && c <= 70) {
		return (c - 55);
	}
	if (c >= 97 && c <= 102) {
		return (c - 87);
	}
	return -1;
}

// TODO: Pridat popis
// TODO: Fce v str.c by mozna mely vracet errors, ne int, ale to je drobnost
// TODO: Kontrolujeme vsude i addChars / copyString atp.? Nestaci jen Init.
// TODO: Na kontrolu operaci by mozna slo makro
// TODO: Interpret nekontroluje navratovou hodnotu vestavenych funkci

errors str_escape_sequences(string * s)
{
	string from;

	if (strDuplicate(&from, s) != ERR_OK) {
		print_error_msg("internal error!");
		return ERR_INTERNAL;
	}
	strClear(s);

	int i = 0;
	int state = 0;
	int firstHexa = 0;
	int secondHexa = 0;
	char firstHexaChar = '0';
	for (i = 0; i < strGetLength(&from); i++) {
		char znak = from.str[i];
		switch (state) {
		case 0:
			// Faze cteni normalniho textu
			if (znak == '$') {
				// Samostatny dolar se v textu nemuze vyskytovat
				strFree(&from);
				/* strFree(s); */
				print_error_msg("invalid char $ in string!");
				return ERR_LEXAN;
			}
			if (znak != '\\') {
				// Normalni znak pripraven k vytisknuti
				CHECK_ERR_VAL_INN(strAddChar(s, znak), strFree(&from));
			} else {
				// Narazil jsem na lomitko
				state = 1;
			}
			break;
		case 1:
			// Faze tesne za lomitkem
			switch (znak) {
			case 'n':
				// Sekvence \n - novy radek
				CHECK_ERR_VAL_INN(strAddChar(s, '\n'), strFree(&from));
				state = 0;
				break;
			case 't':
				// Sekvence \t - odsazeni
				CHECK_ERR_VAL_INN(strAddChar(s, '\t'), strFree(&from));
				state = 0;
				break;
			case '\\':
				// Sekvence \\ - zpetne lomitko
				CHECK_ERR_VAL_INN(strAddChar(s, '\\'), strFree(&from));
				state = 0;
				break;
			case '"':
				// Sekvence \" - uvozovky
				CHECK_ERR_VAL_INN(strAddChar(s, '"'), strFree(&from));
				state = 0;
				break;
			case '$':
				// Sekvence \$ - dolar
				CHECK_ERR_VAL_INN(strAddChar(s, '$'), strFree(&from));
				state = 0;
				break;
			case 'x':
				// Sekvence se znakem \x##
				state = 2;
				break;
			default:
				// Neplatna sekvence - vytisknout jak je
				CHECK_ERR_VAL_INN(strAddChar(s, '\\'), strFree(&from));
				CHECK_ERR_VAL_INN(strAddChar(s, znak), strFree(&from));
				state = 0;
				break;
			}
			break;
		case 2:
			// Faze nacitani prvniho znaku za \x
			if (znak != '\\') {
				//Osetreni vyjimky - dolar
				if (znak == '$') {
					strFree(&from);
					print_error_msg("invalid char $ in string!");
					return ERR_LEXAN;
				} else {
					// Znak neni dalsi zpetne lomitko
					firstHexa = (hexa_to_int(znak)*16);
					if (firstHexa < 0) {
						// Neni platny zapis \x, vytisknout tedy tak jak je
						CHECK_ERR_VAL_INN(strAddChar(s, '\\'), strFree(&from));
						CHECK_ERR_VAL_INN(strAddChar(s, 'x'), strFree(&from));
						CHECK_ERR_VAL_INN(strAddChar(s, znak), strFree(&from));
						state = 0;
					} else {
						firstHexaChar = znak;
						state = 3;
					}
				}
			} else {
				// Znak je zpetne lomitko - vytiskni vse normalni a zacni novou sekvenci
				CHECK_ERR_VAL_INN(strAddChar(s, '\\'), strFree(&from));
				CHECK_ERR_VAL_INN(strAddChar(s, 'x'), strFree(&from));
				state = 1;
			}
			break;
		case 3:
			// Faze nacitani druheho znaku za \x
			if (znak != '\\') {
				//Osetreni vyjimky - dolar
				if (znak == '$') {
					strFree(&from);
					print_error_msg("invalid char $ in string!");
					return ERR_LEXAN;
				} else {
					// Znak neni dalsi zpetne lomitko
					secondHexa = hexa_to_int(znak);
					if (secondHexa < 0) {
						// Neni platny zapis \x, vytisknout tedy tak jak je
						CHECK_ERR_VAL_INN(strAddChar(s, '\\'), strFree(&from));
						CHECK_ERR_VAL_INN(strAddChar(s, 'x'), strFree(&from));
						CHECK_ERR_VAL_INN(strAddChar(s, firstHexaChar), strFree(&from));
						CHECK_ERR_VAL_INN(strAddChar(s, znak), strFree(&from));
					} else {
						CHECK_ERR_VAL_INN(strAddChar(s, (char) (firstHexa + secondHexa)), strFree(&from));
					}
					state = 0;
				}
			} else {
				// Znak je zpetne lomitko - vytiskni vse normalni a zacni novou sekvenci
				CHECK_ERR_VAL_INN(strAddChar(s, '\\'), strFree(&from));
				CHECK_ERR_VAL_INN(strAddChar(s, 'x'), strFree(&from));
				CHECK_ERR_VAL_INN(strAddChar(s, firstHexaChar), strFree(&from));
				state = 1;
			}
			break;
		}
	}

	strFree(&from);

	// Kontrola neukoncene escape sekvence
	switch (state) {
	case 0:
		// Retezec je v poradku
		break;

	case 1:
		// Nacteno prazdne \ ...
		CHECK_ERR_VAL_INN(strAddChar(s, '\\'),
		{});
		break;

	case 2:
		// Nacteno prazdne \x
		CHECK_ERR_VAL_INN(strAddChar(s, '\\'),
		{});

		CHECK_ERR_VAL_INN(strAddChar(s, 'x'),
		{});
		break;

	case 3:
		// Nacreno neukoncene \x#
		CHECK_ERR_VAL_INN(strAddChar(s, '\\'),
		{});

		CHECK_ERR_VAL_INN(strAddChar(s, 'x'),
		{});

		CHECK_ERR_VAL_INN(strAddChar(s, firstHexaChar),
		{});
		break;
	}
	return ERR_OK;
}