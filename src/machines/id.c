
#include "../LexicalAnalyzer.h"
#include "../Token.h"
#include "../symbol_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ID_MAX_LENGTH 10

static int state = 0;
static int count = 0;

static Token *advance() {
	char cur = next_char();

	Token *result_token;

	switch(state) {
	case 0:
		if(isalpha(cur)) {
			state = 1;
			count++;
			result_token = advance();
		} else {
			result_token = token_new(TOK_BLOCKED, 0);
		}
		break;
	case 1:
		if(isalpha(cur) || isdigit(cur)) {
			count++;
			result_token = advance();
		} else {
			move_f_back();
			if(count > ID_MAX_LENGTH) {
				result_token = token_new(TOK_LEXERR, 1);
			} else {
				//todo: check if reserved word

				void *m = get_or_add_symbol(get_lexeme());
				result_token = token_mem_new(TOK_ID, m);
			}

		}
	}

	return result_token;
}

Token *run_id() {
	state = 0;
	count = 0;
	return advance();
}


