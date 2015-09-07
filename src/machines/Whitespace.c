/*
 * Whitespace.c
 *
 *  Created on: Aug 28, 2015
 *      Author: Evan
 */


#include "../LexicalAnalyzer.h"
#include "../Token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int match_ws(char c) {
	return c == '\t' || c == ' ';
}

Token *run_ws() {
	char c = next_char();

	if(!match_ws(c)) {
		Token *blocked_token = token_new(TOK_BLOCKED, 0);
		return blocked_token;
	}

	while(match_ws(c)) {
		c = next_char();
	}

	move_f_back();

	Token *my_token = token_new(TOK_WS, 0);
	return my_token;
}

