/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   dynamic string header
*   string with auto realocking
*
*   author:     Karolína Klepáčková (xklepa04)
*   created:    2015-11-18
*   modified:   2015-11-18
*
*****************************************************************************************/

#ifndef STRING_H
#define STRING_H

// size of string after initialization
#define STRING_BASE_SIZE 16

// string structure
typedef struct {
    unsigned int length;
    unsigned int allocatedSize;
    char *data;
} string_T;

// initialization of 'string'
int initS(string_T *string);

// destroy 'string' and free allocated memory
void destroyS(string_T *string);

// add 'character' to end of 'string'
int addCharacterS(string_T *string, char character);

// set string to length 0 (alocated memory are not free)
void cleanS(string_T *string);

// allocation memory for 'string' + 'freeSpace' and copy them
char *allocString(char *string, int freeSpace);

#endif