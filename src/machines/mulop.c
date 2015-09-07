
#include "mulop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexan.h"
#include "../token.h"


Token *run_mulop() {
	char c = next_char();
	Token *result_token;

	if(c=='*') {
		result_token = token_new(TOK_MULOP, MULOP_MULTIPLY);
	} else {
		if(c=='/') {
			result_token = token_new(TOK_MULOP, MULOP_DIVIDE);
		} else {
			result_token = token_new(TOK_BLOCKED, 0);
		}
	}

	return result_token;
}

