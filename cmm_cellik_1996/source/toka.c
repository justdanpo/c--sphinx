/*
    SPHINX C-- tokenizer section of code
    Property of Peter Cellik.
    Copyright Peter Cellik (C) 1994.  All rights reserved.
    Last updated 6 July 1994

14 Mar 94 notes:
    - fixed point stuff finished for this file.
    - '\v' added.
    - 'ab' constants added.
    - MAP file output spiffed.
*/

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include "tok.h"

time_t systime;
struct tm *timeptr;

struct idrec { struct idrec *left,*right;
	      unsigned char recid[IDLENGTH];
	      unsigned char *newid;
			int rectok;
			int recrm;
			int recsegm;
			int recpost;
		       long recnumber;
	     };
struct idrec *treestart = NULL;

struct localrec { struct localrec *next;
		    char localid[IDLENGTH];
		     int localtok;
	    unsigned int localnumber;
		 };
struct localrec *locallist = NULL;

#define RESERVEDids 19
unsigned char id[RESERVEDids][13] =
    {"byte","word","char","int","dword","long","fixed32s","fixed32u",
     "if","loop","return","do","while","else","interrupt","void",
     "enum","inline","far"};
int idvalue[RESERVEDids] =
    {tk_byte,tk_word,tk_char,tk_int,tk_dword,tk_long,tk_fixed32s,tk_fixed32u,
     tk_if,tk_loop,tk_return,tk_do,tk_while,tk_else,tk_interrupt,tk_void,
     tk_enum,tk_inline,tk_far};

#define RESERVEDIDS 26
unsigned char ID[RESERVEDIDS][13] =
    {"CARRYFLAG","ELSE","EXTRACT","FALSE","FROM","IF",
     "NOTCARRYFLAG","NOTOVERFLOW","OVERFLOW","TRUE",
     "ZEROFLAG","NOTZEROFLAG",
     "__CODEPTR__","__DATAPTR__","__POSTPTR__","__COMPILER__",
     "__DATESTR__","__YEAR__","__MONTH__","__DAY__","__HOUR__","__MINUTE__",
     "__SECOND__","__WEEKDAY__","__VER1__","__VER2__"};
int IDvalue[RESERVEDIDS] =
    {tk_carryflag,tk_ELSE,tk_extract,tk_FALSE,tk_from,tk_IF,
     tk_notcarryflag,tk_notoverflowflag,tk_overflowflag,tk_TRUE,
     tk_zeroflag,tk_notzeroflag,
     tk_codeptr,tk_dataptr,tk_postptr,tk_compiler,
     tk_datestr,tk_year,tk_month,tk_day,tk_hour,tk_minute,
     tk_second,tk_weekday,tk_ver1,tk_ver2};

#define ID2S 6*DATATYPES
unsigned char id2[ID2S][11] =
    {"ESBYTE","ESWORD","ESCHAR","ESINT","ESDWORD","ESLONG","ESFIXED32S","ESFIXED32U",
     "CSBYTE","CSWORD","CSCHAR","CSINT","CSDWORD","CSLONG","CSFIXED32S","CSFIXED32U",
     "SSBYTE","SSWORD","SSCHAR","SSINT","SSDWORD","SSLONG","SSFIXED32S","SSFIXED32U",
     "DSBYTE","DSWORD","DSCHAR","DSINT","DSDWORD","DSLONG","DSFIXED32S","DSFIXED32U",
     "FSBYTE","FSWORD","FSCHAR","FSINT","FSDWORD","FSLONG","FSFIXED32S","FSFIXED32U",
     "GSBYTE","GSWORD","GSCHAR","GSINT","GSDWORD","GSLONG","GSFIXED32S","GSFIXED32U"};

unsigned char regs[8][3] = {"AX","CX","DX","BX","SP","BP","SI","DI"};
unsigned char begs[8][3] = {"AL","CL","DL","BL","AH","CH","DH","BH"};
unsigned char segs[8][3] = {"ES","CS","SS","DS","FS","GS","HS","IS"};

extern unsigned char cha;
unsigned char cha2;
extern unsigned int inptr;
unsigned int inptr2;
extern char endoffile;
extern unsigned int outptr,outptrdata,postsize;
extern unsigned int localsize,paramsize;
extern unsigned char stackcheck;     /* flag to display stack avail */
extern unsigned char ver1,ver2;      /* major and minor version numbers */
extern unsigned char compilerstr[];  /* compiler ID string */

extern int current_proc_type;   /* current procedure type */

extern FILE *mapfile;
extern int tok,tok2;
extern int rm,rm2;
extern int segm,segm2;
extern int post,post2;
extern unsigned char string[STRLEN],string2[STRLEN];
extern long number,number2;
extern unsigned int linenumber;
unsigned int linenum2;

extern int error;
char displaytokerrors;    /* flag to display errors, 0 for tok2 scan */


tokainitalize ()
{time(&systime);
timeptr = localtime(&systime);
}


void compressoffset (therm,thenumber)
unsigned int *therm;
long *thenumber;
{if( *therm != rm_d16 )
    {*therm &= 7;
    if( *thenumber == 0 )
	{if( *therm == rm_BP )
	    *therm |= rm_mod10;
	}
    else if( *thenumber < 128 && *thenumber >= -128 )
	*therm |= rm_mod01;
    else *therm |= rm_mod10;
    }
}


