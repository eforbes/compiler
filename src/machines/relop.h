
#include "../Token.h"

#ifndef MACHINES_RELOP_H_
#define MACHINES_RELOP_H_

#define RELOP_EQUAL 0
#define RELOP_NOT_EQUAL 1
#define RELOP_LESS_THAN 2
#define RELOP_LESS_THAN_OR_EQUAL 3
#define RELOP_GREATER_THAN 4
#define RELOP_GREATER_THAN_OR_EQUAL 5

Token *run_relop();

#endif /* MACHINES_RELOP_H_ */
