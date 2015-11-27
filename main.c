/*****************************************************************************************
*
*   IFJ - interpret of IFJ15 language
*
*   main module
*   open input file and call others modules to process it and interpret the code 
*
*   author:     Miroslav Karásek (xkaras31)
*   created:    2015-10-30
*   modified:   2015-11-17
*
*****************************************************************************************/

#include <stdio.h>
#include "parser.h"
#include "instlist.h"
#include "interpret.h"

int main(int argc, char const **argv)
{
    FILE *programFile = NULL;
    block_T mainBlock;

    // opening program file
    if (argc == 2)
    {
        programFile = fopen(argv[1], "r");
        if (programFile == NULL)
        {
            fprintf(stderr, "File '%s' can not be opened\n", argv[1]);
            return 99;
        }
    }
    else if (argc < 2)
    {
        fprintf(stderr, "No source file selected\n");
        return 99;
    }
    else
    {
        fprintf(stderr, "Bad count of arguments\n");
        return 99;
    }

    // will be set to nonezero value if some fails
    int errorCode = 0;

    // parse program and generate instructions
    if (errorCode == 0)
        errorCode = parse(programFile, &mainBlock);

    // interpret code
    if (errorCode == 0)
        errorCode = interpret(&mainBlock);

    // closing program file
    if (fclose(programFile) == EOF)
    {
        fprintf(stderr, "Error when closing '%s'\n", argv[1]);
        if (errorCode == 0)
            errorCode = 99;
    }

    // if some operation fails return code is not 0
    return errorCode;
}