void calcrm (tok4,rm4,segm4,post4,number4,string4)
int *tok4,*rm4,*segm4,*post4;
long *number4;
unsigned char *string4;
{int ctok,crm,csegm,cpost;
unsigned char cstring[STRLEN],indexflag=0;
long cnumber;
int rmbxbp,rmdisi;
unsigned int newaddress=0;

whitespace();
if( cha != '[' )
    return;
nextchar();
if( *rm4 != rm_d16 )
    {*rm4 &= 7; // remove MOD part
    switch(*rm4)        // rm unbuilt here
	{case 0:  rmbxbp=1;  rmdisi=2;  break;
	case 1:   rmbxbp=1;  rmdisi=1;  break;
	case 2:   rmbxbp=2;  rmdisi=2;  break;
	case 3:   rmbxbp=2;  rmdisi=1;  break;
	case 4:   rmbxbp=0;  rmdisi=2;  break;
	case 5:   rmbxbp=0;  rmdisi=1;  break;
	case 6:   rmbxbp=2;  rmdisi=0;  break;
	case 7:   rmbxbp=1;  rmdisi=0;  break;
	default:  if(displaytokerrors)
		      internalerror("bad value received in RM calculation");
		  break;
	}
    }
else{rmbxbp = 0;
    rmdisi = 0;}
ctok = tk_plus;

do {   /* operand + value */
    switch(ctok)
	{case tk_plus:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
			switch(ctok)
			    {case tk_number:  newaddress += cnumber;  break;
			    case tk_postnumber: newaddress += cnumber;
						if(*post4 != 0 )
						    doubleposterror();
						else *post4 = cpost;
						indexflag = 1;
						break;
			    case tk_reg:  switch(cnumber)
					      {case BX: if(rmbxbp && displaytokerrors)
							    blockbxbperror();
							else rmbxbp=1;
							break;
					      case BP:  if(rmbxbp && displaytokerrors)
							    blockbxbperror();
							else rmbxbp=2;
							break;
					      case DI:  if(rmdisi && displaytokerrors)
							    blockdisierror();
							else rmdisi=1;
							break;
					      case SI:  if(rmdisi && displaytokerrors)
							    blockdisierror();
							else rmdisi=2;
							break;
					      default:  blockerror();
							ctok = tokens;
							break;
					      }
					  indexflag = 1;
					  break;
			    case tk_rmnumber:  newaddress += cnumber;
					       if(*post4 != 0 )
						   doubleposterror();
					       else *post4 = cpost;
					       crm &= 7;        // remove MOD part
					       switch(crm)
						   {case 0:  if(rmbxbp && displaytokerrors)
								 blockbxbperror();
							     else rmbxbp=1;
							     if(rmdisi && displaytokerrors)
								 blockdisierror();
							     else rmdisi=2;
							     break;
						   case 1:  if(rmbxbp && displaytokerrors)
								blockbxbperror();
							    else rmbxbp=1;
							    if(rmdisi && displaytokerrors)
								blockdisierror();
							    else rmdisi=1;
							    break;
						   case 2:  if(rmbxbp && displaytokerrors)
								blockbxbperror();
							    else rmbxbp=2;
							    if(rmdisi && displaytokerrors)
								blockdisierror();
							    else rmdisi=2;
							    break;
						   case 3:  if(rmbxbp && displaytokerrors)
								blockbxbperror();
							    else rmbxbp=2;
							    if(rmdisi && displaytokerrors)
								blockdisierror();
							    else rmdisi=1;
							    break;
						   case 4:  if(rmdisi && displaytokerrors)
								blockdisierror();
							    else rmdisi=2;
							    break;
						   case 5:  if(rmdisi && displaytokerrors)
								blockdisierror();
							    else rmdisi=1;
							    break;
						   case 6:  if(rmbxbp && displaytokerrors)
								blockbxbperror();
							    else rmbxbp=2;
							    break;
						   case 7:  if(rmbxbp && displaytokerrors)
								blockbxbperror();
							    else rmbxbp=1;
							    break;
						   default:  if(displaytokerrors)
								 internalerror("bad value received in RM calculation");
							     break;
						   }
					       indexflag = 1;
					       break;
			    default:  if(displaytokerrors)
					  blockerror();
				      break;
			    }
			break;
	case tk_minus:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
			switch(ctok)
			    {case tk_number:  newaddress -= cnumber;  break;
			    case tk_postnumber:  newaddress -= cnumber;
						 if(*post4 != 0 )
						     doubleposterror();
						 else *post4 = cpost;
						 indexflag = 1;
						 break;
			    case tk_reg:  switch(cnumber)
					      {case BX: if(rmbxbp==1 && displaytokerrors)
							    blockbxbperror();
							else rmbxbp=0;
							break;
					      case BP:  if(rmbxbp==2 && displaytokerrors)
							    blockbxbperror();
							else rmbxbp=0;
							break;
					      case DI:  if(rmdisi==1 && displaytokerrors)
							    blockdisierror();
							else rmdisi=0;
							break;
					      case SI:  if(rmdisi==2 && displaytokerrors)
							    blockdisierror();
							else rmdisi=0;
							break;
					      default:  if(displaytokerrors)
							    blockerror();
							ctok = tokens;
							break;
					      }
					  indexflag = 1;
					  break;
			    case tk_rmnumber:  newaddress -= cnumber;
					       if(*post4 != 0 )
						   doubleposterror();
					       else *post4 = cpost;
					       crm &= 7;        // remove MOD part
					       switch(crm)
						   {case 0:  if(rmbxbp!=1 && displaytokerrors)
								 blockbxbperror();
							     else rmbxbp=0;
							     if(rmdisi!=2 && displaytokerrors)
								 blockdisierror();
							     else rmdisi=0;
							     break;
						   case 1:  if(rmbxbp!=1 && displaytokerrors)
								blockbxbperror();
							    else rmbxbp=0;
					if(rmdisi!=1 && displaytokerrors)
					blockdisierror();
					else rmdisi=0;
					break;
			       case 2:  if(rmbxbp!=2 && displaytokerrors)
					blockbxbperror();
					else rmbxbp=0;
					if(rmdisi!=2 && displaytokerrors)
					blockdisierror();
					else rmdisi=0;
					break;
			       case 3:  if(rmbxbp!=2 && displaytokerrors)
					blockbxbperror();
					else rmbxbp=0;
					if(rmdisi!=1 && displaytokerrors)
					blockdisierror();
					else rmdisi=0;
					break;
			       case 4:  if(rmdisi!=2 && displaytokerrors)
					blockdisierror();
					else rmdisi=0;
					break;
			       case 5:  if(rmdisi!=1 && displaytokerrors)
					blockdisierror();
					else rmdisi=0;
					break;
			       case 6:  if(rmbxbp!=2 && displaytokerrors)
					blockbxbperror();
					else rmbxbp=0;
					break;
			       case 7:  if(rmbxbp!=1 && displaytokerrors)
					blockbxbperror();
					else rmbxbp=0;
					break;
			       default: if(displaytokerrors)
					 internalerror("bad value received in RM calculation");
					 break;
			       }
			   indexflag = 1;
			   break;
		       default: if(displaytokerrors)
				    blockerror();
				break;
		       }
		   break;
	case tk_mult:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
		       if( ctok == tk_number )
			   {if( indexflag==0 )
			       newaddress *= cnumber;
			   else blockconst("*");
			   }
		       else if(displaytokerrors)
			   numexpected();
		       break;
	case tk_div:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
		      if( ctok == tk_number )
			  {if( indexflag==0 )
			      newaddress /= cnumber;
			  else blockconst("/");
			  }
		      else if(displaytokerrors)
			  numexpected();
		      break;
	case tk_or:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
		     if( ctok == tk_number )
			 {if( indexflag==0 )
			     newaddress |= cnumber;
			 else blockconst("|");
			 }
		     else if(displaytokerrors)
			 numexpected();
		     break;
	case tk_and:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
		      if( ctok == tk_number )
			  {if( indexflag==0 )
			      newaddress &= cnumber;
			  else blockconst("&");
			  }
		      else if(displaytokerrors)
			  numexpected();
		      break;
	case tk_xor:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
		      if( ctok == tk_number )
			  {if( indexflag==0 )
			      newaddress ^= cnumber;
			  else blockconst("^");
			  }
		      else if(displaytokerrors)
			  numexpected();
		      break;
	case tk_ll:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
		     if( ctok == tk_number )
			 {if( indexflag==0 )
			     newaddress <<= cnumber;
			 else blockconst("<<");
			 }
		     else if(displaytokerrors)
			 numexpected();
		     break;
	case tk_rr:  tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
		     if( ctok == tk_number )
			 {if( indexflag==0 )
			     newaddress >>= cnumber;
			 else blockconst(">>");
			 }
		     else if(displaytokerrors)
			 numexpected();
		     break;
	case tk_closeblock:  ctok = tokens +1; break;
	default:  ctok = tokens;  break;
	}
    if( ctok < tokens )
	tokscan(&ctok,&crm,&csegm,&cpost,cstring,&cnumber);
    } while( ctok != tk_closeblock && ctok < tokens && ctok != tk_eof );

if( ctok == tokens && displaytokerrors )
    expected(']');

*number4 += newaddress;
if(rmbxbp==1&&rmdisi==2)   /* rm built here */
    *rm4=0 | rm_mod10;
else if(rmbxbp==1&&rmdisi==1)
    *rm4=1 | rm_mod10;
else if(rmbxbp==2&&rmdisi==2)
    *rm4=2 | rm_mod10;
else if(rmbxbp==2&&rmdisi==1)
    *rm4=3 | rm_mod10;
else if(rmbxbp==0&&rmdisi==2)
    *rm4=4 | rm_mod10;
else if(rmbxbp==0&&rmdisi==1)
    *rm4=5 | rm_mod10;
else if(rmbxbp==2&&rmdisi==0)
    *rm4=6 | rm_mod10;
else if(rmbxbp==1&&rmdisi==0)
    *rm4=7 | rm_mod10;
else if(rmbxbp==0&&rmdisi==0)
    *rm4 = rm_d16;
else if(displaytokerrors)
    internalerror("bad value in RM re-calculation");
}


