
#include "token.h"

#include <stdlib.h>

Token *token_new(int token_in, int attribute_in) {
    Token *tok = malloc(sizeof(Token));
    tok -> token = token_in;
    tok -> attribute = attribute_in;
    return tok;
}

Token *token_id_new(int token_in, char *lex_in) {
    Token *tok = malloc(sizeof(Token));
    tok -> token = token_in;
    tok -> lex = lex_in;
    return tok;
}
