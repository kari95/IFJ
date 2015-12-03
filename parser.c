/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   parser module
*   do a syntax analysis
*
*   author:     Miroslav Karásek (xkaras31)
*   created:    2015-11-17
*   modified:   2015-11-22
*
*****************************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "pointerstack.h"
#include "instlist.h"
#include "pointerstack.h"
#include "scanner.h"
#include "parser.h"
#include "ial.h"

typedef enum {
    LEFT_PR,        // < 
    RIGHT_PR,       // > 
    EQUAL_PR,       // = 
    SUCESS_PR,      // S
    ERROR_PR,       // E
} precedent_T;

// table with functions
static symbolTable_T functions;
static token_T token;
static int errorCode = 0;


block_T *allocBlock(block_T *block)
{
    if (!block)
    {
        block = malloc(sizeof(block_T));
    }
    if (block)
    {
        block->parrent = NULL;
        initST(&block->symbols);
        initIL(&block->program);
    }
    return block;
}

void getToken(token_T *token)
{
    errorCode = getTokenSC(token);
    if (errorCode)
    {
        fprintf(stderr, "error with token %d\n", token->type);
        token->type = EOF_TO;
    }
}

symbol_T *allocSymbol()
{
    return malloc(sizeof(symbol_T));
}
// stack for constants
pointerStack_T constants; 

// LL gramatic rules
static bool programLL();
static bool functionListLL();
static bool functionLL();
static bool nextFunctionListLL();
static bool parametrListLL(symbol_T *function);
static bool parametrLL(symbol_T *function);
static bool nextParameterListLL(symbol_T *function);
static bool fBlockLL(symbol_T *function);
static bool blockLL(block_T *block);
static bool statListLL(block_T *block);
static bool statLL(block_T *block);
static bool declarationLL(block_T *block);
static bool definitionLL(block_T *block, void **value);
static bool outListLL(block_T *block);
static bool nextOutListLL(block_T *block);
static bool inListLL(block_T *block);
static bool nextInListLL(block_T *block);
static bool rvalLL(block_T *block, void **value);
static bool callParameterListLL(block_T *block);
static bool callParameterLL(block_T *block, void **value);
static bool nextCallParameterLL(block_T *block);

// precedent analysis for expression processing
static bool expression(block_T *block, void **value);

// print syntax error to stderr
static void syntaxError(token_T *token)
{
    fprintf(stderr, "%u:%u: syntax error: unexpected token %d\n", token->row, token->col, token->type);
    errorCode = 2;
}

// parse 'programFile' an generate list of instructions 'program'
int parse(FILE *programFile, block_T *block)
{
    initST(&functions);     // initialization of global symbol table
    initSC(programFile);    // initialiration of scanner 

    allocBlock(block);

    // insert main function to table of functions
    symbol_T *function =  insertST(&functions, "main", (symbol_T) {FUNCTION_ST, INT_TY, false, {.data = block}});
    insertST(&functions, "sort", (symbol_T) {FUNCTION_ST + 1, STRING_TY, true, {.data = block}});   // string sort(string s)
    insertST(&functions, "length", (symbol_T) {FUNCTION_ST + 2, INT_TY, true, {.data = block}});    // int length(string s)
    insertST(&functions, "concat", (symbol_T) {FUNCTION_ST + 3, STRING_TY, true, {.data = block}}); // string concat(string s1, string s2)
    insertST(&functions, "substr", (symbol_T) {FUNCTION_ST + 4, STRING_TY, true, {.data = block}}); // string substr(string s, int i, int n)
    insertST(&functions, "find", (symbol_T) {FUNCTION_ST + 5, INT_TY, true, {.data = block}});      // int find(string s, string search)

    // for parameters for build-in functions
    insertST(&block->symbols, "$$BI1", (symbol_T){VARIABLE_ST, STRING_TY, false, {.intValue = 0}});
    insertST(&block->symbols, "$$BI2", (symbol_T){VARIABLE_ST, STRING_TY, false, {.intValue = 1}});
    insertST(&block->symbols, "$$BI3", (symbol_T){VARIABLE_ST, STRING_TY, false, {.intValue = 2}});

    initPS(&constants);

    getToken(&token);
    programLL(); // starting nonterminal
        
    // !destruction
    return errorCode;
}