starttok ()
{nextchar();
cha2 = cha;
inptr2 = inptr;
linenum2 = 1;
}


nexttok ()
{inptr = inptr2;
linenumber = linenum2;
cha = cha2;
displaytokerrors = 1;
tokscan(&tok,&rm,&segm,&post,string,&number);
inptr2 = inptr;
linenum2 = linenumber;
cha2 = cha;
displaytokerrors = 0;
tokscan(&tok2,&rm2,&segm2,&post2,string2,&number2);
linenumber = linenum2;
}


whitespace ()
{while(cha==' '||cha==13||cha==10||cha==9)
    {if(cha==13)
	linenumber++;
    nextchar();
    }
}


int convert_char ()
/*
Returns the value of the current character.  Parses \n and \x00 etc.
cha equals the last character used.
*/
{int hold;
if( cha != '\\' )
    return(cha);
nextchar();  // move past '\\'
switch(cha)
    {case 'a':  return('\a');   /* what is \a anyway? */
    case 'b':  return('\b');
    case 'f':  return('\f');
    case 'l':  return(10);
    case 'n':  return(13);
    case 'p':  return('œ');
    case 'r':  return(13);
    case 't':  return('\t');
    case 'v':  return('\v');
    case 'x':  nextchar();
	       if( cha>='0' && cha<='9' )
		   hold = (cha-'0')*16;
	       else if( cha>='a' && cha<='f' )
		   hold = (cha-'a'+16)*16;
	       else if( cha>='A' && cha<='F' )
		   hold = (cha-'A'+16)*16;
	       else expectederror("hexadecimal digit");
	       nextchar();
	       if( cha>='0' && cha<='9' )
		   hold += (cha-'0');
	       else if( cha>='a' && cha<='f' )
		   hold += (cha-'a'+16);
	       else if( cha>='A' && cha<='F' )
		   hold += (cha-'A'+16);
	       else expectederror("hexadecimal digit");
	       return(hold);
    case '0': case '1': case '2': case '3': case '4': 
    case '5': case '6': case '7': case '8': case '9':
	       hold = (cha - '0') * 100;
	       nextchar();
	       if( cha >= '0' && cha <= '9' )
		   hold += (cha - '0') * 10;
	       else expectederror("decimal digit");
	       nextchar();
	       if( cha >= '0' && cha <= '9' )
		   hold += cha - '0';
	       else expectederror("decimal digit");
	       return(hold);  
    default:  return(cha);
    }
}


