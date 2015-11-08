
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symbol_table.h"
#include "type.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

#define COLOR_GREEN 1
#define COLOR_BLUE 2

SymbolTableNode *head;

SymbolTableNode *symbol_table_node_new(char *sym, int type_in, int color_in, SymbolTableNode *nex) {
	SymbolTableNode *n = malloc(sizeof(SymbolTableNode));
    n -> symbol = sym;
    n -> type = type_in;
    n -> color = color_in;
    n -> param_count = 0;
    n -> next = nex;
    return n;
}

//void *get_or_add_symbol(char *sym) {
//	if(!symbol_table_initialized) {
//		init_symbol_table();
//	}
//
//	SymbolTableNode *cur = head;
//	while(cur -> next != NULL) {
//		if(strcmp(cur -> symbol, sym) == 0) {
//			return cur -> symbol;
//		}
//		cur = cur -> next;
//	}
//
//	int l = strlen(sym);
//	char *new_sym = (char*) malloc((l + 1) * sizeof(char));
//	memcpy(new_sym, sym, l);
//	new_sym[l] = '\0';
//
//	SymbolTableNode *new = symbol_table_node_new(new_sym, head);
//	head = new;
//	return new_sym;
//}

void check_add_green_node(char *fname, int ftype) {
	if(head==NULL) {
		head = symbol_table_node_new(fname, ftype, COLOR_GREEN, NULL);
	} else {
		SymbolTableNode *cur = head;
		while(cur -> next != NULL) {
			if(strcmp(cur -> symbol, fname) == 0) {
				//semerr
				return;
			}
			cur = cur -> next;
		}

		SymbolTableNode *new = symbol_table_node_new(fname, ftype, COLOR_GREEN, head);
		head = new;
	}
}

void check_add_blue_node(char *lexeme, int type) {

}

int get_type(char *lexeme) {
	return TYPE_UNDEFINED;
}
