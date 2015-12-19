
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symbol_table.h"
#include "type.h"
#include "synerr.h"
#include "lexan.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

#define COLOR_GREEN 1
#define COLOR_BLUE 2

#define SYMERR_BUFFER_SIZE 256

char symerr_buffer[SYMERR_BUFFER_SIZE];

SymbolTableNode *head;
FunctionNode *fhead;

SymbolTableNode *symbol_table_node_new(char *sym, int type_in, int array_size_in, int color_in, SymbolTableNode *nex) {
	SymbolTableNode *n = malloc(sizeof(SymbolTableNode));
    n -> symbol = sym;
    n -> type = type_in;
    n -> color = color_in;
    n -> param_count = 0;
    n -> array_size = array_size_in;
    n -> next = nex;

    if(color_in == COLOR_GREEN) {
    	n -> address = 0;
    } else {
    	int addr =  fhead->this->address;
    	int sz = get_size_of(type_in);
    	if(is_array_type(type_in)) {
    		sz *= array_size_in;
    	}
    	n -> address = addr;
    	fhead->this->address+= sz;
    	if(sz>0) {
    		fprintf(address_file, "%s\t%08x (%d)\n", sym, addr, addr);
    	}
    }

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
		head = symbol_table_node_new(fname, ftype, 0, COLOR_GREEN, NULL);
		fhead = function_node_new(head, NULL);
	} else {
		SymbolTableNode *cur = head;
		while(cur != NULL) {
			if(strcmp(cur -> symbol, fname) == 0) {
				sprintf(symerr_buffer,
					"Duplicate identifier for function name in this scope: %s",
					fname);
				semerr(symerr_buffer);
				break;
			}
			cur = cur -> next;
		}

		SymbolTableNode *new = symbol_table_node_new(fname, ftype, 0, COLOR_GREEN, head);
		if(head -> prev == NULL) {
			head -> prev = new;
		}
		head = new;
		FunctionNode *fnew = function_node_new(new, fhead);
		fhead = fnew;
	}
}

void check_add_blue_node(char *lexeme, int type, int array_size) {
	SymbolTableNode *cur = head;
	while(cur != NULL) {
		if(strcmp(cur -> symbol, lexeme) == 0) {
			sprintf(symerr_buffer,
				"Duplicate variable declaration in this scope: %s",
				lexeme);
			semerr(symerr_buffer);
			return;
		}
		if(cur->color == COLOR_GREEN) {
			//OK, add blue node
			SymbolTableNode *new = symbol_table_node_new(lexeme, type, array_size, COLOR_BLUE, head);
			if(head -> prev == NULL) {
				head -> prev = new;
			}
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
	sprintf(symerr_buffer,
		"Undefined identifier in this scope: %s",
		lexeme);
	semerr(symerr_buffer);
	return TYPE_ERR;
}

SymbolTableNode *getfnode(char *lexeme) {
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
}

void set_return_type(int rt) {
	fhead -> this -> return_type = rt;
}

//returns size of type in bits
int get_size_of(int type) {
	if(type==TYPE_INT || type==TYPE_A_INT) return 4;
	if(type==TYPE_REAL || type == TYPE_A_REAL) return 8;

	return 0;
}

void set_array_size(int sz) {
	head->array_size = sz;
}