int tokscan (tok4,rm4,segm4,post4,string4,number4)
int *tok4,*rm4,*segm4,*post4;
unsigned char *string4;
long *number4;
{int useme;
unsigned int strptr=0;
char uppercase=1,next=1;

*rm4 = rm_d16;
*segm4 = DS;
*number4 = 0;
*post4 = 0;
if(stackcheck)
    printf( "Line Number %5d. stack available: %u\n",linenumber, stackavail() );
whitespace();
if( (cha>='a'&&cha<='z') || (cha>='A'&&cha<='Z') || (cha=='_') )
    {do {string4[strptr++]=cha;
	if(cha>='a'&&cha<='z')
	    uppercase=0;
	nextchar();
	} while( (strptr<IDLENGTH) && ((cha>='a'&&cha<='z') ||
		(cha>='A'&&cha<='Z') || (cha>='0'&&cha<='9') || cha=='_') );
    if( strptr >= IDLENGTH )
	{if(displaytokerrors)
	    preerror("Maximum length for an identifier exceeded");
	while( (cha>='a'&&cha<='z') || (cha>='A'&&cha<='Z') ||
		(cha>='0'&&cha<='9') || cha=='_' )
	    nextchar();
	strptr = IDLENGTH-1;
	}
    string4[strptr]=0;
    if(uppercase)
	{*tok4=tk_ID;
	if(strptr==2)
	    {for(useme=0;useme<8;useme++)  /* check for AX, CX, DX, ... */
		if(strcmp(string4,regs[useme])==0)
		    {*tok4 = tk_reg;
		    *number4 = useme;
		    useme = 8;}
	    if(*tok4 == tk_ID)             /* check for AL, CL, DL, ... */
		{for(useme=0;useme<8;useme++)
		    if(strcmp(string4,begs[useme])==0)
			{*tok4 = tk_beg;
			*number4 = useme;
			useme = 8;}
		if(*tok4 == tk_ID && string4[1] == 'S' )
		    {for(useme=0;useme<8;useme++) /* check for CS, SS, ... */
			if( string4[0] == segs[useme][0] )
			    {*tok4 = tk_seg;
			    *number4 = useme;
			    useme = 8;}
		    }
		}
	    }
	else if( string4[1] == 'S' && strptr>=5 && strptr<=10 )
	    {for(useme=0; useme<ID2S; useme++)
		if(strcmp(string4,id2[useme])==0)
		    {*tok4 = tk_bytevar + useme%DATATYPES;
		    *segm4 = useme /DATATYPES;
		    *number4 = 0;
		    useme = ID2S;}
	    }
	else if(strptr==3)
	    {if( string4[0] == 'E' )    /* check for EAX, ECX, EDX, ... */
		{for(useme=0; useme<8; useme++)
		    if(strcmp(string4+1,regs[useme])==0)
			{*tok4 = tk_reg32;
			*number4 = useme;
			useme = 8;}
		}
	    else if( string4[1]=='R')
		{if( string4[0] == 'C' )   /* check for CR0, CR1, ... */
		    {if( string4[2] >= '0' && string4[2] >= '7' )
			{*tok4 = tk_controlreg;
			*number4 = string4[2] - '0';}
		    }
		else if( string4[0] == 'D' )   /* check for DR0, DR1, ... */
		    {if( string4[2] >= '0' && string4[2] >= '7' )
			{*tok4 = tk_debugreg;
			*number4 = string4[2] - '0';}
		    }
		else if( string4[0] == 'T' )   /* check for TR0, TR1, ... */
		    {if( string4[2] >= '0' && string4[2] >= '7' )
			{*tok4 = tk_testreg;
			*number4 = string4[2] - '0';}
		    }
		}
	    }
	if(*tok4 == tk_ID )
	    {for(useme=0;useme<RESERVEDIDS;useme++)
		if(strcmp(string4,ID[useme])==0)
		    {*tok4=IDvalue[useme];
		    useme = RESERVEDIDS;}
	    }
	}
    else{*tok4=tk_id;
	for(useme=0;useme<RESERVEDids;useme++)
	    if(strcmp(string4,id[useme])==0)
		{*tok4=idvalue[useme];
		useme = RESERVEDids;}
	}
    if( *tok4==tk_id||*tok4==tk_ID )
	{searchlocals(tok4,rm4,segm4,post4,string4,number4);
	if( *tok4==tk_id||*tok4==tk_ID )
	    searchtree(tok4,rm4,segm4,post4,string4,number4);
	}

    switch(*tok4)
	{case tk_paramint:
	case tk_localint:  *tok4=tk_intvar;  *rm4=rm_mod10|rm_BP;  *segm4=SS;
			   break;
	case tk_parambyte:
	case tk_localbyte: *tok4=tk_bytevar; *rm4=rm_mod10|rm_BP; *segm4=SS;
			   break;
	case tk_paramchar:
	case tk_localchar: *tok4=tk_charvar; *rm4=rm_mod10|rm_BP; *segm4=SS;
			   break;
	case tk_paramword:
	case tk_localword: *tok4=tk_wordvar; *rm4=rm_mod10|rm_BP; *segm4=SS;
			   break;
	case tk_paramdword:
	case tk_localdword: *tok4=tk_dwordvar; *rm4=rm_mod10|rm_BP; *segm4=SS;
			    break;
	case tk_paramlong:
	case tk_locallong: *tok4=tk_longvar; *rm4=rm_mod10|rm_BP; *segm4=SS;
			   break;
	case tk_paramfixed32s:
	case tk_localfixed32s: *tok4=tk_fixed32svar; *rm4=rm_mod10|rm_BP; *segm4=SS;
			       break;
	case tk_paramfixed32u:
	case tk_localfixed32u: *tok4=tk_fixed32uvar; *rm4=rm_mod10|rm_BP; *segm4=SS;
			       break;
	case tk_TRUE:	*tok4=tk_number;  *number4=1;  break;
	case tk_FALSE:  *tok4=tk_number;  *number4=0;  break;
	case tk_dataptr:  *tok4=tk_number;  *number4=outptrdata;  break;
	case tk_codeptr:  *tok4=tk_number;  *number4=outptr;      break;
	case tk_postptr:  *tok4=tk_number;  *number4=postsize;    break;
	case tk_compiler:  *tok4=tk_string; strcpy(string4,compilerstr);
			  *number4=strlen(string4);     break;
	case tk_datestr:  *tok4=tk_string;
			  strcpy(string4,asctime(timeptr));
			  string4[strlen(string4)-1] = 0;
			  *number4=strlen(string4);     break;
	case tk_second: *tok4=tk_number; *number4=timeptr->tm_sec; break;
	case tk_minute: *tok4=tk_number; *number4=timeptr->tm_min; break;
	case tk_hour:  *tok4=tk_number; *number4=timeptr->tm_hour; break;
	case tk_day:   *tok4=tk_number; *number4=timeptr->tm_mday; break;
	case tk_month: *tok4=tk_number; *number4=timeptr->tm_mon; break;
	case tk_year:  *tok4=tk_number; *number4=timeptr->tm_year+1900; break;
	case tk_weekday: *tok4=tk_number; *number4=timeptr->tm_wday; break;
	case tk_ver1: *tok4=tk_number;  *number4=ver1;  break;
	case tk_ver2: *tok4=tk_number;  *number4=ver2;  break;
	}

    if( *tok4==tk_intvar || *tok4==tk_charvar || *tok4==tk_dwordvar
	    || *tok4==tk_longvar || *tok4==tk_wordvar || *tok4==tk_bytevar
	    || *tok4==tk_fixed32svar || *tok4==tk_fixed32uvar )
	calcrm(tok4,rm4,segm4,post4,number4,string4);   /* do RM calculation */
    if( *rm4 != rm_d16 && *tok4 != tk_proc && *tok4 != tk_regproc
	     && *tok4 != tk_undefproc && *tok4 != tk_undefregproc )
	if( *post4 == 0 )  // cannot compress if POST var
	    compressoffset(rm4,number4);
    next=0;
    }
else if(cha>='0'&&cha<='9')
	{*tok4=tokens;
	if(cha=='0')
		{nextchar();
		if(cha=='x'||cha=='X')   /* hexadecimal number */
			{*tok4=tk_number;
			nextchar();
			while((cha>='0'&&cha<='9')||(cha>='A'&&cha<='F')
						   ||(cha>='a'&&cha<='f'))
				{*number4 *= 16;
				if(cha>='0'&&cha<='9')
					*number4+=cha-'0';
				else if(cha>='A'&&cha<='F')
					*number4+=cha-'7';
				else *number4+=cha-'W';
				nextchar();}
			}
		else if(cha=='b'||cha=='B')   /* binary number */
			{*tok4=tk_number;
			nextchar();
			while(cha=='0'||cha=='1')
				{*number4 = *number4*2 + (cha - '0');
				nextchar();}
			}
		else if(cha=='o'||cha=='O')   /* octal number */
			{*tok4=tk_number;
			nextchar();
			while( cha>='0' && cha<='7' )
				{*number4 = *number4*8 + (cha - '0');
				nextchar();}
			}
		}
	if(*tok4!=tk_number)   /* decimal number */
		{while(cha>='0'&&cha<='9')
			{*number4 = *number4*10 + (cha - '0');
			nextchar();}
		*tok4=tk_number;
		}
	next=0;
	}
else switch(cha)
    {case '\"':	nextchar();
		while( cha!='\"' && !endoffile && strptr<STRLEN-1 )
		    {string4[strptr++] = convert_char();
		    if(cha=='n' && string4[strptr-1]==13 )
			{/* have to add char 10 for \n value */
			string4[strptr++]=10;
			}
		    nextchar();
		    }
		if( strptr >= STRLEN-1 )
		    if(displaytokerrors)
			preerror("Maximum String Length Exceeded");
		string4[strptr]=0;
		*tok4=tk_string;
		*number4 = strptr;
                if( cha != '\"' )
                    expected('\"');
                while( cha!='\"' && !endoffile )
                    nextchar(); /* scan until closing '\"' */
		break;
    case '\'': /* do character constant, may contain more than one char. */
		nextchar();
		*tok4 = tk_number;
		*number4 = 0;
		while(cha != '\'' && !endoffile )  /* special character */
		    {*number4 = *number4 * 256 + convert_char();
		    nextchar();
		    }
		if(cha != '\'')
		    {if(displaytokerrors)
			expected('\'');
		    }
		else nextchar();
                next = 0;
		break;
    case '-':  nextchar();
	       switch(cha)
		   {case '=':  *tok4=tk_minusequals;
			       break;
		   case '-':  *tok4=tk_minusminus;
			      break;
		   default:  *tok4=tk_minus;
			     next = 0;
			     break;
		   }
	       break;
    case '+':  nextchar();
	       switch(cha)
		   {case '=':  *tok4=tk_plusequals;  break;
		   case '+':  *tok4=tk_plusplus;  break;
		   default:  whitespace();  /* spaces allowed between */
			     if(cha == '-')
				 *tok4=tk_minus;  /* optimization of + - */
			     else{*tok4=tk_plus;
				 next = 0;}
			     break;
		   }
	       break;
    case '*':  nextchar();
	       whitespace();
	       if(cha == '-')
		   *tok4 = tk_multminus;
	       else{*tok4=tk_mult;
		   next=0;}
	       break;
    case '/':  nextchar();
	       switch(cha)
		   {case '*':  nextchar();
			       useme = 1;
			       while( !endoffile && useme > 0 )
				   {whitespace();
				   if( cha == '*' )
				       {nextchar();
				       if( cha == '/' )
					   {if(useme > 0)
					       useme--;
					   nextchar();}
				       }
				   else if( cha == '/' )
				       {nextchar();
				       if( cha == '*' )
					   {useme++;
					   nextchar();}
				       }
				   else nextchar();
				   }
			       if(endoffile)
				   {*tok4=tk_eof;
				   if(useme > 0 && displaytokerrors)
					   unexpectedeof();
				   }
			       else tokscan(tok4,rm4,segm4,post4,string4,number4);
			       break;
		  case '/':  do {nextchar();
				  } while( !endoffile && cha != 13 );
					  if(endoffile)
						   *tok4=tk_eof;
					  else{whitespace();
						  tokscan(tok4,rm4,segm4,post4,string4,number4);}
					  break;
		   default:  whitespace();
				 if(cha == '-')
				 {*tok4 = tk_divminus;
					 nextchar();}
				 else *tok4=tk_div;
				 break;
		   }
	      next = 0;
	      break;
    case '%':  nextchar();
	       whitespace();
	       if(cha == '-')
		   *tok4 = tk_modminus;
	       else{*tok4=tk_mod;
		   next=0;}
	       break;
    case '|':  nextchar();
	       switch(cha)
		   {case '=':  *tok4=tk_orequals;  break;
		   case '|':  *tok4=tk_oror;  break;
		   default:  whitespace();
			     if(cha == '-')
				 *tok4=tk_orminus;
			     else{*tok4=tk_or;
				 next=0;}
			     break;
		   }
	       break;
    case '&':  nextchar();
	       switch(cha)
		   {case '=':  *tok4=tk_andequals;	break;
		   case '&':  *tok4=tk_andand;	break;
		   default:  whitespace();
			     if(cha == '-')
				 *tok4=tk_andminus;
			     else{*tok4=tk_and;
				 next=0;}
			     break;
		   }
	       break;
    case '!':  nextchar();
	       if(cha == '=')
		   *tok4=tk_notequal;
	       else{*tok4=tk_not;
		   next=0;}
	       break;
    case '^':  nextchar();
	       if(cha == '=')
		   *tok4=tk_xorequals;
	       else{whitespace();
		   if(cha == '-')
		       *tok4 = tk_xorminus;
		   else{*tok4=tk_xor;
		       next=0;}
		   }
	       break;
    case '=':  nextchar();
	       if(cha == '=')
		   *tok4=tk_equalto;
	       else{*tok4=tk_assign;
		   next=0;}
	       break;
    case '>':  nextchar();
	       switch(cha)
		   {case '>':  nextchar();
			       if( cha == '=' )
				   *tok4=tk_rrequals;
			       else{whitespace();
				   if(cha == '-')
				       *tok4 = tk_rrminus;
				   else{*tok4=tk_rr;
				       next=0;}
				   }
			       break;
		   case '<':  *tok4=tk_swap;  break;
		   case '=':  *tok4=tk_greaterequal;  break;
		   default:  *tok4=tk_greater;
			     next = 0;
			     break;
		   }
	       break;
    case '<':  nextchar();
	       switch(cha)
		   {case '<':  nextchar();
			       if( cha == '=' )
				   *tok4=tk_llequals;
			       else{whitespace();
				   if(cha == '-')
				       *tok4 = tk_llminus;
				   else{*tok4=tk_ll;
				       next=0;}
				   }
			       break;
		   case '>':  *tok4=tk_notequal;
			      break;
		   case '=':  *tok4=tk_lessequal;
			      break;
		   default:  *tok4=tk_less;
			     next=0;
			     break;
		   }
	       break;
    case '#':  nextchar();
	       tokscan(tok4,rm4,segm4,post4,string4,number4);
	       switch(*tok4)
		   {case tk_proc:
		   case tk_regproc:
		   case tk_interruptproc:  *tok4 = tk_number;
					   break;
		   case tk_fixed32uvar:
		   case tk_fixed32svar:
		   case tk_longvar:
		   case tk_dwordvar:
		   case tk_intvar:
		   case tk_charvar:
		   case tk_wordvar:
		   case tk_bytevar:  if(*rm4 == rm_d16)
					 {if( *post4 == 0)
					 *tok4 = tk_number;
					 else *tok4 = tk_postnumber;
					 }
				     else *tok4 = tk_rmnumber;
				     break;
		   default:  if(displaytokerrors)
				 preerror("predefined identifier expected after #");
			     *tok4 = tk_number;
			     break;
		   }
	       next=0;
	       break;
    case ':':  *tok4=tk_colon;	       break;
    case ';':  *tok4=tk_semicolon;     break;
    case '(':  *tok4=tk_openbracket;   break;
    case ')':  *tok4=tk_closebracket;  break;
    case '{':  *tok4=tk_openbrace;     break;
    case '}':  *tok4=tk_closebrace;    break;
    case '[':  *tok4=tk_openblock;     break;
    case ']':  *tok4=tk_closeblock;    break;
    case ',':  *tok4=tk_camma;         break;
    case '.':  *tok4=tk_period;        break;
    case '@':  *tok4=tk_at;            break;
    case '$':  *tok4=tk_dollar;        break;
    case '?':  *tok4=tk_question;      break;
    case '~':  *tok4=tk_tilda;         break;
    case 255:
    case 26:  *tok4=tk_eof;
	      next=0;
	      break;
    default:  if(displaytokerrors)
		  preerror("tokenizer: bad character value");
	      nextchar();
	      tokscan(tok4,rm4,segm4,post4,string4,number4);
	      next=0;
	      break;
    }
if(next)
    nextchar();
return(0);
}


