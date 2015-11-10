
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symbol_table.h"
#include "type.h"
#include "synerr.h"

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
	printf("GREEN NODE: %s, %d\n", fname, ftype);
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
	printf("BLUE NODE: %s, %d\n", lexeme, type);
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
	printf("Completing function\n");
	//move head pointer to top of fstack
	head = fhead -> this;
	//pop from fstack
	fhead = fhead -> next;
}

int get_type(char *lexeme) {
	printf("getting type of %s\n", lexeme);
	SymbolTableNode *cur = head;
	while(cur != NULL) {
		printf("%s?\n", cur -> symbol);
		if(strcmp(cur -> symbol, lexeme) == 0) {
			printf("type: %d\n", cur -> type);
			return cur -> type;
		}
		cur = cur -> next;
	}
	semerr("Undefined identifier");
	return TYPE_ERR;
}

SymbolTableNode *getfnode(char *lexeme) {
	printf("gettingfnode: %s\n", lexeme);
	SymbolTableNode *cur = head;
	while(cur != NULL) {
		if(strcmp(cur -> symbol, lexeme) == 0) {
			return cur;
		}
		cur = cur -> next;
	}
	//this should never happen
	return NULL;
}

int get_return_type(char *lexeme) {
	return getfnode(lexeme)->return_type;
}

void set_param_count(int c) {
	fhead -> this -> param_count = c;
	printf("setting param count for %s %d\n", fhead->this->symbol, c);
}

void set_return_type(int rt) {
	fhead -> this -> return_type = rt;
	printf("setting return type for %s %d\n", fhead->this->symbol, rt);
}
