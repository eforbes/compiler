/*
 Evan Forbes
 Fall 2015
 CS 4013, Dr. Shenoi
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "machines/Whitespace.h"
#include "machines/id.h"
#include "machines/long_real.h"
#include "machines/real.h"
#include "machines/int.h"
#include "machines/assignop.h"
#include "machines/punctuation.h"
#include "machines/relop.h"
#include "machines/addop.h"
#include "machines/mulop.h"
#include "machines/catch_all.h"

#include "Token.h"
#include "symbol_table.h"
#include "reserved_words.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

#define LINE_MAX 72

char buffer[LINE_MAX];
FILE *inputFile, *outputFile, *tokenFile;

int line_number = 1;
int b = 0, f = 0;

void open_files(char *inputFileName) {
	char outputFileName[strlen(inputFileName)+6];
	strcpy(outputFileName, inputFileName);
	strcat(outputFileName, ".lexan");

	char tokenFileName[strlen(inputFileName)+6];
	strcpy(tokenFileName, inputFileName);
	strcat(tokenFileName, ".token");

	inputFile = fopen(inputFileName, "r");
	outputFile = fopen(outputFileName, "w");
	tokenFile = fopen(tokenFileName, "w");
}

char* load_line(char *inputFileName) {
	if(inputFile==NULL && inputFileName!=NULL) {
		open_files(inputFileName);
	}
	if(inputFile == NULL || outputFile == NULL) {
		fprintf(stderr, "Can't open input or output file");
		exit(1);
	} else {
		return fgets(buffer, LINE_MAX, inputFile);
	}
}

int current_machine = 0;

Token *run_next_machine() {
	switch(current_machine) {
		case 0: return run_ws();
		case 1: return run_id();
		case 2: return run_long_real();
		case 3: return run_real();
		case 4: return run_int();
		case 5: return run_assignop();
		case 6: return run_punctuation();
		case 7: return run_relop();
		case 8: return run_addop();
		case 9: return run_mulop();
		default: return run_catch_all();
	}
}


void process_file(char *inputFileName) {
	init_reserved_words();
	init_symbol_table();

	while(load_line(inputFileName) != NULL) {
		fprintf(outputFile, "%d\t\t%s", line_number, buffer);

		f = 0;
		b = 0;
		while(buffer[f] != '\n') {
			current_machine = 0;
			Token *result = run_next_machine();
			while(result -> token == TOK_BLOCKED) {
				f = b; // reset f
				current_machine++; // move to next machine
				result = run_next_machine();
			}

			int length = f - b;
			char lexeme[length + 1];
			memcpy(lexeme, &buffer[b], length);
			lexeme[length] = '\0';

			if(result -> token == TOK_LEXERR) {
				//TODO: print helpful error message
				fprintf(outputFile, "LEXERR:\t%s (lexerr #%d)\n", lexeme, result -> attribute);
			}

			if(result -> token != TOK_WS) {
				if(result ->token != TOK_ID) {
					printf("%s (%d, %d)\n", lexeme, result->token, result->attribute);
					fprintf(tokenFile, "%d\t%s\t%d\t%d\n", line_number, lexeme, result->token, result->attribute);
				} else {
					printf("%s (%d, %p)\n", lexeme, result->token, result->mem);
					fprintf(tokenFile, "%d\t%s\t%d\t%p\n", line_number, lexeme, result->token, result->mem);
				}

			}

			b = f; // advance b

		}

		line_number++;
	}

	fclose(inputFile);
	fclose(outputFile);
	fclose(tokenFile);
}

char *get_lexeme() {
	int length = f - b;
	char *lexeme = (char*) malloc((length+1)*sizeof(char));
	memcpy(lexeme, &buffer[b], length);
	lexeme[length] = '\0';
	return lexeme;
}

char next_char() {
	return buffer[f++];
}

void move_f_back() {
	f--;
}

int main(void) {
	process_file("example.txt");
	puts("Done");
	return 0;
}
