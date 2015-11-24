/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	scaner module header
* 	do a lexical analysis
*
* 	author: 	Karolína Klepáčková (xklepa04)
* 	created: 	2015-10-30
* 	modified: 	2015-11-20
*
*****************************************************************************************/
/*

	ROUGH OUTLINE OF THE PROBLEM SOLUTION


*/


#ifndef SCANNER_H
#define SCANNER_H

#include "string.h"

FILE *inputFile;
unsigned int global_column = 1;
unsigned int global_row = 1;


// init scanner an set 'programFile' as input file
void initSC(FILE *programFile)
{
 inputFile = programFile;
}

void tokenPosition(unsigned int global_row, unsigned int global_column)
{
token.row = global_row;
token.column = global_column;
}

// write ? read token to structure 'token'
// return zero if success
//gets values into 'token' structure
int getTokenSC(token_T *token)
{
bool error = false ; // if smth goes wrong I should return error = 1 if it's bug of lexeme structure


while ((error == false ) && (sign = fgetc (inputFile))) // fgetc gets characters from inputFile 
{
  switch (state)
  {
  	if (sign != EOF)



	case BEGIN_S:    //begining state of automata
		 if (sign == '=') //could be assign  = or equal ==
		 {

		 }
		 else if (sign == '(')
		 {
		 	token.type = LBRACKET_TO;
		 	token->stringValue = "("; // or should it be stringValue = sigń ?
		 	tokenPosition (global_row, global_column);
		 }
		 else if (sign == ')')
		 {
		 	token.type = RBRACKET_TO;
		 	token->stringValue = ")";
			tokenPosition (global_row, global_column);
		 }
		 else if (sign == ';')
		 {
		 	token.type = SEMI_TO;
		 	token->stringValue = ";";
		 	tokenPosition (global_row, global_column);
		 }
		 else if (sign == ',')
		 {
		 	token.type = COMMA_TO;
		 	token->stringValue = ","; 
		 	tokenPosition (global_row, global_column);
		 }
		 else if (sign == '{')
		 {
		 	token.type = LCBRACKET_TO;
		 	token->stringValue = "{";
		 	tokenPosition (global_row, global_column);
		 }
		 else if (sign == '}')
		 {
		 	token.type = RCBRACKET_TO;
		 	token->stringValue = "}";
		 	tokenPosition (global_row, global_column);
		 }
		 else if (sign == '<')/* could be just < or <= or << */
		 {
		 	
		 }
		 else if (sign == '>') /* could be just > or >= or >> */
		 {
		 	
		 }
		 else if (sign == '+')
		 {
			token.type = PLUS_TO;
			token->stringValue = "+";
			tokenPosition (global_row, global_column);		 	
		 }
		 else if (sign == '-')
		 {
		 	token.type = MINUS_TO;
		 	token->stringValue = "-";
			tokenPosition (global_row, global_column);		 	

		 }
		 else if (sign == '*') /*  could it be in string or not? - should I solve it? and gloss with ending *  ???*/
		 {
		 	token.type = DIV_TO;
		 	token->stringValue = "*";
			tokenPosition (global_row, global_column);		 	

		 }
		 else if (sign == '/')  /*slash could be begining of gloss with * or /     */
		 {

		 }
		 else if (sign == '!') /*exclamation mark could be ! or != */
		 {
		 	
		 }
		 else if (sign == EOF)  /*end of file*/
  		 {
		 	token.type = EOF_TO;
		 	token->stringValue = EOF;
			tokenPosition (global_row, global_column);		 	

		 }
		 else if (sign == '\\')  /* backslash */
		 {

		 }
		 else if (sign == '\"')  /* double quote */
		 {
		 	
		 }
		 else if (sign == '\t')  /* horizontal tab */
		 {
		 	
		 }
		 else if (sign == '\xdd') /*  arbitrary hexadecimal value byte dd where dd means 01-FF letters could be a-f or A-F */
		 {
		 	
		 }
		 else if (sign == '\n')
		 {	
		 	global_row++;
		 }
		 
		 else if (sign == '_')
		 {
		 	state = ID_S; 		/*  begining of identificator or smth in string*/
		 }
		 else if (sign == '.')
		 {
		 	state = DOUBLE_S;	/* probably continuous double or smth in string */
		 }
		 else if (isalpha(sign))
		 {
		 	state = ID_S;
		 }
		 else if (isdigit(sign))
		 {
		 	state = NUMBER_S;
		 }

	break;

	case ID_S:          // identificator

	break;	

    case TYPE_S:        // int/double/string/auto
    
    break;

    case CIN_S:         // cin
    
    break;

    case COUT_S:        // cout
    
    break;

    case RETURN_S:      // return
    
    break;

    case IF_S:          // if
    
    break;

    case ELSE_S:        // else
    break;

    case FOR_S:         // for
    break;

    case INT_S:         // xxxxx
    break;

    case DOUBLE_S:      // x.xxx
    break;

    case STRING_S:      // ""
    break;

    case ASSIGN_S:      // =
    break;

    case OUT_S:         // <<
    break;

    case IN_S:          // >>
    break;

    case MUL_S:         // *
    break;

    case DIV_S:         // /
    break;

    case G_S:           // >
    break;

    case L_S:           // <
    break;

    case GE_S:          // >=
    break;

    case LE_S:          // <=
    break;

    case E_S:           // ==
    break;

    case NE_S:          // !=
    break;

    case NUMBER_S: //case for convertion from string to int/double number
    break;


  }


/*
prectu jeden znak z inputfile 
porovnam ho:

if (!isspace (sign))
unget (sign, inputFile)

if (isprint(sign))
printable?


pokud je netisknutelny ignoruji ho ale prictu k column ++
pokud najdu konec radku udelam column = 0  row ++
(row se mi jen zvysuji column se musi i nulovat)

pokud je konec mezery tak prave odchyceny znak pomoci sign = fgetc(inputFile) musim dat zpatky cili unget do inputFile

funkce pro zjisteni co je to za znak - isblank () zjisti jestli to je mezera nebo tabulator

isalnum () je ekvivalentni s (isalpha() || isdigit ())

isalpha (sign)
- zjisti zda-li to je pismenko nebo ne (pokud ano vraci 0)
 isdigit(sign) ... sign = int





*/


return 0;
}



#endif 
