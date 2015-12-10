/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	parser module header
* 	do a syntax analysis
*
* 	author: 	Miroslav Karásek (xkaras31)
* 	created: 	2015-10-30
* 	modified: 	2015-11-17
*
*****************************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include "ial.h"
#include "instlist.h"
#include "scanner.h"

// struc of {} block
typedef struct block {
    struct block *parrent;
    symbolTable_T symbols;
    instList_T program;
} block_T;

// parse 'programFile' an generate list of instructions 'program'
int parse(FILE *programFile, block_T *block);

// parse 'programFile' an generate list of instructions 'program'
void freeParser();

#endif