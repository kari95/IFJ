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

#include "instlist.h"
#include "scanner.h"

// parse 'programFile' an generate list of instructions 'program'
int parse(FILE *programFile, instList_T *program);

#endif