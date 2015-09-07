
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../lexan.h"
#include "../token.h"

#define INT_MAX_LENGTH 10

static int count = 0;
static int leading_zero = 0;

Token *run_int() {
	count = 0;
	leading_zero = 0;

	char cur = next_char();

	if(!isdigit(cur)) {
		Token *blocked_token = token_new(TOK_BLOCKED, 0);
		return blocked_token;
	}

	if(cur == '0') {
		leading_zero = 1;
	}

	while(isdigit(cur)) {
		count++;
		cur = next_char();
	}

	move_f_back();

	int error_code = 0;

	if(leading_zero && count > 1) {
		error_code += 1;
	}
	if(count > INT_MAX_LENGTH) {
		error_code += 2;
	}

	if(error_code > 0) {
		Token *error_token = token_new(TOK_LEXERR, 128 + error_code);
		return error_token;
	}

	Token *result_token = token_new(TOK_NUM, NUM_INT);
	return result_token;
}

