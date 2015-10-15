
#include "lexan.h"
#include "token.h"
#include "parsers.h"
#include "synerr.h"

Token *tok;

#define ERR_BUFFER_SIZE 256

char err_buffer[ERR_BUFFER_SIZE];

void match(int t) {
	printf("Matching %s\n", get_token_desc(t));
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
