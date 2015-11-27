/*****************************************************************************************
* 
* 	IFJ - interpret of IFJ15 language
*
* 	scaner module header
* 	makes a lexical analysis
*
* 	author: 	Karolína Klepáčková (xklepa04)
* 	created: 	2015-10-30
* 	modified: 	2015-11-26
*
*****************************************************************************************/

/*
NEDODELANE - dorozliseni typu TYPE_TO - implementace reseni identifikatoru/rezervovanych slov v ID_S - podle toho co je to za "IDENTIFIKATOR"
hexadecimal escape sequence -> v STRING_S -> HEXADECIMAL... _S + v BEGIN_S to neni dodelane co tam volat


STRING _ SMAZAT VYPISY
*/

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include "scanner.h"
#include "string.h"

char reserved_words[10][7] = {
    "int",
    "double",
    "string",
    "auto",
    "cin",
    "cout",
    "return",
    "if",
    "else",
    "for",
};

enum
{
BEGIN_S,
ID_S,
STRING_S,
ESCAPE_SEQUENCE_S,
HEXADECIMAL_ESC_SEQ_S,
NUMBER_S,
DOUBLE_S,
DIV_S,
ROW_COMMENT_S,
BLOCK_COMMENT_S,
BLOCK_COMMENT_END_S,
NE_COMPARE_S,
NE_S,
ASSIGN_OR_EQUAL_S,
EQUAL_S,
L_LE_OUT_S,
LE_S,
IN_S,
OUT_S,
G_GE_IN_S,
GE_S

};

FILE *inputFile;
unsigned int global_column = 1;
unsigned int global_row = 1;
string_T string;
int state = BEGIN_S;

// init scanner an set 'programFile' as input file
void initSC(FILE *programFile)
{
    inputFile = programFile;
    initS(&string);
}

void tokenInitialisation(token_T *token, tokenType_T type_of_token)
{
token->type = type_of_token;	
token->row = global_row;
token->col = global_column;
global_column++;
}

void alphanumerical (char sign, token_T *token, tokenType_T type_of_token)
{
	tokenInitialisation (token, type_of_token);
	ungetc(sign, inputFile);
}
void blank (token_T *token, tokenType_T type_of_token)
{
	tokenInitialisation (token, type_of_token);
	global_column++;
}

void end_of_row ()
{
	global_row++;
    global_column = 1;
    state = BEGIN_S; 
}

