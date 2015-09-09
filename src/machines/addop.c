
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "addop.h"
#include "../lexan.h"
#include "../token.h"

Token *run_addop() {
	char cur = next_char();

	switch(cur) {
	case '+':
		return token_new(TOK_ADDOP, ADDOP_ADD);
	case '-':
		return token_new(TOK_ADDOP, ADDOP_SUBTRACT);
	default:
		return token_new(TOK_BLOCKED, 0);
	}
}

