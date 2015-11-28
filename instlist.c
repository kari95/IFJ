/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   list of instructions
*   implementation of instructions list 
*
*   author:     Karolína Klepáčková (xklepa04)
*   created:    2015-11-14
*   modified:   2015-11-17
*
*****************************************************************************************/
#include <stdlib.h>
#include "instlist.h"

// init 'list'
void initIL(instList_T *list)
{
    list->head    = NULL;
    list->tail    = NULL;
    list->active  = NULL;
    list->count   = 0;
}

// destroy 'list' (free all alocated memory) 
void destroyIL(instList_T *list)
{
    list->active  = NULL;
    list->tail    = NULL;

    while (list->head != NULL)
    {
        instListItem_T *pom = list->head;
        list->head         = pom->right;
        free(pom);
    }  
    list->head = NULL;
}

// insert 'instruction' to 'list'
int insertFirstIL(instList_T *list, instruction_T instruction)
{
    instListItem_T *pom = malloc(sizeof(instListItem_T));
    if (pom == NULL)
        return 99; //exception - out of memory - err 99 by IFJ documentation 
    else
    {
        pom->right  = list->head;
        pom->left   = NULL;
        pom->data   = instruction;

        if (list->head != NULL)
            list->head->left = pom;
        
        list->head = pom;

        if (list->tail == NULL)
        list->tail = pom;
        list->count++;
    }

    return 0;
}

int insertLastIL(instList_T *list, instruction_T instruction)
{
    instListItem_T *pom = malloc(sizeof(instListItem_T));
    if (pom == NULL)
        return 99; //exception - out of memory - err 99 by IFJ documentation 
    else
    {
        pom->right  = NULL;
        pom->left   = list->tail;
        pom->data   = instruction;
        if (list->tail != NULL)
            list->tail->right = pom;
        
        list->tail = pom;

        if (list->head == NULL)
        list->head = pom;
        list->count++;
    }
    return 0;
}

// returns pointer to active item from 'list'
instruction_T *getActiveIL(instList_T *list)
{
    if (list->active != NULL)
        return &list->active->data;
    else
        return NULL;
}

// move activity to next item (instruction) in 'list'
void nextIL(instList_T *list)
{
    if (list->active != NULL)
        list->active = list->active->right;
}

// move activity to nex item (instruction) in 'list'
void startIL(instList_T *list)
{
    list->active = list->head;
}

instruction_T *getNextIL(instList_T *list, instruction_T *instruction)
{
    if (instruction)
    {
        return ((instListItem_T *) instruction)->right;
    }
    else
        return &list->head->data;
}

/*void preInsertIL(instList_T *list, instListItem_T instruction);
void postInsertIL(instList_T *list, instListItem_T instruction);

// move activity in 'list' by 'distance'
void jumpLI(instList_T *list, int distance);*/

