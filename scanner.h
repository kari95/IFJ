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

} tokenType_T;

// struct to save token
typedef struct {
	tokenType_T type;
	void *data;
} token_T;

// read token from 'programFile'
token_T getToken(FILE *programFile);

#endif 