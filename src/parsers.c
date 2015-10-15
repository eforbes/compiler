
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsers.h"
#include "compiler.h"
#include "synerr.h"
#include "machines/addop.h"

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

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
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

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_prog_tt() {
	switch(tok->token) {
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
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_PROGRAM),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_idlst() {
	switch(tok->token) {
	case TOK_ID:
		match(TOK_ID);
		p_idlst_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_idlst_t() {
	switch(tok->token) {
	case TOK_COMMA:
		match(TOK_COMMA);
		match(TOK_ID);
		p_idlst_t();
		break;
	case TOK_PARENS_CLOSE:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_COMMA),
				get_token_desc(TOK_PARENS_CLOSE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_decls() {
	switch(tok->token) {
	case TOK_VAR:
		match(TOK_VAR);
		match(TOK_ID);
		match(TOK_COLON);
		p_type();
		match(TOK_SEMICOLON);
		p_decls_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_VAR),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_decls_t() {
	switch(tok->token) {
	case TOK_VAR:
		match(TOK_VAR);
		match(TOK_ID);
		match(TOK_COLON);
		p_type();
		match(TOK_SEMICOLON);
		p_decls_t();
		break;
	case TOK_FUNCTION:
	case TOK_BEGIN:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_VAR),
				get_token_desc(TOK_FUNCTION),
				get_token_desc(TOK_BEGIN),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_type() {
	switch(tok->token) {
	case TOK_INTEGER:
	case TOK_REAL:
		p_stdtype();
		break;
	case TOK_ARRAY:
		match(TOK_ARRAY);
		match(TOK_SQUARE_BRACKET_OPEN);
		match(TOK_NUM);
		match(TOK_DOT_DOT);
		match(TOK_NUM);
		match(TOK_SQUARE_BRACKET_CLOSE);
		match(TOK_OF);
		p_stdtype();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_INTEGER),
				get_token_desc(TOK_REAL),
				get_token_desc(TOK_ARRAY),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_stdtype() {
	switch(tok->token) {
	case TOK_INTEGER:
		match(TOK_INTEGER);
		break;
	case TOK_REAL:
		match(TOK_REAL);
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_INTEGER),
				get_token_desc(TOK_REAL),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subprogdecls() {
	switch(tok->token) {
	case TOK_FUNCTION:
		p_subprogdecl();
		match(TOK_SEMICOLON);
		p_subprogdecls_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_FUNCTION),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subprogdecls_t() {
	switch(tok->token) {
	case TOK_FUNCTION:
		p_subprogdecl();
		match(TOK_SEMICOLON);
		p_subprogdecls_t();
		break;
	case TOK_ID:
	case TOK_BEGIN:
	case TOK_IF:
	case TOK_WHILE:
	case TOK_END:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_FUNCTION),
				get_token_desc(TOK_ID),
				get_token_desc(TOK_BEGIN),
				get_token_desc(TOK_IF),
				get_token_desc(TOK_WHILE),
				get_token_desc(TOK_END),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subprogdecl() {
	switch(tok->token) {
	case TOK_FUNCTION:
		p_subproghead();
		p_subprogdecl_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_FUNCTION),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subprogdecl_t() {
	switch(tok->token) {
	case TOK_VAR:
		p_decls();
		p_subprogdecl_tt();
		break;
	case TOK_FUNCTION:
		p_subprogdecls();
		p_cmpndstmt();
		break;
	case TOK_BEGIN:
		p_cmpndstmt();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_VAR),
				get_token_desc(TOK_FUNCTION),
				get_token_desc(TOK_BEGIN),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subprogdecl_tt() {
	switch(tok->token) {
	case TOK_FUNCTION:
		p_subprogdecls();
		p_cmpndstmt();
		break;
	case TOK_BEGIN:
		p_cmpndstmt();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_FUNCTION),
				get_token_desc(TOK_BEGIN),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subproghead() {
	switch(tok->token) {
	case TOK_FUNCTION:
		match(TOK_FUNCTION);
		match(TOK_ID);
		p_subproghead_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_FUNCTION),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subproghead_t() {
	switch(tok->token) {
	case TOK_PARENS_OPEN:
		p_args();
		match(TOK_COLON);
		p_stdtype();
		match(TOK_SEMICOLON);
		break;
	case TOK_COLON:
		match(TOK_COLON);
		p_stdtype();
		match(TOK_SEMICOLON);
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_COLON),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_args() {
	switch(tok->token) {
	case TOK_PARENS_OPEN:
		match(TOK_PARENS_OPEN);
		p_paramlst();
		match(TOK_PARENS_CLOSE);
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_paramlst() {
	switch(tok->token) {
	case TOK_ID:
		match(TOK_ID);
		match(TOK_COLON);
		p_type();
		p_paramlst_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_paramlst_t() {
	switch(tok->token) {
	case TOK_SEMICOLON:
		match(TOK_SEMICOLON);
		match(TOK_ID);
		match(TOK_COLON);
		p_type();
		p_paramlst_t();
		break;
	case TOK_PARENS_CLOSE:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_SEMICOLON),
				get_token_desc(TOK_PARENS_CLOSE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_cmpndstmt() {
	switch(tok->token) {
	case TOK_BEGIN:
		match(TOK_BEGIN);
		p_cmpndstmt_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_BEGIN),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_cmpndstmt_t() {
	switch(tok->token) {
	case TOK_ID:
	case TOK_BEGIN:
	case TOK_IF:
	case TOK_WHILE:
		p_optstmts();
		match(TOK_END);
		break;
	case TOK_END:
		match(TOK_END);
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_BEGIN),
				get_token_desc(TOK_IF),
				get_token_desc(TOK_WHILE),
				get_token_desc(TOK_END),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_optstmts() {
	switch(tok->token) {
	case TOK_ID:
	case TOK_BEGIN:
	case TOK_IF:
	case TOK_WHILE:
		p_stmtlst();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_BEGIN),
				get_token_desc(TOK_IF),
				get_token_desc(TOK_WHILE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_stmtlst() {
	switch(tok->token) {
	case TOK_ID:
	case TOK_BEGIN:
	case TOK_IF:
	case TOK_WHILE:
		p_stmt();
		p_stmtlst_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_BEGIN),
				get_token_desc(TOK_IF),
				get_token_desc(TOK_WHILE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_stmtlst_t() {
	switch(tok->token) {
	case TOK_SEMICOLON:
		match(TOK_SEMICOLON);
		p_stmt();
		p_stmtlst_t();
		break;
	case TOK_END:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_SEMICOLON),
				get_token_desc(TOK_END),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_stmt() {
	switch(tok->token) {
	case TOK_ID:
		p_variable();
		match(TOK_ASSIGNOP);
		p_expr();
		break;
	case TOK_BEGIN:
		p_cmpndstmt();
		break;
	case TOK_IF:
		match(TOK_IF);
		p_expr();
		match(TOK_THEN);
		p_stmt();
		p_stmt_t();
		break;
	case TOK_WHILE:
		match(TOK_WHILE);
		p_expr();
		match(TOK_DO);
		p_stmt();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_BEGIN),
				get_token_desc(TOK_IF),
				get_token_desc(TOK_WHILE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_stmt_t() {
	switch(tok->token) {
	case TOK_ELSE:
		match(TOK_ELSE);
		p_stmt();//TODO: is this right?
		break;
	case TOK_SEMICOLON:
	case TOK_END:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ELSE),
				get_token_desc(TOK_SEMICOLON),
				get_token_desc(TOK_END),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_variable() {
	switch(tok->token) {
	case TOK_ID:
		match(TOK_ID);
		p_variable_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_variable_t() {
	switch(tok->token) {
	case TOK_SQUARE_BRACKET_OPEN:
		match(TOK_SQUARE_BRACKET_OPEN);
		p_expr();
		match(TOK_SQUARE_BRACKET_CLOSE);
		break;
	case TOK_ASSIGNOP:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_SQUARE_BRACKET_OPEN),
				get_token_desc(TOK_ASSIGNOP),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_exprlst() {
	switch(tok->token) {
	case TOK_ID:
	case TOK_NUM:
	case TOK_PARENS_OPEN:
	case TOK_NOT:
		p_expr();
		p_exprlst_t();
		break;
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			p_expr();
			p_exprlst_t();
			break;
		}
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_NUM),
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_NOT),
				"+",
				"-",
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_exprlst_t() {
	switch(tok->token) {
	case TOK_COMMA:
		match(TOK_COMMA);
		p_expr();
		p_exprlst_t();
		break;
	case TOK_PARENS_CLOSE:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_COMMA),
				get_token_desc(TOK_PARENS_CLOSE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_expr() {
	switch(tok->token) {
	case TOK_ID:
	case TOK_NUM:
	case TOK_PARENS_OPEN:
	case TOK_NOT:
		p_smplexpr();
		p_expr_t();
		break;
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			p_smplexpr();
			p_expr_t();
			break;
		}
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_NUM),
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_NOT),
				"+",
				"-",
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_expr_t() {
	switch(tok->token) {
	case TOK_RELOP:
		match(TOK_RELOP);
		p_smplexpr();
		break;
	case TOK_SEMICOLON:
	case TOK_END:
	case TOK_ELSE:
	case TOK_THEN:
	case TOK_DO:
	case TOK_SQUARE_BRACKET_CLOSE:
	case TOK_COMMA:
	case TOK_PARENS_CLOSE:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_RELOP),
				get_token_desc(TOK_SEMICOLON),
				get_token_desc(TOK_END),
				get_token_desc(TOK_ELSE),
				get_token_desc(TOK_THEN),
				get_token_desc(TOK_DO),
				get_token_desc(TOK_SQUARE_BRACKET_CLOSE),
				get_token_desc(TOK_COMMA),
				get_token_desc(TOK_PARENS_CLOSE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_smplexpr() {
	switch(tok->token) {
	case TOK_ID:
	case TOK_NUM:
	case TOK_PARENS_OPEN:
	case TOK_NOT:
		p_term();
		p_smplexpr_t();
		break;
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			p_sign();
			p_term();
			p_smplexpr_t();
			break;
		}
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_NUM),
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_NOT),
				"+",
				"-",
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_smplexpr_t() {
	switch(tok->token) {
	case TOK_ADDOP:
		match(TOK_ADDOP);
		p_term();
		p_smplexpr_t();
		break;
	case TOK_RELOP:
	case TOK_SEMICOLON:
	case TOK_END:
	case TOK_ELSE:
	case TOK_THEN:
	case TOK_DO:
	case TOK_SQUARE_BRACKET_CLOSE:
	case TOK_COMMA:
	case TOK_PARENS_CLOSE:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ADDOP),
				get_token_desc(TOK_RELOP),
				get_token_desc(TOK_SEMICOLON),
				get_token_desc(TOK_END),
				get_token_desc(TOK_ELSE),
				get_token_desc(TOK_THEN),
				get_token_desc(TOK_DO),
				get_token_desc(TOK_SQUARE_BRACKET_CLOSE),
				get_token_desc(TOK_COMMA),
				get_token_desc(TOK_PARENS_CLOSE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_term() {
	switch(tok->token) {
	case TOK_ID:
	case TOK_NUM:
	case TOK_PARENS_OPEN:
	case TOK_NOT:
		p_factor();
		p_term_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_NUM),
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_NOT),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_term_t() {
	switch(tok->token) {
	case TOK_MULOP:
		match(TOK_MULOP);
		p_factor();
		p_term_t();
		break;
	case TOK_ADDOP:
	case TOK_RELOP:
	case TOK_SEMICOLON:
	case TOK_END:
	case TOK_ELSE:
	case TOK_THEN:
	case TOK_DO:
	case TOK_SQUARE_BRACKET_CLOSE:
	case TOK_COMMA:
	case TOK_PARENS_CLOSE:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_MULOP),
				get_token_desc(TOK_ADDOP),
				get_token_desc(TOK_RELOP),
				get_token_desc(TOK_SEMICOLON),
				get_token_desc(TOK_END),
				get_token_desc(TOK_ELSE),
				get_token_desc(TOK_THEN),
				get_token_desc(TOK_DO),
				get_token_desc(TOK_SQUARE_BRACKET_CLOSE),
				get_token_desc(TOK_COMMA),
				get_token_desc(TOK_PARENS_CLOSE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_factor() {
	switch(tok->token) {
	case TOK_ID:
		match(TOK_ID);
		p_factor_t();
		break;
	case TOK_NUM:
		match(TOK_NUM);
		break;
	case TOK_PARENS_OPEN:
		match(TOK_PARENS_OPEN);
		p_expr();
		match(TOK_PARENS_CLOSE);
		break;
	case TOK_NOT:
		match(TOK_NOT);
		p_factor();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_NUM),
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_NOT),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_factor_t() {
	switch(tok->token) {
	case TOK_PARENS_OPEN:
		match(TOK_PARENS_OPEN);
		p_exprlst();
		match(TOK_PARENS_CLOSE);
		break;
	case TOK_SQUARE_BRACKET_OPEN:
		match(TOK_SQUARE_BRACKET_OPEN);
		p_expr();
		match(TOK_SQUARE_BRACKET_CLOSE);
		break;
	case TOK_MULOP:
	case TOK_ADDOP:
	case TOK_RELOP:
	case TOK_SEMICOLON:
	case TOK_END:
	case TOK_ELSE:
	case TOK_THEN:
	case TOK_DO:
	case TOK_SQUARE_BRACKET_CLOSE:
	case TOK_COMMA:
	case TOK_PARENS_CLOSE:
		// nop
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_SQUARE_BRACKET_OPEN),
				get_token_desc(TOK_MULOP),
				get_token_desc(TOK_ADDOP),
				get_token_desc(TOK_RELOP),
				get_token_desc(TOK_SEMICOLON),
				get_token_desc(TOK_END),
				get_token_desc(TOK_ELSE),
				get_token_desc(TOK_THEN),
				get_token_desc(TOK_DO),
				get_token_desc(TOK_SQUARE_BRACKET_CLOSE),
				get_token_desc(TOK_COMMA),
				get_token_desc(TOK_PARENS_CLOSE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_sign() {
	switch(tok->token) {
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			match(TOK_ADDOP);
			break;
		}
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				"+",
				"-",
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}