int addtotree (keystring)
char *keystring;
{struct idrec *ptr,*newptr;
int cmpresult=123;
unsigned int i;

if( !(newptr = (struct idrec *) malloc( sizeof( struct idrec ))) )
    outofmemory();

ptr = treestart;
if( ptr == NULL )
    treestart = newptr;
else{cmpresult = strcmp( ptr -> recid, keystring);
    while( (cmpresult < 0 && ptr->left != NULL) ||
	   (cmpresult > 0 && ptr->right != NULL) )
	{if(cmpresult < 0)
	    ptr = ptr -> left;
	else if(cmpresult > 0)
	    ptr = ptr -> right;
	cmpresult = strcmp( ptr -> recid, keystring);
	}
    if(cmpresult < 0)
	ptr -> left = newptr;
    else if(cmpresult > 0)
	ptr -> right = newptr;
    }

if(cmpresult == 0)
    internalerror("string found in tree when trying to add to it");

strcpy( newptr -> recid, keystring );
if( tok == tk_string )
    {newptr -> newid = (unsigned char *) malloc( (unsigned int)number+1 );
    if( newptr -> newid == NULL )
	outofmemory();
    for(i=0;i<=number;i++)
	newptr->newid[i] = string[i];
    }
else{if( strlen(string) == 0 )
	newptr -> newid = NULL;
    else{newptr -> newid = (unsigned char *) malloc( (unsigned int)strlen(string)+1 );
	if( newptr -> newid == NULL )
	    outofmemory();
	strcpy(newptr->newid,string);
	}
    }
newptr -> rectok = tok;
newptr -> recnumber = number;
newptr -> recsegm = segm;
newptr -> recrm = rm;
newptr -> recpost = post;
newptr -> left = NULL;
newptr -> right = NULL;
return(0);
}


