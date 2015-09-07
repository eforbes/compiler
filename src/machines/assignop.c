
#include "../LexicalAnalyzer.h"
#include "../Token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Token *run_assignop() {
	char cur = next_char();

	Token *result_token;

	if(cur==':') {
		cur = next_char();
		if(cur=='=') {
			result_token = token_new(TOK_ASSIGNOP, 0);
		} else {
			result_token = token_new(TOK_BLOCKED, 0);
		}
	} else {
		result_token = token_new(TOK_BLOCKED, 0);
	}

	return result_token;
}

