/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	pointer stack header
* 	stack to calling functions in interpret or for temp variables
*
* 	author: 	Libor Janíček (xjanic21)
* 	created: 	2015-11-16
* 	modified: 	2015-11-16
*
*****************************************************************************************/

#ifndef ADR_STACK_H
#define ADR_STACK_H

// structure of stack item
typedef struct pointerStackItem {
	void *data;
	struct pointerStackItem *next;
} pointerStackItem_T;

// stack structure
typedef struct {
	pointerStackItem_T *top;
} pointerStack_T;

// init 'stack'
void initPS(pointerStack_T *stack);

// destroy 'stack' and free allocated memory
void destroyPS(pointerStack_T *stack);

// add new item with 'data' to 'stack'
int pushPS(pointerStack_T *stack, void *data);

// delete item on top from 'stack' and free allocated memory
void popPS(pointerStack_T *stack);

// return data of item on top 'stack'
void *topPS(pointerStack_T *stack);

#endif
