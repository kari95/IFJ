/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	list of instructions header
* 	imlementation of instructions list 
*
* 	author: 	Karolína Klepáčková (xklepa04)
* 	created: 	2015-11-02
* 	modified: 	2015-11-14
*
*****************************************************************************************/

#ifndef INSTR_LIST_H
#define INSTR_LIST_H

// instructions
typedef enum {
	RETURN_I,
	ADD_I,
	SUB_I,
	MUL_I,
	DIV_I;
} instType_T;

// instruction structure (item of instructon list)
typedef struct instruction {
	struct instruction *left;
	struct instruction *right;
	instType_T type;
	void *destination;
	void *source1;
	void *source2;
} instruction_T;

// structure of instruction list
typedef struct {
	instruction_T *head;
	instruction_T *tail;
	instruction_T *active;
} instrList_T;

// init 'list'
void initIL(instrList_T *list);

// destroy 'list' (free all alocated memory) 
void destroiIL(instrList_T *list);

// insert 'instruction' to 'list'
void insertFirstIL(instrList_T *list, instruction_T instruction);
void insertLastIL(instrList_T *list, instruction_T instruction);
void preInsertIL(instrList_T *list, instruction_T instruction);
void postInsertIL(instrList_T *list, instruction_T instruction);

// move activity in 'list' by 'distance'
void jumpLI(instrList_T *list, int distance);

// move activity to next item (instruction) in 'list'
void nextLI(instrList_T *list);

// returns pointer to active item from 'list'
instruction_T *getActiveIL(instrList_T *list);

#endif