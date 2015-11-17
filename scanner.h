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
    ID_TO,          // identificator
    INT_TO,         // xxxxx
    DOUBLE_TO,      // x.xxx
    STRING_TO,      // ""
    ASSIGN_TO,      // =
    LBRACKET_TO,    // (
    RBRACKET_TO,    // )  
    SEMI_TO,        // ;
    COMMA_TO,       // ,
    LCBRACKET_TO,   // {
    RCBRACKET_TO,   // }  
    OUT_TO,         // <<
    IN_TO,          // >>
    PLUS_TO,        // +
    MINUS_TO,       // -
    MUL_TO,         // *
    DIV_TO,         // /
    G_TO,           // >
    L_TO,           // <
    GE_TO,          // >=
    LE_TO,          // <=
    EOF_TO          // end of file
} tokenType_T;

// struct to save token
typedef struct {
	tokenType_T type;
	void *data;
} token_T;

// read token from 'programFile' to structure 'token'
// return zero if success
int getToken(FILE *programFile, token_T *token);

#endif 