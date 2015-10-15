/*
 * synerr.h
 *
 *  Created on: Sep 26, 2015
 *      Author: Evan
 */

#ifndef SYNERR_H_
#define SYNERR_H_

void synerr(char *message);
char *get_token_desc(int token_id);
void synch(int *synch_tokens, int size);

#endif /* SYNERR_H_ */
