/*
   SPHINX C-- toke.c source file for C--.EXE
   Property of PETER CELLIK,  Copyright (C) 1994,  All rights reserved.
   Last Updated: 14 Mar 1994

14 Mar 94 notes:
    - datatype_expected() added
    - no changes required for fixed32 stuff, except possibly adding
      doconstfixed32math
*/

#include <stdio.h>
#include <io.h>
#include <fcntl.h>	  /* O_ constant definitions */
#include <malloc.h>
#include "tok.h"

extern int error;
extern unsigned int linenumber,maxerrors;
extern unsigned char currentfilename[FILENAMESIZE];
extern char displaytokerrors,makemapfile;
extern FILE *mapfile;

extern int tok,tok2;
extern signed long number,number2;
extern unsigned char string[STRLEN],string2[STRLEN];
extern int segm,rm,post,segm2,rm2,post2;

unsigned char *input;	   /* [MAXINPUT] dynamic input buffer */
unsigned int endinptr;	   /* end index of input array */
unsigned int inptr; 	   /* index in input buffer */
unsigned char cha;	   /* pipe byte for token production */
char endoffile; 	   /* end of input file flag */

/* ================ input procedures start ================= */

int loadinputfile(inputfile)
char *inputfile;
{int filehandle;
long length;
unsigned int bufsize;
unsigned char filename[FILENAMESIZE];

filehandle = open( inputfile, O_BINARY | O_RDONLY );
if( filehandle == -1 )
	return(-2);
length = filelength(filehandle);
if( length > MAXINPUT )
    {preerror("Input file too large");
    close(filehandle);
    return(-1);
    }
bufsize = length;
input = (unsigned char *) malloc( (size_t) bufsize );
if(input == NULL)
	{preerror("Not enough memory for input buffer");
	close(filehandle);
	return(-1);
	}
if( read( filehandle, input, bufsize ) != bufsize )
	{preerror("File Read error");
	close(filehandle);
	return(-1);}
close(filehandle);

endinptr = bufsize;
endoffile = 0;
inptr = 0;

return(0);
}


nextchar()
{if( inptr < endinptr )
    cha = input[inptr++];
else cha = 26;
if(cha == 26)
    endoffile = 1;
else endoffile = 0;
}


freeinput ()
{free(input);
}


/* -------------- constant math procedures start --------------- */

unsigned long doconstdwordmath ()
{unsigned long value;
if(tok == tk_minus)
	{nexttok();
	if(tok != tk_number)
		{numexpected();
		return(0);}
	number = -number;}
else if(tok != tk_number)
	{numexpected();
	return(0);}
value = number;
while( tok2isopperand() )
	{nexttok();
	if(tok2 != tk_number)
		return(value);
	switch(tok)
		{case tk_minus:  value -= number2;	break;
		case tk_plus:	 value += number2;	break;
		case tk_xor:	 value ^= number2;	break;
		case tk_and:	 value &= number2;	break;
		case tk_or:	 value |= number2;	break;
		case tk_mod:	 value %= number2;	break;
		case tk_div:	 value /= number2;	break;
		case tk_mult:	 value *= number2;	break;
		case tk_rr:	 value >>= number2;  break;
		case tk_ll:	 value <<= number2;  break;
		case tk_xorminus:	value ^= -number2;	break;
		case tk_andminus:	value &= -number2;	break;
		case tk_orminus:	value |= -number2;	break;
		case tk_modminus:	value %= -number2;	break;
		case tk_divminus:	value /= -number2;	break;
		case tk_multminus:	value *= -number2;	break;
		case tk_rrminus:	value >>= -number2;  break;
		case tk_llminus:	value <<= -number2;  break;
		default:  beep();  break;
		}
	nexttok();
	}
nexttok();
return(value);
}


