/*
 * Token.c
 *
 *  Created on: Aug 29, 2015
 *      Author: Evan
 */

#include "token.h"

#include <stdlib.h>

Token *token_new(int token_in, int attribute_in) {
    Token *tok = malloc(sizeof(Token));
    tok->token = token_in;
    tok->attribute = attribute_in;
    return tok;
}

Token *token_mem_new(int token_in, void *attribute_in) {
    Token *tok = malloc(sizeof(Token));
    tok->token = token_in;
    tok->mem = attribute_in;
    return tok;
}
