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
static instList_T mainFunction;
static token_T token;
static int errorCode = 0;

// LL gramatic rules
static bool programLL();
static bool functionListLL();
static bool functionLL();
static bool nextFunctionListLL();
static bool parametrListLL();
static bool parametrLL();
static bool nextParameterListLL();
static bool fBlockLL();
static bool blockLL();
static bool statListLL();
static bool statLL();
static bool declarationLL();
static bool definitionLL();
static bool outListLL();
static bool nextOutListLL();
static bool inListLL();
static bool nextInListLL();
static bool rvalLL();
static bool callParameterListLL();
static bool callParameterLL();
static bool nextCallParameterLL();

// precedent analysis for expression processing
static bool expression();

// print syntax error to stderr
static void syntaxError(token_T *token)
{
    fprintf(stderr, "%u:%u: syntax error: unexpected token %d\n", token->row, token->col, token->type);
}

// parse 'programFile' an generate list of instructions 'program'
int parse(FILE *programFile, instList_T *program)
{
    initST(&functions);     // initialization of global symbol table
    initIL(&mainFunction);  // initialization of main instruction list
    initSC(programFile);    // initialiration of scanner 

    errorCode = getTokenSC(&token);
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
        //
        errorCode = getTokenSC(&token);
        if (token.type == ID_TO)
        {
            //
            errorCode = getTokenSC(&token);
            if (token.type == LBRACKET_TO)
            {
                errorCode = getTokenSC(&token);
                if (parametrListLL())
                {
                    if (token.type == RBRACKET_TO)
                    {
                        errorCode = getTokenSC(&token);
                        if (fBlockLL())
                            return true;
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

bool parametrListLL()
{
    if (token.type == TYPE_TO)
    {
        // PARAMETER_LIST -> PARAMETER NEXT_PARAMETER_LIST
        if (parametrLL())
            if (nextParameterListLL())
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

bool parametrLL()
{
    if (token.type == COMMA_TO)
    {
        // PARAMETER -> type id
        //
        errorCode = getTokenSC(&token);
        if (token.type == ID_TO)
        {
            //
            errorCode = getTokenSC(&token);
            return true;            
        }
        else
            syntaxError(&token);
    }
    else
        syntaxError(&token);
    return false;
}

bool nextParameterListLL()
{
    if (token.type == TYPE_TO)
    {
        // NEXT_PARAMETER_LIST -> , PARAMETER NEXT_PARAMETER_LIST
        errorCode = getTokenSC(&token);
        if (parametrLL())
            if (nextParameterListLL())
                return true;
    }
    else if (token.type == RBRACKET_TO)
    {
        // NEXT_PARAMETER_LIST -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool fBlockLL()
{
    if (token.type == LCBRACKET_TO)
    {
        // FBLOCK -> BLOCK
        if (blockLL())
            return true;
    }
    else if (token.type == SEMI_TO)
    {
        // FBLOCK -> ;
        errorCode = getTokenSC(&token);
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool blockLL()
{
    if (token.type == LCBRACKET_TO)
    {
        // BLOCK -> { STAT_LIST }
        errorCode = getTokenSC(&token);
        if (statListLL()) 
        {
            if (token.type == RCBRACKET_TO)
            {
                errorCode = getTokenSC(&token);
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

bool statListLL()
{
    if (token.type == LCBRACKET_TO || token.type == ID_TO || token.type == CIN_TO || token.type == COUT_TO ||  \
        token.type == RETURN_TO || token.type == IF_TO || token.type == FOR_TO || token.type == TYPE_TO)
    {
        // STAT_LIST -> STAT STAT_LIST
        if (statLL())
            if (statListLL())
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

bool statLL()
{
    if (token.type == LCBRACKET_TO)
    {
        // STAT -> BLOCK
        if (blockLL())
            return true; 
    }
    else if (token.type == ID_TO)
    {
        // STAT -> id = RVAL ;
        //
        errorCode = getTokenSC(&token);
        if (token.type == ASSIGN_TO)
        {
            errorCode = getTokenSC(&token);
            if (rvalLL())
            {
                if (token.type == SEMI_TO)
                {
                    errorCode = getTokenSC(&token);
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
        //
        errorCode = getTokenSC(&token);
        if (token.type == LBRACKET_TO)
        {
            errorCode = getTokenSC(&token);
            if (expression())
            {
                if (token.type == RBRACKET_TO)
                {
                    errorCode = getTokenSC(&token);
                    if (blockLL())
                    {
                        if (token.type == ELSE_TO)
                        {
                            errorCode = getTokenSC(&token);
                            if (blockLL())
                                return true;
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
        //
        errorCode = getTokenSC(&token);
        if (token.type == LBRACKET_TO)
        {
            errorCode = getTokenSC(&token);
            if (declarationLL())
            {
                if (token.type == SEMI_TO)
                {
                    errorCode = getTokenSC(&token);
                    if (expression())
                    {
                        if (token.type == SEMI_TO)
                        {
                            errorCode = getTokenSC(&token);
                            if (token.type == ID_TO)
                            {
                                errorCode = getTokenSC(&token);
                                if (token.type == ASSIGN_TO)
                                {
                                    errorCode = getTokenSC(&token);
                                    if (expression())
                                    {
                                        if (token.type == RBRACKET_TO)
                                        {
                                            errorCode = getTokenSC(&token);
                                            if (blockLL())
                                                return true;
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
        //
        errorCode = getTokenSC(&token);
        if (expression())
        {
            if (token.type == SEMI_TO)
            {
                errorCode = getTokenSC(&token);
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
        errorCode = getTokenSC(&token);
        if (token.type == IN_TO)
        {
            errorCode = getTokenSC(&token);
            if (inListLL())
            {
                if (token.type == SEMI_TO)
                {
                    errorCode = getTokenSC(&token);
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
        //
        errorCode = getTokenSC(&token);
        if (token.type == OUT_TO)
        {
            errorCode = getTokenSC(&token);
            if (outListLL())
            {
                if (token.type == SEMI_TO)
                {
                    errorCode = getTokenSC(&token);
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
        if (declarationLL())
        {
            if (token.type == SEMI_TO)
            {
                errorCode = getTokenSC(&token);
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

bool declarationLL()
{
    if (token.type == TYPE_TO)
    {
        // DECLARATION -> type id DEFINITION
        errorCode = getTokenSC(&token);
        if (token.type == ID_TO)
        {
            errorCode = getTokenSC(&token);
            if (definitionLL())
                return true;
        }
    }
    else
        syntaxError(&token);
    return false;
}

bool definitionLL()
{
    if (token.type == ASSIGN_TO)
    {
        // DEFINITION -> = EXPRESSION
        errorCode = getTokenSC(&token);
        if (expression())
            return true;
    }
    else if (token.type == SEMI_TO)
    {
        // DEFINITION -> ε
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool outListLL()
{
    if (token.type == ID_TO || token.type == INT_TO || token.type == DOUBLE_TO || token.type == STRING_TO)
    {
        // OUT_LIST -> CALL_PARAMETER NEXT_OUT_LIST
        if (callParameterLL())
            if (nextOutListLL())
                return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool nextOutListLL()
{
    if (token.type == OUT_TO)
    {
        // NEXT_OUT_LIST -> << CALL_PARAMETER NEXT_OUT_LIST
        errorCode = getTokenSC(&token);
        if (callParameterLL())
            if (nextOutListLL())
                return true;
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

bool inListLL()
{
    if (token.type == ID_TO)
    {
        // IN_LIST -> id NEXT_IN_LIST
        errorCode = getTokenSC(&token);
        if (nextInListLL())
            return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool nextInListLL()
{
    if (token.type == IN_TO)
    {
        // NEXT_IN_LIST -> >> id NEXT_IN_LIST
        errorCode = getTokenSC(&token);
        if (token.type == ID_TO)
        {
            errorCode = getTokenSC(&token);
            if (nextInListLL())
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

bool rvalLL()
{
    if (token.type == ID_TO)
    {
        if (searchST(&functions, token.stringValue))
            token.type = ID_TO - 1;
    }
    if (token.type == ID_TO - 1) // == FID
    {
        // RVAL -> fid ( CALL_PARAMETER_LIST )
        errorCode = getTokenSC(&token);
        if (token.type == LBRACKET_TO)
        {
            errorCode = getTokenSC(&token);
            if (callParameterListLL())
            {
                if (token.type == RBRACKET_TO)
                {
                    errorCode = getTokenSC(&token);
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
        if (expression())
            return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool callParameterListLL()
{
    if (token.type == ID_TO || token.type == INT_TO || token.type == DOUBLE_TO || token.type == STRING_TO)
    {
        // CALL_PARAMETER_LIST -> CALL_PARAMETER NEXT_CALL_PARAMETER_LIST
        if (callParameterLL())
            if (nextCallParameterLL())
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

bool nextCallParameterLL()
{
    if (token.type == COMMA_TO)
    {
        // NEXT_CALL_PARAMETER_LIST -> , CALL_PARAMETER NEXT_CALL_PARAMETER_LIST
        errorCode = getTokenSC(&token);
        if (callParameterLL())
            if (nextCallParameterLL())
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

bool callParameterLL()
{
    if (token.type == ID_TO)
    {
        // CALL_PARAMETER -> id
        errorCode = getTokenSC(&token);
        return true;
    }
    else if (token.type == INT_TO)
    {
        // CALL_PARAMETER -> int
        errorCode = getTokenSC(&token);
        return true;
    }
    else if (token.type == DOUBLE_TO)
    {
        // CALL_PARAMETER -> double
        errorCode = getTokenSC(&token);
        return true;
    }
    else if (token.type == STRING_TO)
    {
        // CALL_PARAMETER -> string
        errorCode = getTokenSC(&token);
        return true;
    }
    else
        syntaxError(&token);
    return false;
}

bool expression()
{
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

    bool stop = false;
    bool bracketSucess = false;
    while (!stop)
    {
        //printf("%d %d\n", (int) topPS(&stack), (int) token.type);
        if (token.type > EOF_TO)
        {
            syntaxError(&token);
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
                if (token.type >= ID_TO && token.type <= STRING_TO)
                {
                    pushPS(&operandStack, (void *) token.type);
                }
                else if (token.type >= PLUS_TO && token.type <= RBRACKET_TO)
                {
                    pushPS(&stack, (void *) LEFT_PR);
                    pushPS(&stack, (void *) token.type);
                }
                bracketSucess = true;
                errorCode = getTokenSC(&token);
                break;
            }
            case RIGHT_PR:  // >
            {
                int top;
                while ((top = (int) topPS(&stack)) != LEFT_PR && top != EOF_TO)
                {
                    if (top >= PLUS_TO && top <= NE_TO)
                    {
                        if ((int)topPS(&operandStack) != EOF_TO)
                            popPS(&operandStack);
                        else
                            syntaxError(&token);
                        if ((int)topPS(&operandStack) != EOF_TO)
                            popPS(&operandStack);
                        else
                            syntaxError(&token);
                        pushPS(&operandStack, (void *) ID_TO);
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
                errorCode = getTokenSC(&token);
                break;
            }
            case SUCESS_PR: // S
            {
                popPS(&operandStack);
                if ((int) topPS(&stack) == EOF_TO && (int) topPS(&operandStack) == EOF_TO)
                {
                    stop = true;
                }
                else
                {
                    syntaxError(&token);
                    stop = true;
                }
                break;
            }
            case ERROR_PR:  // E
            {
                stop = true;
                syntaxError(&token);
                break;
            }
        }
    }

    destroyPS(&stack);
    destroyPS(&operandStack);
    return true;
}