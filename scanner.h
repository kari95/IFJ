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

#include <stdio.h>
#include "string.h"

// types of token
typedef enum {
    ID_TO,          // identificator
    INT_TO,         // xxxxx
    DOUBLE_TO,      // x.xxx
    STRING_TO,      // ""
    PLUS_TO,        // + 
    MINUS_TO,       // - 5
    MUL_TO,         // *
    DIV_TO,         // /
    G_TO,           // >
    L_TO,           // < 
    GE_TO,          // >= 10
    LE_TO,          // <=
    E_TO,           // ==
    NE_TO,          // !=
    LBRACKET_TO,    // (
    RBRACKET_TO,    // ) 15
    SEMI_TO,        // ;
    EOF_TO,         // end of file
    TYPE_TO,        // int/double/string/auto
    CIN_TO,         // cin
    COUT_TO,        // cout 21
    RETURN_TO,      // return
    IF_TO,          // if
    ELSE_TO,        // else
    FOR_TO,         // for
    ASSIGN_TO,      // = 25
    COMMA_TO,       // ,
    LCBRACKET_TO,   // {
    RCBRACKET_TO,   // }  
    OUT_TO,         // << 
    IN_TO           // >> 30
} tokenType_T;

typedef enum {
    INT_TY = INT_TO,
    DOUBLE_TY,
    STRING_TY,
    AUTO_TY
} dataType_T;

// struct to save token
typedef struct {
	tokenType_T type;       // type of token
	union {                 // union to save value of token
        int intValue;       // for INT_TO
        double doubleValue; // for DOUBLE_TO
        char *stringValue;  // for STRING_TO or ID_TO
        dataType_T dataType;// for TYPE_TO
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