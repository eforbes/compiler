
#include "addop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexan.h"
#include "../token.h"


Token *run_addop() {
	char c = next_char();
	Token *result_token;

	if(c=='+') {
		result_token = token_new(TOK_ADDOP, ADDOP_ADD);
	} else {
		if(c=='-') {
			result_token = token_new(TOK_ADDOP, ADDOP_SUBTRACT);
		} else {
			result_token = token_new(TOK_BLOCKED, 0);
		}
	}

	return result_token;
}