signed long doconstlongmath ()
{long value;
if(tok == tk_minus)
	{nexttok();
	if(tok != tk_number)
		{numexpected();
		return(0);}
	number = -number;}
else if(tok != tk_number)
	{numexpected();
	return(0);}
value = number;
while( tok2isopperand() )
	{nexttok();
	if(tok2 != tk_number)
		return(value);
	switch(tok)
		{case tk_minus:  value -= number2;	break;
		case tk_plus:	 value += number2;	break;
		case tk_xor:	 value ^= number2;	break;
		case tk_and:	 value &= number2;	break;
		case tk_or:	 value |= number2;	break;
		case tk_mod:	 value %= number2;	break;
		case tk_div:	 value /= number2;	break;
		case tk_mult:	 value *= number2;	break;
		case tk_rr:	 value >>= number2;  break;
		case tk_ll:	 value <<= number2;  break;
		case tk_xorminus:	value ^= -number2;	break;
		case tk_andminus:	value &= -number2;	break;
		case tk_orminus:	value |= -number2;	break;
		case tk_modminus:	value %= -number2;	break;
		case tk_divminus:	value /= -number2;	break;
		case tk_multminus:	value *= -number2;	break;
		case tk_rrminus:	value >>= -number2;  break;
		case tk_llminus:	value <<= -number2;  break;
		default:  beep();  break;
		}
	nexttok();
	}
nexttok();
return(value);
}


/******************** token storage stuff starts ***********************/

storecurrenttoken (holdtoken)
struct tokenrec *holdtoken;
{strcpy(holdtoken->tr_string,string);
holdtoken->tr_tok = tok;
holdtoken->tr_rm = rm;
holdtoken->tr_segm = segm;
holdtoken->tr_post = post;
holdtoken->tr_number = number;
}


restorecurrenttoken (holdtoken)
struct tokenrec *holdtoken;
{strcpy(string,holdtoken->tr_string);
tok = holdtoken->tr_tok;
rm = holdtoken->tr_rm;
segm = holdtoken->tr_segm;
post = holdtoken->tr_post;
number = holdtoken->tr_number;
}


/* ================= simple syntax procedures start =================== */

void nextseminext ()
{nexttok();
if(tok != tk_semicolon)
    expected(';');
nexttok();
}


void seminext ()
{if(tok != tk_semicolon)
    expected(';');
nexttok();
}


beep () 			 /* beep for any internal errors */
{printf("%c",7);
}


int expecting (want)
/* compares current token with want token.	If different, issues error
   message and returns 1, else advances to next token and returns 0 */
int want;
{if( want != tok )
    {switch(want)
	{case tk_closebracket:	expected(')');	break;
	case tk_openbracket:	expected('(');	break;
	case tk_semicolon:	expected(';');	break;
	case tk_colon:		expected(':');	break;
	case tk_openblock:	expected('[');	break;
	case tk_closeblock:	expected(']');	break;
	case tk_openbrace:	expected('{');	break;
	case tk_closebrace:	expected('}');	break;
	case tk_camma:		expected(',');	break;
	default:  preerror("expecting a different token");  break;
	}
    return(1);
    }
nexttok();
return(0);
}

/* ================== error messages start =========================== */

preerror (str)	/* error on currentline with line number and file name */
unsigned char *str;
{if( error < maxerrors )
    {error++;
    printf("%s(%d)#%d> %s.\n",currentfilename,linenumber,error,str);
    if( makemapfile )
	fprintf(mapfile,"ERROR %d line # %d>> %s.\n",error,linenumber,str);
    }
else toomanyerrors();
}


preerror2 (str)    /* error without a line number or file name */
unsigned char *str;
{if(error<maxerrors)
    {error++;
    printf("ERROR %d>> %s.\n",error,str);
    if( makemapfile )
	fprintf(mapfile,"ERROR %d>> %s.\n",str);
    }
else toomanyerrors();
}


