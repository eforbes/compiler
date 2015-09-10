
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "reserved_words.h"
#include "token.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

#define NUMBER_OF_RESERVED_WORDS 20

ReservedWordNode *reserved_word_node_new(char *sym,
		int t,
		int a,
		ReservedWordNode *nex) {
	ReservedWordNode *n = malloc(sizeof(ReservedWordNode));
    n -> word = sym;
    n -> token = t;
    n -> attr = a;
    n -> next = nex;
    return n;
}

ReservedWordNode *head_n;
char w[NUMBER_OF_RESERVED_WORDS][11];

void init_reserved_words() {
	head_n = reserved_word_node_new("", 0, 0, NULL);

	FILE *file = fopen("reserved_words.txt", "r");

	int i, t, a;
	for(i = 0; i < NUMBER_OF_RESERVED_WORDS; i++)
	{
		fscanf (file, "%s\t%d\t%d", w[i], &t, &a);

		ReservedWordNode *new_node = reserved_word_node_new(w[i], t, a, head_n);
		head_n = new_node;
	}

	fclose(file);
}

Token *get_reserved_token(char *sym) {
	ReservedWordNode *curn = head_n;

	while(curn -> next != NULL) {
		if(strcmp(curn -> word, sym) == 0) {
			Token *result_token = token_new(curn -> token, curn -> attr);
			return result_token;
		}
		curn = (curn -> next);
	}

	Token *not_reserved_word_token = token_new(TOK_BLOCKED, 0);
	return not_reserved_word_token;
}
