
#include "../symbol_table.h"
#include "../reserved_words.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../lexan.h"
#include "../token.h"

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
				char *lexeme = get_lexeme();
				result_token = get_reserved_token(lexeme);
				if(result_token->token == TOK_BLOCKED) {
					//not reserved word
					void *m = get_or_add_symbol(lexeme);
					result_token = token_mem_new(TOK_ID, m);
				}
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