preerror3 (str,line)   // error message at a different than current line
unsigned char *str;
unsigned int line;
{if( error < maxerrors )
    {error++;
    printf("%s(%d)#%d> %s.\n",currentfilename,line,error,str);
    if( makemapfile )
	fprintf(mapfile,"ERROR %d line # %d>> %s.\n",error,line,str);
    }
else toomanyerrors();
}


internalerror (str)	  // serious internal compiler error message
unsigned char *str;
{error++;
printf("%s(%d)#%d> *** SERIOUS COMPILER INTERNAL ERROR ***\n>%s.\n",
		 currentfilename,linenumber,error,str);
printf("STRING:%s\n",string);
printf("TOK:%d SEGM:%d POST:%d RM:%d number:%ld\n",
	 tok,	segm,	 post,	  rm,	 number);
printf("STRING2:%s\n",string2);
printf("TOK2:%d SEGM2:%d POST2:%d RM2:%d number2:%ld\n",
	 tok2,	 segm2,   post2,   rm2,  number2);
printf("Oh no.\n");
if( makemapfile )
    fclose( mapfile );
exit( e_internalerror );
}


toomanyerrors ()
{if( makemapfile )
    fclose( mapfile );
exit( e_toomanyerrors );
}


outofmemory ()
{preerror("Compiler out of memory");
if( makemapfile )
    fclose( mapfile );
exit( e_outofmemory );
}

outofmemory2 ()
{printf("ERROR> Not enough memory for the compiler's buffers.\n");
exit( e_outofmemory );
}

blockerror ()
{preerror("illegal syntax within [ ]");
}

blockbxbperror ()
{preerror("only one of BX or BP allowed within [ ]");
}

blockdisierror ()
{preerror("only one of DI or SI allowed within [ ]");
}

undefinedid ()
{unsigned char holdstr[80];
sprintf(holdstr,"%s undefined",string);
preerror(holdstr);
}

expected (ch)
char ch;
{unsigned char holdstr[80];
sprintf(holdstr,"'%c' expected",ch);
preerror(holdstr);
}

numexpected ()
{preerror("'number' expected");
}

varexpected ()
{preerror("'variable' expected");
}

stringexpected ()
{preerror("'string' expected");
}

regexpected ()
{preerror("'word register' expected");
}

reg32expected ()
{preerror("'32 bit register' expected");
}

clornumberexpected ()
{preerror("'CL or constant' expected");
}

begexpected ()
{preerror("'byte register' expected");
}

valueexpected ()
{preerror("'value' expected");
}

idexpected ()
{preerror("undefined 'identifier' expected");
}

directiveexpected ()
{preerror("compiler directive expected");
}

wordvalexpected ()
{preerror("word value expected");
}

dwordvalexpected ()
{preerror("32 bit word value expected");
}

axalexpected ()
{preerror("EAX, AX or AL expected");
}

bytedxexpected ()
{preerror("Byte constant or DX expected");
}

codeexpected ()
{preerror("Assembly opcode expected");
}

bytevalexpected ()
{preerror("byte value expected");
}

operatorexpected ()
{preerror("operator identifier expected");
}

unexpectedeof ()
{preerror("unexpected END OF FILE");
}

swaperror ()
{preerror("invalid or incompatable swap item");
}

segoperror ()
{preerror("only '=' opperand valid with segment register");
}

notyet ()
{preerror("specified syntax not handled in this version of C--");
}

idoverflow ()
{preerror("compiler identifier overflow");
}

idalreadydefined ()
{unsigned char holdstr[80];
sprintf(holdstr,"identifier %s already defined",string);
preerror(holdstr);
nexttok();
}

elseerror ()
{preerror("else without preceeding if or IF");
nexttok();
}

ELSEerror ()
{preerror("ELSE without preceeding IF or if");
nexttok();
}

IFjumperror (line)
unsigned int line;
{preerror3("IF jump distance too large, use if",line);
}

ELSEjumperror (line)
unsigned int line;
{preerror3("ELSE jump distance too large, use else",line);
}

unuseableinput ()
{preerror("unuseable input");
nexttok();
}