// write token to structure 'token'
// return zero if success
//gets values into 'token' structure
int getTokenSC(token_T *token)
{
bool error = false ; // if smth goes wrong I should return error = 1 if it's bug of lexeme structure
bool string_test = false;
bool first_char = false;
int local_column;
tokenType_T type_of_token;
int sign;
cleanS (&string);

while ((error == false ) && (sign = fgetc (inputFile))) // fgetc gets characters from inputFile 
{
  switch (state)
  {
  	if (sign != EOF)
	{
	  case BEGIN_S:    //begining state of automata
			if (sign == '=') //could be assign = or equal ==
		 	{
		 		state = ASSIGN_OR_EQUAL_S;
		 	}
		 	else if (sign == '(')
		 	{
		 		type_of_token = LBRACKET_TO;
		 		tokenInitialisation (token, type_of_token);
                return 0;       
		 	}
		 	else if (sign == ')')
		 	{
		 		type_of_token = RBRACKET_TO;
				tokenInitialisation (token, type_of_token);
                return 0;       
		 	}
		 	else if (sign == ';')
		 	{
		 		type_of_token = SEMI_TO;
		 		tokenInitialisation (token, type_of_token);
                return 0;       
		 	}
		 	else if (sign == ',')
		 	{
		 		type_of_token = COMMA_TO;
		 		tokenInitialisation (token, type_of_token);
                return 0;       
		 	}
		 	else if (sign == '{')
		 	{
		 		type_of_token = LCBRACKET_TO;
		 		tokenInitialisation (token, type_of_token);
                return 0;       
		 	}
		 	else if (sign == '}')
		 	{
			 	type_of_token = RCBRACKET_TO;
		 		tokenInitialisation (token, type_of_token);
                return 0;       
		 	}
		 	else if (sign == '<')/* could be just < or <= or << */
		 	{
		 		state = L_LE_OUT_S;
		 	}
		 	else if (sign == '>') /* could be just > or >= or >> */
		 	{
		 		state = G_GE_IN_S;
		 	}
		 	else if (sign == '+')
		 	{
				type_of_token = PLUS_TO;
				tokenInitialisation (token, type_of_token);	
                return 0;	 	
		 	}
		 	else if (sign == '-')
		 	{
		 		type_of_token = MINUS_TO;
				tokenInitialisation (token, type_of_token);
                return 0;       		 	
		 	}
		 	else if (sign == '*') /*  could it be in string or not? - should I solve it? and gloss with ending *  ???*/
		 	{
		 		type_of_token = DIV_TO;
				tokenInitialisation (token, type_of_token);	
                return 0;       	 	
		 	}
		 	else if (sign == '/')  /*slash could be begining of gloss with * or /  or div   */
		 	{	
		 		state = DIV_S;
		 	}
		 	else if (sign == '!') /*exclamation mark != */
		 	{
		 		state = NE_S;
		 	}
		 	else if (sign == '"')
		 	{
		 		state = STRING_S;
		 	}
			else if (sign == '\\')  /* backslash but it means just one */
		 	{
		 		if (string_test == false)
		 			error = true;
		 	}
			else if (sign == '\"')  /* double quote */
		 	{
		 		if (string_test == false)
		 			error = true;
		 	}
		 	else if (sign == '\t')  /* horizontal tab just one sign */
		 	{
		 		if (string_test == false)
		 			error = true;
		 	}
		 	else if (sign == '\xdd') 
		 	{
                if (string_test == false)
                    error = true;
                // asi  :D HEXADECIMAL_ESC_SEQ_S
            /*  nacist prvni cislo -> prevod na cislo komkretni * 16 aby bylo hexadec ,
            u cisel udelat cislo  '0'
            u pismen asi pismeno - znak 'a'
            nebo znak 'A' + asi 10 
            arbitrary hexadecimal value byte dd where dd means 01-FF letters could be a-f or A-F */
		 	
		 	}
		 	else if (sign == '\n')  //  else sign could be written in 2 versions
		 	{	
		 		end_of_row ();
		 	}
		 
		 	else if (sign == '_')
		 	{
		 		first_char = true;
		 		state = ID_S; 	
		 		addCharacterS (&string, sign);	/*  begining of identificator*/
			}
		 	else if (isalpha(sign))
		 	{
		 		first_char = true;
		 		state = ID_S;
		 		addCharacterS (&string, sign);
		 	}
		 	else if (isdigit(sign))
		 	{
		 		first_char = true;
		 		state = NUMBER_S;
		 		addCharacterS (&string, sign);
		 	}
		 	else if (isspace(sign))
		 	{
		 		state = BEGIN_S;
		 		global_column++;
		 	}
			
	  break;


	/***************************************************** end of BEGIN_S	*****************************************************/


	case ID_S:          // identificator
						//necessary to copmare if ID isn't any of reserved words
	if (first_char)
	{
		local_column = global_column;
	}
										// if it begins with _ it cannot be a reserved word 
	if (isalnum(sign) || sign == '_')
	{
		global_column++;
		addCharacterS (&string, sign);
		state = ID_S;
	}
	else 
	{
		
		token->stringValue = string.data;  //ulozit string jako identifikator

		for (int i=0; i<10; i++)
		{
			//printf("%s %s\n", string.data, reserved_words[i]);
			if (strcmp (string.data, reserved_words[i]) == 0) //are equal - testing for reserved words
			{
				if (i == 0 )
				{	
					token->dataType = INT_TY;
					type_of_token = TYPE_TO; // test for type "int" 

					break;
				}
				else if (i == 1)
				{
					token->dataType = DOUBLE_TY;
					type_of_token = TYPE_TO; // test for type "double"
					break;
				}
				else if (i == 2)
				{
					token->dataType = STRING_TY;
					type_of_token = TYPE_TO; // test for type "string"
					break;
				}
				else if (i == 3)
				{
					token->dataType = AUTO_TY;
					type_of_token = TYPE_TO; // test for type "auto"
					break;
				}
				else if (i == 4)
				{
					type_of_token = CIN_TO;
					break;
				}
				else if (i == 5)
				{
					type_of_token = COUT_TO;
					break;
				}
				else if (i == 6)
				{
					type_of_token = RETURN_TO;
					break;
				}
				else if (i == 7)
				{
					type_of_token = IF_TO;
					break;
				}
				else if (i == 8)
				{
					type_of_token = ELSE_TO;
					break;
				}
				else if (i == 9)
				{
					type_of_token = FOR_TO;
					break;
				}
			}
			else
			{
				type_of_token = ID_TO; //if it's not reserved word
			}

		}
	
		tokenInitialisation (token, type_of_token); //inicialization of token identificator
		state = BEGIN_S;
		ungetc(sign, inputFile);   
        return 0;
	}

	first_char = false;

	break;	

   
 	case STRING_S:      // "" need some dynamical structure to keep whole string saved
	string_test = true;
	local_column = global_column;
	global_column++;

	if (sign == '"')     // end of string
	{
		token->stringValue = string.data;
		blank(token, STRING_TO);
		string_test = false;
        state = BEGIN_S;
		//printf("end\n");
        return 0;
	}
	else if (sign == '\\')  /* it means just one \ */
	{
		state = ESCAPE_SEQUENCE_S;
	}
	else
	{
		//printf("%c\n", sign);
		addCharacterS(&string, sign);
    	state = STRING_S;
    }
    break;

    case ESCAPE_SEQUENCE_S:
    global_column++;
    if (sign == '\\') /* means \\ in our source code*/
    {
    	addCharacterS(&string, '\\');
    }
    else if (sign == '"')
    {
    	addCharacterS (&string, '\"');
    }
    else if (sign == 'n')
    {
    	addCharacterS (&string, '\n');
    }
    else if (sign == 't')
    {
    	addCharacterS (&string, '\t');
    }
    else if (sign == 'x') //hexadecimal code of escape sequence
    {
    	state = HEXADECIMAL_ESC_SEQ_S;
    }
    else
    {
    	addCharacterS (&string, sign);
    }
    state = STRING_S;
    break;

    case HEXADECIMAL_ESC_SEQ_S: //for hexadecimal number od escape sequence                       **********NOT YET   *  HEXADECIMAL ESC SEQ*********
    global_column++;
    //nacitam dokud neni konec toho hexadec cisla
    break;


    case NUMBER_S: //case for convertion from string to int/double number
    if (first_char)
    {
    local_column = global_column;
    global_column++;
    }
    if (sign == '.' || sign == 'e' || sign == 'E')
    {
    	addCharacterS(&string, sign);
    	state = DOUBLE_S;
    	global_column++;
    }
    else if (isdigit(sign))
    {
    	addCharacterS(&string, sign);
    	state = NUMBER_S;
    	global_column++;
    }
    else 
    {
    	int val_int = 0;
    	val_int = atoi(string.data);
    	type_of_token = INT_TO;
    	token->intValue = val_int;
    	tokenInitialisation (token, type_of_token);
    	ungetc(sign, inputFile);
    	state = BEGIN_S;
    	return 0;
    }

    first_char = false;
    break;


    case DOUBLE_S:      // x.xxx for double numbers
    if (isdigit(sign))
    {
		addCharacterS(&string, sign);
    	state = DOUBLE_S;
    	global_column++;
    }
    else //anything else than number 
    {
    	double val_double = 0;
    	val_double = atof(string.data);
    	type_of_token = DOUBLE_TO;
    	token->doubleValue = val_double;
    	tokenInitialisation (token, type_of_token);
    	ungetc(sign, inputFile);
    	state = BEGIN_S;
    	return 0;
    }

    break;


    case DIV_S:         // /
    if (sign == '/')
    {
    	state = ROW_COMMENT_S;
    }
    else if (sign == '*')
    {
    	global_column++;
    	state = BLOCK_COMMENT_S;
    }
    else if (isalnum(sign))
    {
		type_of_token = NE_TO;
		alphanumerical (sign, token, type_of_token);
		state = BEGIN_S;
		return 0;
    }
    else if (isblank(sign))
    {
    	type_of_token = NE_TO;
		blank (token, type_of_token);
		state = BEGIN_S;
        return 0;
   	}
   	else if (sign == '\n')
   	{
   		end_of_row ();
   		state = BEGIN_S;
   	}
   	else
   		error = true;
    
    break;

    case ROW_COMMENT_S: // coment begining with //
    if (sign == '\n')
    {
    	end_of_row (); //needs next state as BEGIN_S
    }
    else
    {
    	state = ROW_COMMENT_S;
    }
    break;

    case BLOCK_COMMENT_S: //comment begining with /*
    if (sign == '*')
    {
    	state = BLOCK_COMMENT_END_S;
    	global_column++;
    }
    else if (sign == '\n')
    {
    	global_row++;
    	global_column = 1;
    	state = BLOCK_COMMENT_S;
    }
    else 
    {
    	state = BLOCK_COMMENT_S;
    	global_column++;
    }
    break;
    
    case BLOCK_COMMENT_END_S:
    if (sign == '/') 
    {
    	state = BEGIN_S;
    	global_column++;
    }
    else if (sign == '\n')
    {
    	global_row++;
    	global_column = 1;
    	state = BLOCK_COMMENT_S;
    }
    else
    {
    	state = BLOCK_COMMENT_S;
    	global_column++;
    }
    break;


   
    case NE_S:          // !=
    if (sign == '=')
    {
    	state = NE_COMPARE_S;
    	local_column = global_column;
    	global_column++;
    }
    else    			//anything what goes after ! and isn't '=' is mistake
    	error = true;
    break;

    case NE_COMPARE_S:          // !=
    if (isalnum(sign))
    {
    	type_of_token = NE_TO;
		alphanumerical (sign, token, type_of_token);
		state = BEGIN_S;
        return 0;       
    }
    else if (isblank(sign))
    {
    	type_of_token = NE_TO;
		blank (token, type_of_token);
		state = BEGIN_S;
        return 0;
    }
    else if (sign == '\n')
    {
    	end_of_row ();
    	state = BEGIN_S;
        return 0;
    }
    else
    	error = true;
    break;




    case ASSIGN_OR_EQUAL_S: //for = == 

    if (sign == '=') // then it is '=='
    {
    	state = EQUAL_S;
    	local_column = global_column;
    	global_column++;
    }
    else if (isalnum(sign))  //if alphanumerical
    {
    	type_of_token = ASSIGN_TO;
		alphanumerical (sign, token, type_of_token);
		state = BEGIN_S;
        return 0;
    }
    else if (isblank(sign)) //if nonprintable sign
    {
    	type_of_token = ASSIGN_TO;
		blank (token, type_of_token);
        state = BEGIN_S;
        return 0;
    }
   /* else if (sign == '\n')
    {
    	end_of_row ();
        return 0;
    }*/
    else // if other operator 
    	error = true;
    break;
    

    case EQUAL_S: // =  if it's assign
  
    if (isalnum (sign))
    {
    	type_of_token = E_TO;
		alphanumerical (sign, token, type_of_token);
		state = BEGIN_S;
        return 0;
    }
    else if (isblank(sign))
    {
    	type_of_token = E_TO;
    	blank (token, type_of_token);
    	state = BEGIN_S;
        return 0;
    }
    else if (sign == '\n')
    {
    	end_of_row ();
    	state = BEGIN_S;
        return 0;
    }
    else
        error = true;
	break;
    



    case L_LE_OUT_S:  //for < <= <<
    if (sign == '=')
    {
    	state = LE_S;
    	local_column = global_column;
    	global_column++;

    }
    else if (sign == '<')
    {
    	state = OUT_S;
    	local_column = global_column;
    	global_column++;
    }
    else if (isalnum(sign))
    {
    	type_of_token = L_TO;
    	alphanumerical (sign, token, type_of_token);
    	state = BEGIN_S;
        return 0;
    }
    else if (isblank(sign))
    {	
    	type_of_token = L_TO;
    	blank (token, type_of_token);
    	state = BEGIN_S;
        return 0;
    }
    else if (sign == '\n')
    {
    	end_of_row ();
    	state = BEGIN_S;
        return 0;
    }
    else
    	error = true;

    break;

	case LE_S:          // <=
     if (isalnum (sign))
    {
    	type_of_token = LE_TO;
		alphanumerical (sign, token, type_of_token);
		state = BEGIN_S;
        return 0;
    }
    else if (isblank(sign))
    {
    	type_of_token = LE_TO;
    	blank (token, type_of_token);
    	state = BEGIN_S;
        return 0;
    }
    else if (sign == '\n')
    {
    	end_of_row ();
    	state = BEGIN_S;
        return 0;
    }
    else
        error = true;
    break;

    case OUT_S:         // << 
     if (isalnum (sign))
    {
    	type_of_token = OUT_TO;
		alphanumerical (sign, token, type_of_token);
		state = BEGIN_S;
        return 0;
    }
    else if (isblank(sign))
    {
    	type_of_token = OUT_TO;
    	blank (token, type_of_token);
    	state = BEGIN_S;
        return 0;
    }
    else if (sign == '\n')
    {
		end_of_row ();
		state = BEGIN_S;
        return 0;
    }
    else
        error = true;
    break;


	


    case G_GE_IN_S:  //for > >= >>
	if (sign == '=')
    {
    	state = GE_S;
    	local_column = global_column;
    	global_column++;
    }
    else if (sign == '>')
    {
    	state = IN_S;
    	local_column = global_column;
    	global_column++;
    }
    else if (isalnum(sign))
    {
    	type_of_token = G_TO;
    	alphanumerical (sign, token, type_of_token);
    	state = BEGIN_S;
        return 0;
    }
    else if (isblank(sign))
    {
    	type_of_token = G_TO;
    	blank (token, type_of_token);
    	state = BEGIN_S;
        return 0;
    }
    else if (sign == '\n')
    {
    	end_of_row ();
    	state = BEGIN_S;
        return 0;
    }
    else
    	error = true;
    break;


	case IN_S:          // >>
	if (isalnum (sign))
    {
        state = BEGIN_S;
    	type_of_token = IN_TO;
		alphanumerical (sign, token, type_of_token);
        return 0;
    }
    else if (isblank(sign))
    {
        state = BEGIN_S;
    	type_of_token = IN_TO;
    	blank (token, type_of_token);
        return 0;
    }
    else if (sign == '\n')
    {
    	end_of_row ();
        return 0;
    }
    else
        error = true;
    break;

    
    case GE_S:          // >=
    if (isalnum (sign))
    {
        state = BEGIN_S;
    	type_of_token = GE_TO;
		alphanumerical (sign, token, type_of_token);
        return 0;
    }
    else if (isblank(sign))
    {
        state = BEGIN_S;
    	type_of_token = GE_TO;
    	blank (token, type_of_token);
        return 0;
    }
    else if (sign == '\n')
    {
    	end_of_row ();
        return 0;
    }
    else
        error = true;
    break;


/************************	END OF CASES	************************************/



}
  else if (sign == EOF)  /*end of file   */
         {
            type_of_token = EOF_TO;
            tokenInitialisation (token, type_of_token);          
         }

  }
}
if (error == true)
	return 1;
return 0;
}

/*
isspace OR isblank ???

isalnum () je ekvivalentni s (isalpha() || isdigit ())

*/
