
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
FunctionNode *fhead;

SymbolTableNode *symbol_table_node_new(char *sym, int type_in, int color_in, SymbolTableNode *nex) {
	SymbolTableNode *n = malloc(sizeof(SymbolTableNode));
    n -> symbol = sym;
    n -> type = type_in;
    n -> color = color_in;
    n -> param_count = 0;
    n -> next = nex;
    return n;
}

FunctionNode *function_node_new(SymbolTableNode *fnode, FunctionNode *next) {
	FunctionNode *n = malloc(sizeof(FunctionNode));
	n -> this = fnode;
	n -> next = next;
	return n;
}

//function nodes
void check_add_green_node(char *fname, int ftype) {
	if(head==NULL) {
		head = symbol_table_node_new(fname, ftype, COLOR_GREEN, NULL);
		fhead = function_node_new(head, NULL);
	} else {
		SymbolTableNode *cur = head;
		while(cur != NULL) {
			if(strcmp(cur -> symbol, fname) == 0) {
				//semerr
				return;
			}
			cur = cur -> next;
		}

		SymbolTableNode *new = symbol_table_node_new(fname, ftype, COLOR_GREEN, head);
		head -> prev = new;
		head = new;
		FunctionNode *fnew = function_node_new(new, fhead);
		fhead = fnew;
	}
}

void check_add_blue_node(char *lexeme, int type) {
	SymbolTableNode *cur = head;
	while(cur != NULL) {
		if(strcmp(cur -> symbol, lexeme) == 0) {
			//semerr duplicate variable declaration in this scope
			return;
		}
		if(cur->color == COLOR_GREEN) {
			//OK, add blue node
			SymbolTableNode *new = symbol_table_node_new(lexeme, type, COLOR_BLUE, head);
			head -> prev = new;
			head = new;
			return;
		}
		cur = cur -> next;
	}
	//this should never happen
	return;
}

void complete_function() {
	//move head pointer to top of fstack
	head = fhead -> this;
	//pop from fstack
	fhead = fhead -> next;
}

int get_type(char *lexeme) {
	SymbolTableNode *cur = head;
	while(cur != NULL) {
		if(strcmp(cur -> symbol, lexeme) == 0) {
			return cur -> type;
		}
		cur = cur -> next;
	}

	return TYPE_ERR;
}

SymbolTableNode *getfnode(char *lexeme) {
	FunctionNode *cur = fhead;
	while(cur != NULL) {
		if(strcmp(cur -> this -> symbol, lexeme) == 0) {
			return cur -> this;
		}
		cur = cur -> next;
	}
	//this should never happen
	return NULL;
}
