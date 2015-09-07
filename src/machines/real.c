
#include "../LexicalAnalyzer.h"
#include "../Token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define REAL_MAX_X_LENGTH 5
#define REAL_MAX_Y_LENGTH 5

static int count_x = 0;
static int count_y = 0;
static int leading_zero = 0;
static int trailing_zero = 0;

Token *run_real() {
	count_x = 0;
	count_y = 0;
	leading_zero = 0;
	trailing_zero = 0;

	char cur = next_char();

	if(!isdigit(cur)) {
		Token *blocked_token = token_new(TOK_BLOCKED, 0);
		return blocked_token;
	}

	if(cur == '0') {
		leading_zero = 1;
	}

	while(isdigit(cur)) {
		count_x++;
		cur = next_char();
	}

	if(cur != '.') {
		Token *blocked_token = token_new(TOK_BLOCKED, 0);
		return blocked_token;
	}

	cur = next_char();
	if(!isdigit(cur)) {
		Token *blocked_token = token_new(TOK_BLOCKED, 0);
		return blocked_token;
	}
	while(isdigit(cur)) {
		trailing_zero = (cur == '0');
		count_y++;
		cur = next_char();
	}

	move_f_back();

	int error_code = 0;

	if(leading_zero && count_x > 1) {
		error_code += 1;
	}
	if(count_x > REAL_MAX_X_LENGTH) {
		error_code += 2;
	}
	if(trailing_zero && count_y > 1) {
		error_code += 4;
	}
	if(count_y > REAL_MAX_Y_LENGTH) {
		error_code += 8;
	}

	if(error_code > 0) {
		Token *error_token = token_new(TOK_LEXERR, 256 + error_code);
		return error_token;
	}

	Token *result_token = token_new(TOK_NUM, NUM_REAL);
	return result_token;
}

