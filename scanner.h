/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	scaner module header
* 	do a lexical analysis
*
* 	author: 	Karolína Klepáčková (xklepa04)
* 	created: 	2015-10-30
* 	modified: 	2015-10-30
*
*****************************************************************************************/

#ifndef SCANNER_H
#define SCANNER_H

typedef struct {
	int type;
	void *data;
} token_t;

token_t getToken(FILE *programFile);

#endif 