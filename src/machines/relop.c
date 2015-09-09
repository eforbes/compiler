
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "relop.h"
#include "../lexan.h"
#include "../token.h"

static int state = 0;

static Token *advance() {
	char cur = next_char();
	Token *result_token;

	switch(state) {
	case 0:
		if(cur == '<') {
			state = 1;
			return advance();
		}
		if(cur == '>') {
			state = 2;
			return advance();
		}
		if(cur == '=') {
			result_token = token_new(TOK_RELOP, RELOP_EQUAL);
			return result_token;
		}
		result_token =  token_new(TOK_BLOCKED, 0);
		return result_token;
	case 1:
		if(cur == '>') {
			result_token = token_new(TOK_RELOP, RELOP_NOT_EQUAL);
			return result_token;
		}
		if(cur == '=') {
			result_token = token_new(TOK_RELOP, RELOP_LESS_THAN_OR_EQUAL);
			return result_token;
		}
		move_f_back();
		result_token = token_new(TOK_RELOP, RELOP_LESS_THAN);
		return result_token;
	case 2:
		if(cur == '=') {
			result_token = token_new(TOK_RELOP, RELOP_GREATER_THAN_OR_EQUAL);
			return result_token;
		}
		move_f_back();
		result_token = token_new(TOK_RELOP, RELOP_GREATER_THAN);
		return result_token;
	default:
		result_token =  token_new(TOK_BLOCKED, 0);
		return result_token;
	}
}

Token *run_relop() {
	state = 0;
	return advance();
}
