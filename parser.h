/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	parser module header
* 	do a syntax analysis
*
* 	author: 	Miroslav Karásek (xkaras31)
* 	created: 	2015-10-30
* 	modified: 	2015-10-30
*
*****************************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include "instlist.h"

int parse(FILE *programFile, instList_T *program);

#endif