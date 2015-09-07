
#include "../LexicalAnalyzer.h"
#include "../Token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Token *run_catch_all() {
	next_char();

	Token *unknown_token = token_new(TOK_LEXERR, 99);
	return unknown_token;
}

