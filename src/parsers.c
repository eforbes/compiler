
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsers.h"
#include "compiler.h"
#include "synerr.h"
#include "type.h"
#include "symbol_table.h"
#include "machines/addop.h"
#include "machines/mulop.h"
#include "machines/relop.h"

#define SYNERR_BUFFER_SIZE 256

char synerr_buffer[SYNERR_BUFFER_SIZE];

int dparam_count;

ParamCheckNode *param_check_head = NULL;

ParamCheckNode *param_check_node_new(SymbolTableNode *this_in, int param_total_in, int param_count_in, ParamCheckNode *next) {
	ParamCheckNode *n = malloc(sizeof(ParamCheckNode));
	n -> this = this_in;
	n -> param_total = param_total_in;
	n -> param_count = param_count_in;
	n -> next = next;
	return n;
}

void check_fparam(int expr_type) {
	SymbolTableNode *param_cur_node = param_check_head -> this;
	if(expr_type != TYPE_ERR && param_cur_node!=NULL) {
		if(is_fp_type(param_cur_node -> type)) {
			if(fp_to_type(param_cur_node -> type) != expr_type) {
				//semerr expected parameter fparam_count of type fp_to_type(fparam_cur_node->type) received expr_type
				semerr("Incorrect function parameter type");
			}
			param_check_head -> this = param_check_head -> this -> prev;
		}
	}
	param_check_head -> param_count++;
}

void pre_check_fparams(char *lex) {
	printf("precheck %s\n", lex);
	SymbolTableNode *n = getfnode(lex);

	ParamCheckNode *new = param_check_node_new(n->prev, n->param_count, 0, param_check_head);
	param_check_head = new;
}

