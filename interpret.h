/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	interpret module header
* 	interpret internal code
*
* 	author: 	Libor Janíček (xjanic21)
* 	created: 	2015-10-30
* 	modified: 	2015-11-02
*
*****************************************************************************************/

#ifndef INTERPRET_H
#define INTERPRET_H

#include "instlist.h"
#include "parser.h"

int interpret(block_T *block);

#endif