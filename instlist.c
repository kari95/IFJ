/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 
* 	imlementation of instructions list 
*
* 	author: 	Karolína Klepáčková (xklepa04)
* 	created: 	2015-11-14
* 	modified: 	2015-11-15
*
*****************************************************************************************/
#include <instlist.h>

// init 'list'
void initIL(instrList_T *list)
{
  list->head 	= NULL;
  list->tail  	= NULL;
  list->active 	= NULL;
}
// destroy 'list' (free all alocated memory) 
void destroiIL(instrList_T *list)
{
  list->active 	= NULL;
  list->tail  	= NULL;

 while (list->head != NULL)
 {
	instruction_T pom = list->head;
	list->head 		  = pom->right;
	free(list->head);
 }	
  list->head = NULL;
}

// insert 'instruction' to 'list'
int insertFirstIL(instrList_T *list, instruction_T instruction)
{
 instruction_T pom = malloc(sizeof(struct instrList_T));
	if (pom == NULL)
		return 99; //exception - out of memory - err 99 by IFJ documentation 
	else
	 {
		pom->right 		 = list->head;
		pom->type 		 = instruction.type;
		pom->destination = instruction->destination;
		pom->source1 	 = instruction->source1;
		pom->source2	 = instruction->source2;
		pom->left = NULL;

		if (list->head != NULL)
		    list->head->left = pom;
		
		list->head = pom;

	  	if (list->tail == NULL)
	  	list->tail = pom;
	 }

 return 0;
}

void insertLastIL(instrList_T *list, instruction_T instruction)
{
instruction_T pom = malloc(sizeof(struct instrList_T));
	if (pom == NULL)
		return 99; //exception - out of memory - err 99 by IFJ documentation 
	else
	 {
		pom->left 		 = list->tail;
		pom->type 		 = instruction.type;
		pom->destination = instruction->destination;
		pom->source1 	 = instruction->source1;
		pom->source2	 = instruction->source2;
		pom->right = NULL;

		if (list->tail != NULL)
		    list->tail->right = pom;
		
		list->tail = pom;

	  	if (list->head == NULL)
	  	list->head = pom;
	 }
 return 0;
}

// returns pointer to active item from 'list'
instruction_T *getActiveIL(instrList_T *list)
{
 return list->active != NULL;
}

// move activity to next item (instruction) in 'list'
void nextIL(instrList_T *list)
{
 if (list->active != NULL)
		list->active = list->active->right;
}

// move activity to nex item (instruction) in 'list'
void startIL(instrList_T *list)
{
 list->active = list->head;
}

/*void preInsertIL(instrList_T *list, instruction_T instruction);
void postInsertIL(instrList_T *list, instruction_T instruction);

// move activity in 'list' by 'distance'
void jumpLI(instrList_T *list, int distance);*/

