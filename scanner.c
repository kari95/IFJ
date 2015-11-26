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
NEDODELANE - dorozliseni typu TYPE_TO - implementace reseni identifikatoru/rezervovanych slov v ID_S
hexadecimal escape sequence -> v STRING_S -> HEXADECIMAL... _S + v BEGIN_S to neni dodelane co tam volat

jeste si nejsem uplne jista jestli string_T string mam dobre inicializovany a pak zruseny - ale to resit pokud to nepojede
nerucim ze zde jsou vsude spravne pocty zavorek a ze se v tom nekdo vyzna
kdyztak dejte echo zkusim na to jeste ve ctvrtek mrknout,ale asi spis az v patek
*/

#ifndef SCANNER_H
#define SCANNER_H


#include "string.h"

const char *reserved_words[10];
reserved_words[0] = "int";
reserved_words[1] = "double";
reserved_words[2] = "string";
reserved_words[3] = "auto";
reserved_words[4] = "cin";
reserved_words[5] = "cout";
reserved_words[6] = "return";
reserved_words[7] = "if";
reserved_words[8] = "else";
reserved_words[9] = "for";

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

}

FILE *inputFile;
unsigned int global_column = 1;
unsigned int global_row = 1;
string_T string;
int state = BEGIN_S;

// init scanner an set 'programFile' as input file
void initSC(FILE *programFile)
{
 inputFile = programFile;
}

void tokenInitialisation(tokenType_T type_of_token, unsigned int global_row, unsigned int global_column)
{
token.type = type_of_token;	
token.row = global_row;
token.column = global_column;
global_column++;
}

void alphanumerical (tokenType_T type_of_token, unsigned int global_row, unsigned int global_column)
{
	tokenInitialisation (type_of_token, global_row, global_column);
	unget(sign, inputFile);
}
void blank (tokenType_T type_of_token, unsigned int global_row, unsigned int global_column)
{
	tokenInitialisation (type_of_token, global_row, global_column);
	global_column++;
}

