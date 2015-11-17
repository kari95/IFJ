/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	scaner module header
* 	do a lexical analysis
*
* 	author: 	Karolína Klepáčková (xklepa04)
* 	created: 	2015-10-30
* 	modified: 	2015-11-17
*
*****************************************************************************************/

#ifndef SCANNER_H
#define SCANNER_H

// types of token
typedef enum {
    ID_TO,
    INT_TO,
    DOUBLE_TO,
    STRING_TO,

} tokenType;

// struct to save token
typedef struct {
	int type;
	void *data;
} token_t;

// read token from 'programFile'
token_t getToken(FILE *programFile);

#endif 