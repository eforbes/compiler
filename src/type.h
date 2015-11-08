/*
 * type.h
 *
 *  Created on: Nov 7, 2015
 *      Author: Evan
 */

#ifndef TYPE_H_
#define TYPE_H_

#define TYPE_UNDEFINED 0

#define TYPE_INT 1
#define TYPE_REAL 2
#define TYPE_BOOL 3

#define TYPE_A_INT 11
#define TYPE_A_REAL 12

#define TYPE_F_NAME 100
#define TYPE_FP_INT 101
#define TYPE_FP_REAL 102
#define TYPE_FP_A_INT 111
#define TYPE_FP_A_REAL 112

#define TYPE_PGM_NAME 300
#define TYPE_PGM_PARAM 301

#define TYPE_OK 400

#define TYPE_ERR 998
#define TYPE_ERR_NEW 999

int array_to_type(int atype);
int type_to_array(int type);
int fp_to_type(int fptype);
int is_fp_type(int fptype);
int is_array_type(int type);

#endif /* TYPE_H_ */
