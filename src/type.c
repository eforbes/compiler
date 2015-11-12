
#include "type.h"

int array_to_type(int atype) {
	if(atype==TYPE_A_INT || atype == TYPE_FP_A_INT) {
		return TYPE_INT;
	}
	if(atype==TYPE_A_REAL || atype ==  TYPE_FP_A_REAL) {
		return TYPE_REAL;
	}
	return TYPE_ERR;
}
int type_to_array(int type) {
	if(type==TYPE_INT) {
		return TYPE_A_INT;
	}
	if(type==TYPE_REAL) {
		return TYPE_A_REAL;
	}
	return TYPE_ERR;
}

int fp_to_type(int fptype) {
	if(fptype==TYPE_FP_A_INT) {
		return TYPE_A_INT;
	}
	if(fptype==TYPE_FP_A_REAL) {
		return TYPE_A_REAL;
	}
	if(fptype==TYPE_FP_INT) {
		return TYPE_INT;
	}
	if(fptype==TYPE_FP_REAL) {
		return TYPE_REAL;
	}
	return TYPE_ERR;
}

int is_fp_type(int fptype) {
	if(fptype==TYPE_FP_A_INT || fptype==TYPE_FP_A_REAL || fptype==TYPE_FP_INT || fptype==TYPE_FP_REAL) {
		return 1;
	}

	return 0;
}

int is_array_type(int type) {
	if(type == TYPE_A_INT || type == TYPE_A_REAL || type==TYPE_FP_A_INT || type==TYPE_FP_A_REAL) {
		return 1;
	}

	return 0;
}

int type_to_fp(int type){
	if(type == TYPE_INT) {
		return TYPE_FP_INT;
	}
	if(type == TYPE_REAL) {
		return TYPE_FP_REAL;
	}
	if(type == TYPE_A_INT) {
		return TYPE_FP_A_INT;
	}
	if(type == TYPE_A_REAL) {
		return TYPE_FP_A_REAL;
	}
	return TYPE_ERR;
}
