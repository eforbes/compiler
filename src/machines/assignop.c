
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexan.h"
#include "../token.h"

Token *run_assignop() {
	char cur = next_char();

	if(cur == ':') {
		cur = next_char();

		if(cur == '=') {
			return token_new(TOK_ASSIGNOP, 0);
		}
	}

	return token_new(TOK_BLOCKED, 0);
}

