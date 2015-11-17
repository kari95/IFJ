/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   ial module header
*   implmementation of algorithms to IAL
*
*   author:     Libor Janíček (xjanic21)
*               Jakup Kolb (xkolbj00)
*   created:    2015-10-30
*   modified:   2015-11-16
*
*****************************************************************************************/

#ifndef IAL_H
#define IAL_H

// find in 'string' substring 'find'
// returns position of first character of 'find' in 'string'
// if 'string' is empty returns 0, if not found returns -1
// implemented by Knuth-Morris-Pratt alhorithm
int find(char *string, char *search);

// sort characters in 'string'
// use Quick sort algorithm
void sort(char *string);

/****************************************************************************************/

#define SYMBOL_TABLE_SIZE 10

// types of symbols
typedef enum {
	ERR_T,
    INT_T,
    DOUBLE_T,
    STRING_T,
    FUNCTION_T,
} symbolType_T;

// symbol structure
typedef struct {
    symbolType_T typ;
    double data;
} symbol_T;

// symbol table item
typedef struct symbolTableItem {
    symbol_T data;
    struct symbolTableItem *next;
    char key[];
} symbolTableItem_T;

// symbol table structure
typedef struct symbolTable {
    struct symbolTable *parrent;
    symbolTableItem_T *items[SYMBOL_TABLE_SIZE];
} symbolTable_T;

// init 'table'
void initST(symbolTable_T *table);

// destroy 'table' and free all alocated memory
void destroyST(symbolTable_T *table);

// search in 'table' by the 'key'
// return data of table item 
symbol_T *searchST(symbolTable_T *table, char *key);

// insert in to 'table' item with 'data' and 'key'
// if 'key' exist, will be 'data' updated
// return zero if sucess
int insertST(symbolTable_T *table, char *key, symbol_T data);

#endif
