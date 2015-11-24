/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   dynamic string 
*   string with auto realocking
*
*   author:     Karolína Klepáčková (xklepa04)
*   created:    2015-11-22
*   modified:   2015-11-22
*
*****************************************************************************************/

#include <stdlib.h>
#include "string.h"

// initialization of 'string'
int initS(string_T *string)
{
    string->length = 0;
    string->allocatedSize = STRING_BASE_SIZE;
    string->data = malloc(STRING_BASE_SIZE + 1); // +1 for '\0'
    if (string->data == NULL)
    {
        string->allocatedSize = 0; // alocation error
        return 99;
    }
    string->data[0] = '\0'; // string terminator
    return 0;
}

// destroy 'string' and free allocated memory
void destroyS(string_T *string)
{
    string->length = 0;
    string->allocatedSize = 0;
    free(string->data);
}

// add 'character' to end of 'string'
int addCharacterS(string_T *string, char character)
{
    if (string->length >= string->allocatedSize)
    {
        int newSize = 2 * string->allocatedSize;
        char *newData = realloc(string->data, newSize + 1);
        if (newData == NULL)
            return 99;
        string->allocatedSize = newSize;
        string->data = newData;
    }
    string->data[string->length] = character;
    string->length++;
    string->data[string->length] = '\0';
    return 0;
}

// set string to length 0 (alocated memory are not free)
void cleanS(string_T *string)
{
    string->length = 0;
    string->data[0] = '\0'; // string terminator
}