notwithinblock ()
{preerror("cannot declare variables within procedure { } block");
nexttok();
}

regnameerror ()
{preerror("register name cannot be used as an identifier");
nexttok();
}

segbyteerror ()
{preerror("Segment registers can not be used in byte or char math");
}

maxwordpostserror ()
{preerror("maximum number of word post location references exceeded");
}

regcallerror ()
{preerror("procedure calls with non-EAX/AX/AL math is illegal");
}

begcallerror ()
{preerror("procedure calls with non-AL register math is illegal");
}

regmathoperror ()
{preerror("invalid operation for non-AX register math");
}

begmathoperror ()
{preerror("invalid operation for non-AL register math");
}

negregerror ()
{preerror("negative non-constant invalid for non-AX register math");
}

regbyteerror ()
{preerror("byte or char operands invalid for non-AX register math");
}

reg32regerror ()
{preerror("word or int operands invalid for non-EAX register math");
}

begworderror ()
{preerror("specified 16 bit operand invalid for non-AL register math");
}

negcomperror ()
{preerror("only negative of constants valid within compairsons");
}

regshifterror ()
{preerror("only CL or 1 valid for non AX or AL register bit shifting");
}

unknowncompop ()
{preerror("unknown comparison operator");
}

blockconst (str)
char *str;
{unsigned char holdstr[80];
if(displaytokerrors)
    {sprintf(holdstr,"'%s' valid only for initial constant expression in []",str);
    preerror(holdstr);
    }
}

doubleposterror ()
{preerror("Only ONE post address valid for each variable reference");
}

maxoutputerror ()
{preerror("Maximum output code size exceeded");
exit( e_outputtoobig );
}

maxdataerror ()
{preerror("Maximum output data size exceeded");
exit( e_outputtoobig );
}

expectederror ( str )
char *str;
{unsigned char holdstr[80];
if(displaytokerrors)
    {sprintf(holdstr,"%s expected",str);
    preerror(holdstr);
    }
}

regmatherror ()
{preerror("Invalid operand for non-AX register math");
}

errorreadingfile ()
{preerror("Error reading file");
}

unableopenfile ()
{unsigned char holdstr[80];
sprintf(holdstr,"unable to open file '%s'",string);
preerror(holdstr);
}

thisundefined (str)
unsigned char *str;
{unsigned char holdstr[80];
sprintf(holdstr,"%s undefined",str);
preerror(holdstr);
}

unabletoopen (str)
unsigned char *str;
{unsigned char holdstr[80];
sprintf(holdstr,"unable to open inputfile '%s'",str);
preerror(holdstr);
}

shortjumperror ()
{preerror("Invalid operand for SHORT jump");
}

nearjumperror ()
{preerror("Invalid operand for NEAR jump");
}

invalidfarjumpitem ()
{preerror("Invalid operand for FAR jump");
}

invalidfarcallitem ()
{preerror("Invalid operand for FAR call");
}

shortjumptoolarge ()
{preerror("SHORT jump distance too large");
}

invalidoperand ()
{preerror("Invalid operand");
}

undefinederror ()
{unsigned char holdstr[80];
sprintf(holdstr,"'%s' undefined",string);
preerror(holdstr);
}

unknownmacro ()
{unsigned char holdstr[80];
sprintf(holdstr,"unknown macro '%s'",string);
preerror(holdstr);
}

localunresolved (str)
char *str;
{unsigned char holdstr[80];
sprintf(holdstr,"local jump label '%s' unresolved",str);
preerror(holdstr);
}

whileerror ()
{preerror("Only 'do { } while' loops available in C--, NOT 'while { }' loops");
}

badreturntype ()
{preerror("Return type already assumed as more significant");
}

dynamiclabelerror ()
{preerror("Global labels illegal within dynamic procedures");
}

datatype_expected ()
{preerror("byte, word, int, char, dword, long, fixed32u or fixed32s expected");
}

/* end of TOKE.C */
