
#ifndef LEXAN_H_
#define LEXAN_H_

#include "token.h"
#include <stdio.h>

char *input_file_name;
char next_char();
void move_f_back();
char *get_lexeme();
Token *get_token();
FILE *output_file;

#endif /* LEXAN_H_ */
