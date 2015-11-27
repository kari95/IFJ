/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   ial module
*   implmementation of algorithms to IAL
*
*   author:     Libor Janíček (xjanic21)
*               Jakup Kolb (xkolbj00)
*   created:    2015-11-16
*   modified:   2015-11-16
*
*****************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "ial.h"

// find in 'string' substring 'find'
// returns position of first character of 'find' in 'string'
// if 'string' is empty returns 0, if not found returns -1
// implemented by Knuth-Morris-Pratt alhorithm
/* int find(char *string, char *search); */

// sort characters in 'string'
// use Quick sort algorithm
/* void sort(char *string); */

/****************************************************************************************/

// hash function
int hashFunction (char* key) 
{
    int retval = 1;
    int keylen = strlen(key);
    for ( int i=0; i<keylen; i++ )
        retval += key[i];
    return ( retval % SYMBOL_TABLE_SIZE );
}

// init 'table'
void initST(symbolTable_T *table)
{
    for (int i = 0; i < SYMBOL_TABLE_SIZE; ++i)
        table->items[i] = NULL;
    table->count = 0;
    table->parrent = NULL;
}

// destroy 'table' and free all alocated memory
void destroyST(symbolTable_T *table)
{
    for (int i = 0; i < SYMBOL_TABLE_SIZE; ++i)
    {
        for (symbolTableItem_T *item = table->items[i]; item != NULL; item = item->next)
            free(item);
        table->items[i] = NULL;
    }
    table->count = 0;
    table->parrent = NULL;
}

// search in 'table' by the 'key'
// return data of table item 
symbol_T *searchST(symbolTable_T *table, char *key)
{
    int hash = hashFunction(key);
    for (symbolTableItem_T *item = table->items[hash]; item != NULL; item = item->next)
    {
        if (strcmp(key, item->key) == 0)
            return &item->data;
    }
    if (table->parrent) 
    {
        // search in parrent table
        return searchST(table->parrent, key);
    }
    return NULL;
}

// insert in to 'table' item with 'data' and 'key'
// if 'key' exist, will be 'data' updated
// return NULL if fail
symbol_T *insertST(symbolTable_T *table, char *key, symbol_T data)
{
    int hash = hashFunction(key);
    symbol_T *itemData = NULL;
    for (symbolTableItem_T *item = table->items[hash]; item != NULL; item = item->next)
    {
        if (strcmp(key, item->key) == 0)
            itemData = &item->data;
    }
    symbolTableItem_T *item;
    if (itemData == NULL)
    {
        item = malloc(sizeof(symbolTableItem_T) + strlen(key) + 1);
        if (item == NULL) 
            return NULL;
        strcpy(item->key, key);
        item->data = data;
        item->next = table->items[hash];
        table->items[hash] = item;
        table->count++;
    }
    else
        *itemData = data;
    return &item->data;
}

