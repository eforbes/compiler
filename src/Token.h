
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

#define TOK_PROGRAM 1000
#define TOK_VAR 1001
#define TOK_ARRAY 1002
#define TOK_OF 1003
#define TOK_FUNCTION 1004
#define TOK_PROCEDURE 1005
#define TOK_BEGIN 1006
#define TOK_END 1007
#define TOK_IF 1008
#define TOK_THEN 1009
#define TOK_ELSE 1010
#define TOK_WHILE 1011
#define TOK_DO 1012
#define TOK_NOT 1013
#define TOK_INTEGER 1014
#define TOK_REAL 1015

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
Token *token_mem_new(int token_in, void *mem_in);

#endif /* TOKEN_H_ */
