/*
 * Token.h
 *
 *  Created on: Aug 29, 2015
 *      Author: Evan
 */

#ifndef TOKEN_H_
#define TOKEN_H_

#define TOK_BLOCKED 0
#define TOK_WS 1
#define TOK_ID 2
#define TOK_NUM 3

#define TOK_RELOP 6
#define TOK_ASSIGNOP 7
#define TOK_ADDOP 9
#define TOK_MULOP 10

#define TOK_PERIOD 11
#define TOK_SEMICOLON 12
#define TOK_COMMA 13
#define TOK_PARENS_OPEN 14
#define TOK_PARENS_CLOSE 15
#define TOK_COLON 16
#define TOK_SQUARE_BRACKET_OPEN 17
#define TOK_SQUARE_BRACKET_CLOSE 18

#define TOK_LEXERR 99

#define NUM_INT 0
#define NUM_REAL 1
#define NUM_LONG_REAL 2

typedef struct Token {
	int token;
	union {
		int attribute;
		void *mem;
	};
} Token;

Token *token_new(int token_in, int attribute_in);
Token *token_mem_new(int token_in, void *attribute_in);

#endif /* TOKEN_H_ */
