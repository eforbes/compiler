/**
 * Evan Forbes
 * Dr. Shenoi
 * Compilers
 * Fall 2015
 */
#include "lexan.h"
#include "token.h"
#include "parsers.h"
#include "synerr.h"
#include "type.h"

Token *tok;

#define ERR_BUFFER_SIZE 256

char err_buffer[ERR_BUFFER_SIZE];

void match(int t) {
	if(tok->token == t) {
		if(t != TOK_EOF) {
			tok = get_token();
		} else {
			puts("Parse complete");
		}
	} else {
		//SYNERR
		sprintf(err_buffer,
				"Expected {%s}; received {%s}",
				get_token_desc(t),
				get_token_desc(tok->token));
		synerr(err_buffer);

		tok = get_token();

	}
}

//get type of num
int match2(int t, int attr) {
	if(t == TOK_NUM) {
		if(attr==NUM_INT) {
			tok = get_token();
			return TYPE_INT;
		}
		if(attr==NUM_REAL) {
			tok = get_token();
			return TYPE_REAL;
		}
	}
	return TYPE_ERR_NEW;
}

void parse() {
	tok = get_token();
	p_prog();
	match(TOK_EOF);
}

int main(int argc, char **argv) {
	if(argc != 2) {
		puts("File name required as argument");
		return -1;
	}

	input_file_name = argv[1];

	parse();

	return 0;
}