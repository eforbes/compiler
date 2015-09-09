
#include "mulop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexan.h"
#include "../token.h"

Token *run_mulop() {
	char cur = next_char();

	switch(cur) {
	case '*':
		return token_new(TOK_MULOP, MULOP_MULTIPLY);
	case '/':
		return token_new(TOK_MULOP, MULOP_DIVIDE);
	default:
		return token_new(TOK_BLOCKED, 0);
	}
}