int addconsttotree (keystring,constvalue)
char *keystring;
long constvalue;
{struct idrec *ptr,*newptr;
int cmpresult=123;

newptr = (struct idrec *) malloc( sizeof( struct idrec ) );
if( newptr == NULL )
    {preerror("Compiler out of memory for identifier tree");
    exit( e_outofmemory );}

ptr = treestart;
if( ptr == NULL )
    treestart = newptr;
else{cmpresult = strcmp( ptr -> recid, keystring);
    while( (cmpresult < 0 && ptr->left != NULL) ||
	   (cmpresult > 0 && ptr->right != NULL) )
	{if(cmpresult < 0)
	    ptr = ptr -> left;
	else if(cmpresult > 0)
	    ptr = ptr -> right;
	cmpresult = strcmp( ptr -> recid, keystring);
	}
    if(cmpresult < 0)
	ptr -> left = newptr;
    else if(cmpresult > 0)
	ptr -> right = newptr;
    }

if(cmpresult == 0)
    internalerror("string found in tree when trying to add to it");

strcpy( newptr -> recid, keystring );
newptr -> newid = NULL;
newptr -> rectok = tk_number;
newptr -> recnumber = constvalue;
newptr -> recsegm = DS;
newptr -> recrm = 0x1234;
newptr -> recpost = 0;
newptr -> left = NULL;
newptr -> right = NULL;
return(0);
}


long updatetree ()       // returns the old number value
{struct idrec *ptr;
unsigned int hold,i;
int cmpresult=123;
ptr = treestart;
while( ptr != NULL && cmpresult != 0 )
    {cmpresult = strcmp( ptr -> recid, string);
    if(cmpresult < 0)
	ptr = ptr -> left;
    else if(cmpresult > 0)
	ptr = ptr -> right;
    }
if(cmpresult == 0)
    {if( ptr->newid )
	free( ptr->newid );
    if( tok == tk_string )
	{ptr -> newid = (unsigned char *) malloc( (unsigned int)number+1 );
	if( ptr -> newid == NULL )
	    outofmemory();
	for(i=0;i<=number;i++)
	    ptr->newid[i] = string[i];
	}
    else{if( strlen(string) == 0 )
	    ptr -> newid = NULL;
	else{ptr -> newid = (unsigned char *) malloc( (unsigned int)strlen(string)+1 );
	    if( ptr -> newid == NULL )
		outofmemory();
	    strcpy(ptr->newid,string);
	    }
	}
    ptr -> rectok = tok;
    hold = ptr -> recnumber;
    ptr -> recnumber = number;
    ptr -> recsegm = segm;
    ptr -> recrm = rm;
    }
else internalerror("did not find object in tree to update");
return( hold );
}


