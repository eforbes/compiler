
#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

struct SymbolTableNode {
	char *symbol;
	struct SymbolTableNode *next;
};

typedef struct SymbolTableNode SymbolTableNode;

SymbolTableNode *symbol_table_node_new(char *sym, SymbolTableNode *next);

void *get_or_add_symbol(char *sym);
void init_symbol_table();

#endif /* SYMBOL_TABLE_H_ */
