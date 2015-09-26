/*
 * parsers.h
 *
 *  Created on: Sep 26, 2015
 *      Author: Evan
 */

#ifndef PARSERS_H_
#define PARSERS_H_

void p_prog();
void p_prog_t();
void p_prog_tt();
void p_idlst();
void p_idlst_t();
void p_decls();
void p_decls_t();
void p_type();
void p_stdtype();
void p_subprogdecls();
void p_subprogdecls_t();
void p_subprogdecl();
void p_subprogdecl_t();
void p_subprogdecl_tt();
void p_subproghead();
void p_subproghead_t();
void p_args();
void p_paramlst();
void p_paramlst_t();
void p_cmpndstmt();
void p_cmpndstmt_t();
void p_optstmts();
void p_stmtlst();
void p_stmtlst_t();
void p_stmt();
void p_stmt_t();
void p_variable();
void p_variable_t();
void p_exprlst();
void p_exprlst_t();
void p_expr();
void p_expr_t();
void p_smplexpr();
void p_smplexpr_t();
void p_term();
void p_term_t();
void p_factor();
void p_factor_t();
void p_sign();

#endif /* PARSERS_H_ */