void check_fparams() {
	printf("checks %d %d\n",param_check_head->param_count,param_check_head->param_total);
	if(param_check_head->param_count!=param_check_head->param_total) {
		semerr("Function parameter count does not match definition");
	}
	param_check_head = param_check_head -> next;
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
		complete_function();
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
	int subproghead_t_type;
	switch(tok->token) {
	case TOK_FUNCTION:
		match(TOK_FUNCTION);
		char *lex = match3(tok);
		check_add_green_node(lex, TYPE_F_NAME);
		dparam_count = 0;
		subproghead_t_type = p_subproghead_t();
		set_return_type(subproghead_t_type);
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

int p_subproghead_t() {
	int type_type;

	switch(tok->token) {
	case TOK_PARENS_OPEN:
		p_args();
		match(TOK_COLON);
		type_type = p_stdtype();
		match(TOK_SEMICOLON);
		return type_type;
	case TOK_COLON:
		match(TOK_COLON);
		type_type = p_stdtype();
		match(TOK_SEMICOLON);
		return type_type;
	default:
		sprintf(synerr_buffer,
				"Expecting one of {%s}, {%s}; received {%s}",
				get_token_desc(TOK_PARENS_OPEN),
				get_token_desc(TOK_COLON),
				get_token_desc(tok->token));
		synerr(synerr_buffer);

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_PARENS_OPEN, TOK_COLON, TOK_FUNCTION, TOK_BEGIN, TOK_VAR};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

void p_args() {
	switch(tok->token) {
	case TOK_PARENS_OPEN:
		match(TOK_PARENS_OPEN);
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
			//ERR
			return;
		}
		if(variable_type == TYPE_INT && expr_type == TYPE_INT) {
			//OK
			return;
		}
		if(variable_type == TYPE_REAL && expr_type == TYPE_REAL) {
			//OK
			return;
		}
		if(variable_type == TYPE_A_INT && expr_type == TYPE_A_INT) {
			//OK
			return;
		}
		if(variable_type == TYPE_A_REAL && expr_type == TYPE_A_REAL) {
			//OK
			return;
		}

		//ERR*
		semerr("Assigment types do not match");
		break;
	case TOK_BEGIN:
		p_cmpndstmt();
		break;
	case TOK_IF:
		match(TOK_IF);
		expr_type = p_expr();
		if(expr_type != TYPE_BOOL && expr_type != TYPE_ERR) {
			semerr("if condition must be of type boolean");
		}
		match(TOK_THEN);
		p_stmt();
		p_stmt_t();
		break;
	case TOK_WHILE:
		match(TOK_WHILE);
		expr_type = p_expr();
		if(expr_type != TYPE_BOOL && expr_type != TYPE_ERR) {
			semerr("while condition must be of type boolean");
		}
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
	char *id_lex;
	int id_type;
	int variable_type;

	switch(tok->token) {
	case TOK_ID:
		id_lex = match3(tok);
		id_type = get_type(id_lex);
		variable_type = p_variable_t();

		if(id_type == TYPE_ERR || variable_type == TYPE_ERR) {
			//ERR
			return TYPE_ERR;
		}
		if(is_fp_type(id_type)) {
			//ERR*
			semerr("cannot assign value to function parameter");
			return TYPE_ERR;
		}
		if(id_type==TYPE_A_INT && variable_type==TYPE_ARRAY_INDEX) {
			return TYPE_INT;
		}
		if(id_type==TYPE_A_REAL && variable_type==TYPE_ARRAY_INDEX) {
			return TYPE_REAL;
		}
		if(variable_type==TYPE_ARRAY_INDEX) {
			//ERR*
			semerr("Cannot index non-array type");
			return TYPE_ERR;
		}
		if(id_type == TYPE_A_INT && variable_type == TYPE_OK) {
			return TYPE_A_INT;
		}
		if(id_type == TYPE_A_REAL && variable_type == TYPE_OK) {
			return TYPE_A_REAL;
		}
		if(id_type == TYPE_INT && variable_type == TYPE_OK) {
			return TYPE_INT;
		}
		if(id_type == TYPE_REAL && variable_type == TYPE_OK) {
			return TYPE_REAL;
		}
		if(id_type == TYPE_F_NAME && variable_type == TYPE_OK) {
			return get_return_type(id_lex);
		}
		//???
		semerr("Uknown error: variable/TOK_ID");
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

		if(expr_type == TYPE_ERR) {
			//ERR
			return TYPE_ERR;
		}
		if(expr_type == TYPE_INT) {
			return TYPE_ARRAY_INDEX;
		}

		//ERR*
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

		if(smplexpr_type == TYPE_INT && expr_type == TYPE_OK) {
			return TYPE_INT;
		}

		if(smplexpr_type == TYPE_REAL && expr_type == TYPE_OK) {
			return TYPE_REAL;
		}

		if(smplexpr_type == TYPE_BOOL && expr_type == TYPE_OK) {
			return TYPE_BOOL;
		}

		if(smplexpr_type == TYPE_INT && expr_type == TYPE_INT) {
			return TYPE_BOOL;
		}

		if(smplexpr_type == TYPE_REAL && expr_type == TYPE_REAL) {
			return TYPE_BOOL;
		}

		if(smplexpr_type == TYPE_A_INT && expr_type == TYPE_OK) {
			return TYPE_A_INT;
		}

		if(smplexpr_type == TYPE_A_REAL && expr_type == TYPE_OK) {
			return TYPE_A_REAL;
		}

		//???
		semerr("uknown error: expr/TOK_ID");
		return TYPE_ERR;
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			smplexpr_type = p_smplexpr();
			expr_type = p_expr_t();

			if(expr_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
				return TYPE_ERR;
			}

			if(smplexpr_type == TYPE_INT && expr_type == TYPE_OK) {
				return TYPE_INT;
			}

			if(smplexpr_type == TYPE_REAL && expr_type == TYPE_OK) {
				return TYPE_REAL;
			}

			if(smplexpr_type == TYPE_BOOL && expr_type == TYPE_OK) {
				return TYPE_BOOL;
			}

			if(smplexpr_type == TYPE_INT && expr_type == TYPE_INT) {
				return TYPE_BOOL;
			}

			if(smplexpr_type == TYPE_REAL && expr_type == TYPE_REAL) {
				return TYPE_BOOL;
			}

			if(smplexpr_type == TYPE_A_INT && expr_type == TYPE_OK) {
				return TYPE_A_INT;
			}

			if(smplexpr_type == TYPE_A_REAL && expr_type == TYPE_OK) {
				return TYPE_A_REAL;
			}

			//???
			semerr("uknown error: expr/TOK_ADDOP");
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
	int relop_attr;

	switch(tok->token) {
	case TOK_RELOP:
		relop_attr = tok->attribute;
		match(TOK_RELOP);
		smplexpr_type = p_smplexpr();

		if(smplexpr_type == TYPE_INT) {
			return TYPE_INT;
		}
		if(smplexpr_type == TYPE_REAL) {
			return TYPE_REAL;
		}
		if((relop_attr == RELOP_EQUAL || relop_attr == RELOP_NOT_EQUAL) && smplexpr_type == TYPE_BOOL) {
			return TYPE_BOOL;
		}
		if(smplexpr_type == TYPE_BOOL) {
			//ERR*
			semerr("can only compare boolean types with equal and not equal");
			return TYPE_ERR;
		}
		if(smplexpr_type == TYPE_A_INT || smplexpr_type == TYPE_A_REAL) {
			//ERR*
			semerr("cannot compare array types");
			return TYPE_ERR;
		}
		if(smplexpr_type == TYPE_ERR) {
			//ERR
			return TYPE_ERR;
		}
		//???
		semerr("uknown error: expr_t/TOK_RELOP");
		return TYPE_ERR;
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

		if(term_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		if(term_type == TYPE_INT && (smplexpr_type == TYPE_INT || smplexpr_type == TYPE_OK)) {
			return TYPE_INT;
		}
		if(term_type == TYPE_REAL && (smplexpr_type == TYPE_REAL || smplexpr_type == TYPE_OK)) {
			return TYPE_REAL;
		}
		if(term_type == TYPE_BOOL && (smplexpr_type == TYPE_OK || TYPE_BOOL)) {
			return TYPE_BOOL;
		}
		if(term_type == TYPE_A_INT && smplexpr_type == TYPE_OK) {
			return TYPE_A_INT;
		}
		if(term_type == TYPE_A_REAL && smplexpr_type == TYPE_OK) {
			return TYPE_A_REAL;
		}
		//ERR*
		semerr("Expression type mismatch");
		return TYPE_ERR;
	case TOK_ADDOP:
		if(tok->attribute==ADDOP_ADD || tok->attribute==ADDOP_SUBTRACT) {
			p_sign();
			term_type = p_term();
			smplexpr_type = p_smplexpr_t();

			if(term_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
				return TYPE_ERR;
			}
			if(term_type == TYPE_INT && (smplexpr_type == TYPE_INT || smplexpr_type == TYPE_OK)) {
				return TYPE_INT;
			}
			if(term_type == TYPE_REAL && (smplexpr_type == TYPE_REAL || smplexpr_type == TYPE_OK)) {
				return TYPE_REAL;
			}
			if(term_type == TYPE_BOOL) {
				//ERR*
				semerr("this language supports unsigned booleans only");
				return TYPE_ERR;
			}
			if(term_type == TYPE_A_INT || term_type == TYPE_A_REAL) {
				//ERR*
				semerr("Array types cannot be signed");
				return TYPE_ERR;
			}

			//ERR*
			semerr("Expression type mismatch");
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
	int addop_attr;
	int term_type;
	int smplexpr_type;

	switch(tok->token) {
	case TOK_ADDOP:
		addop_attr = tok->attribute;
		match(TOK_ADDOP);
		term_type = p_term();
		smplexpr_type = p_smplexpr_t();

		if(term_type == TYPE_ERR || smplexpr_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		if(term_type==TYPE_A_INT || term_type == TYPE_A_REAL) {
			//ERR*
			semerr("cannot perform arithmetic on array types");
			return TYPE_ERR;
		}
		if((addop_attr==ADDOP_ADD || addop_attr==ADDOP_SUBTRACT)
				&& term_type == TYPE_INT
				&& (smplexpr_type ==TYPE_OK ||smplexpr_type == TYPE_INT)) {
			return TYPE_INT;
		}
		if((addop_attr==ADDOP_ADD || addop_attr==ADDOP_SUBTRACT)
				&& term_type == TYPE_REAL
				&& (smplexpr_type ==TYPE_OK ||smplexpr_type == TYPE_REAL)) {
			return TYPE_REAL;
		}
		if(addop_attr == ADDOP_ADD || addop_attr == ADDOP_SUBTRACT) {
			//ERR*
			semerr("can only add and subtract numbers of the same type");
			return TYPE_ERR;
		}
		if(addop_attr == ADDOP_OR
				&& term_type==TYPE_BOOL
				&& (smplexpr_type==TYPE_OK || smplexpr_type==TYPE_BOOL)) {
			return TYPE_BOOL;
		}
		if(addop_attr ==ADDOP_OR) {
			//ERR*
			semerr("can only or boolean types");
			return TYPE_ERR;
		}
		//???
		semerr("unknown error: smplexpr_t/TOK_ADDOP");
		return TYPE_ERR;
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

		if(factor_type == TYPE_ERR || term_type == TYPE_ERR) {
			return TYPE_ERR;
		}

		if(factor_type == TYPE_INT && (term_type == TYPE_INT || term_type == TYPE_OK)) {
			return TYPE_INT;
		}
		if(factor_type == TYPE_REAL && (term_type == TYPE_REAL || term_type == TYPE_OK)) {
			return TYPE_REAL;
		}
		if(factor_type == TYPE_BOOL && (term_type == TYPE_BOOL || term_type == TYPE_OK)) {
			return TYPE_BOOL;
		}
		if(factor_type == TYPE_A_INT && term_type == TYPE_OK) {
			return TYPE_A_INT;
		}
		if(factor_type == TYPE_A_REAL && term_type == TYPE_OK) {
			return TYPE_A_REAL;
		}
		//ERR*
		semerr("Term type mismatch");
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

		int array[] = {TOK_SEMICOLON, TOK_EOF, TOK_ID, TOK_NUM, TOK_PARENS_OPEN, TOK_NOT, TOK_RELOP, TOK_END, TOK_ELSE, TOK_THEN, TOK_DO, TOK_SQUARE_BRACKET_CLOSE, TOK_COMMA, TOK_PARENS_CLOSE};
		synch(array, sizeof(array)/sizeof(array[0]));
		return TYPE_ERR;
	}
}

int p_term_t() {
	int mulop_attr;
	int factor_type;
	int term_type;

	switch(tok->token) {
	case TOK_MULOP:
		mulop_attr = tok->attribute;
		match(TOK_MULOP);
		factor_type = p_factor();
		term_type = p_term_t();

		if(factor_type == TYPE_ERR || term_type == TYPE_ERR) {
			return TYPE_ERR;
		}
		if((mulop_attr == MULOP_MULTIPLY || mulop_attr == MULOP_DIV || mulop_attr == MULOP_MOD)
				&& factor_type == TYPE_INT
				&& (term_type == TYPE_INT || term_type == TYPE_OK)) {
			return TYPE_INT;
		}
		if(mulop_attr == MULOP_DIVIDE
				&& factor_type == TYPE_INT
				&& (term_type == TYPE_INT || term_type == TYPE_OK)) {
			//ERR*
			semerr("cannot divide integers (use div or mod)");
			return TYPE_ERR;
		}
		if((mulop_attr == MULOP_MULTIPLY || mulop_attr == MULOP_DIVIDE)
				&& factor_type == TYPE_REAL
				&& (term_type == TYPE_REAL || term_type == TYPE_OK)) {
			return TYPE_REAL;
		}
		if((mulop_attr == MULOP_DIV || mulop_attr == MULOP_MOD)
						&& factor_type == TYPE_REAL
						&& (term_type == TYPE_REAL || term_type == TYPE_OK)) {
			//ERR*
			semerr("cannot div or mod reals");
			return TYPE_ERR;
		}
		if(factor_type == TYPE_A_INT || factor_type == TYPE_A_REAL) {
			//ERR*
			semerr("cannot perform arithmetic on array types");
			return TYPE_ERR;
		}
		if(mulop_attr == MULOP_MULTIPLY || mulop_attr == MULOP_DIVIDE || mulop_attr == MULOP_MOD || mulop_attr == MULOP_DIV) {
			//ERR*
			semerr("can only perform multiply, divide, mod, and div on matched number types");
		}
		if(mulop_attr == MULOP_AND && factor_type == TYPE_BOOL && (term_type==TYPE_OK || term_type ==TYPE_BOOL)) {
			return TYPE_BOOL;
		}
		if(mulop_attr == MULOP_AND) {
			//ERR*
			semerr("can only and boolean types");
			return TYPE_ERR;
		}
		//???
		semerr("Unknown error: term_t/TOK_MULOP");
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
	char *id_lex;
	int id_type;
	int factor_type;

	int expr_type;

	switch(tok->token) {
	case TOK_ID:
		id_lex = match3(tok);
		id_type = get_type(id_lex);

		if(id_type==TYPE_F_NAME) {
			pre_check_fparams(id_lex);
		}

		factor_type = p_factor_t();

		if(id_type==TYPE_F_NAME) {
			check_fparams();
		}

		//table 12
		if(id_type == TYPE_ERR || factor_type ==TYPE_ERR) {
			return TYPE_ERR;
		}
		if((id_type == TYPE_INT || id_type == TYPE_FP_INT) && factor_type == TYPE_OK) {
			return TYPE_INT;
		}
		if((id_type == TYPE_REAL || id_type == TYPE_FP_REAL) && factor_type == TYPE_OK) {
			return TYPE_REAL;
		}
		if((id_type == TYPE_A_INT || id_type == TYPE_FP_A_INT) && factor_type == TYPE_ARRAY_INDEX) {
			return TYPE_INT;
		}
		if((id_type == TYPE_A_REAL || id_type == TYPE_FP_A_REAL) && factor_type == TYPE_ARRAY_INDEX) {
			return TYPE_REAL;
		}
		if((id_type == TYPE_A_INT || id_type == TYPE_FP_A_INT) && factor_type == TYPE_OK) {
			return TYPE_A_INT;
		}
		if((id_type == TYPE_A_REAL || id_type == TYPE_FP_A_REAL) && factor_type == TYPE_OK) {
			return TYPE_A_REAL;
		}
		if(id_type == TYPE_F_NAME && (factor_type == TYPE_OK || factor_type == TYPE_PARAM_LIST)) {
			return get_return_type(id_lex);
		}
		if(factor_type==TYPE_ARRAY_INDEX) {
			//ERR*
			semerr("cannot index non-array type");
			return TYPE_ERR;
		}
		//???
		semerr("unknown error: factor/TOK_ID");
		return TYPE_ERR;
	case TOK_NUM:
		return match2(TOK_NUM, tok->attribute);
	case TOK_PARENS_OPEN:
		match(TOK_PARENS_OPEN);
		expr_type = p_expr();
		match(TOK_PARENS_CLOSE);
		return expr_type;
	case TOK_NOT:
		match(TOK_NOT);
		factor_type = p_factor();

		if(factor_type == TYPE_BOOL) {
			return TYPE_BOOL;
		}

		if(factor_type==TYPE_ERR) {
			//ERR
			return TYPE_ERR;
		}
		//ERR*
		semerr("Not can only be used on boolean types");
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

int p_factor_t() {
	int expr_type;

	switch(tok->token) {
	case TOK_PARENS_OPEN:
		match(TOK_PARENS_OPEN);
		p_exprlst();
		match(TOK_PARENS_CLOSE);
		return TYPE_PARAM_LIST;
	case TOK_SQUARE_BRACKET_OPEN:
		match(TOK_SQUARE_BRACKET_OPEN);
		expr_type = p_expr();
		match(TOK_SQUARE_BRACKET_CLOSE);

		if(expr_type == TYPE_INT) {
			return TYPE_ARRAY_INDEX;
		}
		if(expr_type == TYPE_ERR) {
			//ERR
			return TYPE_ERR;
		}
		//ERR*
		semerr("Array index must be integer");
		return TYPE_ERR;
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
		return TYPE_OK;
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
		return TYPE_ERR;
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
