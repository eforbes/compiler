
#include "token.h"
#include "compiler.h"
#include "lexan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESC_LENGTH 15

void synerr(char *message) {
	fprintf(output_file, "SYNERR:\t%s\n", message);
	printf("SYNERR:\t%s\n", message);
}

char *get_token_desc(int token_id) {
	char *desc = (char*) malloc(MAX_DESC_LENGTH*sizeof(char));

	switch(token_id) {
	case TOK_ADDOP: sprintf(desc, "an addop"); break;
	case TOK_ARRAY: sprintf(desc, "array"); break;
	case TOK_ASSIGNOP: sprintf(desc, "an assignop"); break;
	case TOK_BEGIN: sprintf(desc, "begin"); break;
	case TOK_COLON: sprintf(desc, ":"); break;
	case TOK_COMMA: sprintf(desc, ","); break;
	case TOK_DO: sprintf(desc, "do"); break;
	case TOK_DOT_DOT: sprintf(desc, ".."); break;
	case TOK_ELSE: sprintf(desc, "else"); break;
	case TOK_END: sprintf(desc, "end"); break;
	case TOK_EOF: sprintf(desc, "end of file"); break;
	case TOK_FUNCTION: sprintf(desc, "function"); break;
	case TOK_ID: sprintf(desc, "an identifier"); break;
	case TOK_IF: sprintf(desc, "if"); break;
	case TOK_INTEGER: sprintf(desc, "integer"); break;
	case TOK_MULOP: sprintf(desc, "a mulop"); break;
	case TOK_NOT: sprintf(desc, "not"); break;
	case TOK_NUM: sprintf(desc, "a number"); break;
	case TOK_OF: sprintf(desc, "of"); break;
	case TOK_PARENS_CLOSE: sprintf(desc, ")"); break;
	case TOK_PARENS_OPEN: sprintf(desc, "("); break;
	case TOK_PERIOD: sprintf(desc, "."); break;
	case TOK_PROGRAM: sprintf(desc, "program"); break;
	case TOK_REAL: sprintf(desc, "real"); break;
	case TOK_RELOP: sprintf(desc, "a relop"); break;
	case TOK_SEMICOLON: sprintf(desc, ";"); break;
	case TOK_SQUARE_BRACKET_CLOSE: sprintf(desc, "]"); break;
	case TOK_SQUARE_BRACKET_OPEN: sprintf(desc, "["); break;
	case TOK_THEN: sprintf(desc, "then"); break;
	case TOK_VAR: sprintf(desc, "var"); break;
	case TOK_WHILE: sprintf(desc, "while"); break;
	default: sprintf(desc, "uknown"); break;
	}
	return desc;
}

void synch() {
	while(tok->token!=TOK_EOF) {
		tok = get_token();
	}
}
