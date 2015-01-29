/* 
 * Project:  Implementace interpretu imperativního jazyka IFJ13.
 * File:   scanner.h
 * Authors:
 *		Lukáš Horký,		xhorky21
 *		Lukáš Huták,		xhutak01
 *
 * Created on 11.10.2013
 */

#ifndef SCANNER_H
#define	SCANNER_H

#include <stdio.h>
/* dalsi includy jsou dole - pralo se to pri prekladu */

/* Typy tokenů předávaných z lexikálního analyzátoru */
typedef enum {
    T_START,    /* <?php */
    T_END,      /* EOF */
    T_VARIABLE, /* název proměnné */
    T_FUNCTION, /* název funkce */
    T_INTEGER,  /* hodnota typu integer */
    T_DOUBLE,   /* hodnota typu double */
    T_STRING,   /* řetězec znaků */
    T_BOOLEAN,  /* TRUE | FALSE */
    T_NULL,     /* null */
    T_OPERATOR, /* operátory z enum "operators" */
    T_SEMICOL,  /* ; */
    T_COMMA,    /* , */
    T_BRACKET,  /* závorky z enum "brackets" */
    T_KEYWORD,  /* klíčová slova z enum "keywords" */
	T_EXPR,		/* expression - potrebne v parseru u uzlu stromu */
	T_UNSET		/* nenastaven - taktez u uzlu stromu */
} tokens;

/* Závorky */
typedef enum {
    B_LEFT_PAR,     /* ( */
    B_RIGHT_PAR,    /* ) */
    B_LEFT_BRACE,   /* { */
    B_RIGHT_BRACE,  /* } */
} brackets;


/* Operátory */
typedef enum {
	OP_UMINUS,
    OP_MULTIPLY,    /* * */
    OP_DIVIDE,      /* / */
    OP_PLUS,        /* + */
    OP_MINUS,       /* - */
    OP_CONCAT,      /* . */
    OP_LESS,        /* < */
    OP_LESS_EQ,     /* <= */
    OP_GREATER,     /* > */
    OP_GREATER_EQ,  /* >= */
    OP_TRIPLE_EQ,   /* === */
    OP_NOT_EQ,      /* !== */
    OP_ASSIGN,      /* = */
} operators;


/* Klíčová slova */
typedef enum {
    KEY_IF,         /* if */
    KEY_ELSE,       /* else */
	KEY_ELSEIF,
    KEY_RETURN,     /* return */
    KEY_WHILE,      /* while */
    KEY_FUNCTION,   /* function */
    KEY_TRUE,       /* true */
    KEY_FALSE,      /* false */
    KEY_BOOLVAL,    /* boolval */
    KEY_DOUBLEVAL,
    KEY_INTVAL,
    KEY_STRVAL,
    KEY_GET_STRING,
    KEY_PUT_STRING,
    KEY_STRLEN,
    KEY_GET_SUBSTRING,
    KEY_FIND_STRING,
    KEY_SORT_STRING,
} keywords;

/* Vnitřní stavy lexikálního analyzátoru */
typedef enum {
    IN_DEFAULT,         /* výchozí stav */
    IN_ONE_EQ,          /* načtení "=" */
    IN_DOUBLE_EQ,       /* načtení "==" */
    IN_SLASH,           /* načtení "/" */
    IN_COMMENT_LINE,    /* načtení "//" */
    IN_COMMENT_BLOCK,   /* načtení "/ *" */
	IN_COMMENT_BLOCK_ST,/* načtení "*" uvnitř "/ *" */
    IN_STRING,          /* načtení ' " ' */
    IN_STRING_SLASH,    /* načtení "\" */
    IN_LESS,            /* načtení "<" */
    IN_GREATER,         /* načtení ">" */
    IN_EXCLAM,          /* načtení "!" */
    IN_EXCLAM_EQ,       /* načtení "!=" */
    IN_INTEGER,         /* načtení celého čísla */
    IN_DOUBLE,          /* načtení "." */
    IN_EXPONENCIAL,     /* načtení "e" */
    IN_VARIABLE,        /* načtení "$" */
    IN_KEY_OR_ID        /* načtení jiného znaku */
} inner_states;

typedef enum {
	FALSE,
	TRUE
} bool_value;

#include "str.h"

typedef union value_s {
	int int_val;
	string str_val;
	double double_val;
	bool_value bool_val;
} value_t;

/* Struktura tokenu */
typedef struct token_s {
	tokens type;
	value_t value;
} token_t;

#include "common.h"


/* Naplnit token, vrátit hodnotu z errors (vše v pořádku == E_OK) */
errors get_token(token_t *token);

/* Vrátí pozici, kde se zahájilo čtení posledního tokenu nebo kde nastala 
 * chyba 
 */
void get_last_token_position(int *row, int *col);

/* Nastaví zdroj čtení tokenů */
void set_source(FILE *source);

/* Nastavi token na NULL, BOOL, KEYWORD nebo FUNCTION dle textu */
void set_token_from_str(string *str, token_t * token);

/* Zpracovani escape sekvenci v retezcovem literalu */
errors str_escape_sequences(string * s);


#endif	/* SCANNER_H */

