
#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

struct SymbolTableNode {
	char *symbol;
	int type;

	int color;

	int param_count;
	int return_type;
	struct SymbolTableNode *next;
	struct SymbolTableNode *prev;
};

typedef struct SymbolTableNode SymbolTableNode;

SymbolTableNode *symbol_table_node_new(char *sym, int type_in, int color_in, SymbolTableNode *nex);

struct FunctionNode {
	SymbolTableNode *this;
	struct FunctionNode *next;
};

typedef struct FunctionNode FunctionNode;

FunctionNode *function_node_new(SymbolTableNode *fnode, FunctionNode *next);

void check_add_green_node(char *fname, int ftype);
void check_add_blue_node(char *lexeme, int type);

int get_type(char *lexeme);

SymbolTableNode *getfnode(char *lexeme);

void complete_function();

void set_param_count(int c);
void set_return_type(int rt);

#endif /* SYMBOL_TABLE_H_ */
