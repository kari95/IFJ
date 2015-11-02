/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   main module
*   open input file and call others modules to process it and interpret the code 
*
*   author:     Name Surname (xlogin00)
*   created:    2015-10-30
*   modified:   2015-11-02
*
*****************************************************************************************/

#ifndef IAL_H
#define IAL_H

// find in string 'string' substring 'find'
// returns position of first character of 'find' in 'string'
// if 'string' is empty returns 0, if not found returns -1
// implemented by Knuth-Morris-Pratt alhorithm
int find(char *string, char *search);

// sort characters in string 'string'
// use Quick sort algorithm
void sort(char *string);

#endif
