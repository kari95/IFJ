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

int parti(char a[],int p, int r)
{
    char x = a[r];
    int i = p-1;
    int j;
    char c;
    char d;
    for (j = p; j < r; j++){
        if (a[j]<=x){
            i++;
            c = a[i];
            a[i] = a[j];
            a[j] = c;
        }
    }
    d = a[i + 1];
    a[i + 1] = a[r];
    a[r] = d;
    return i+1;

}
void quicksort(char *string, int p, int r)
{
    if (p<r)
    {
        int q = parti(string, p, r);
        quicksort(string, p, q-1);
        quicksort(string, q+1, r);
    }
}

// find in 'string' substring 'find'
// returns position of first character of 'find' in 'string'
// if 'string' is empty returns 0, if not found returns -1
// implemented by Knuth-Morris-Pratt alhorithm
int find(char *s, char *search)
{
    int delka=strlen(search);
    int delkaretezce=strlen(s);
    int i,j=0;
    int k=0,l=0;
    int nasel=0;
    for(i=0;i<delkaretezce;i++){
        if(i>0 && j>0){
                if(s[i]==search[l]){
                    l++;
                    k=i;
                }else{
                    l=0;
                    k=0;
                }
        }
        if(s[i]==search[j]){
            j++;
            if(delka==j){
                nasel=1;
                break;
            }
        }else{
            if(l>0){
                i=k;
                j=l;
            }else{
                i=i-j;
                j=0;
            }
        }
    }
    if(nasel==1){
        return i-delka+1;
    }else if(delka==0 && delkaretezce>0){
        return 0;
    }else{
        return -1;
    }
}

// sort characters in 'string'
// use Quick sort algorithm
void sort(char *string)
{
    int length = strlen(string);
    quicksort(string, 0, length - 1);
}

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
// recursive means that symbol will be search in parrent tables
symbol_T *searchST(symbolTable_T *table, char *key, bool recursive)
{
    int hash = hashFunction(key);
    for (symbolTableItem_T *item = table->items[hash]; item != NULL; item = item->next)
    {
        if (strcmp(key, item->key) == 0)
            return &item->data;
    }
    if (table->parrent && recursive) 
    {
        // search in parrent table
        return searchST(table->parrent, key, true);
    }
    return NULL;
}

// insert in to 'table' item with 'data' and 'key'
// if 'key' exist, will be 'data' updated
// return NULL if fail
symbol_T *insertST(symbolTable_T *table, char *key, symbol_T data)
{
    int hash = hashFunction(key);
    symbol_T *itemData = searchST(table, key, false);
    if (itemData == NULL)
    {
        symbolTableItem_T *item = malloc(sizeof(symbolTableItem_T) + strlen(key) + 1);
        if (item == NULL) 
            return NULL;
        strcpy(item->key, key);
        item->data = data;
        item->next = table->items[hash];
        table->items[hash] = item;
        table->count++;
        itemData = &item->data;
    }
    else
        *itemData = data;
    return itemData;
}

