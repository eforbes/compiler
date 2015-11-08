
#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

struct SymbolTableNode {
	char *symbol;
	int type;

	int color;

	int param_count;
	struct SymbolTableNode *next;
};

typedef struct SymbolTableNode SymbolTableNode;

SymbolTableNode *symbol_table_node_new(char *sym, SymbolTableNode *next);

void check_add_green_node(char *fname, int ftype);
void check_add_blue_node(char *lexeme, int type);

int get_type(char *lexeme);

#endif /* SYMBOL_TABLE_H_ */
