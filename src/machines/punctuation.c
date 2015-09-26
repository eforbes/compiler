
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexan.h"
#include "../token.h"

Token *run_punctuation() {
	char cur = next_char();

	switch(cur){
	case '.':
		cur = next_char();
		if(cur == '.') {
			return token_new(TOK_DOT_DOT, 0);
		} else {
			move_f_back();
			return token_new(TOK_PERIOD, 0);
		}
	case ';':
		return token_new(TOK_SEMICOLON, 0);
	case ',':
		return token_new(TOK_COMMA, 0);
	case '(':
		return token_new(TOK_PARENS_OPEN, 0);
	case ')':
		return token_new(TOK_PARENS_CLOSE, 0);
	case ':':
		return token_new(TOK_COLON, 0);
	case '[':
		return token_new(TOK_SQUARE_BRACKET_OPEN, 0);
	case ']':
		return token_new(TOK_SQUARE_BRACKET_CLOSE, 0);
	default:
		return token_new(TOK_BLOCKED, 0);
	}
}