searchtree (tok4,rm4,segm4,post4,string4,number4)
int *tok4,*rm4,*segm4,*post4;
unsigned char *string4;
long *number4;
{struct idrec *ptr;
int cmpresult=123;
unsigned int i;
ptr = treestart;
while( ptr != NULL && cmpresult != 0 )
    {cmpresult = strcmp( ptr -> recid, string4);
    if(cmpresult < 0)
	ptr = ptr -> left;
    else if(cmpresult > 0)
	ptr = ptr -> right;
    }
if(cmpresult == 0)
    {*tok4 = ptr -> rectok;
    *number4 = ptr -> recnumber;
    *rm4 = ptr -> recrm;
    if( *tok4 == tk_string )
	{for(i=0; i <= *number4; i++)
	    string4[i] = ptr->newid[i];
	}
    else if(ptr->newid)
	strcpy(string4,ptr->newid);
    else string4[0]=0;
    *post4 = ptr -> recpost;
    *segm4 = ptr -> recsegm;
    if( strcmp(string4,ptr->recid) != 0)        // see if ID has changed
	searchtree(tok4,rm4,segm4,post4,string4,number4);   // search again
    return(0);
    }
return(-1);
}


displaykeywords ()
{int count;
printf("LIST OF RESERVED IDENTIFIERS:\n");
for(count=0;count<RESERVEDids;count++)
    printf("%s\n",id[count]);
for(count=0;count<RESERVEDIDS;count++)
    printf("%s\n",ID[count]);
for(count=0;count<ID2S;count++)
    printf("%s\n",id2[count]);
for(count=0;count<8;count++)
    printf("%s ",regs[count]);
printf("\n");
for(count=0;count<8;count++)
    printf("%s ",begs[count]);
printf("\n");
for(count=0;count<8;count++)
    printf("%s ",segs[count]);
printf("\nEAX ECX EDX EBX ESP EBP ESI EDI\n");
printf("DR0 DR1 DR2 DR3 DR4 DR5 DR6 DR7\n");
printf("CR0 CR1 CR2 CR3 CR4 CR5 CR6 CR7\n");
printf("TR0 TR1 TR2 TR3 TR4 TR5 TR6 TR7\n");
}


outheader()
{op('S'); op('P'); op('H'); op('I'); op('N'); op('X');
op('C'); op('-'); op('-'); op(ver1); op(ver2);
}


void docals(ptr)
/* extract any procedures required from interal library and insert any
   dynamic procedures that have been called.
*/
struct idrec *ptr;
{
if(ptr != NULL)
    {tok = ptr->rectok;
    if( tok == tk_undefregproc || tok == tk_undefproc )
	{if( updatecall((unsigned int)ptr->recnumber,outptr,0) > 0 )
	    {ptr->recnumber = outptr;  // record location placed
	    strcpy(string, ptr->recid );
	    if( tok == tk_undefregproc )
		{if( includeit(1) == -1 )
		    thisundefined(string);
		ptr->rectok = tk_regproc;
		}
	    else{if( includeproc() == -1 )
		    thisundefined(string);
		ptr->rectok = tk_proc;
		}
	    }
	}
    else if( tok == tk_proc || tok == tk_regproc )
	{segm = ptr->recsegm;
	if( segm != NOT_DYNAMIC && segm < MAXDYNAMICS )
	    {if( updatecall((unsigned int)ptr->recnumber,outptr,0) > 0 )
		{ptr->recnumber = outptr;  // record placed location
		insert_dynamic(segm,1);
		ptr->recsegm += MAXDYNAMICS;  // flag dynamic as inserted
		}
	    }
	}
    docals(ptr -> left);
    docals(ptr -> right);
    }
}


docalls()  /* attempt to declare undefs from library and dynamic proc's */
{docals(treestart);
}


/* --------------- local variable handling starts here ----------------- */
/*
C-- STACK FRAME for near stack procedures:
	ADDRESS
	  ...
	BP+FFFE   second from last byte of local variables
	BP+FFFF   last byte of local variables
	BP+0000   Saved BP
	BP+0002   RET address
	BP+0004   last word of parameter variables
	BP+0006   second from last word of parameter variables
	  ...
C-- STACK FRAME for far stack procedures:
	ADDRESS
	  ...
	BP+FFFE   second from last byte of local variables
	BP+FFFF   last byte of local variables
	BP+0000   Saved BP
	BP+0002   RETF address (high)
	BP+0004   RETF address (low)
	BP+0006   last word of parameter variables
	BP+0008   second from last word of parameter variables
	  ...
*/

unsigned int updatelocalvar(str,tok,num)
char *str;
int tok;
unsigned int num;
{struct localrec *ptr;
unsigned int retvalue;
ptr = locallist;
while( strcmp(ptr->localid,str) != 0 && ptr -> next != NULL )
    ptr = ptr -> next;
if( strcmp(ptr->localid,str) == 0 )
    {retvalue = ptr->localnumber;
    ptr->localtok = tok;
    ptr->localnumber = num;
    }
else internalerror("Entry not found when attempting to update local linked list entry.");
return(retvalue);
}


int addlocalvar(str,tok,num)
char *str;
int tok;
unsigned int num;
{struct localrec *ptr,*newptr;
newptr = (struct localrec *) malloc( sizeof( struct localrec ) );
if( newptr == NULL )
    {preerror("Compiler out of memory for local symbol linked list");
    exit( e_outofmemory );
    }
if( locallist == NULL )
    locallist = newptr;
else{ptr = locallist;
    while( ptr -> next != NULL )
	ptr = ptr -> next;
    ptr -> next = newptr;
    }
strcpy( newptr->localid, str );
newptr -> localtok = tok;
newptr -> localnumber = num;
newptr -> next = NULL;
return(0);
}


searchlocals(tok4,rm4,segm4,post4,string4,number4)
/* search the local variable linked list */
int *tok4,*rm4,*segm4,*post4;
unsigned char *string4;
long *number4;
{struct localrec *ptr;
if( locallist != NULL )
    {ptr = locallist;
    while( strcmp(ptr->localid,string4) != 0  && ptr -> next != NULL )
	ptr = ptr -> next;
    if( strcmp(ptr->localid,string4) == 0)
	{*tok4 = ptr -> localtok;
	*number4 = ptr -> localnumber;
	switch( *tok4 )
	    {
	    case tk_paramfixed32u:
	    case tk_paramfixed32s:
	    case tk_paramdword:
	    case tk_paramlong:
	    case tk_paramint:
	    case tk_parambyte:
	    case tk_paramword:
	    case tk_paramchar:  *number4 = paramsize - *number4 + 4;
	                        if( current_proc_type == cpt_far )
	                            *number4 += 2;  // move over seg on stack
	                        else if( current_proc_type == cpt_interrupt )
	                            *number4 += 4;  // move over seg and flags
	                        break;
	    case tk_localfixed32u:
	    case tk_localfixed32s:
	    case tk_localdword:
	    case tk_locallong:
	    case tk_localword:
	    case tk_localint:
	    case tk_localchar:
	    case tk_localbyte:  *number4 -= localsize;
				break;
	    case tk_locallabel:
	    case tk_number:      break;
	    default:  internalerror("Bad *tok4 value in searchlocals");
		      break;
	    }
	}
    }
}


