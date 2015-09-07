
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexan.h"
#include "../token.h"


Token *run_punctuation() {
	char cur = next_char();

	if(cur=='.') {
		Token *result_token = token_new(TOK_PERIOD, 0);
		return result_token;
	}

	if(cur==';') {
		Token *result_token = token_new(TOK_SEMICOLON, 0);
		return result_token;
	}

	if(cur==',') {
		Token *result_token = token_new(TOK_COMMA, 0);
		return result_token;
	}

	if(cur=='(') {
		Token *result_token = token_new(TOK_PARENS_OPEN, 0);
		return result_token;
	}

	if(cur==')') {
		Token *result_token = token_new(TOK_PARENS_CLOSE, 0);
		return result_token;
	}

	if(cur==':') {
		Token *result_token = token_new(TOK_COLON, 0);
		return result_token;
	}

	if(cur=='[') {
		Token *result_token = token_new(TOK_SQUARE_BRACKET_OPEN, 0);
		return result_token;
	}

	if(cur==']') {
		Token *result_token = token_new(TOK_SQUARE_BRACKET_CLOSE, 0);
		return result_token;
	}


	Token *blocked_token = token_new(TOK_BLOCKED, 0);
	return blocked_token;
}

