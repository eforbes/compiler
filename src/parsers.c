
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsers.h"
#include "compiler.h"
#include "synerr.h"
#include "type.h"
#include "symbol_table.h"
#include "machines/addop.h"

#define SYNERR_BUFFER_SIZE 256

char synerr_buffer[SYNERR_BUFFER_SIZE];

int fparam_total;
int fparam_count;
int fparam_checking;
SymbolTableNode *fparam_cur_node;

int array_checking;
int array_indexed;

int dparam_count;

void check_fparam(int expr_type) {
	if(expr_type != TYPE_ERR) {
		if(is_fp_type(fparam_cur_node->type)) {
			if(fp_to_type(fparam_cur_node->type) != expr_type) {
				//semerr expected parameter fparam_count of type fp_to_type(fparam_cur_node->type) received expr_type
				semerr("Incorrect function parameter type");
			}
			fparam_cur_node = fparam_cur_node -> prev;
		}
	}
	fparam_count++;
}

void p_prog() {
	switch(tok->token) {
	case TOK_PROGRAM:
		match(TOK_PROGRAM);
		char *lex = match3(tok);
		if(lex != NULL) {
			check_add_green_node(lex, TYPE_PGM_NAME);
		}
		match(TOK_PARENS_OPEN);
		dparam_count = 0;
		p_idlst();
		set_param_count(dparam_count);
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_PROGRAM};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_VAR, TOK_FUNCTION, TOK_BEGIN};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_FUNCTION, TOK_BEGIN};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_idlst() {
	char *lex;

	switch(tok->token) {
	case TOK_ID:
		lex = match3(tok);
		if(lex!=NULL) {
			check_add_blue_node(lex, TYPE_PGM_PARAM);
			dparam_count++;
		}
		p_idlst_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_idlst_t() {
	char *lex;
	switch(tok->token) {
	case TOK_COMMA:
		match(TOK_COMMA);
		lex = match3(tok);
		if(lex!=NULL) {
			check_add_blue_node(lex, TYPE_PGM_PARAM);
			dparam_count++;
		}
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_decls() {
	char *lex;
	int type_type;

	switch(tok->token) {
	case TOK_VAR:
		match(TOK_VAR);
		lex = match3(tok);
		match(TOK_COLON);
		type_type = p_type();
		if(lex!=NULL && type_type!=TYPE_ERR) {
			check_add_blue_node(lex, type_type);
		}
		match(TOK_SEMICOLON);
		p_decls_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_VAR),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_VAR, TOK_FUNCTION, TOK_BEGIN};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_decls_t() {
	char *lex;
	int type_type;

	switch(tok->token) {
	case TOK_VAR:
		match(TOK_VAR);
		lex = match3(tok);
		match(TOK_COLON);
		type_type = p_type();
		if(lex!=NULL && type_type!=TYPE_ERR) {
			check_add_blue_node(lex, type_type);
		}
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_VAR, TOK_FUNCTION, TOK_BEGIN};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

int p_type() {
	int n1;
	int n2;
	int type_type;

	switch(tok->token) {
	case TOK_INTEGER:
	case TOK_REAL:
		return p_stdtype();
	case TOK_ARRAY:
		match(TOK_ARRAY);
		match(TOK_SQUARE_BRACKET_OPEN);
		n1 = match2(TOK_NUM, tok->attribute);
		match(TOK_DOT_DOT);
		n2 = match2(TOK_NUM, tok->attribute);
		match(TOK_SQUARE_BRACKET_CLOSE);
		match(TOK_OF);
		type_type = p_stdtype();
		if(n1 != TYPE_INT || n2 != TYPE_INT) {
			semerr("Array bounds must be integers");
			return TYPE_ERR;
		}
		return type_to_array(type_type);
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_INTEGER),
				get_token_desc(TOK_REAL),
				get_token_desc(TOK_ARRAY),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_INTEGER, TOK_REAL, TOK_ARRAY, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

int p_stdtype() {
	switch(tok->token) {
	case TOK_INTEGER:
		match(TOK_INTEGER);
		return TYPE_INT;
	case TOK_REAL:
		match(TOK_REAL);
		return TYPE_REAL;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_INTEGER),
				get_token_desc(TOK_REAL),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_INTEGER, TOK_REAL, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

void p_subprogdecls() {
	switch(tok->token) {
	case TOK_FUNCTION:
		p_subprogdecl();
		complete_function();
		match(TOK_SEMICOLON);
		p_subprogdecls_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_FUNCTION),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_FUNCTION, TOK_ID, TOK_BEGIN, TOK_IF, TOK_WHILE, TOK_END};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_FUNCTION, TOK_ID, TOK_BEGIN, TOK_IF, TOK_WHILE, TOK_END};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_FUNCTION};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_VAR, TOK_FUNCTION, TOK_BEGIN};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_FUNCTION, TOK_BEGIN};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subproghead() {
	switch(tok->token) {
	case TOK_FUNCTION:
		match(TOK_FUNCTION);
		char *lex = match3(tok);
		check_add_green_node(lex, TYPE_F_NAME);
		p_subproghead_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_FUNCTION),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_FUNCTION, TOK_BEGIN, TOK_VAR};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_subproghead_t() {
	int type_type;

	switch(tok->token) {
	case TOK_PARENS_OPEN:
		p_args();
		match(TOK_COLON);
		type_type = p_stdtype();
		set_return_type(type_type);
		match(TOK_SEMICOLON);
		break;
	case TOK_COLON:
		match(TOK_COLON);
		type_type = p_stdtype();
		set_return_type(type_type);
		match(TOK_SEMICOLON);
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_COLON),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_PARENS_OPEN, TOK_COLON, TOK_FUNCTION, TOK_BEGIN, TOK_VAR};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_args() {
	switch(tok->token) {
	case TOK_PARENS_OPEN:
		match(TOK_PARENS_OPEN);
		dparam_count = 0;
		p_paramlst();
		set_param_count(dparam_count);
		match(TOK_PARENS_CLOSE);
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_PARENS_OPEN, TOK_COLON};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_paramlst() {
	char *lex;
	int type_type;

	switch(tok->token) {
	case TOK_ID:
		lex = match3(tok);
		match(TOK_COLON);
		type_type = p_type();

		if(lex!=NULL && type_type!=TYPE_ERR) {
			check_add_blue_node(lex, type_to_fp(type_type));
			dparam_count++;
		}

		p_paramlst_t();
		break;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_paramlst_t() {
	char *lex;
	int type_type;

	switch(tok->token) {
	case TOK_SEMICOLON:
		match(TOK_SEMICOLON);
		lex = match3(tok);
		match(TOK_COLON);
		type_type = p_type();
		if(lex!=NULL && type_type!=TYPE_ERR) {
			check_add_blue_node(lex, type_to_fp(type_type));
			dparam_count++;
		}
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_PARENS_CLOSE};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_BEGIN, TOK_PERIOD, TOK_END, TOK_ELSE};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_BEGIN, TOK_IF, TOK_WHILE, TOK_END, TOK_PERIOD, TOK_ELSE};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_BEGIN, TOK_IF, TOK_WHILE, TOK_END};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_BEGIN, TOK_IF, TOK_WHILE, TOK_END};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_END};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_stmt() {
	int variable_type;
	int expr_type;


	switch(tok->token) {
	case TOK_ID:
		variable_type = p_variable();
		match(TOK_ASSIGNOP);
		expr_type = p_expr();
		if(variable_type == TYPE_ERR || expr_type == TYPE_ERR) {
			return;
		}
		if(variable_type == TYPE_INT && expr_type == TYPE_INT) {
			return;
		}
		if(variable_type == TYPE_REAL && expr_type == TYPE_REAL) {
			return;
		}
		semerr("Assigment types don't match");

		break;
	case TOK_BEGIN:
		p_cmpndstmt();
		break;
	case TOK_IF:
		match(TOK_IF);
		expr_type = p_expr();
		match(TOK_THEN);
		p_stmt();
		p_stmt_t();

		if(expr_type != TYPE_BOOL) {
			//synerr boolean expr required
			return;
		}

		return;
	case TOK_WHILE:
		match(TOK_WHILE);
		expr_type = p_expr();
		match(TOK_DO);
		p_stmt();

		if(expr_type != TYPE_BOOL) {
			//synerr boolean expr required
			return;
		}

		return;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_BEGIN),
				get_token_desc(TOK_IF),
				get_token_desc(TOK_WHILE),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_BEGIN, TOK_IF, TOK_WHILE, TOK_END, TOK_ELSE};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ELSE, TOK_END};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

int p_variable() {
	int id_type;
	int variable_type;

	switch(tok->token) {
	case TOK_ID:
		id_type = get_type(tok -> lex);
		match(TOK_ID);
		variable_type = p_variable_t();
		if(id_type == TYPE_ERR || variable_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		if((id_type == TYPE_INT || id_type == TYPE_REAL) && variable_type == TYPE_OK) {
			return id_type;
		}
		if((id_type == TYPE_A_INT || id_type == TYPE_A_REAL) && variable_type == TYPE_INT) {
			return array_to_type(id_type);
		}
		//semerr assigning to array var or using basic type as array
		if((id_type == TYPE_INT || id_type == TYPE_REAL) && variable_type == TYPE_INT) {
			semerr("Attempting to use standard type as array");
		}
		if((id_type == TYPE_A_INT || id_type == TYPE_A_REAL) && variable_type == TYPE_OK) {
			semerr("Attempting to assign a value to an entire array");
		}
		return TYPE_ERR;
	default:
		sprintf(synerr_buffer,
				"Expecting {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_ASSIGNOP};
		synch(array, sizeof(array)/sizeof(array[0]));

		return TYPE_ERR;
	}
}

int p_variable_t() {
	int expr_type;

	switch(tok->token) {
	case TOK_SQUARE_BRACKET_OPEN:
		match(TOK_SQUARE_BRACKET_OPEN);
		expr_type = p_expr();
		match(TOK_SQUARE_BRACKET_CLOSE);

		if(expr_type == TYPE_INT) {
			return TYPE_INT;
		}
		if(expr_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		semerr("Array index must be integer");
		return TYPE_ERR;
	case TOK_ASSIGNOP:
		// nop
		return TYPE_OK;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_SQUARE_BRACKET_OPEN),
				get_token_desc(TOK_ASSIGNOP),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_SQUARE_BRACKET_OPEN, TOK_ASSIGNOP};
		synch(array, sizeof(array)/sizeof(array[0]));

		return TYPE_ERR;
	}
}

void p_exprlst() {
	int expr_type;

	switch(tok->token) {
	case TOK_ID:
	case TOK_NUM:
	case TOK_PARENS_OPEN:
	case TOK_NOT:
		expr_type = p_expr();

		check_fparam(expr_type);

		p_exprlst_t();
		break;
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			expr_type = p_expr();

			check_fparam(expr_type);

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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_NUM, TOK_PARENS_OPEN, TOK_NOT, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

void p_exprlst_t() {
	int expr_type;
	switch(tok->token) {
	case TOK_COMMA:
		match(TOK_COMMA);
		expr_type = p_expr();

		check_fparam(expr_type);

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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}

int p_expr() {
	int smplexpr_type;
	int expr_type;

	switch(tok->token) {
	case TOK_ID:
	case TOK_NUM:
	case TOK_PARENS_OPEN:
	case TOK_NOT:
		smplexpr_type = p_smplexpr();
		expr_type = p_expr_t();
		if(expr_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		if(expr_type == TYPE_OK) {
			return smplexpr_type;
		}
		if(expr_type == TYPE_BOOL) {
			return TYPE_BOOL;
		}
		//TODO: semerr? can you compare int and reals? 5 < 9.3
		return TYPE_ERR;
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			smplexpr_type = p_smplexpr();
			expr_type = p_expr_t();
			if(expr_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
				return TYPE_ERR;
			}
			if(expr_type == TYPE_OK) {
				return smplexpr_type;
			}
			if(expr_type == TYPE_BOOL) {
				return TYPE_BOOL;
			}
			//semerr?
			return TYPE_ERR;
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_NUM, TOK_PARENS_OPEN, TOK_NOT, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

int p_expr_t() {
	int smplexpr_type;

	switch(tok->token) {
	case TOK_RELOP:
		match(TOK_RELOP);
		smplexpr_type = p_smplexpr();
		if(smplexpr_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		return TYPE_BOOL;
	case TOK_SEMICOLON:
	case TOK_END:
	case TOK_ELSE:
	case TOK_THEN:
	case TOK_DO:
	case TOK_SQUARE_BRACKET_CLOSE:
	case TOK_COMMA:
	case TOK_PARENS_CLOSE:
		// nop
		return TYPE_OK;
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_RELOP, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));

		return TYPE_ERR;
	}
}

int p_smplexpr() {
	int term_type;
	int smplexpr_type;

	switch(tok->token) {
	case TOK_ID:
	case TOK_NUM:
	case TOK_PARENS_OPEN:
	case TOK_NOT:
		term_type = p_term();
		smplexpr_type = p_smplexpr_t();
		if(term_type == TYPE_INT && (smplexpr_type == TYPE_INT || smplexpr_type == TYPE_OK)) {
			return TYPE_INT;
		}
		if(term_type == TYPE_REAL && (smplexpr_type == TYPE_REAL || smplexpr_type == TYPE_OK)) {
			return TYPE_REAL;
		}
		if(term_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		semerr("Attempting to perform arithmetic on mismatched types");
		return TYPE_ERR_NEW;
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			p_sign();
			term_type = p_term();
			smplexpr_type = p_smplexpr_t();
			if(term_type == TYPE_INT && (smplexpr_type == TYPE_INT || smplexpr_type == TYPE_OK)) {
				return TYPE_INT;
			}
			if(term_type == TYPE_REAL && (smplexpr_type == TYPE_REAL || smplexpr_type == TYPE_OK)) {
				return TYPE_REAL;
			}
			if(term_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
				return TYPE_ERR;
			}
			semerr("Attempting to perform arithmetic on mismatched types");
			return TYPE_ERR;
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_NUM, TOK_PARENS_OPEN, TOK_NOT, TOK_RELOP, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

int p_smplexpr_t() {
	int term_type;
	int smplexpr_type;

	switch(tok->token) {
	case TOK_ADDOP:
		match(TOK_ADDOP);
		term_type = p_term();
		smplexpr_type = p_smplexpr_t();
		if(term_type == TYPE_INT && (smplexpr_type == TYPE_INT || smplexpr_type == TYPE_OK)) {
			return TYPE_INT;
		}
		if(term_type == TYPE_REAL && (smplexpr_type == TYPE_REAL || smplexpr_type == TYPE_OK)) {
			return TYPE_REAL;
		}
		if(term_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		semerr("Attempting to perform arithmetic on mismatched types");
		return TYPE_ERR_NEW;
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
		return TYPE_OK;
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ADDOP, TOK_RELOP, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

int p_term() {
	int factor_type;
	int term_type;

	switch(tok->token) {
	case TOK_ID:
	case TOK_NUM:
	case TOK_PARENS_OPEN:
	case TOK_NOT:
		factor_type = p_factor();
		term_type = p_term_t();
		if(factor_type == TYPE_INT && (term_type == TYPE_INT || term_type == TYPE_OK)) {
			return TYPE_INT;
		}
		if(factor_type == TYPE_REAL && (term_type == TYPE_REAL || term_type == TYPE_OK)) {
			return TYPE_REAL;
		}
		if(factor_type == TYPE_ERR || term_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		semerr("Attempting to perform arithmetic on mismatched types");
		return TYPE_ERR_NEW;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_NUM),
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_NOT),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_NUM, TOK_PARENS_OPEN, TOK_NOT, TOK_RELOP, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

int p_term_t() {
	switch(tok->token) {
	case TOK_MULOP:
		match(TOK_MULOP);
		int factor_type = p_factor();
		int term_type = p_term_t();
		if(factor_type == TYPE_INT && (term_type == TYPE_INT || term_type == TYPE_OK)) {
			return TYPE_INT;
		}
		if(factor_type == TYPE_REAL && (term_type == TYPE_REAL || term_type == TYPE_OK)) {
			return TYPE_REAL;
		}
		if(factor_type == TYPE_ERR || term_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		semerr("Attempting to perform arithmetic on mismatched types");
		return TYPE_ERR;
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
		return TYPE_OK;
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_MULOP, TOK_RELOP, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

int p_factor() {
	int id_type;

	switch(tok->token) {
	case TOK_ID:
		id_type = get_type(tok->lex);

		if(id_type != TYPE_ERR) {
			if(id_type == TYPE_F_NAME) {
				SymbolTableNode *fnode = getfnode(tok->lex);
				fparam_total = fnode -> param_count;
				fparam_count = 0;
				fparam_cur_node = fnode -> next;
				fparam_checking = 1;
			} else {
				fparam_checking = 0;
			}

			array_checking = is_array_type(id_type);
			array_indexed = 0;

		} else {
			fparam_checking = 0;
		}
		match(TOK_ID);

		p_factor_t();

		if(fparam_checking) {
			if(fparam_count != fparam_total) {
				//semerr received fparam_count parameters, expected fparam_total
				semerr("Incorrect number of function parameters");
			}
			return getfnode(tok->lex)->return_type;
		}

		if(array_checking) {
			if(!array_indexed) {
				semerr("Attempting to perform arithmetic on an entire array");
			}
			return array_to_type(id_type);
		}

		return id_type;
	case TOK_NUM:
		return match2(TOK_NUM, tok->attribute);
	case TOK_PARENS_OPEN:
		match(TOK_PARENS_OPEN);
		int expr_type = p_expr();
		match(TOK_PARENS_CLOSE);
		return expr_type;
	case TOK_NOT:
		match(TOK_NOT);
		int factor_type = p_factor();

		if(factor_type == TYPE_BOOL) {
			return TYPE_BOOL;
		}

		if(factor_type!=TYPE_ERR) {
			semerr("Not can only be used on booleans");
			return TYPE_ERR;
		}
		return TYPE_ERR;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}, {%s}, {%s}; received {%s}",
				get_token_desc(TOK_ID),
				get_token_desc(TOK_NUM),
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_NOT),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_NUM, TOK_PARENS_OPEN, TOK_NOT, TOK_MULOP, TOK_RELOP, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
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
		int expr_type = p_expr();
		match(TOK_SQUARE_BRACKET_CLOSE);
		array_indexed = 1;
		if(expr_type != TYPE_INT) {
			semerr("Array index must be integer");
		}
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_PARENS_OPEN, TOK_SQUARE_BRACKET_OPEN, TOK_MULOP, TOK_RELOP, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_NUM, TOK_PARENS_OPEN, TOK_NOT};
		synch(array, sizeof(array)/sizeof(array[0]));
	}
}
