/*
 * lexerr.c
 *
 *  Created on: Sep 7, 2015
 *      Author: Evan
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexerr.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

#define NUMBER_OF_LEXERR 51

LexerrNode *lexerr_node_new(int err_n, char* err_t, LexerrNode *nex) {
	LexerrNode *n = malloc(sizeof(LexerrNode));
    n->err_no = err_n;
    n->err_text = err_t;
    n->next = nex;
    return n;
}

LexerrNode *head_lexerr;

char *get_lexerr_text(int err_n) {
	LexerrNode *cur = head_lexerr;
	while(cur->next!=NULL) {
		if(err_n==cur->err_no) {
			return cur->err_text;
		}
		cur = cur->next;
	}

	return "Unknown error";
}

char lexerr_w[NUMBER_OF_LEXERR][256];

void init_lexerr() {
	head_lexerr = lexerr_node_new(-1, "", NULL);

	FILE* file = fopen ("lexerr.txt", "r");

	int i, n;
	for(i=0; i < NUMBER_OF_LEXERR; i++)
	{
		fscanf (file, "%d %256[0-9a-zA-Z, ]", &n, lexerr_w[i]);

		LexerrNode *new_node = lexerr_node_new(n, lexerr_w[i], head_lexerr);
		head_lexerr = new_node;

	}
	fclose (file);
}
