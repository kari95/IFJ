/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   interpret module header
*   interpret internal code
*
*   author:     Libor Janíček (xjanic21)
*   created:    2015-11-16
*   modified:   2015-11-16
*
*****************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "instlist.h"
#include "pointerstack.h"
#include "ial.h"
#include "interpret.h"

typedef union {
    int intValue;
    double doubleValue;
    char *stringValue;
} frame_T;

frame_T *allocFrame(block_T *block)
{
    unsigned int size = block->symbols.count;
    //printf("size %d\n",size );
    frame_T *frame = malloc(sizeof(frame_T) * size);
    for (int i = 0; i < size; ++i)
    {
        frame[i].intValue = 0;
    }
    return frame;
}

int interpret(block_T *b)
{
    pointerStack_T frameStack;
    pointerStack_T blockStack;
    pointerStack_T callBlockStack;
    pointerStack_T instructionStack;
    pointerStack_T returnAddressStack;
    initPS(&frameStack);
    initPS(&blockStack);
    initPS(&callBlockStack);
    initPS(&instructionStack);
    initPS(&returnAddressStack);

    block_T *block = b;
    frame_T *frame = allocFrame(block);
    instruction_T *instruction = NULL;
    symbol_T *returnAddress = NULL;

    int paramaterCounter = 0;

    while (1)
    {
        instruction = getNextIL(&block->program, instruction);
        //printf("%d: ", instruction->type);

        frame_T *source1 = NULL;
        frame_T *source2 = NULL;
        frame_T *destination = NULL;
        dataType_T source1Type = INT_TY;
        dataType_T source2Type = INT_TY;
        dataType_T destinationType = INT_TY;

        if (!instruction)
        {
            if (block = topPS(&blockStack))
            {
                instruction = topPS(&instructionStack);
                popPS(&blockStack);
                popPS(&instructionStack);
                continue;
            }
            else
                break;
        }

        if (instruction->type >= ADD_I && instruction->type <= SCAN_I)
        {
            if (instruction->type != PRINT_I)
            {
                destination = &frame[(int) ((symbol_T *)instruction->destination)->data];
                destinationType = ((symbol_T *)instruction->destination)->dataType;
            }
            if (instruction->type != SCAN_I)
            {
                if (((symbol_T *)instruction->source1)->type == VARIABLE_ST)
                    source1 = &frame[(int) ((symbol_T *)instruction->source1)->data];
                else
                    source1 = &((symbol_T *)instruction->source1)->data;
                source1Type = ((symbol_T *)instruction->source1)->dataType;
            }
            if (instruction->type != ASSIGN_I && instruction->type != PRINT_I && instruction->type != SCAN_I)
            {
                if (((symbol_T *)instruction->source2)->type == VARIABLE_ST)
                    source2 = &frame[(int) ((symbol_T *)instruction->source2)->data];
                else
                    source2 = &((symbol_T *)instruction->source2)->data;
                source2Type = ((symbol_T *)instruction->source2)->dataType;
            }
            //if (instruction->type >= G_I && instruction->type <= NEQ_I)
            /*if (instruction->type == PRINT_I || instruction->type == ASSIGN_I)
            {
                //printf("%d\n", ((symbol_T *)instruction->source1)->type);
                //printf("%d\n", ((symbol_T *)instruction->source1)->dataType);
                printf("index %d\n", ((symbol_T *)instruction->source1)->intValue);
            }*/
        }
        //printf("I: %d\n", instruction->type);
        switch (instruction->type)
        {
            case NEW_FRAME_I:
            {
                //printf("newframe %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                pushPS(&frameStack, frame);
                frame = allocFrame(instruction->destination);
                paramaterCounter = 0;
                break;
            }
            case SET_PARAMETR_I:
            {
                //printf("serparam %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                frame_T *oldFrame = topPS(&frameStack);
                if (((symbol_T *)instruction->source1)->type == VARIABLE_ST)
                    frame[paramaterCounter] = oldFrame[(int) ((symbol_T *)instruction->source1)->data];
                else
                    frame[paramaterCounter].doubleValue = ((symbol_T *)instruction->source1)->doubleValue; 
                paramaterCounter++;
                break;
            }
            case SUBSTRING_I:
            {
                //printf("substring %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                frame_T *oldFrame = topPS(&frameStack);
                int returnIndex = ((symbol_T *) instruction->source1)->intValue;
                int index = frame[1].intValue;
                int count = frame[2].intValue;
                char *string = frame[0].stringValue;
                int size = strlen(string);
                oldFrame[returnIndex].stringValue = allocString("", count);
                if (index + count <= size)
                {
                    memcpy(oldFrame[returnIndex].stringValue, &frame[0].stringValue[index], count );
                    oldFrame[returnIndex].stringValue[count] = '\0';
                }
                else
                    fprintf(stderr, "substr out of range\n");
                frame = oldFrame;
                popPS(&frameStack);
                break;
            }
            case LENGTH_I:
            {
                //printf("lengtg %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                frame_T *oldFrame = topPS(&frameStack);
                oldFrame[((symbol_T *) instruction->source1)->intValue].intValue = strlen(frame[0].stringValue);
                frame = oldFrame;
                popPS(&frameStack);
                break;
            }
            case FIND_I:
            {
                //printf("find %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                frame_T *oldFrame = topPS(&frameStack);
                oldFrame[((symbol_T *) instruction->source1)->intValue].intValue = find(frame[0].stringValue, frame[1].stringValue);
                frame = oldFrame;
                popPS(&frameStack);
                break;
            }
            case SORT_I:
            {
                //printf("sort %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                frame_T *oldFrame = topPS(&frameStack);
                oldFrame[((symbol_T *) instruction->source1)->intValue].stringValue = allocString(frame[0].stringValue, 0);
                sort(oldFrame[((symbol_T *) instruction->source1)->intValue].stringValue);
                frame = oldFrame;
                popPS(&frameStack);
                break;
            }
            case CONCAT_I:
            {
                //printf("concat %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                frame_T *oldFrame = topPS(&frameStack);
                int size = strlen(frame[1].stringValue);
                oldFrame[((symbol_T *) instruction->source1)->intValue].stringValue = allocString(frame[0].stringValue, size);
                strcat(oldFrame[((symbol_T *) instruction->source1)->intValue].stringValue, frame[1].stringValue);
                frame = oldFrame;
                popPS(&frameStack);
                break;
            }
            case CALL_I:
            {
                //printf("call %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                pushPS(&blockStack, block);
                pushPS(&callBlockStack, block);
                pushPS(&instructionStack, instruction);
                pushPS(&returnAddressStack, returnAddress);
                block = instruction->destination;
                returnAddress = instruction->source1;
                instruction = NULL;
                break;
            }
            case RETURN_I:
            {
                //printf("return %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                frame_T *oldFrame = topPS(&frameStack);
                if (oldFrame)
                {
                    if (instruction->source1)
                    {
                        if (((symbol_T *)instruction->source1)->type == VARIABLE_ST)
                            oldFrame[returnAddress->intValue] = frame[((symbol_T *)instruction->source1)->intValue];
                        else if (((symbol_T *)instruction->source1)->dataType == STRING_TY && returnAddress->dataType == STRING_TY)
                            oldFrame[returnAddress->intValue].stringValue = allocString(((symbol_T *)instruction->source1)->stringValue, 0);
                        else
                            oldFrame[returnAddress->intValue].doubleValue = ((symbol_T *)instruction->source1)->doubleValue;
                    }
                    frame = oldFrame;
                    popPS(&frameStack);
                    block_T *callBlock = topPS(&callBlockStack);
                    int i = 0;
                    do
                    {
                        i++;
                        block = topPS(&blockStack);
                        //printf("%p\n", block);
                        popPS(&blockStack);
                        instruction = topPS(&instructionStack);
                        popPS(&instructionStack);
                    }
                    while (callBlock != block);
                    //printf("pop: %d\n", i);
                    popPS(&callBlockStack);
                    returnAddress = topPS(&returnAddressStack);
                    popPS(&returnAddressStack);
                    continue;
                }
                else
                    return 0;
            }
            case IF_I:
            {
                //printf("if %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                int source1 = frame[(int) ((symbol_T *)instruction->source1)->data].intValue;
                //printf("S %d\n", source1);
                if (source1)
                {
                    pushPS(&blockStack, block);
                    pushPS(&instructionStack, instruction);
                    block = instruction->destination;
                    instruction = NULL;
                    continue;
                    //printf("if IC: %d\n", block->program.count);
                }
                else
                {
                    pushPS(&blockStack, block);
                    pushPS(&instructionStack, instruction);
                    block = instruction->source2;
                    instruction = NULL;
                    continue;
                    //printf("else IC: %d\n", block->program.count);
                }
                break;
            }
            case IF_NOT_BREAK_I:
            {
                //printf("innotbreak %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                int source1 = frame[(int) ((symbol_T *)instruction->source1)->data].intValue;
                if (!source1)
                {
                    block = topPS(&blockStack);
                    instruction = topPS(&instructionStack);
                    popPS(&blockStack);
                    popPS(&instructionStack);
                    continue;
                }
                break;
            }
            case BLOCK_I:
            {
                //printf("block %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                pushPS(&blockStack, block);
                pushPS(&instructionStack, instruction);
                block = instruction->destination;
                instruction = NULL;
                continue;
            }
            case JUMP_START_I:
            {
                //printf("jmpstart %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                int declarationLength = (int) instruction->destination;
                instruction = NULL;
                for (int i = 0; i < declarationLength; ++i)
                    instruction = getNextIL(&block->program, instruction);
                break;
            }
            case ADD_I: // +
            {
                //printf("add %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (destinationType == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    destination->doubleValue = op1 + op2;
                }
                else
                    destination->intValue = source1->intValue + source2->intValue;
                break;
            }
            case SUB_I: // -
            {
                //printf("sub %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (destinationType == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    destination->doubleValue = op1 - op2;
                }
                else
                    destination->intValue = source1->intValue - source2->intValue;
                break;
            }
            case MUL_I: // *
            {
                //printf("mul %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (destinationType == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    destination->doubleValue = op1 * op2;
                }
                else
                    destination->intValue = source1->intValue * source2->intValue;
                break;
            }
            case DIV_I: // /
            {
                //printf("div %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (destinationType == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    if (op2 == 0)
                        return 9;
                    destination->doubleValue = op1 / op2;
                }
                else
                {
                    if (source2->intValue == 0)
                        return 9;
                    destination->intValue = source1->intValue / source2->intValue;
                }
                break;
            }
            case EQ_I: // ==
            {
                //printf("== %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (source1Type == STRING_TY && source2Type == STRING_TY)
                {
                    destination->intValue = strcmp(source1->stringValue, source2->stringValue) == 0;
                }
                else if (source1Type == DOUBLE_TY || source2Type == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    //printf("%f %f\n", op1, op2);
                    destination->intValue = op1 == op2;
                }
                else
                    destination->intValue = source1->intValue == source2->intValue;
                break;
            }
            case NEQ_I: // !=
            {
                //printf("!= %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (source1Type == STRING_TY || source2Type == STRING_TY)
                    destination->intValue = strcmp(source1->stringValue, source2->stringValue) != 0;
                else if (source1Type == DOUBLE_TY || source2Type == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    //printf("%f %f\n", op1, op2);
                    destination->intValue = op1 != op2;
                }
                else
                    destination->intValue = source1->intValue != source2->intValue;
                break;
            }
            case G_I: // >
            {
                //printf("> %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (source1Type == STRING_TY || source2Type == STRING_TY)
                    destination->intValue = strcmp(source1->stringValue, source2->stringValue) > 0;
                else if (source1Type == DOUBLE_TY || source2Type == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    //printf("%f %f\n", op1, op2);
                    destination->intValue = op1 > op2;
                }
                else
                    destination->intValue = source1->intValue > source2->intValue;
                break;
            }
            case L_I: // <
            {
                //printf("< %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (source1Type == STRING_TY || source2Type == STRING_TY)
                    destination->intValue = strcmp(source1->stringValue, source2->stringValue) < 0;
                else if (source1Type == DOUBLE_TY || source2Type == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    //printf("%f %f\n", op1, op2);
                    destination->intValue = op1 < op2;
                }
                else
                    destination->intValue = source1->intValue < source2->intValue;
                break;
            }
            case GE_I: // >=
            {
                //printf(">= %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (source1Type == STRING_TY || source2Type == STRING_TY)
                    destination->intValue = strcmp(source1->stringValue, source2->stringValue) >= 0;
                else if (source1Type == DOUBLE_TY || source2Type == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    //printf("%f %f\n", op1, op2);
                    destination->intValue = op1 >= op2;
                }
                else
                    destination->intValue = source1->intValue >= source2->intValue;
                break;
            }
            case LE_I: // <=
            {
                //printf("<= %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (source1Type == STRING_TY || source2Type == STRING_TY)
                    destination->intValue = strcmp(source1->stringValue, source2->stringValue) <= 0;
                else if (source1Type == DOUBLE_TY || source2Type == DOUBLE_TY)
                {
                    double op1, op2;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    if (source2Type == DOUBLE_TY)
                        op2 = source2->doubleValue;
                    else
                        op2 = (double) source2->intValue;
                    //printf("%f %f\n", op1, op2);
                    destination->intValue = op1 <= op2;
                }
                else
                    destination->intValue = source1->intValue <= source2->intValue;
                break;
            }
            case ASSIGN_I: // =
            {
                //printf("= %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                if (destinationType == DOUBLE_TY)
                {
                    double op1;
                    if (source1Type == DOUBLE_TY)
                        op1 = source1->doubleValue;
                    else
                        op1 = (double) source1->intValue;
                    destination->doubleValue = op1;
                }
                else if (destinationType == INT_TY)
                {
                    int op1;
                    if (source1Type == DOUBLE_TY)
                        op1 = (int) source1->doubleValue;
                    else
                        op1 = source1->intValue;
                    destination->intValue = op1;
                }
                else
                {
                    destination->stringValue = allocString(source1->stringValue, 0);
                }
                break;
            }
            case PRINT_I: // printf()
            {
                //printf("print %d %p %p %p\n", source1Type, instruction->destination, instruction->source1, instruction->source2);
                switch (source1Type)
                {
                    case INT_TY:
                        printf("%d", source1->intValue);
                        break;
                    case DOUBLE_TY:
                        printf("%f", source1->doubleValue);
                        break;
                    case STRING_TY:
                        printf("%s", source1->stringValue);
                        break;
                }
                break;
            }
            case SCAN_I: // scanf()
            {
                //printf("scan %p %p %p\n", instruction->destination, instruction->source1, instruction->source2);
                string_T tempString;
                initS(&tempString);
                switch (destinationType)
                {
                    case INT_TY:
                        scanf("%d", &destination->intValue);
                        break;
                    case DOUBLE_TY:
                        scanf("%lf", &destination->doubleValue);
                        break;
                    case STRING_TY:
                    {
                        int sign;
                        int state = 0;
                        while ((sign = getchar()) != EOF)
                        {
                            switch (state)
                            {
                                case 0:
                                    if (!isspace(sign))
                                    {
                                        ungetc(sign, stdin);
                                        state = 1;
                                    }
                                    continue;
                                case 1:
                                    if (isspace(sign))
                                    {
                                        ungetc(sign, stdin);
                                        state = 2;
                                    }
                                    else
                                        addCharacterS(&tempString, sign);
                                    continue;
                            }
                            break;
                        }
                        destination->stringValue = tempString.data;
                        break;
                    }
                }
                break;
            }
        }
    }
    return 0;
}