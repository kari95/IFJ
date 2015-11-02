/*****************************************************************************************
*
*   IFJ - interpret of IFJ15 language
*
*   main module
*   open input file and call others modules to process it and interpret the code 
*
*   author:     Miroslav Karásek (xkaras31)
*   created:    2015-10-30
*   modified:   2015-10-30
*
*****************************************************************************************/

#include <stdio.h>
#include "parser.h"
#include "generator.h"
#include "interpret.h"

int main(int argc, char const **argv)
{
    FILE *programFile = NULL;

    // opening program file
    if (argc == 2)
    {
        programFile = fopen(argv[1], "r");
        if (!programFile)
        {
            fprintf(stderr, "File '%s' can not be opened\n", argv[1]);
            return 1; //?
        }
    }
    else
    {
        fprintf(stderr, "Bad count of arguments\n");
        return 1; //?
    }

    // parse program
    //parse(programFile, someStruct);

    // generate internal code
    //generate(someStruct, instructionList);

    // interpret code
    //interpret(instructionList);

    fclose(programFile);
    return 0;
}