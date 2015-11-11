
#include "token.h"
#include "compiler.h"
#include "lexan.h"
#include "type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESC_LENGTH 20

int synerr_count, semerr_count;

void synerr(char *message) {
	fprintf(output_file, "SYNERR:\t%s\n", message);
	synerr_count++;
}

void semerr(char *message) {
	fprintf(output_file, "SEMERR:\t%s\n", message);
	semerr_count++;
}

char *get_token_desc(int token_id) {
	char *desc = (char*) malloc(MAX_DESC_LENGTH*sizeof(char));

	switch(token_id) {
	case TOK_ADDOP: sprintf(desc, "an addop"); break;
	case TOK_ARRAY: sprintf(desc, "array"); break;
	case TOK_ASSIGNOP: sprintf(desc, ":="); break;
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
	case TOK_LEXERR: sprintf(desc, "a lexerr"); break;
	default: sprintf(desc, "unknown"); break;
	}
	return desc;
}

char *get_type_desc(int type) {
	char *desc = (char*) malloc(MAX_DESC_LENGTH*sizeof(char));
	switch(type) {
	case TYPE_INT:
	case TYPE_FP_INT:
		sprintf(desc, "an integer");
		break;
	case TYPE_REAL:
	case TYPE_FP_REAL:
		sprintf(desc, "a real");
		break;
	case TYPE_A_INT:
	case TYPE_FP_A_INT:
		sprintf(desc, "an integer array");
		break;
	case TYPE_A_REAL:
	case TYPE_FP_A_REAL:
		sprintf(desc, "a real array");
		break;
	case TYPE_BOOL:
		sprintf(desc, "a boolean");
		break;
	case TYPE_F_NAME:
		sprintf(desc, "a function");
		break;
	default: sprintf(desc, "an unknown type"); break;
	}
	return desc;
}

void synch(int *synch_tokens, int size) {
	int i;
	int found = 0;
	while( !found ) {
		for(i = 0; i < size; i++) {
			if(tok->token == synch_tokens[i])
				found = 1;
		}
		if(!found)
			tok = get_token();
	}
}
