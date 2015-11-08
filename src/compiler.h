/*
 * compiler.h
 *
 *  Created on: Sep 26, 2015
 *      Author: Evan
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#include "token.h"

Token *tok;
void match(int t);
int match2(int t, int attr);
char *match3(Token *t);

#endif /* COMPILER_H_ */
