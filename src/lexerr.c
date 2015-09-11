
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexerr.h"

void print_lexerr_text(FILE* f, char* lexeme, int err_n) {
	if(err_n & (1<<LEXERR_FLAG_BIT_NUM)) {
		if(err_n & 1) {
			fprintf(f,
				"LEXERR:\t%s: %s\n",
				"Number whole part has leading zeros",
				lexeme);
		}
		if(err_n & 2) {
			fprintf(f,
				"LEXERR:\t%s: %s\n",
				"Number whole part is too long",
				lexeme);
		}
		if(err_n & 4) {
			fprintf(f,
				"LEXERR:\t%s: %s\n",
				"Number fractional part has trailing zeros",
				lexeme);
		}
		if(err_n & 8) {
			fprintf(f,
				"LEXERR:\t%s: %s\n",
				"Number fractional part is too long",
				lexeme);
		}
		if(err_n & 16) {
			fprintf(f,
				"LEXERR:\t%s: %s\n",
				"Number exponent part is too long",
				lexeme);
		}
	} else {
		if(err_n == LEXERR_ID_TOO_LONG) {
			fprintf(f,
				"LEXERR:\t%s: %s\n",
				"Identifier is too long",
				lexeme);
		}
		if(err_n == LEXERR_UNREC_SYMBOL) {
			fprintf(f,
				"LEXERR:\t%s: %s\n",
				"Unrecognized symbol",
				lexeme);
		}
	}
}