// implementation of rules
bool programLL()
{
    if (token.type == TYPE_TO)
    {
        // PROGRAM -> FUNCTION_LIST 
        if (functionListLL())
            return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool functionListLL()
{
    if (token.type == TYPE_TO)
    {
        // FUNCTION_LIST -> FUNCTION NEXT_FUNCTION_LIST
        if (functionLL())
            if (nextFunctionListLL())
                return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool functionLL()
{
    if (token.type == TYPE_TO)
    {
        // FUNCTION -> type fid ( PARAMETER_LIST ) FBLOCK
        dataType_T dataType = token.dataType;
        symbol_T *function;
        getToken(&token);
        if (token.type == ID_TO)
        {
            function = searchST(&functions, token.stringValue);
            if (function == NULL)
            {
                function = insertST(&functions, token.stringValue, (symbol_T){FUNCTION_ST, dataType, false, NULL});
                function->data = allocBlock(NULL);
            }
            //printf("data type: %d\n", function->dataType);
            getToken(&token);
            if (token.type == LBRACKET_TO)
            {
                getToken(&token);
                if (parametrListLL(function))
                {
                    if (token.type == RBRACKET_TO)
                    {
                        getToken(&token);
                        if (fBlockLL(function))
                        {
                            /*startIL(&((block_T *) function->data)->program);
                            instruction_T *instr;
                            do
                            {
                                instr = getActiveIL(&((block_T *) function->data)->program);
                                if (instr)
                                {
                                    int off1 = -1;
                                    int off2 = -1;
                                    int off3 = -1;
                                    if (instr->destination)
                                        off1 = (int) ((symbol_T *)instr->destination)->data;
                                    if (instr->source1)
                                        off2 = (int) ((symbol_T *)instr->source1)->data;
                                    if (instr->source2)
                                        off3 = (int) ((symbol_T *)instr->source2)->data;
                                    printf("%d %d %d %d\n", instr->type, off1, off2, off3);
                                }
                                nextIL(&((block_T *) function->data)->program);
                            }
                            while (instr);*/
                            return true;
                        }
                    }
                    else
                        syntaxError(&token);
                }
            }
            else
                syntaxError(&token);
            
        }
        else
            syntaxError(&token);
    }
    else
        syntaxError(&token);
    return false;
}

bool nextFunctionListLL()
{
    if (token.type == TYPE_TO)
    {
        // NEXT_FUNCTION_LIST -> FUNCTION NEXT_FUNCTION_LIST
        if (functionLL())
            if (nextFunctionListLL())
                return true;
    }
    else if (token.type == EOF_TO)
    {
        // NEXT_FUNCTION_LIST -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool parametrListLL(symbol_T *function)
{
    if (token.type == TYPE_TO)
    {
        // PARAMETER_LIST -> PARAMETER NEXT_PARAMETER_LIST
        if (parametrLL(function))
            if (nextParameterListLL(function))
                return true;
    }
    else if (token.type == RBRACKET_TO)
    {
        // PARAMETER_LIST -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool parametrLL(symbol_T *function)
{
    if (token.type == TYPE_TO)
    {
        // PARAMETER -> type id
        dataType_T dataType = token.dataType;
        getToken(&token);
        if (token.type == ID_TO)
        {
            int offset = ((block_T *) function->data)->symbols.count;
            insertST(&((block_T *) function->data)->symbols, token.stringValue, (symbol_T){VARIABLE_ST, dataType, false, {.intValue = offset}});
            getToken(&token);
            return true;            
        }
        else
            syntaxError(&token);
    }
    else
        syntaxError(&token);
    return false;
}

bool nextParameterListLL(symbol_T *function)
{
    if (token.type == COMMA_TO)
    {
        getToken(&token);
        if (token.type == TYPE_TO)
        {
            // NEXT_PARAMETER_LIST -> , PARAMETER NEXT_PARAMETER_LIST
            if (parametrLL(function))
                if (nextParameterListLL(function))
                    return true;
        }
    }
    else if (token.type == RBRACKET_TO)
    {
        // NEXT_PARAMETER_LIST -> ε
        return true;;
    }
    else
        syntaxError(&token);
    return false;
}

bool fBlockLL(symbol_T *function)
{
    if (token.type == LCBRACKET_TO)
    {
        // FBLOCK -> BLOCK
        function->defined = true;
        block_T *block = function->data;
        if (blockLL(block))
        {
            insertLastIL(&block->program, (instruction_T){RETURN_I, NULL, NULL, NULL});
            return true;
        }
    }
    else if (token.type == SEMI_TO)
    {
        // FBLOCK -> ;
        getToken(&token);
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool blockLL(block_T *block)
{
    if (token.type == LCBRACKET_TO)
    {
        // BLOCK -> { STAT_LIST }
        getToken(&token);
        if (statListLL(block)) 
        {
            if (token.type == RCBRACKET_TO)
            {
                getToken(&token);
                return true;
            }
            else
                syntaxError(&token);
        }
    }
    else
        syntaxError(&token);
    return true;
}

bool statListLL(block_T *block)
{
    if (token.type == LCBRACKET_TO || token.type == ID_TO || token.type == CIN_TO || token.type == COUT_TO ||  \
        token.type == RETURN_TO || token.type == IF_TO || token.type == FOR_TO || token.type == TYPE_TO)
    {
        // STAT_LIST -> STAT STAT_LIST
        if (statLL(block))
            if (statListLL(block))
                return true; 
    }
    else if (token.type == RCBRACKET_TO)
    {
        // STAT_LIST -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool statLL(block_T *block)
{
    if (token.type == LCBRACKET_TO)
    {
        // STAT -> BLOCK
        block_T *newBlock = allocBlock(NULL);
        instruction_T newInstruction;
        newInstruction.type = BLOCK_I;
        newInstruction.destination = newBlock;
        newInstruction.source1 = NULL;
        newInstruction.source2 = NULL;
        insertLastIL(&block->program, newInstruction);
        newBlock->symbols.parrent = &block->symbols;
        newBlock->symbols.count = block->symbols.count;
        newBlock->parrent = block;
        if (blockLL(newBlock))
        {
            block->symbols.count = newBlock->symbols.count;
            return true; 
        }
    }
    else if (token.type == ID_TO)
    {
        // STAT -> id = RVAL ;
        instruction_T newInstruction;
        newInstruction.type = ASSIGN_I;
        symbol_T *symbol = searchST(&block->symbols, token.stringValue);
        if (symbol == NULL)
        {
            fprintf(stderr, "udefined symbol %s\n", token.stringValue);
            return false;
        }
        newInstruction.destination = symbol;
        getToken(&token);
        if (token.type == ASSIGN_TO)
        {
            void *expressionOutput = symbol;
            getToken(&token);
            if (rvalLL(block, &expressionOutput))
            {
                newInstruction.source1 = expressionOutput;
                newInstruction.source2 = NULL;
                if (((symbol_T *) newInstruction.destination)->dataType == AUTO_TY)
                    ((symbol_T *) newInstruction.destination)->dataType = ((symbol_T *) newInstruction.source1)->dataType;
                if (((symbol_T *) newInstruction.destination)->dataType != ((symbol_T *) newInstruction.source1)->dataType)
                    if (((symbol_T *) newInstruction.destination)->dataType == STRING_TY || ((symbol_T *) newInstruction.source1)->dataType == STRING_TY)
                        fprintf(stderr, "bad conversion\n");
                if (token.type == SEMI_TO)
                {
                    if (newInstruction.source1 != newInstruction.destination)
                        insertLastIL(&block->program, newInstruction);
                    getToken(&token);
                    return true;
                }
                else
                    syntaxError(&token);
            }
        }
        else
            syntaxError(&token);
    }
    else if (token.type == IF_TO)
    {
        // STAT -> if ( EXPRESSION ) BLOCK else BLOCK
        instruction_T newInstruction;
        newInstruction.type = IF_I;
        getToken(&token);
        if (token.type == LBRACKET_TO)
        {
            getToken(&token);
            void *expressionOutput;
            if (expression(block, &expressionOutput))
            {
                newInstruction.source1 = expressionOutput;
                if (token.type == RBRACKET_TO)
                {
                    getToken(&token);
                    block_T *newBlock = allocBlock(NULL);
                    newBlock->parrent = block;
                    newBlock->symbols.parrent = &block->symbols;
                    newBlock->symbols.count = block->symbols.count;
                    newInstruction.destination = newBlock;
                    if (blockLL(newInstruction.destination))
                    {
                        block->symbols.count = newBlock->symbols.count;
                        if (token.type == ELSE_TO)
                        {
                            getToken(&token);
                            newBlock = allocBlock(NULL);
                            newBlock->parrent = block;
                            newBlock->symbols.parrent = &block->symbols;
                            newBlock->symbols.count = block->symbols.count;
                            newInstruction.source2 = newBlock;
                            if (blockLL(newInstruction.source2))
                            {
                                block->symbols.count = newBlock->symbols.count;
                                insertLastIL(&block->program, newInstruction);
                                return true;
                            }
                        }
                        else
                            syntaxError(&token);
                    }
                }
                else
                    syntaxError(&token);
            }
        }
        else
            syntaxError(&token);
    }
    else if (token.type == FOR_TO)
    {
        // STAT -> for ( DECLARATION ; EXPRESSION ; id = EXPRESSION ) BLOCK
        instruction_T newInstruction;
        newInstruction.type = BLOCK_I;
        block_T *newBlock = allocBlock(NULL);
        block_T *tempBlock = allocBlock(NULL);
        newInstruction.destination = newBlock;
        newInstruction.source1 = NULL;
        newInstruction.source2 = NULL;
        newBlock->symbols.parrent = &block->symbols;
        newBlock->parrent = block;
        newBlock->symbols.parrent = &block->symbols;
        //printf("CO %d\n", block->symbols.count);
        newBlock->symbols.count = block->symbols.count;
        tempBlock->symbols.parrent = &newBlock->symbols;
        getToken(&token);
        if (token.type == LBRACKET_TO)
        {
            getToken(&token);
            if (declarationLL(newBlock))
            {
                unsigned int declarationLength = newBlock->program.count;
                if (token.type == SEMI_TO)
                {
                    getToken(&token);
                    void *expressionOutput;
                    if (expression(newBlock, &expressionOutput))
                    {
                        insertLastIL(&newBlock->program, (instruction_T) {IF_NOT_BREAK_I, NULL, expressionOutput, NULL});
                        if (token.type == SEMI_TO)
                        {
                            getToken(&token);
                            if (token.type == ID_TO)
                            {
                                symbol_T *symbol = searchST(&newBlock->symbols, token.stringValue);
                                getToken(&token);
                                if (token.type == ASSIGN_TO)
                                {
                                    getToken(&token);
                                    tempBlock->symbols.count = newBlock->symbols.count;
                                    if (expression(tempBlock, &expressionOutput))
                                    {
                                        newBlock->symbols.count = tempBlock->symbols.count;
                                        if (symbol->dataType == AUTO_TY)
                                            symbol->dataType = ((symbol_T *) expressionOutput)->dataType;
                                        insertLastIL(&tempBlock->program, (instruction_T) {ASSIGN_I, symbol, expressionOutput, NULL});
                                        if (token.type == RBRACKET_TO)
                                        {
                                            getToken(&token);
                                            if (blockLL(newBlock))
                                            {
                                                block->symbols.count = newBlock->symbols.count;
                                                startIL(&tempBlock->program);
                                                instruction_T *instruction;
                                                //printf("%d\n", tempBlock->program.count);
                                                while (instruction = getActiveIL(&tempBlock->program))
                                                {
                                                    insertLastIL(&newBlock->program, *instruction);
                                                    nextIL(&tempBlock->program);
                                                }
                                                insertLastIL(&newBlock->program, (instruction_T) {JUMP_START_I, (void *) declarationLength, NULL, NULL});
                                                insertLastIL(&block->program, newInstruction);
                                                return true;
                                            }
                                        }
                                        else
                                            syntaxError(&token);
                                    }
                                }
                                else
                                    syntaxError(&token);
                            }
                            else
                                syntaxError(&token);
                        }
                        else
                            syntaxError(&token);
                    }
                }
                else
                    syntaxError(&token);
            }
        }
        else
            syntaxError(&token);
    }
    else if (token.type == RETURN_TO)
    {
        // STAT -> return EXPRESSION ;
        getToken(&token);
        void *expressionOutput;
        if (expression(block, &expressionOutput))
        {
            if (token.type == SEMI_TO)
            {
                insertLastIL(&block->program, (instruction_T) {RETURN_I, NULL, expressionOutput, NULL});
                getToken(&token);
                return true;
            }
            else
                syntaxError(&token);
        }
    }
    else if (token.type == CIN_TO)
    {
        // STAT -> cin >> IN_LIST ;
        //
        getToken(&token);
        if (token.type == IN_TO)
        {
            getToken(&token);
            if (inListLL(block))
            {
                if (token.type == SEMI_TO)
                {
                    getToken(&token);
                    return true;
                }
                else
                    syntaxError(&token);
            }
        }
        else
            syntaxError(&token);
    }
    else if (token.type == COUT_TO)
    {
        // STAT -> cout << OUT_LIST ;
        getToken(&token);
        if (token.type == OUT_TO)
        {
            getToken(&token);
            if (outListLL(block))
            {
                if (token.type == SEMI_TO)
                {
                    getToken(&token);
                    return true;
                }
                else
                    syntaxError(&token);
            }
        }
        else
            syntaxError(&token);
    }
    else if (token.type == TYPE_TO)
    {
        // STAT -> DECLARATION ;
        //
        if (declarationLL(block))
        {
            if (token.type == SEMI_TO)
            {
                getToken(&token);
                return true;
            }
            else
                syntaxError(&token);
        }
    }
    else
        syntaxError(&token);
    return false;
}

bool declarationLL(block_T *block)
{
    if (token.type == TYPE_TO)
    {
        // DECLARATION -> type id DEFINITION
        symbol_T *symbol;
        dataType_T dataType = token.dataType;
        getToken(&token);
        if (token.type == ID_TO)
        {
            //printf("CO %d\n", block->symbols.count);
            int offset = block->symbols.count;
            symbol = insertST(&block->symbols, token.stringValue, (symbol_T){VARIABLE_ST, dataType, false, {.intValue = offset}});
            //printf("%s %p offset %d\n",token.stringValue, symbol, offset);
            getToken(&token);
            void *value;
            if (definitionLL(block, &value))
            {
                if (value)
                {
                    instruction_T newInstruction;
                    newInstruction.type = ASSIGN_I; 
                    newInstruction.destination = symbol;
                    if (((symbol_T *) newInstruction.destination)->dataType == AUTO_TY)
                        ((symbol_T *) newInstruction.destination)->dataType = ((symbol_T *) value)->dataType;
                    newInstruction.source1 = value;
                    newInstruction.source2 = NULL;
                    insertLastIL(&block->program, newInstruction);
                }
                return true;
            }
        }
    }
    else
        syntaxError(&token);
    return false;
}

bool definitionLL(block_T *block, void **value)
{
    if (token.type == ASSIGN_TO)
    {
        // DEFINITION -> = EXPRESSION
        getToken(&token);
        if (expression(block, value))
            return true;
    }
    else if (token.type == SEMI_TO)
    {
        // DEFINITION -> ε
        *value = NULL;
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool outListLL(block_T *block)
{
    if (token.type == ID_TO || token.type == INT_TO || token.type == DOUBLE_TO || token.type == STRING_TO)
    {
        // OUT_LIST -> CALL_PARAMETER NEXT_OUT_LIST
        void *outputOperand;
        instruction_T newInstruction;
        newInstruction.type = PRINT_I;
        if (callParameterLL(block, &outputOperand))
        {
            newInstruction.destination = NULL;
            newInstruction.source1 = outputOperand;
            newInstruction.source2 = NULL;
            insertLastIL(&block->program, newInstruction);
            if (nextOutListLL(block))
                return true;
        }
    }
    else
        syntaxError(&token);
    return false;
}

bool nextOutListLL(block_T *block)
{
    if (token.type == OUT_TO)
    {
        // NEXT_OUT_LIST -> << CALL_PARAMETER NEXT_OUT_LIST
        void *outputOperand;
        instruction_T newInstruction;
        newInstruction.type = PRINT_I;
        getToken(&token);
        if (callParameterLL(block, &outputOperand))
        {
            newInstruction.destination = NULL;
            newInstruction.source1 = outputOperand;
            newInstruction.source2 = NULL;
            insertLastIL(&block->program, newInstruction);
            if (nextOutListLL(block))
                return true;
        }
    }
    else if (token.type == SEMI_TO)
    {
        // NEXT_OUT_LIST -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool inListLL(block_T *block)
{
    if (token.type == ID_TO)
    {
        // IN_LIST -> id NEXT_IN_LIST
        instruction_T newInstruction;
        newInstruction.type = SCAN_I;
        symbol_T *symbol = searchST(&block->symbols, token.stringValue);
        if (symbol == NULL)
        {
            fprintf(stderr, "udefined symbol %s\n", token.stringValue);
            return false;
        }
        newInstruction.destination = symbol;
        newInstruction.source1 = NULL;
        newInstruction.source2 = NULL;
        insertLastIL(&block->program, newInstruction);
        getToken(&token);
        if (nextInListLL(block))
            return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool nextInListLL(block_T *block)
{
    if (token.type == IN_TO)
    {
        // NEXT_IN_LIST -> >> id NEXT_IN_LIST
        getToken(&token);
        if (token.type == ID_TO)
        {
            instruction_T newInstruction;
            newInstruction.type = SCAN_I;
            symbol_T *symbol = searchST(&block->symbols, token.stringValue);
            if (symbol == NULL)
            {
                fprintf(stderr, "udefined symbol %s\n", token.stringValue);
                return false;
            }
            newInstruction.destination = symbol;
            newInstruction.source1 = NULL;
            newInstruction.source2 = NULL;
            insertLastIL(&block->program, newInstruction);
            getToken(&token);
            if (nextInListLL(block))
                return true;
        }
        else
            syntaxError(&token);
    }
    else if (token.type == SEMI_TO)
    {
        // NEXT_IN_LIST -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool rvalLL(block_T *block, void **value)
{
    symbol_T *function;
    //printf("%d\n", token.type);
    if (token.type == ID_TO)
    {
        if (function = searchST(&functions, token.stringValue))
            token.type = ID_TO - 1;
        //printf("%p\n", function);
    }
    if (token.type == ID_TO - 1) // == FID
    {
        // RVAL -> fid ( CALL_PARAMETER_LIST )
        getToken(&token);
        if (token.type == LBRACKET_TO)
        {
            insertLastIL(&block->program, (instruction_T) {NEW_FRAME_I, function->data, NULL, NULL});
            getToken(&token);
            if (callParameterListLL(block))
            {
                if (token.type == RBRACKET_TO)
                {
                    instType_T istrType = CALL_I;
                    if (function->type == FUNCTION_ST + 1)
                        istrType = SORT_I;
                    if (function->type == FUNCTION_ST + 2)
                        istrType = LENGTH_I;
                    if (function->type == FUNCTION_ST + 3)
                        istrType = CONCAT_I;
                    if (function->type == FUNCTION_ST + 4)
                        istrType = SUBSTRING_I;
                    if (function->type == FUNCTION_ST + 5)
                        istrType = FIND_I;
                    insertLastIL(&block->program, (instruction_T) {istrType, function->data, *value, NULL});
                    getToken(&token);
                    return true;
                }
                else
                    syntaxError(&token);
            }
        }
        else
            syntaxError(&token);
    }
    else if (token.type >= ID_TO && token.type <= RBRACKET_TO)
    {
        // RVAL -> EXPRESSION
        if (expression(block, value))
            return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool callParameterListLL(block_T *block)
{
    if (token.type == ID_TO || token.type == INT_TO || token.type == DOUBLE_TO || token.type == STRING_TO)
    {
        // CALL_PARAMETER_LIST -> CALL_PARAMETER NEXT_CALL_PARAMETER_LIST
        if (callParameterLL(block, NULL))
            if (nextCallParameterLL(block))
                return true;
    }
    else if (token.type == RBRACKET_TO)
    {
        // NEXT_IN_LIST -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool nextCallParameterLL(block_T *block)
{
    if (token.type == COMMA_TO)
    {
        // NEXT_CALL_PARAMETER_LIST -> , CALL_PARAMETER NEXT_CALL_PARAMETER_LIST
        getToken(&token);
        if (callParameterLL(block, NULL))
            if (nextCallParameterLL(block))
                return true;
    }
    else if (token.type == RBRACKET_TO)
    {
        // NEXT_CALL_PARAMETER_LIST -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool callParameterLL(block_T *block, void **value)
{
    if (token.type == ID_TO)
    {
        // CALL_PARAMETER -> id
        symbol_T *symbol = searchST(&block->symbols, token.stringValue);
        if (symbol == NULL)
        {
            fprintf(stderr, "udefined symbol %s\n", token.stringValue);
            return false;
        }
        if (value)
            *value = symbol;
        else
            insertLastIL(&block->program, (instruction_T) {SET_PARAMETR_I, NULL, symbol, NULL});
        getToken(&token);
        return true;
    }
    else if (token.type >= INT_TO && token.type <= STRING_TO)
    {
        // CALL_PARAMETER -> int
        symbol_T *newConstant = allocSymbol();
        newConstant->type = CONST_ST;
        newConstant->dataType = token.type;
        newConstant->defined = true;
        switch (token.type)
        {
            case INT_TO:
                newConstant->intValue = token.intValue;
                break;
            case DOUBLE_TO:
                newConstant->doubleValue = token.doubleValue;
                break;
            case STRING_TO:
                newConstant->stringValue = allocString(token.stringValue, 0);

                break;
        }
        pushPS(&constants, newConstant);
        getToken(&token);
        if (value)
            *value = newConstant;
        else
            insertLastIL(&block->program, (instruction_T) {SET_PARAMETR_I, NULL, newConstant, NULL});
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool expression(block_T *block, void **value)
{
    // precedent table
    static precedent_T table[][17] = {
        {ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, ERROR_PR, RIGHT_PR, RIGHT_PR}, // id
        {ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, ERROR_PR, RIGHT_PR, RIGHT_PR}, // int
        {ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, ERROR_PR, RIGHT_PR, RIGHT_PR}, // double
        {ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, ERROR_PR, RIGHT_PR, RIGHT_PR}, // string
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},        // +
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},        // -
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},      // *
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},      // /
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},          // >
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},          // <
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},          // >=
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},          // <=
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},          // ==
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, LEFT_PR, RIGHT_PR, RIGHT_PR},          // !=
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, EQUAL_PR, ERROR_PR},                // (
        {ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, RIGHT_PR, ERROR_PR, RIGHT_PR, RIGHT_PR}, // )
        {ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR, ERROR_PR}, // ; ERROR
        {LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, LEFT_PR, SUCESS_PR, SUCESS_PR}               // EOF
    };
    pointerStack_T stack;
    pointerStack_T operandStack;
    initPS(&stack);
    initPS(&operandStack);

    pushPS(&stack, (void *) EOF_TO);
    pushPS(&operandStack, (void *) EOF_TO);

    static unsigned int tempCounter = 0;

    bool stop = false;
    bool bracketSucess = false;
    while (!stop)
    {
        //printf("%d %d\n", (int) topPS(&stack), (int) token.type);
        if (token.type > EOF_TO)
        {
            syntaxError(&token);
            return false;
            break;
        }
        precedent_T action = table[(int) topPS(&stack)][(int) token.type];
        if ((int) topPS(&stack) == EOF_TO && token.type == RBRACKET_TO)
        {
            if (bracketSucess) 
                action = SUCESS_PR;
            else 
                action = ERROR_PR;
        }
        switch (action)
        {
            case LEFT_PR:   // <
            {
                if (token.type == ID_TO)
                {
                    symbol_T *symbol = searchST(&block->symbols, token.stringValue);
                    if (symbol == NULL)
                    {
                        fprintf(stderr, "udefined symbol %s\n", token.stringValue);
                        return false;
                    }
                    pushPS(&operandStack, (void *) symbol);
                }
                else if (token.type >= INT_TO && token.type <= STRING_TO)
                {
                    symbol_T *newConstant = allocSymbol();
                    newConstant->type = CONST_ST;
                    newConstant->dataType = token.type;
                    newConstant->defined = true;
                    switch (token.type)
                    {
                        case INT_TO:
                            newConstant->intValue = token.intValue;
                            break;
                        case DOUBLE_TO:
                            newConstant->doubleValue = token.doubleValue;
                            break;
                        case STRING_TO:
                            newConstant->stringValue = allocString(token.stringValue, 0);
                            break;
                    }
                    pushPS(&constants, newConstant);
                    pushPS(&operandStack, (void *) newConstant);
                }
                else if (token.type >= PLUS_TO && token.type <= RBRACKET_TO)
                {
                    pushPS(&stack, (void *) LEFT_PR);
                    pushPS(&stack, (void *) token.type);
                }
                bracketSucess = true;
                getToken(&token);
                break;
            }
            case RIGHT_PR:  // >
            {
                int top;
                while ((top = (int) topPS(&stack)) != LEFT_PR && top != EOF_TO)
                {
                    if (top >= PLUS_TO && top <= NE_TO)
                    {
                        instruction_T newInstruction;
                        newInstruction.type = top;
                        symbol_T *source1;
                        symbol_T *source2;
                        if ((source2 = topPS(&operandStack)) != EOF_TO)
                            popPS(&operandStack);
                        else
                        {
                            syntaxError(&token);
                            return false;
                        }
                        if ((source1 = topPS(&operandStack)) != EOF_TO)
                            popPS(&operandStack);
                        else
                        {
                            syntaxError(&token);
                            return false;
                        }
                        char tempName[10];
                        sprintf(tempName, "$%u", tempCounter);
                        tempCounter++;
                        int offset = block->symbols.count;
                        dataType_T type = INT_TY;
                        if (top >= PLUS_TO && top <= DIV_TO)
                        {
                            if (source1->dataType == DOUBLE_TY || source2->dataType == DOUBLE_TY)
                                type = DOUBLE_TY;
                            else if (source1->dataType == STRING_TY || source2->dataType == STRING_TY)
                            {    
                                fprintf(stderr, "bad operrand type\n");
                                return false;
                            }
                        }
                        else
                        {
                            if (source1->dataType != source2->dataType && (source1->dataType == STRING_TY || source2->dataType == STRING_TY))
                            {
                                fprintf(stderr, "bad operrand type\n");
                                return false;
                            }   
                        }
                        symbol_T *tempSymbol = insertST(&block->symbols, tempName, (symbol_T){VARIABLE_ST, type, true, {.intValue = offset}});
                        //printf("TMP %d\n", offset);
                        //printf("%s %d\n", tempName, type);
                        newInstruction.source1 = source1;
                        newInstruction.source2 = source2;
                        newInstruction.destination = tempSymbol;
                        pushPS(&operandStack, (void *) newInstruction.destination);
                        //if (newInstruction.type == EQ_I)
                        //    printf("E %p %p\n", newInstruction.source1, newInstruction.source2);
                        insertLastIL(&block->program, newInstruction);
                    }
                    popPS(&stack);
                }
                if (top == LEFT_PR)
                    popPS(&stack);
                break;
            }
            case EQUAL_PR:  // =
            {
                pushPS(&stack, (void *) token.type);
                getToken(&token);
                break;
            }
            case SUCESS_PR: // S
            {
                *value = topPS(&operandStack);
                popPS(&operandStack);
                if ((int) topPS(&stack) == EOF_TO && (int) topPS(&operandStack) == EOF_TO)
                {
                    stop = true;
                }
                else
                {
                    syntaxError(&token);
                    stop = true;
                    return false;
                }
                break;
            }
            case ERROR_PR:  // E
            {
                stop = true;
                syntaxError(&token);
                return false;
            }
        }
    }
    // memory leak
    destroyPS(&stack);
    destroyPS(&operandStack);
    return true;
}