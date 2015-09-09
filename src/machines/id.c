
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../lexan.h"
#include "../token.h"
#include "../symbol_table.h"
#include "../reserved_words.h"

#define ID_MAX_LENGTH 10

static int count = 0;

Token *run_id() {
	count = 0;
	char cur = next_char();

	if(!isalpha(cur)) {
		return token_new(TOK_BLOCKED, 0);
	}

	while(isalpha(cur) || isdigit(cur)) {
		count++;
		cur = next_char();
	}

	move_f_back();

	if(count > ID_MAX_LENGTH) {
		return token_new(TOK_LEXERR, 1);
	}

	char *lexeme = get_lexeme();
	Token *result_token = get_reserved_token(lexeme);

	if(result_token -> token == TOK_BLOCKED) {
		//not a reserved word
		void *m = get_or_add_symbol(lexeme);
		result_token = token_mem_new(TOK_ID, m);
	}

	return result_token;
}


