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

#include "string.h"

// types of token
typedef enum {
    ID_TO,          // identificator
    TYPE_TO,        // int/double/string/auto
    CIN_TO,         // cin
    COUT_TO,        // cout
    RETURN_TO,      // return
    IF_TO,          // if
    ELSE_TO,        // else
    FOR_TO,         // for
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
    E_TO,           // ==
    NE_TO,          // !=
    EOF_TO          // end of file
} tokenType_T;

// struct to save token
typedef struct {
	tokenType_T type;       // type of token
	union {                 // union to save value of token
        int intValue;       // for INT_TO
        double doubleValue; // for DOUBLE_TO
        string_T *stringValue;  // for STRING_TO or ID_TO
    };
    unsigned int col;    // position in source code (from 1)
    unsigned int row;
} token_T;

// init scanner an set 'programFile' as input file
void initSC(FILE *programFile);

// read token to structure 'token'
// return zero if success
int getTokenSC(token_T *token);

#endif 