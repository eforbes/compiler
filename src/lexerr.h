
#ifndef LEXERR_H_
#define LEXERR_H_

#define LEXERR_ID_TOO_LONG 1
#define LEXERR_UNREC_SYMBOL 99
#define LEXERR_FLAG_BIT_NUM 8

void print_lexerr_text(FILE* f, char* lexeme, int err_n);

#endif /* LEXERR_H_ */
