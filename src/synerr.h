/*
 * synerr.h
 *
 *  Created on: Sep 26, 2015
 *      Author: Evan
 */

#ifndef SYNERR_H_
#define SYNERR_H_

int synerr_count;
int semerr_count;

void synerr(char *message);
void semerr(char *message);
char *get_token_desc(int token_id);
char *get_type_desc(int type);
void synch(int *synch_tokens, int size);

#endif /* SYNERR_H_ */
