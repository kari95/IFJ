/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	pointer stack
* 	stack to calling functions in interpret or for temp variables
*
* 	author: 	Libor Janíček (xjanic21)
* 	created: 	2015-11-16
* 	modified: 	2015-11-16
*
*****************************************************************************************/

#include <stdlib.h>
#include "pointerstack.h"

// init 'stack'
void initPS(pointerStack_T *stack)
{
	stack->top = NULL;
}

// destroy 'stack' and free allocated memory
void destroyPS(pointerStack_T *stack)
{
	while (stack->top)
		popPS(stack);
}

// add new item with 'data' to 'stack'
int pushPS(pointerStack_T *stack, void *data)
{
	pointerStackItem_T *newItem = malloc(sizeof(pointerStackItem_T));
	if (newItem)
	{
		newItem->data = data;
		newItem->next = stack->top;
		stack->top = newItem;
		return 0;
	}
	else
		return 99;
}

// delete item on top from 'stack' and free allocated memory
void popPS(pointerStack_T *stack)
{
	pointerStackItem_T *delItem = stack->top;
	if (delItem)
	{
		stack->top = delItem->next;
		free(delItem);
	}
}

// return data of item on top 'stack'
void *topPS(pointerStack_T *stack)
{
	if (stack->top)
		return stack->top->data;
	else 
		return NULL;
}

