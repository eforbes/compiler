/*
 * type.c
 *
 *  Created on: Nov 7, 2015
 *      Author: Evan
 */

#include "type.h"

int array_to_type(int atype) {
	if(atype==TYPE_A_INT) {
		return TYPE_INT;
	}
	if(atype==TYPE_A_REAL) {
		return TYPE_REAL;
	}
	return TYPE_ERR_NEW;
}
int type_to_array(int type) {
	if(type==TYPE_INT) {
		return TYPE_A_INT;
	}
	if(type==TYPE_REAL) {
		return TYPE_A_REAL;
	}
	return TYPE_ERR_NEW;
}
