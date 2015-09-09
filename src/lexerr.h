
#ifndef LEXERR_H_
#define LEXERR_H_

struct LexerrNode {
	int err_no;
	char *err_text;
	struct LexerrNode *next;
};

typedef struct LexerrNode LexerrNode;

LexerrNode *lexerr_node_new(int err_n, char *err_t, LexerrNode *nex);

char *get_lexerr_text(int err_n);
void init_lexerr();

#endif /* LEXERR_H_ */