void end_of_row (unsigned int global_row, unsigned int global_column)
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
tokenType_T type_of_token;
int sign;
cleanS (string);

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
		 		tokenInitialisation (type_of_token, global_row, global_column);
		 	}
		 	else if (sign == ')')
		 	{
		 		type_of_token = RBRACKET_TO;
				tokenInitialisation (type_of_token, global_row, global_column);
		 	}
		 	else if (sign == ';')
		 	{
		 		type_of_token = SEMI_TO;
		 		tokenInitialisation (type_of_token, global_row, global_column);
		 	}
		 	else if (sign == ',')
		 	{
		 		type_of_token = COMMA_TO;
		 		tokenInitialisation (type_of_token, global_row, global_column);
		 	}
		 	else if (sign == '{')
		 	{
		 		type_of_token = LCBRACKET_TO;
		 		tokenInitialisation (type_of_token, global_row, global_column);
		 	}
		 	else if (sign == '}')
		 	{
			 	type_of_token = RCBRACKET_TO;
		 		tokenInitialisation (type_of_token, global_row, global_column);
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
				tokenInitialisation (type_of_token, global_row, global_column);		 	
		 	}
		 	else if (sign == '-')
		 	{
		 		type_of_token = MINUS_TO;
				tokenInitialisation (type_of_token, global_row, global_column);		 	
		 	}
		 	else if (sign == '*') /*  could it be in string or not? - should I solve it? and gloss with ending *  ???*/
		 	{
		 		type_of_token = DIV_TO;
				tokenInitialisation (type_of_token, global_row, global_column);		 	
		 	}
		 	else if (sign == '/')  /*slash could be begining of gloss with * or /  or div   */
		 	{	
		 		state = DIV_S;
		 	}
		 	else if (sign == '!') /*exclamation mark != */
		 	{
		 		state = NE_S;
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
				if (string_test == false)
		 			error = true;
		 		// asi  :D HEXADECIMAL_ESC_SEQ_S
			/*  nacist prvni cislo -> prevod na cislo komkretni * 16 aby bylo hexadec ,
			u cisel udelat cislo  '0'
			u pismen asi pismeno - znak 'a'
			nebo znak 'A' + asi 10 
 		 	arbitrary hexadecimal value byte dd where dd means 01-FF letters could be a-f or A-F */
		 	{
		 	
		 	}
		 	else if (sign == '\n')  //  else sign could be written in 2 versions
		 	{	
		 		end_of_row (global_row, global_column);
		 	}
		 
		 	else if (sign == '_')
		 	{
		 		first_char = true;
		 		state = ID_S; 	
		 		addCharacterS (string, sign);	/*  begining of identificator*/
			}
		 	else if (isalpha(sign))
		 	{
		 		first_char = true;
		 		state = ID_S;
		 		addCharacterS (string, sign);
		 	}
		 	else if (isdigit(sign))
		 	{
		 		first_char = true;
		 		state = NUMBER_S;
		 		addCharacterS (string, sign);
		 	}
		 	else if (isspace(sign))
		 	{
		 		state = BEGIN_S;
		 		global_column++;
		 	}
		 	else if (sign == '"')
		 	{
		 		state = STRING_S;
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
		addCharacterS (string, sign);
		state = ID_S;
	}
	else 
	{
		
		token->stringValue = string->data;  //ulozit string jako identifikator

		for (int i=0, i<10, i++)
		{
			if (strcmp (string->data, reserved_words[i]) == 0) //are equal - testing for reserved words
			{
				if (i == 0 || i == 1 || i == 2|| i == 3)
					type_of_token = TYPE_TO;
				else if (i == 4)
				{
					type_of_token = CIN_TO;
				}
				else if (i == 5)
				{
					type_of_token = COUT_TO;
				}
				else if (i == 6)
				{
					type_of_token = RETURN_TO;
				}
				else if (i == 7)
				{
					type_of_token = IF_TO;
				}
				else if (i == 8)
				{
					type_of_token = ELSE_TO;
				}
				else if (i == 9)
				{
					type_of_token = FOR_TO;
				}
			}
			else
			{
				type_of_token = ID_TO; //if it's not reserved word
			}

		}
	
		tokenInitialisation (type_of_token, global_row, local_column); //inicialization of token identificator
		state = BEGIN_S;
		unget(sign, inputFile);
	}

	first_char = false;

	break;	

   
 	case STRING_S:      // "" need some dynamical structure to keep whole string saved
	string_test = true;
	local_column = global_column;
	global_column++;

	if (sign == '"')     // end of string
	{
		token->stringValue = string->data;
		blank(global_row, local_column+1);
		string_test = false;
	}
	else if (sign == '\\')  /* it means just one \ */
	{
		state = ESCAPE_SEQUENCE_S;
	}
	else
	{
		addCharacterS(string, sign);
    	state = STRING_S;
    }
    break;

    case ESCAPE_SEQUENCE_S:
    global_column++;
    if (sign == '\\') /* means \\ in our source code*/
    {
    	addCharacterS(string, "\\")
    }
    else if (sign == '"')
    {
    	addCharacterS (string, "\"");
    }
    else if (sign == 'n')
    {
    	addCharacterS (string, '\n');
    }
    else if (sign == 't')
    {
    	addCharacterS (string, '\t');
    }
    else if (sign == 'x') //hexadecimal code of escape sequence
    {
    	state = HEXADECIMAL_ESC_SEQ_S;
    }
    else
    {
    	addCharacterS (string, sign)
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
    	addCharacterS(string, sign);
    	state = DOUBLE_S;
    	global_column++;
    }
    else if (isdigit(sign))
    {
    	addCharacterS(string, sign);
    	state = NUMBER_S;
    	global_column++;
    }
    else 
    {
    	int val_int = 0;
    	val_int = atoi(string->data);
    	type_of_token = INT_TO;
    	token.intValue = val_int;
    	tokenInitialisation(type_of_token, global_row, local_column)
    	unget(sign, inputFile);
    }

    first_char = false;
    break;


    case DOUBLE_S:      // x.xxx for double numbers
    if (isdigit(sign))
    {
		addCharacterS(string, sign);
    	state = DOUBLE_S;
    	global_column++;
    }
    else //anything else than number 
    {
    	double val_double = 0;
    	val_double = atof(string->data);
    	type_of_token = DOUBLE_TO;
    	token.doubleValue = val_double;
    	tokenInitialisation(type_of_token, global_row, local_column)
    	unget(sign, inputFile);
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
		alphanumerical (type_of_token, global_row, local_column);
    }
    else if (isblank(sign))
    {
    	type_of_token = NE_TO;
		blank (type_of_token, global_row, local_column);
   	}
   	else if (sign == '\n')
   	{
   		end_of_row (global_row, global_column);
   	}
   	else
   		error = true;
    
    break;

    case ROW_COMMENT_S: // coment begining with //
    if (sign == '\n')
    {
    	end_of_row (global_row, global_column); //needs next state as BEGIN_S
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
			alphanumerical (type_of_token, global_row, local_column);
    }
    else if (isblank(sign))
    {
    	type_of_token = NE_TO;
		blank (type_of_token, global_row, local_column);
    }
    else if (sign == '\n')
    {
    	end_of_row (global_row, global_column);
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
		alphanumerical (type_of_token, global_row, global_column);
    }
    else if (isblank(sign)) //if nonprintable sign
    {
    	type_of_token = ASSIGN_TO;
		blank (type_of_token, global_row, global_column);
    }
    else if (sign == '\n')
    {
    	end_of_row (global_row, global_column);
    }
    else // if other operator 
    	error = true;
    break;
    

    case EQUAL_S: // =  if it's assign
  
    if (isalnum (sign))
    {
    	type_of_token = E_TO;
		alphanumerical (type_of_token, global_row, local_column);
    }
    else if (isblank(sign))
    {
    	type_of_token = E_TO;
    	blank (type_of_token, global_row, local_column);
    }
    else if (sign == '\n')
    {
    	end_of_row (global_row, global_column);
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
    	state == OUT_S;
    	local_column = global_column;
    	global_column++;
    }
    else if (isalnum(sign))
    {
    	type_of_token = L_TO;
    	alphanumerical (type_of_token, global_row, global_column);
    }
    else if (isblank(sign))
    {	
    	type_of_token = L_TO
    	blank (type_of_token, global_row, global_column);
    }
    else if (sign == '\n')
    {
    	end_of_row (global_row, global_column);
    }
    else
    	error = true;

    break;

	case LE_S:          // <=
     if (isalnum (sign))
    {
    	type_of_token = LE_TO;
		alphanumerical (type_of_token, global_row, local_column);
    }
    else if (isblank(sign))
    {
    	type_of_token = LE_TO;
    	blank (type_of_token, global_row, local_column);
    }
    else if (sign == '\n')
    {
    	end_of_row (global_row, global_column);
    }
    else
        error = true;
    break;

    case OUT_S:         // << 
     if (isalnum (sign))
    {
    	type_of_token = OUT_TO;
		alphanumerical (type_of_token, global_row, local_column);
    }
    else if (isblank(sign))
    {
    	type_of_token = OUT_TO;
    	blank (type_of_token, global_row, local_column);
    }
    else if (sign == '\n')
    {
		end_of_row (global_row, global_column);
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
    	state == IN_S;
    	local_column = global_column;
    	global_column++;
    }
    else if (isalnum(sign))
    {
    	type_of_token = G_TO;
    	alphanumerical (type_of_token, global_row, global_column);
    }
    else if (isblank(sign))
    {
    	type_of_token = G_TO
    	blank (type_of_token, global_row, global_column);
    }
    else if (sign == '\n')
    {
    	end_of_row (global_row, global_column);
    }
    else
    	error = true;
    break;


	case IN_S:          // >>
	if (isalnum (sign))
    {
    	type_of_token = IN_TO;
		alphanumerical (type_of_token, global_row, local_column);
    }
    else if (isblank(sign))
    {
    	type_of_token = IN_TO;
    	blank (type_of_token, global_row, local_column);
    }
    else if (sign == '\n')
    {
    	end_of_row (global_row, global_column);
    }
    else
        error = true;
    break;

    
    case GE_S:          // >=
    if (isalnum (sign))
    {
    	type_of_token = GE_TO;
		alphanumerical (type_of_token, global_row, local_column);
    }
    else if (isblank(sign))
    {
    	type_of_token = GE_TO;
    	blank (type_of_token, global_row, local_column);
    }
    else if (sign == '\n')
    {
    	end_of_row (global_row, global_column);
    }
    else
        error = true;
    break;


/************************	END OF CASES	************************************/
  }



}
  else if (sign == EOF)  /*end of file   */
  		 {
		 	type_of_token = EOF_TO;
			tokenInitialisation (type_of_token, global_row, global_column);		 	 
		 }

return 0;
}
if (error == true)
	return 1;

#endif 


/*
isspace OR isblank ???

isalnum () je ekvivalentni s (isalpha() || isdigit ())

*/