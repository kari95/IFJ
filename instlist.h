/*****************************************************************************************
* 
*   IFJ - interpret of IFJ15 language
*
*   list of instructions header
*   implementation of instructions list 
*
*   author:     Karolína Klepáčková (xklepa04)
*   created:    2015-11-02
*   modified:   2015-11-14
*
*****************************************************************************************/

#ifndef INSTR_LIST_H
#define INSTR_LIST_H

// instructions
typedef enum {
    ADD_I,      // +        
    SUB_I,      // -
    MUL_I,      // *
    DIV_I,      // /
    EQ_I,       // ==
    NEQ_I,      // !=
    G_I,        // >
    L_I,        // <
    GE_I,       // >=
    LE_I,       // <=
    LENGTH_I,   // length()
    SORT_I,     // sort()
    FIND_I,     // find()
    CONCAT_I,   // concat()
    SUBSTRING_I,// substring()
    PRINT_I,    // printf()
    SCAN_I,     // scanf()
    JUMP_I,     // jump
    IF_JUMP_I,  // if ( E ) jump
    CALL_I,     // call 
    RETURN_I,   // return   
} instType_T;

// instruction structure (item of instructon list)
typedef struct {
    instType_T type;
    void *destination;
    void *source1;
    void *source2;
} instruction_T;

// item of instruction list
typedef struct instListItem {
    struct instListItem *left;
    struct instListItem *right;
    instruction_T data;
} instListItem_T;

// structure of instruction list
typedef struct {
    instListItem_T *head;
    instListItem_T *tail;
    instListItem_T *active;
} instList_T;

// init 'list'
void initIL(instList_T *list);

// destroy 'list' (free all alocated memory) 
void destroiIL(instList_T *list);

// insert 'instruction' to 'list'
int insertFirstIL(instList_T *list, instruction_T instruction);
int insertLastIL(instList_T *list, instruction_T instruction);
int preInsertIL(instList_T *list, instruction_T instruction);
int postInsertIL(instList_T *list, instruction_T instruction);

// move activity in 'list' by 'distance'
void jumpIL(instList_T *list, int distance);

// move activity to firt instruction in 'list'
void startIL(instList_T *list);

// move activity to next item (instruction) in 'list'
void nextIL(instList_T *list);

// returns pointer to active item from 'list'
instruction_T *getActiveIL(instList_T *list);

#endif