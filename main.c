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
    instList_T instructionList;

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
    int returnCode = 0;

    // initialization of instruction list
    initIL(&instructionList);

    // parse program and generate instructions
    if (returnCode == 0)
        returnCode = parse(programFile, &instructionList);

    // interpret code
    if (returnCode == 0)
        returnCode = interpret(&instructionList);

    // destroy instruction list
    destroyIL(&instructionList);

    // closing program file
    if (fclose(programFile) == EOF)
    {
        fprintf(stderr, "Error when closing '%s'\n", argv[1]);
        if (returnCode == 0)
            returnCode = 99;
    }

    // if some operation fails return code is not 0
    return returnCode;
}