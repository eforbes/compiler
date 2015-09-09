
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symbol_table.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

SymbolTableNode *head;

SymbolTableNode *symbol_table_node_new(char *sym, SymbolTableNode *nex) {
	SymbolTableNode *n = malloc(sizeof(SymbolTableNode));
    n -> symbol = sym;
    n -> next = nex;
    return n;
}

void *get_or_add_symbol(char *sym) {
	SymbolTableNode *cur = head;
	while(cur -> next != NULL) {
		if(strcmp(cur -> symbol, sym) == 0) {
			return cur -> symbol;
		}
		cur = cur -> next;
	}

	int l = strlen(sym);
	char *new_sym = (char*) malloc((l + 1) * sizeof(char));
	memcpy(new_sym, sym, l);
	new_sym[l] = '\0';

	SymbolTableNode *new = symbol_table_node_new(new_sym, head);
	head = new;
	return new_sym;
}

void init_symbol_table() {
	head = symbol_table_node_new("", NULL);
}