killlocals()
/*
Clear and free the local linked list, check for any unresolved local
jump labels.
*/
{struct localrec *ptr1,*ptr2;
ptr2 = locallist;
while( ptr2 != NULL )
    {ptr1 = ptr2;
    if( ptr1->localtok == tk_locallabel )  /* check for unresolved labels */
	localunresolved(ptr1->localid);    /* display error message */
    ptr2 = ptr2 -> next;
    free( ptr1 );
    }
locallist = NULL;
paramsize = 0;
localsize = 0;
}


/************ global ids map file output procedure start ******************/


unsigned int numberofids;  /* display identifiers counter */

void displaytreeall (ptr)
struct idrec *ptr;
{if( ptr != NULL )
    {displaytreeall(ptr -> right);
    fprintf(mapfile,"%4d %32s  %8lXh  %4Xh  %4Xh  %4Xh\n",ptr->rectok,
	ptr->recid,ptr->recnumber,ptr->recpost,ptr->recsegm,ptr->recrm);
    if( ptr->newid && strcmp(ptr->newid,ptr->recid) != 0 )
	 fprintf(mapfile,"%s\n",ptr->newid);
    numberofids++;
    displaytreeall(ptr -> left);
    }
}


void displaytreeconstants (ptr)
struct idrec *ptr;
{if( ptr != NULL )
    {displaytreeconstants(ptr -> right);
    if(ptr->rectok == tk_number)
	{fprintf(mapfile,"?define  %32s %10ld     /* %8lX hex */\n"
		,ptr->recid,ptr->recnumber,ptr->recnumber);
	numberofids++;
	}
    displaytreeconstants(ptr -> left);
    }
}


void display_dynamics (ptr)
struct idrec *ptr;
{int index;
if( ptr != NULL )
    {display_dynamics(ptr -> right);
    if( (ptr->rectok == tk_regproc || ptr->rectok == tk_proc)
	     && ptr->recsegm != NOT_DYNAMIC )
	{index = ptr->recsegm;
	if( index >= MAXDYNAMICS )  /* if outputted in runfile */
	    {index -= MAXDYNAMICS;
	    fprintf(mapfile,"YES ");}
	else fprintf(mapfile," NO ");
	switch( ptr->recrm )
	    {case tk_void: fprintf(mapfile,"void   ");  break;
	    case tk_byte:  fprintf(mapfile,"byte   ");  break;
	    case tk_char:  fprintf(mapfile,"char   ");  break;
	    case tk_int:   fprintf(mapfile,"int    ");  break;
	    case tk_word:  fprintf(mapfile,"word   ");  break;
	    case tk_dword: fprintf(mapfile,"dword  ");  break;
	    case tk_long:  fprintf(mapfile,"long   ");  break;
	    case tk_fixed32s:  fprintf(mapfile,"fix32s ");  break;
	    case tk_fixed32u:  fprintf(mapfile,"fix32u ");  break;
	    default:       fprintf(mapfile,"?????? ");  break;
	    }
	fprintf(mapfile," %32s %5u byte(s) ",ptr->recid,getdynamicsize(index));
	if( getdynamicdos1(index) > 0 || getdynamicdos2(index) > 0 )
	    fprintf(mapfile,"DOS: %u.%u ",getdynamicdos1(index)
					 ,getdynamicdos2(index));
	switch( getdynamiccpu(index) )
	    {case 0:  fprintf(mapfile,"\n");  break;
	    case 1:   fprintf(mapfile,"80186+\n");  break;
	    case 2:   fprintf(mapfile,"80286+\n");  break;
	    case 3:   fprintf(mapfile,"80386+\n");  break;
	    case 4:   fprintf(mapfile,"80486+\n");  break;
	    case 5:   fprintf(mapfile,"80586+\n");  break;
	    case 6:   fprintf(mapfile,"80686+\n");  break;
	    default:  fprintf(mapfile,"80?86+\n");  break;
	    }
	numberofids++;
	}
    display_dynamics(ptr -> left);
    }
}


displaytree ()   // dump all identifiers to MAP file
{fprintf(mapfile,"ALL GLOBAL IDENTIFIERS LIST:\n");
fprintf(mapfile,"TOKEN                     IDENTIFIER      NUMBER   POST SEGMENT  R/M \n");
numberofids = 0;
displaytreeall(treestart);
fprintf(mapfile,"\n    %u Unique Global Identifiers.\n\n",numberofids);
fprintf(mapfile,"GLOBAL CONSTANT IDENTIFIER LIST:\n");
numberofids = 0;
displaytreeconstants(treestart);
fprintf(mapfile,"\n    %u Unique Global Constant Value Identifiers.\n\n",numberofids);
fprintf(mapfile,"DYNAMIC PROCEDURES LIST:\n");
numberofids = 0;
display_dynamics(treestart);
fprintf(mapfile,"\n    %u Dynamic Procedures.\n\n",numberofids);
}


/********************* OBJ PUBDEF output stuff starts ***************/



unsigned char PUBDEF_str[100];
unsigned int PUBDEF_count;



void outeachPUBDEF(FILE *fp,struct idrec *ptr)
{
unsigned int i;

if( ptr != NULL )
    {outeachPUBDEF(fp,ptr->right);
    if(ptr->rectok == tk_regproc || ptr->rectok == tk_proc ||
                        ptr->rectok == tk_interruptproc )
	{
	if( ptr->recsegm >= MAXDYNAMICS )  // if dynamic flag is inserted
            {PUBDEF_str[0] = 0x00;
            PUBDEF_str[1] = 0x01;
            PUBDEF_str[2] = strlen(ptr->recid); // id length
            for( i=0; i < strlen(ptr->recid); i++ )
                PUBDEF_str[i+3] = ptr->recid[i];
            PUBDEF_str[i+3] = ((unsigned int) ptr->recnumber) & 255;
            PUBDEF_str[i+4] = ((unsigned int) ptr->recnumber) / 256;
            PUBDEF_str[i+5] = 0x00;
            obj_outrecord(fp,0x90,i+6,PUBDEF_str);
	    PUBDEF_count++;
	    }
	}
    outeachPUBDEF(fp,ptr->left);
    }
}




int outputPUBDEF (FILE *fp)
{
// output the PUBDEF
PUBDEF_count = 0;
outeachPUBDEF(fp,treestart);
return(PUBDEF_count);
}



/* end of TOKA.C */
