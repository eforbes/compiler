
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsers.h"
#include "compiler.h"
#include "synerr.h"

#define SYNERR_BUFFER_SIZE 256

char synerr_buffer[SYNERR_BUFFER_SIZE];

void p_prog() {
	switch(tok->token) {
	case TOK_PROGRAM:
		match(TOK_PROGRAM);
		match(TOK_ID);
		match(TOK_PARENS_OPEN);
		p_idlst();
		match(TOK_PARENS_CLOSE);
		match(TOK_SEMICOLON);
		p_prog_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_PROGRAM),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		synch();
	}
}

void p_prog_t() {
	switch(tok->token) {
	case TOK_VAR:
		p_decls();
		p_prog_tt();
		break;
	case TOK_FUNCTION:
		p_subprogdecls();
		p_cmpndstmt();
		match(TOK_PERIOD);
		break;
	case TOK_BEGIN:
		p_cmpndstmt();
		match(TOK_PERIOD);
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_VAR),
				get_token_desc(TOK_FUNCTION),
				get_token_desc(TOK_BEGIN),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		synch();
	}
}

void p_prog_tt() {
	switch(tok->token) {
	case TOK_FUNCTION:
		p_subprog_decls();
		p_cmpndstmt();
		match(TOK_PERIOD);
		break;
	case TOK_BEGIN:
		p_cmpndstmt();
		match(TOK_PERIOD);
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_PROGRAM),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		synch();
	}
}


