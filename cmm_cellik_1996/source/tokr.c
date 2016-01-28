/*
	TOKR.C  sub file source of C--.EXE
	Property of: Peter Cellik
	Copyright Peter Cellik (C) 1994, all rights reserved.
	Last Update: 16 Aug 1994 = adding 486 asm stuff

18 Sept 94 notes:
    - added REP alias for REPZ
*/
/*
Things to do:
- mapping of interrupt 21h or 10h to interrupt 3h, thus to make
  debugging more difficult.
- ?ifdef done, but ?if and ?else have to still be done.
- still have to do bit operation asm stuff
*/


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>    // for _getch();
#include "tok.h"

extern int tok,tok2,rm,segm,post;
extern unsigned char string[STRLEN];
extern unsigned int endinptr,outptr,outptrdata,alignersize,startptr;
extern unsigned int maxerrors,error;
extern long number;
extern unsigned char cpu,chip,endoffile,aligner,resizemem,dos1,dos2;
extern unsigned char assumeDSSS,alignword,optimizespeed,makemapfile;
extern unsigned char killctrlc;
extern unsigned char header,jumptomain,parsecommandline,*resizemessage;
extern unsigned char currentfilename[FILENAMESIZE];
extern FILE *mapfile;

extern unsigned char notdoneprestuff,comfile;
extern unsigned int stacksize;

extern char *input;
extern unsigned int inptr2;
extern unsigned char cha2;
extern unsigned int linenum2;

unsigned int  endifcount=0;      // depth count of ?if

doasm ()
{nexttok();
if( tok==tk_ID || tok==tk_id )
    {switch(toupper(string[0]))
	{case 'A':  doasma();  break;
	case 'B':   doasmb();  break;
	case 'C':   doasmc();  break;
	case 'D':   doasmd();  break;
	case 'I':   doasmi();  break;
	case 'J':   doasmj();  break;
	case 'L':   doasml();  break;
	case 'M':   doasmm();  break;
	case 'N':   doasmn();  break;
	case 'O':   doasmo();  break;
	case 'P':   doasmp();  break;
	case 'R':   doasmr();  break;
	case 'S':   doasms();  break;
	case 'W':   doasmw();  break;
	case 'X':   doasmx();  break;
	default:    doasmleftovers();  break;
	}
    }
else if(tok==tk_seg)
    {switch((unsigned int)number)
	{case ES: op(0x26);  break;  /* ES: */
	case SS:  op(0x36);  break;  /* SS: */
	case CS:  op(0x2E);  break;  /* CS: */
	case DS:  op(0x3E);  break;  /* DS: */
	case FS:  op(0x64);          /* FS: */
		  if( cpu < 3 )
		      cpu = 3;
		  break;
	case GS:  op(0x65);          /* GS: */
		  if( cpu < 3 )
		      cpu = 3;
		  break;
	case HS:
	case IS:  preerror("Sorry, I don't yet know the instruction code");
		  break;
	default:  beep();  break;
	}
    nexttok();
    if(tok != tk_colon)
	expected(':');
    nexttok();
    }
else codeexpected();
}


doasma ()
{unsigned char possiblecpu=0,next=1;
if(stricmp("AA",string+1)==0)
	op(0x37);
else if(stricmp("AD",string+1)==0)
	{op(0xD5);  op(0x0A);}
else if(stricmp("AM",string+1)==0)
	{op(0xD4);  op(0x0A);}
else if(stricmp("AS",string+1)==0)
	op(0x3F);
else if(stricmp("DC",string+1)==0)
	{asmtwo1(16,1);
	next=0;}
else if(stricmp("DD",string+1)==0)
	{asmtwo1(0,1);
	next=0;}
else if(stricmp("DRSIZ",string+1)==0 || stricmp("DRSIZE",string+1)==0)
	{op(0x67);
	possiblecpu = 3;}
else if(stricmp("ND",string+1)==0)
	{asmtwo1(32,0);
	next=0;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmb ()
{unsigned char possiblecpu=0,next=1;
if(stricmp("OUND",string+1)==0)
    {asmregmem(0x62,0);
    possiblecpu=2;}
else if(stricmp("SWAP",string+1)==0)
    {nexttok();
    if( tok == tk_reg32 )
        {outword(0x0F66);
        op(0xC8+(unsigned int)number);}       
    else if( tok == tk_reg )
        {op(0x0F);
        op(0xC8+(unsigned int)number);}
    else preerror("Expecting 32 bit Register for BSWAP");
    possiblecpu=4;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmc()
{int faradd;
unsigned int hnumber;
int hsegm,hpost,hrm;
unsigned char next=1,possiblecpu=0,defaultsetting=1;
if(stricmp("ALL",string+1)==0)
    {nexttok();
    if( tok==tk_ID )
	{if( stricmp("FAR",string)==0 )
	    {faradd = 8;
	    defaultsetting = 0;
	    nexttok();}
	else if( stricmp("NEAR",string)==0 )
	    {faradd = 0;
	    defaultsetting = 0;
	    nexttok();}
	}
    else if( tok == tk_far )
        {faradd = 8;
	defaultsetting = 0;
	nexttok();
        }	
    else faradd = 0;
    switch(tok)
	{case tk_proc:
	case tk_regproc:  if(faradd==0)
			      callloc((unsigned int)number);
			  else invalidfarcallitem();
			  break;
	case tk_number:  if(faradd==0)
			     callloc((unsigned int)doconstdwordmath());
			 else{op(0x9A);
			     hnumber = doconstdwordmath();
			     if(tok != tk_colon)
				 expected(':');
			     nexttok();
			     outword( (unsigned int)doconstdwordmath() );
			     outword( hnumber );
			     }
			 next = 0;
			 break;   /* CALL num */
	case tk_postnumber:  if(faradd==0)
				 {op(0xE8);
				 setwordpost();
				 outword((unsigned int)number);}
			     else invalidfarcallitem();
			     break;
	case tk_reg:  if(faradd==0)
			  {op(0xFF);
			  op(0xD0+(unsigned int)number);}    /* CALL reg */
		      else invalidfarcallitem();
		      break;
	case tk_ID:
	case tk_id:  if(faradd==0)
			 {tobedefined(CALL_NEAR,tk_void);
			 callloc(0x0000);}      /* CALL NEAR */
		     else invalidfarcallitem();
		     break;
	case tk_locallabel:
	case tk_undefregproc:
	case tk_undefproc:  if(faradd==0)
				{addacall((unsigned int)number,CALL_NEAR);
				callloc(0x0000);}       /* CALL NEAR */
			    else invalidfarcallitem();
			    break;
	case tk_dwordvar:
	case tk_longvar:  if(defaultsetting)
			      faradd=8;
	case tk_intvar:
	case tk_wordvar:  outseg(segm,post,2);
			  op(0xFF);  op(0x10+rm+faradd);
			  outaddress(rm,(unsigned int)number);
			  break;
	default:  preerror("Invalid item for CALL");
		  break;
	}
    }
else if(stricmp("BD",string+1)==0)
    cbd();
else if(stricmp("BW",string+1)==0)
    cbw();
else if(stricmp("DQ",string+1)==0)
    cdq();
else if(stricmp("LC",string+1)==0)
    op(0xF8);
else if(stricmp("LD",string+1)==0)
    op(0xFC);
else if(stricmp("LI",string+1)==0)
    op(0xFA);
else if(stricmp("LTS",string+1)==0)
    {op(0xF);  op(0x6);
    possiblecpu=2;}
else if(stricmp("MC",string+1)==0)
    op(0xF5);
else if(stricmp("MP",string+1)==0)
    {asmtwo1(56,1);
    next=0;}
else if(stricmp("MPSB",string+1)==0)
    op(0xA6);
else if(stricmp("MPSW",string+1)==0)
    op(0xA7);
else if(stricmp("MPSD",string+1)==0)
    {op(0x66); op(0xA7);
    possiblecpu = 3;}
else if(stricmp("MPXCHG",string+1)==0)
    {nexttok();
    switch(tok)
	{case tk_reg:
		hnumber = (unsigned int)number;
			nexttok();
			if( tok != tk_camma )
				expected(',');
			nexttok();
			switch(tok)
				{case tk_reg:
					outword(0xB10F);
					op(128+64+(unsigned int)number*8+hnumber);
					break;
				case tk_intvar:
				case tk_wordvar:
					outseg(segm,post,3);
					outword(0xB10F);
					op(rm+hnumber*8);
					outaddress(rm,(unsigned int)number);
					break;
				default:  wordvalexpected();
					  break;
				}
			break;
	case tk_reg32:
			hnumber = (unsigned int)number;
			nexttok();
			if( tok != tk_camma )
				expected(',');
			nexttok();
			switch(tok)
				{case tk_reg32:
					op(0x66);
					outword(0xB10F);
					op(128+64+(unsigned int)number*8+hnumber);
					break;
				case tk_longvar:
				case tk_dwordvar:
					outseg(segm,post,4);
					op(0x66);
					outword(0xB10F);
					op(rm+hnumber*8);
					outaddress(rm,(unsigned int)number);
					break;
				default:  wordvalexpected();
						  break;
				}
			break;
	case tk_beg:
			hnumber = (unsigned int)number;
			nexttok();
			if( tok != tk_camma )
				expected(',');
			nexttok();
			switch(tok)
				{case tk_beg:
					outword(0xB00F);
					op(128+64+(unsigned int)number*8+hnumber);
					break;
				case tk_charvar:
				case tk_bytevar:
					outseg(segm,post,3);
					outword(0xB00F);
					op(rm+hnumber*8);
					outaddress(rm,(unsigned int)number);
					break;
				default:  bytevalexpected();
						  break;
				}
			break;
	case tk_charvar:
	case tk_bytevar:  hnumber = (unsigned int)number;
			  hpost = post;
			  hrm = rm;
			  hsegm = segm;
			  nexttok();
			  if(tok != tk_camma)
			      expected(',');
			  nexttok();
			  if(tok != tk_beg)
			      begexpected();
			  outseg(hsegm,hpost,3);
			  outword(0xB00F);
			  op(rm+(unsigned int)number*8);
			  outaddress(hrm,hnumber);
			  break;
	case tk_intvar:
	case tk_wordvar:  hnumber = (unsigned int)number;
			  hpost = post;
			  hrm = rm;
			  hsegm = segm;
			  nexttok();
			  if(tok != tk_camma)
			      expected(',');
			  nexttok();
			  if(tok != tk_reg)
			      regexpected();
			  outseg(hsegm,hpost,3);
			  outword(0xB10F);
			  op(hrm+(unsigned int)number*8);
			  outaddress(hrm,hnumber);
			  break;
	case tk_longvar:
	case tk_dwordvar:  hnumber = (unsigned int)number;
			   hpost = post;
			   hrm = rm;
			   hsegm = segm;
			   nexttok();
			   if(tok != tk_camma)
			       expected(',');
			   nexttok();
			   if(tok != tk_reg32)
			       reg32expected();
			   outseg(hsegm,hpost,4);
			   op(0x66);
 			   outword(0xB10F);
			   op(hrm+(unsigned int)number*8);
			   outaddress(hrm,hnumber);
			   break;
	default:  varexpected();
		  break;
	}
    possiblecpu = 4;
    }
else if(stricmp("WD",string+1)==0)
    cwd();
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmd()
{unsigned char next=1,possiblecpu=0;
unsigned int count;
if(stricmp("AA",string+1)==0)
    op(0x27);
else if(stricmp("AS",string+1)==0)
    op(0x2F);
else if(stricmp("B",string+1)==0)
    {do {nexttok();
	if( tok == tk_number )
	    op((int)doconstlongmath());
	else if( tok == tk_string )
	    {for(count=0;count<(unsigned int)number;count++)
		op(string[count]);
	    nexttok();}
	else{numexpected();
	    nexttok();}
	} while(tok == tk_camma);
    next = 0;
    }
else if(stricmp("D",string+1)==0)
    {do {nexttok();
	if(tok == tk_number)
	    outdword(doconstdwordmath());
	else{numexpected();
	    nexttok();}
	} while(tok == tk_camma);
    next = 0;
    }
else if(stricmp("EC",string+1)==0)
    asmone1(8);
else if(stricmp("IV",string+1)==0)
    asmone2(48);
else if(stricmp("W",string+1)==0)
    {do {nexttok();
	if(tok == tk_number)
	    outword((unsigned int)doconstdwordmath());
	else{numexpected();
	    nexttok();}
	} while(tok == tk_camma);
    next = 0;
    }
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmi ()
{unsigned char next=1,possiblecpu=0;
if(stricmp("DIV",string+1)==0)
    asmone2(56);
else if(stricmp("MUL",string+1)==0)
    asmone2(40);
else if(stricmp("N",string+1)==0)
    {nexttok();
    if( (tok==tk_reg || tok==tk_reg32) && (unsigned int)number==AX)
	{if( tok == tk_reg32 )
	    {op(0x66);
	    possiblecpu = 3;}
	nexttok();
	expecting(tk_camma);
	if(tok==tk_number)
	    {op(0xE5);
	    op((int)doconstlongmath());
	    next = 0;}
	else if(tok==tk_reg && (unsigned int)number==DX)
	    op(0xED);
	else bytedxexpected();
	}
    else if(tok==tk_beg && (unsigned int)number==AL)
	{nexttok();
	expecting(tk_camma);
	if(tok==tk_number)
	    {op(0xE4);
	    op((int)doconstlongmath());
	    next = 0;}
	else if(tok==tk_reg && (unsigned int)number==DX)
	    op(0xEC);
	else bytedxexpected();
	}
    else axalexpected();
    }
else if(stricmp("NSB",string+1)==0)
    {op(0x6C);
    possiblecpu = 2;}
else if(stricmp("NSW",string+1)==0)
    {op(0x6D);
    possiblecpu = 2;}
else if(stricmp("NSD",string+1)==0)
    {op(0x66); op(0x6D);
    possiblecpu = 3;}
else if(stricmp("NT",string+1)==0)
    {nexttok();
    if(tok == tk_number)
	{intnum( (int)doconstlongmath() );
	next = 0;}
    else numexpected();
    }
else if(stricmp("NC",string+1)==0)
    asmone1(0);
else if(stricmp("NTO",string+1)==0)
    op(0xCE);
else if(stricmp("NVD",string+1)==0)
    {outword(0x080F);
    possiblecpu = 4;}
else if(stricmp("NVLPG",string+1)==0)
    {nexttok();
    switch(tok)
	{case tk_reg:  outword(0x010F);
	               op(64+0x38+(unsigned int)number);
	               break;
	case tk_reg32:  op(0x66);
	                outword(0x010F);
	                op(64+0x38+(unsigned int)number);
			break;
	case tk_intvar:
	case tk_wordvar:  outseg(segm,post,3);
	                  outword(0x010F);
			  op(rm+0x38);
			  outaddress(rm,(unsigned int)number);
			  break;
	case tk_longvar:
	case tk_dwordvar:  outseg(segm,post,4);
			   op(0x66);
 	                   outword(0x010F);
			   op(rm+0x38);
			   outaddress(rm,(unsigned int)number);
			   break;
	default:  varexpected();  break;
	}
    possiblecpu = 4;	
    }	
else if(stricmp("RET",string+1)==0)
    op(0xCF);
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


int doasmj ()
{int faradd;
unsigned int hnumber;
unsigned char possiblecpu=0,next=1,defaultsetting;
if(stricmp("O",string+1)==0)
    {asmshortjump(0x70,0x80);
    next=0;}
else if(stricmp("NO",string+1)==0)
    {asmshortjump(0x71,0x81);
    next=0;}
else if( stricmp("B",string+1)==0 || stricmp("NAE",string+1)==0 || stricmp("C",string+1)==0 )
    {asmshortjump(0x72,0x82);
    next=0;}
else if( stricmp("NB",string+1)==0 || stricmp("AE",string+1)==0 || stricmp("NC",string+1)==0 )
    {asmshortjump(0x73,0x83);
    next=0;}
else if( stricmp("E",string+1)==0 || stricmp("Z",string+1)==0   )
    {asmshortjump(0x74,0x84);
    next=0;}
else if( stricmp("NE",string+1)==0 || stricmp("NZ",string+1)==0 )
    {asmshortjump(0x75,0x85);
    next=0;}
else if( stricmp("BE",string+1)==0 || stricmp("NA",string+1)==0 )
    {asmshortjump(0x76,0x86);
    next=0;}
else if( stricmp("NBE",string+1)==0 || stricmp("A",string+1)==0 )
    {asmshortjump(0x77,0x87);
    next=0;}
else if(stricmp("S",string+1)==0)
    {asmshortjump(0x78,0x88);
    next=0;}
else if(stricmp("NS",string+1)==0)
    {asmshortjump(0x79,0x89);
    next=0;}
else if( stricmp("P",string+1)==0 || stricmp("PE",string+1)==0  )
    {asmshortjump(0x7A,0x8A);
    next=0;}
else if( stricmp("NP",string+1)==0 || stricmp("PO",string+1)==0 )
    {asmshortjump(0x7B,0x8B);
    next=0;}
else if( stricmp("L",string+1)==0 || stricmp("NGE",string+1)==0 )
    {asmshortjump(0x7C,0x8C);
    next=0;}
else if( stricmp("NL",string+1)==0 || stricmp("GE",string+1)==0 )
    {asmshortjump(0x7D,0x8D);
    next=0;}
else if( stricmp("LE",string+1)==0 || stricmp("NG",string+1)==0 )
    {asmshortjump(0x7E,0x8E);
    next=0;}
else if( stricmp("NLE",string+1)==0 || stricmp("G",string+1)==0 )
    {asmshortjump(0x7F,0x8F);
    next=0;}
else if(stricmp("CXZ",string+1)==0)
    {asmshortjump(0xE3,0);
    next=0;}
else if(stricmp("MP",string+1)==0)
    {nexttok();
    faradd = 0;
    defaultsetting=1;
    if( tok==tk_ID || tok==tk_id )
	{if( stricmp("FAR",string)==0 )  // case insensitive
	    {faradd = 8;
	    defaultsetting=0;
	    nexttok();}
	else if( stricmp("NEAR",string)==0 )    // case insensitive
	    {defaultsetting=0;
	    nexttok();}
	else if( stricmp("SHORT",string)==0 )   // case insensitive
	    {nexttok();
	    switch(tok)
		{case tk_proc:
		case tk_regproc:  shortjump((unsigned int)number);
				  break;
		case tk_number:  shortjump((unsigned int)doconstlongmath());
				 next=0;
				 break;
		case tk_ID:      addlocaljump(CALL_SHORT);
				 op(0xEB); op(0x00);    /* JMP SHORT */
				 break;
		case tk_id:      tobedefined(CALL_SHORT,tk_void);
				 op(0xEB); op(0x00);    /* JMP SHORT */
				 break;
		case tk_locallabel:
		case tk_undefregproc:
		case tk_undefproc:  addacall((unsigned int)number,CALL_SHORT);
				    op(0xEB); op(0x00); /* JMP SHORT */
				    break;
		default:  shortjumperror();
			  break;
		}
	    if( next )
		nexttok();
	    return(0);
	    }
	}
    else if( tok == tk_far )
        {faradd = 8;
	defaultsetting = 0;
	nexttok();
        }	
    switch(tok)
	{case tk_proc:
	case tk_regproc:  if(faradd==0)
				  jumploc((unsigned int)number);
			  else invalidfarjumpitem();
			  break;
	case tk_number: if(faradd==0)
			    jumploc((unsigned int)doconstdwordmath());
			else{op(0xEA);
			    hnumber = doconstdwordmath();
			    if(tok != tk_colon)
				expected(':');
			    nexttok();
			    outword( (unsigned int)doconstdwordmath() );
			    outword( hnumber );
			    }
			next = 0;
			break;   /* JMP num */
	case tk_postnumber:  if(faradd==0)
				 {op(0xE9);
				 setwordpost();
				 outword((unsigned int)number);}
			     else invalidfarjumpitem();
			     break;
	case tk_reg:  if(faradd==0)
			  {op(0xFF);
			  op(0xE0+(unsigned int)number);}    /* JMP reg */
		      else invalidfarjumpitem();
		      break;
	case tk_ID:  if(faradd==0)
			 {addlocaljump(CALL_NEAR);
			 jumploc(0x0000);}      /* JMP num */
		     else invalidfarjumpitem();
		     break;
	case tk_id:  if(faradd==0)
			 {tobedefined(CALL_NEAR,tk_void);
			 jumploc(0x0000);}      /* JMP num */
		     else invalidfarjumpitem();
		     break;
	case tk_locallabel:
	case tk_undefregproc:
	case tk_undefproc:  if(faradd==0)
				{addacall((unsigned int)number,CALL_NEAR);
				jumploc(0x0000);}       /* JMP num */
			    else invalidfarjumpitem();
			    break;
	case tk_dwordvar:
	case tk_longvar:  if(defaultsetting)
			      faradd=8;
	case tk_intvar:
	case tk_wordvar:  outseg(segm,post,2);
			  op(0xFF);  op(0x20+rm+faradd);
			  outaddress(rm,(unsigned int)number);
			  break;
	default:  preerror("Invalid item for JMP");
		  break;
	}
    }
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasml ()
{unsigned char next=1,possiblecpu=0;
if(stricmp("AHF",string+1)==0)
    op(0x9F);
else if(stricmp("DS",string+1)==0)
    asmregmem(0xC5,0);
else if(stricmp("EA",string+1)==0)
    asmregmem(0x8D,0);
else if(stricmp("EAVE",string+1)==0)
    {op(0xC9);
    possiblecpu = 2;}
else if(stricmp("ES",string+1)==0)
    asmregmem(0xC4,0);
else if(stricmp("FS",string+1)==0)
    {asmregmem(0xF,0xB4);
    possiblecpu=3;}
else if(stricmp("GS",string+1)==0)
    {asmregmem(0xF,0xB5);
    possiblecpu=3;}
else if(stricmp("OADALL",string+1)==0)  // LOADALL 80286 only
    {op(0x0F); op(0x05);
    possiblecpu=2;}
else if(stricmp("ODSB",string+1)==0)
    lodsb();
else if(stricmp("ODSW",string+1)==0)
    lodsw();
else if(stricmp("ODSD",string+1)==0)
    lodsd();
else if(stricmp("OCK",string+1)==0)
    op(0xF0);
else if(stricmp("OOP",string+1)==0)
    {asmshortjump(0xE2,0);
    next=0;}
else if( stricmp("OOPE",string+1)==0 || stricmp("OOPZ",string+1)==0 )
    {asmshortjump(0xE1,0);
    next=0;}
else if( stricmp("OOPNE",string+1)==0 || stricmp("OOPNZ",string+1)==0 )
    {asmshortjump(0xE0,0);
    next=0;}
else if(stricmp("SS",string+1)==0)
    {asmregmem(0xF,0xB6);
    possiblecpu=3;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmm ()
{unsigned char next=1,possiblecpu=0;
if(stricmp("OV",string+1)==0)
    {doasmmov();
    next = 0;}
else if(stricmp("OVSB",string+1)==0)
    movsb();
else if(stricmp("OVSD",string+1)==0)
    movsd();
else if(stricmp("OVSW",string+1)==0)
    movsw();
else if(stricmp("OVSX",string+1)==0)
    asmextend(0xBE);
else if(stricmp("OVZX",string+1)==0)
    asmextend(0xB6);
else if(stricmp("UL",string+1)==0)
    asmone2(32);
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmn ()
{unsigned char next=1,possiblecpu=0;
if(stricmp("EG",string+1)==0)
    asmone2(24);
else if(stricmp("OP",string+1)==0)
    op(0x90);
else if(stricmp("OT",string+1)==0)
    asmone2(16);
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmo ()
{unsigned int hnumber;
unsigned char next=1,possiblecpu=0;
if(stricmp("PSIZ",string+1)==0 || strcmp("PSIZE",string+1)==0)
    {op(0x66);
    possiblecpu = 3;}
else if(stricmp("R",string+1)==0)
    {asmtwo1(8,0);
    next=0;}
else if(stricmp("UT",string+1)==0)
    {nexttok();
    if(tok==tk_number)
	{hnumber = doconstlongmath();
	if(tok != tk_camma)
	    expected(',');
	nexttok();
	if(tok==tk_reg && (unsigned int)number==AX)
	    {op(0xE7);  op(hnumber);}
	else if(tok==tk_reg32 && (unsigned int)number==EAX)
	    {op(0x66); op(0xE7);  op(hnumber);
	    possiblecpu = 3;}
	else if(tok==tk_beg && (unsigned int)number==AL)
	    {op(0xE6);  op(hnumber);}
	else axalexpected();
	}
    else if(tok==tk_reg && (unsigned int)number==DX)
	{nexttok();
	if(tok != tk_camma)
	    expected(',');
	nexttok();
	if(tok==tk_reg && (unsigned int)number==AX)
	    op(0xEF);
	else if(tok==tk_reg32 && (unsigned int)number==EAX)
	    {op(0x66); op(0xEF);
	    possiblecpu = 3;
	    }
	else if(tok==tk_beg && (unsigned int)number==AL)
	    op(0xEE);
	else axalexpected();
	}
    else bytedxexpected();
    }
else if(stricmp("UTSB",string+1)==0)
    {op(0x6E);
    possiblecpu = 2;}
else if(stricmp("UTSW",string+1)==0)
    {op(0x6F);
    possiblecpu = 2;}
else if(stricmp("UTSD",string+1)==0)
    {op(0x66); op(0x6F);
    possiblecpu = 3;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmp ()
{unsigned char possiblecpu=0,next=1;
unsigned char hnumber;
if(stricmp("OP",string+1)==0)
    {nexttok();
    switch(tok)
	{case tk_reg:  op(0x58+(unsigned int)number);  break;
	case tk_reg32:  op(0x66); op(0x58+(unsigned int)number);
			possiblecpu = 3;
			break;
	case tk_seg:  switch((unsigned int)number)
			  {case DS:  op(0x1F);  break;
			  case CS:  op(0x0F);  break;
			  case SS:  op(0x17);  break;
			  case ES:  op(0x07);  break;
			  case FS:  op(0xF);  op(0xA1);
				    possiblecpu = 3;
				    break;
			  case GS:  op(0xF);  op(0xA9);
				    possiblecpu = 3;
				    break;
			  case HS:  op(0xF);  op(0xB1);
				    possiblecpu = 5;
				    break;
			  case IS:  op(0xF);  op(0xB9);
				    possiblecpu = 5;
				    break;
			  default:  beep();
			  }
		      break;
	case tk_wordvar:
	case tk_intvar:  outseg(segm,post,2);
			 op(0x8F);      op(rm);
			 outaddress(rm,(unsigned int)number);
			 break;
	case tk_dwordvar:
	case tk_longvar:  outseg(segm,post,3);
			  op(0x66);
			  op(0x8F); op(rm);
			  outaddress(rm,(unsigned int)number);
			  possiblecpu=3;
			  break;
	default:  preerror("Invalid operand for POP");
		  break;
	}
    }
else if(stricmp("OPA",string+1)==0)
    {op(0x61);
    possiblecpu = 2;}
else if(stricmp("OPAD",string+1)==0)
    {op(0x66); op(0x61);
    possiblecpu = 3;}
else if(stricmp("OPF",string+1)==0)
    popf();
else if(stricmp("OPFD",string+1)==0)
    {op(0x66); op(0x9D);
    possiblecpu = 3;}
else if(stricmp("USH",string+1)==0)
    {nexttok();
    switch(tok)
	{case tk_reg:  op(0x50+(unsigned int)number);  break;
	case tk_reg32:  op(0x66); op(0x50+(unsigned int)number);
			possiblecpu = 3;
			break;
	case tk_seg:  switch((unsigned int)number)
			  {case DS:     op(0x1E);       break;
			  case CS:  op(0x0E);  break;
			  case SS:  op(0x16);  break;
			  case ES:  op(0x06);  break;
			  case FS:  op(0xF);  op(0xA0);
				    possiblecpu = 3;
				    break;
			  case GS:  op(0xF);  op(0xA8);
				    possiblecpu = 3;
				    break;
			  case HS:  op(0xF);  op(0xB0);
				    possiblecpu = 5;
				    break;
			  case IS:  op(0xF);  op(0xB8);
				    possiblecpu = 5;
				    break;
			  }
		      break;
	case tk_number:  hnumber = doconstlongmath();
			 if( short_ok(hnumber) )
			     {op(0x6A);
			     op(hnumber);}
			 else{op(0x68);
			     outword(hnumber);}
			 possiblecpu = 2;
			 next = 0;
			 break;
	case tk_wordvar:
	case tk_intvar:  outseg(segm,post,2);
			 op(0xFF);      op(0x30+rm);
			 outaddress(rm,(unsigned int)number);
			 break;
	case tk_dwordvar:
	case tk_longvar:  outseg(segm,post,3);
			  op(0x66);  op(0xFF);  op(0x30+rm);
			  outaddress(rm,(unsigned int)number);
			  possiblecpu = 3;
			  break;
	default:  preerror("Invalid operand for PUSH");
		  break;
	}
    }
else if(stricmp("USHA",string+1)==0)
    {op(0x60);
    possiblecpu = 2;}
else if(stricmp("USHAD",string+1)==0)
    {op(0x66); op(0x60);
    possiblecpu = 3;}
else if(stricmp("USHF",string+1)==0)
    pushf();
else if(stricmp("USHFD",string+1)==0)
    {op(0x66); op(0x9C);
    possiblecpu = 3;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmr ()
{unsigned char next=1,possiblecpu=0;
if(stricmp("CL",string+1)==0)
    {asmshift(16);
    next = 0;}
else if(stricmp("CR",string+1)==0)
    {asmshift(24);
    next = 0;}
else if(stricmp("EP",string+1)==0)
    op(0xF3);
else if(stricmp("EPE",string+1)==0)
    op(0xF3);
else if(strcmp("EPNE",string+1)==0)
    op(0xF2);
else if(stricmp("EPNZ",string+1)==0)
    op(0xF2);
else if(stricmp("EPZ",string+1)==0)
    op(0xF3);
else if(stricmp("ET",string+1)==0)
    {next = 0;
    nexttok();
    if( tok == tk_number )
	{op(0xC2);
	outword((unsigned int)doconstlongmath());}
    else op(0xC3);}
else if(stricmp("ETF",string+1)==0)
    op(0xCB);
else if(stricmp("OL",string+1)==0)
    {asmshift(0);
    next = 0;}
else if(stricmp("OR",string+1)==0)
    {asmshift(8);
    next = 0;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasms ()
{unsigned char next=1,possiblecpu=0;
if(stricmp("AHF",string+1)==0)
    op(0x9E);
else if(stricmp("AL",string+1)==0)
    {asmshift(32);
    next = 0;}
else if(stricmp("AR",string+1)==0)
    {asmshift(56);
    next = 0;}
else if(stricmp("BB",string+1)==0)
    {asmtwo1(24,1);
    next=0;}
else if(stricmp("CASB",string+1)==0)
    op(0xAE);
else if(stricmp("CASW",string+1)==0)
    op(0xAF);
else if(stricmp("CASD",string+1)==0)
    {op(0x66); op(0xAF);
    possiblecpu = 3;}
else if(stricmp("HL",string+1)==0)
    {asmshift(32);
    next = 0;}
else if(stricmp("HR",string+1)==0)
    {asmshift(40);
    next = 0;}
else if(stricmp("TC",string+1)==0)
    op(0xF9);
else if(stricmp("TD",string+1)==0)
    op(0xFD);
else if(stricmp("TI",string+1)==0)
    op(0xFB);
else if(stricmp("TOSB",string+1)==0)
    stosb();
else if(stricmp("TOSW",string+1)==0)
    stosw();
else if(stricmp("TOSD",string+1)==0)
    stosd();
else if(stricmp("UB",string+1)==0)
    {asmtwo1(40,1);
    next=0;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmw ()
{unsigned char possiblecpu=0,next=1;
if(stricmp("AIT",string+1)==0)
    op(0x9B);
else if(stricmp("BINVD",string+1)==0)
    {outword(0x090F);
    possiblecpu=4;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmx ()
{unsigned char next=1,possiblecpu=0;
unsigned int hnumber;
int hsegm,hpost,hrm;
if(stricmp("ADD",string+1)==0)
    {nexttok();
    switch(tok)
	{case tk_reg:
		hnumber = (unsigned int)number;
		nexttok();
		if( tok != tk_camma )
			expected(',');
		nexttok();
		switch(tok)
			{case tk_reg:
				outword(0xC10F);
				op(128+64+(unsigned int)number*8+hnumber);
				break;
			case tk_intvar:
			case tk_wordvar:
				outseg(segm,post,3);
				outword(0xC10F);
				op(rm+hnumber*8);
				outaddress(rm,(unsigned int)number);
				break;
			default:  wordvalexpected();
				  break;
			}
		break;
	case tk_reg32:
		hnumber = (unsigned int)number;
		nexttok();
		if( tok != tk_camma )
			expected(',');
		nexttok();
		switch(tok)
			{case tk_reg32:
				op(0x66);
				outword(0xC10F);
				op(128+64+(unsigned int)number*8+hnumber);
				break;
			case tk_longvar:
			case tk_dwordvar:
				outseg(segm,post,4);
				op(0x66);
				outword(0xC10F);
				op(rm+hnumber*8);
				outaddress(rm,(unsigned int)number);
				break;
			default:  wordvalexpected();
					  break;
			}
		break;
	case tk_beg:
		hnumber = (unsigned int)number;
		nexttok();
		if( tok != tk_camma )
			expected(',');
		nexttok();
		switch(tok)
			{case tk_beg:
				outword(0xC00F);
				op(128+64+(unsigned int)number*8+hnumber);
				break;
			case tk_charvar:
			case tk_bytevar:
				outseg(segm,post,3);
				outword(0xC00F);
				op(rm+hnumber*8);
				outaddress(rm,(unsigned int)number);
				break;
			default:  bytevalexpected();
					  break;
			}
		break;
	case tk_charvar:
	case tk_bytevar:  hnumber = (unsigned int)number;
			  hpost = post;
			  hrm = rm;
			  hsegm = segm;
			  nexttok();
			  if(tok != tk_camma)
			      expected(',');
			  nexttok();
			  if(tok != tk_beg)
			      begexpected();
			  outseg(hsegm,hpost,3);
			  outword(0xC00F);
			  op(rm+(unsigned int)number*8);
			  outaddress(hrm,hnumber);
			  break;
	case tk_intvar:
	case tk_wordvar:  hnumber = (unsigned int)number;
			  hpost = post;
			  hrm = rm;
			  hsegm = segm;
			  nexttok();
			  if(tok != tk_camma)
			      expected(',');
			  nexttok();
			  if(tok != tk_reg)
			      regexpected();
			  outseg(hsegm,hpost,3);
			  outword(0xC10F);
			  op(hrm+(unsigned int)number*8);
			  outaddress(hrm,hnumber);
			  break;
	case tk_longvar:
	case tk_dwordvar:  hnumber = (unsigned int)number;
			   hpost = post;
			   hrm = rm;
			   hsegm = segm;
			   nexttok();
			   if(tok != tk_camma)
			       expected(',');
			   nexttok();
			   if(tok != tk_reg32)
			       reg32expected();
			   outseg(hsegm,hpost,4);
			   op(0x66);
 			   outword(0xC10F);
			   op(hrm+(unsigned int)number*8);
			   outaddress(hrm,hnumber);
			   break;
	default:  varexpected();
		  break;
	}
    possiblecpu = 4;
    }
else if(stricmp("CHG",string+1)==0)
    {nexttok();
    switch(tok)
	{case tk_reg:
		hnumber = (unsigned int)number;
			nexttok();
			if( tok != tk_camma )
				expected(',');
			nexttok();
			switch(tok)
				{case tk_reg:
					if( hnumber==AX )
						op(0x90+(unsigned int)number);
					else if( (unsigned int)number==AX )
						op(0x90+hnumber);
					else{op(0x87);
						op(128+64+(unsigned int)number*8+hnumber);}
					break;
				case tk_intvar:
				case tk_wordvar:
					outseg(segm,post,2);
					op(0x87);
					op(rm+hnumber*8);
					outaddress(rm,(unsigned int)number);
					break;
				default:  wordvalexpected();
						  break;
				}
			break;
	case tk_reg32:
			hnumber = (unsigned int)number;
			nexttok();
			if( tok != tk_camma )
				expected(',');
			nexttok();
			switch(tok)
				{case tk_reg32:
					op(0x66);
					if( hnumber==EAX )
						op(0x90+(unsigned int)number);
					else if( (unsigned int)number==EAX )
						op(0x90+hnumber);
					else{op(0x87);
						op(128+64+(unsigned int)number*8+hnumber);}
					break;
				case tk_longvar:
				case tk_dwordvar:
					outseg(segm,post,3);
					op(0x66);  op(0x87);
					op(rm+hnumber*8);
					outaddress(rm,(unsigned int)number);
					break;
				default:  wordvalexpected();
						  break;
				}
			if( cpu < 3 )
				cpu = 3;
			break;
	case tk_beg:
			hnumber = (unsigned int)number;
			nexttok();
			if( tok != tk_camma )
				expected(',');
			nexttok();
			switch(tok)
				{case tk_beg:
					op(0x86);
					op(128+64+(unsigned int)number*8+hnumber);
					break;
				case tk_charvar:
				case tk_bytevar:
					outseg(segm,post,2);
					op(0x86);
					op(rm+hnumber*8);
					outaddress(rm,(unsigned int)number);
					break;
				default:  bytevalexpected();
						  break;
				}
			break;
	case tk_charvar:
	case tk_bytevar:  hnumber = (unsigned int)number;
			  hpost = post;
			  hrm = rm;
			  hsegm = segm;
			  nexttok();
			  if(tok != tk_camma)
			      expected(',');
			  nexttok();
			  if(tok != tk_beg)
			      begexpected();
			  outseg(hsegm,hpost,2);
			  op(0x86);
			  op(rm+(unsigned int)number*8);
			  outaddress(hrm,hnumber);
			  break;
	case tk_intvar:
	case tk_wordvar:  hnumber = (unsigned int)number;
			  hpost = post;
			  hrm = rm;
			  hsegm = segm;
			  nexttok();
			  if(tok != tk_camma)
			      expected(',');
			  nexttok();
			  if(tok != tk_reg)
			      regexpected();
			  outseg(hsegm,hpost,2);
			  op(0x87);
			  op(hrm+(unsigned int)number*8);
			  outaddress(hrm,hnumber);
			  break;
	case tk_longvar:
	case tk_dwordvar:  hnumber = (unsigned int)number;
			   hpost = post;
			   hrm = rm;
			   hsegm = segm;
			   nexttok();
			   if(tok != tk_camma)
			       expected(',');
			   nexttok();
			   if(tok != tk_reg32)
			       reg32expected();
			   outseg(hsegm,hpost,3);
			   op(0x66);
			   op(0x87);
			   op(hrm+(unsigned int)number*8);
			   outaddress(hrm,hnumber);
			   if( cpu < 3 )
			       cpu = 3;
			   break;
	default:  varexpected();
		  break;
	}
    }
else if(stricmp("LAT",string+1)==0)
    op(0xD7);
else if(stricmp("LATB",string+1)==0)
    op(0xD7);
else if(stricmp("OR",string+1)==0)
    {asmtwo1(48,0);
    next=0;}
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


doasmleftovers ()
{unsigned char next=1,possiblecpu=0;
int htok,hsegm,hpost,hrm;
unsigned int hnumber;
if(stricmp("ENTER",string)==0)
    {op(0xC8);
    if(tok == tk_number)
	outword((unsigned int)doconstlongmath());
    else{numexpected();
	nexttok();}
    if(tok != tk_camma)
	expected(',');
    nexttok();
    if(tok == tk_number)
	op((int)doconstlongmath());
    else{numexpected();
	nexttok();}
    possiblecpu = 2;
    }
else if(stricmp("HALT",string)==0 || stricmp("HLT",string)==0)
    op(0xF4);
else if(stricmp("TEST",string)==0)
    {nexttok();
    htok = tok;
    hrm = rm;
    hsegm = segm;
    hpost = post;
    if( tok == tk_number )
	hnumber = doconstlongmath();
    else{hnumber = (unsigned int)number;
	nexttok();}
    if(tok != tk_camma)
	expected(',');
    nexttok();
    switch(htok)
	{case tk_reg:  switch(tok)
						   {case tk_reg:  op(0x85);
										  op(128+64+(unsigned int)number*8+hnumber);
										  break;
						   case tk_number:      if(hnumber==AX)
												op(0xA9);
											else{op(0xF7);
												op(128+64+hnumber);}
											outword((unsigned int)doconstlongmath());
											next=0;
											break;
						   case tk_postnumber:  if(hnumber==AX)
													op(0xA9);
												else{op(0xF7);
													op(128+64+hnumber);}
												setwordpost();
												outword(0x0000);
												break;
						   case tk_wordvar:
						   case tk_intvar:      outseg(segm,post,2);
											op(0x85);
											op(hnumber*8+rm);
											outaddress(rm,(unsigned int)number);
											break;
						   default:  invalidoperand();  break;
						   }
					   break;
	case tk_reg32:  switch(tok)
						   {case tk_reg32:      op(0x66);  op(0x85);
											op(128+64+(unsigned int)number*8+hnumber);
											break;
						   case tk_number:      op(0x66);
											if(hnumber==EAX)
												op(0xA9);
											else{op(0xF7);
												op(128+64+hnumber);}
											outdword(doconstdwordmath());
											next=0;
											break;
						   case tk_dwordvar:
						   case tk_longvar:      outseg(segm,post,3);
											 op(0x66);      op(0x85);
											 op(hnumber*8+rm);
											 outaddress(rm,(unsigned int)number);
											 break;
						   default:  invalidoperand();  break;
						   }
					   if( cpu < 3 )
						   cpu = 3;
					   break;
	case tk_beg:  switch(tok)
						   {case tk_beg:  op(0x84);
										  op(128+64+(unsigned int)number*8+hnumber);
										  break;
						   case tk_number:      if(hnumber==AL)
												op(0xA8);
											else{op(0xF6);
												op(128+64+hnumber);}
											op((int)doconstlongmath());
											next=0;
											break;
						   case tk_charvar:
						   case tk_bytevar:     outseg(segm,post,2);
											op(0x84);
											op(hnumber*8+rm);
											outaddress(rm,(unsigned int)number);
											break;
						   default:  invalidoperand();  break;
						   }
					   break;
	case tk_number:  switch(tok)
							 {case tk_reg:  if((unsigned int)number==AX)
												op(0xA9);
											else{op(0xF7);
												op(128+64+(unsigned int)number);}
											outword(hnumber);
											break;
							 case tk_beg:  if((unsigned int)number==AL)
											   op(0xA8);
										   else{op(0xF6);
											   op(128+64+(unsigned int)number);}
										   op(hnumber);
										   break;
							 case tk_charvar:
							 case tk_bytevar:  outseg(segm,post,2);
											   op(0xF6);
											   op(rm);
											   outaddress(rm,(unsigned int)number);
											   op(hnumber);
											   break;
							 case tk_wordvar:
							 case tk_intvar:  outseg(segm,post,2);
											  op(0xF7);
											  op(rm);
											  outaddress(rm,(unsigned int)number);
											  outword(hnumber);
											  break;
							 default:  invalidoperand();  break;
							 }
						 break;
	case tk_postnumber:     switch(tok)
							 {case tk_reg:  if((unsigned int)number==AX)
												op(0xA9);
											else{op(0xF7);
												op(128+64+(unsigned int)number);}
											setwordpost();
											outword(hnumber);
											break;
							 case tk_wordvar:
							 case tk_intvar:  outseg(segm,post,2);
											  op(0xF7);
											  op(rm);
											  outaddress(rm,(unsigned int)number);
											  setwordpost();
											  outword(hnumber);
											  break;
							 default:  invalidoperand();  break;
							 }
						 break;
							 break;
	case tk_dwordvar:
	case tk_longvar:  switch(tok)
							 {case tk_reg32:  outseg(hsegm,hpost,3);
											  op(0x33);  op(0x85);
											  op((unsigned int)number*8+hrm);
											  outaddress(hrm,hnumber);
											  break;
							 case tk_number:  outseg(hsegm,hpost,3);
											  op(0x66);  op(0xF7);
											  op(hrm);
											  outaddress(hrm,hnumber);
											  outdword(doconstdwordmath());
											  next=0;
											  break;
							 default:  invalidoperand();  break;
							 }
						 if( cpu < 3 )
							 cpu = 3;
						 break;
	case tk_wordvar:
	case tk_intvar:  switch(tok)
							 {case tk_reg:  outseg(hsegm,hpost,2);
											op(0x85);
											op((unsigned int)number*8+hrm);
											outaddress(hrm,hnumber);
											break;
							 case tk_number:  outseg(hsegm,hpost,2);
											  op(0xF7);
											  op(hrm);
											  outaddress(hrm,hnumber);
											  outword((unsigned int)doconstlongmath());
											  next = 0;
											  break;
							 case tk_postnumber:  outseg(hsegm,hpost,2);
												  op(0xF7);
												  op(hrm);
												  outaddress(hrm,hnumber);
												  setwordpost();
												  outword((unsigned int)number);
												  break;
							 default:  invalidoperand();  break;
							 }
						 break;
	case tk_charvar:
	case tk_bytevar:  switch(tok)
							 {case tk_beg:  outseg(hsegm,hpost,2);
											op(0x84);
											op((unsigned int)number*8+hrm);
											outaddress(hrm,hnumber);
											break;
							 case tk_number:  outseg(hsegm,hpost,2);
											  op(0xF6);
											  op(hrm);
											  outaddress(hrm,hnumber);
											  op((int)doconstlongmath());
											  next = 0;
											  break;
							 default:  invalidoperand();  break;
							 }
						 break;
	default:  invalidoperand();
		  break;
	}
    }
else codeexpected();
if( cpu < possiblecpu )
    cpu = possiblecpu;
if(next)
    nexttok();
}


asmextend (basecode)   // procedure MOVSX and MOVZX
int basecode;
{int regnum;
char thirtytwoflag=0;
nexttok();
if(tok == tk_reg32)
    thirtytwoflag=1;
else if(tok != tk_reg)
    regexpected();
regnum = (unsigned int)number*8;
nexttok();
if( tok != tk_camma )
    expected(',');
nexttok();
switch(tok)
	{case tk_reg:  if( thirtytwoflag )
					   op(0x66);
				   op(0xF);  op(0xBF);
				   op(128+64+regnum+(unsigned int)number);
				   break;
	case tk_beg:  if( thirtytwoflag )
					  op(0x66);
				  op(0xF);      op(0xBE);
				  op(128+64+regnum+(unsigned int)number);
				  break;
	case tk_wordvar:
	case tk_intvar:  if( thirtytwoflag )
						 {outseg(segm,post,3);
						 op(0x66);}
					 else outseg(segm,post,2);
					 op(0xF);       op(0xBF);
					 op(rm+regnum);
					 outaddress(rm,(unsigned int)number);
					 break;
	case tk_bytevar:
	case tk_charvar:  if( thirtytwoflag )
						  {outseg(segm,post,3);
						  op(0x66);}
					  else outseg(segm,post,2);
					  op(0xF);      op(0xBE);
					  op(rm+regnum);
					  outaddress(rm,(unsigned int)number);
					  break;
	default:  varexpected();  break;
	}
if( cpu < 3 )
    cpu = 3;
}

asmshortjump (shortcode,nearcode)
int shortcode,nearcode;
{unsigned char next=1,shortjump=1,possiblecpu=0;
unsigned int address;
nexttok();
if( tok == tk_id || tok == tk_ID )
    {if( stricmp("FAR",string)==0 )     // case insensitive
	{preerror("FAR jump not available for this instruction");
	nexttok();}
    else if( stricmp("NEAR",string)==0 )        // case insensitive
	{shortjump = 0;
	nexttok();}
    else if( stricmp("SHORT",string)==0 )       // case insensitive
	nexttok();
    }
else if( tok == tk_far )
    {preerror("FAR jump not available for this instruction");
    nexttok();}
if(shortjump)
    {switch(tok)
	{case tk_proc:
	case tk_regproc: (unsigned int)number -= outptr+2;
			 if( short_ok(number) )
			     {op(shortcode);
			     op((unsigned int)number);}
			 else shortjumptoolarge();
			 break;
	case tk_number:  address = doconstdwordmath() - (outptr+2);
			 if( short_ok(address) )
			     {op(shortcode);
			     op(address);}
			 else shortjumptoolarge();
			 next=0;
			 break;
	case tk_ID:  addlocaljump(CALL_SHORT);
		     op(shortcode); op(0x00);   /* JXX SHORT */
		     break;
	case tk_id:  tobedefined(CALL_SHORT,tk_void);
		     op(shortcode); op(0x00);   /* JXX SHORT */
		     break;
	case tk_locallabel:
	case tk_undefregproc:
	case tk_undefproc:  addacall((unsigned int)number,CALL_SHORT);
			    op(shortcode); op(0x00);    /* JXX SHORT */
			    break;
	default:  shortjumperror();
		  break;
	}
    }
else if(nearcode != 0)
    {switch(tok)
	{case tk_proc:
	case tk_regproc: op(0xF); op(nearcode);
			 outword((unsigned int)number-outptr-2);
			 next=0;
			 break;
	case tk_number:  op(0xF); op(nearcode);
			 outword((unsigned int)doconstdwordmath()-outptr-2);
			 next=0;
			 break;
	case tk_postnumber:  op(0xF); op(nearcode);
			 setwordpost();
			 outword((unsigned int)number-outptr-2);
			 break;
	case tk_ID:  op(0xF);   // must go before tobedefined()
		     addlocaljump(CALL_NEAR);
		     op(nearcode); outword(0x0000);      /* JXX NEAR */
		     break;
	case tk_id:  op(0xF);   // must go before tobedefined()
		     tobedefined(CALL_NEAR,tk_void);
		     op(nearcode); outword(0x0000);      /* JXX NEAR */
		     break;
	case tk_locallabel:
	case tk_undefregproc:
	case tk_undefproc:      op(0xF);   // must go before addacall()
				addacall((unsigned int)number,CALL_NEAR);
				op(nearcode); op(0x0000);  /* JXX NEAR */
				break;
	default:  nearjumperror();
		  break;
	}
    if( cpu < 3 )
	cpu = 3;
    }
else preerror("NEAR jump not available for this instruction");
if(next)
    nexttok();
}


asmone1 (basecode)       // used for INC and DEC.
int basecode;
{nexttok();
switch(tok)
	{case tk_reg:  op(64+basecode+(unsigned int)number);    break;
	case tk_reg32:  op(0x66);  op(64+basecode+(unsigned int)number);
					if( cpu < 3 )
						cpu = 3;
					break;
	case tk_beg:  op(254);  op(128+64+basecode+(unsigned int)number);  break;
	case tk_charvar:
	case tk_bytevar:  outseg(segm,post,2);
					  op(254);
					  op(rm+basecode);
					  outaddress(rm,(unsigned int)number);
					  break;
	case tk_intvar:
	case tk_wordvar:  outseg(segm,post,2);
					  op(255);
					  op(rm+basecode);
					  outaddress(rm,(unsigned int)number);
					  break;
	case tk_longvar:
	case tk_dwordvar:  outseg(segm,post,3);
					   op(0x66);  op(255);
					   op(rm+basecode);
					   outaddress(rm,(unsigned int)number);
					   if( cpu < 3 )
						   cpu = 3;
					   break;
	default:  varexpected();  break;
	}
}


asmshift (basecode)      // used for ROL ROR RCL RCR SHL SAL SHR SAR.
int basecode;
{int htok,hrm,hpost,hsegm,precode;
unsigned int hnumber;
unsigned char holdbyte;
char next = 1;
nexttok();
htok = tok;
hrm = rm;
hpost = post;
hsegm = segm;
hnumber = (unsigned int)number;
nexttok();
if(tok != tk_camma)
    expected(',');
nexttok();
if(tok==tk_beg && (unsigned int)number==CL)
    {precode = 0xD2;
    holdbyte = 0;}
else if(tok==tk_number)
    {holdbyte = doconstlongmath();
    if(holdbyte == 1)
	{precode = 0xD0;
	holdbyte = 0;}
    else if(holdbyte != 0)
	{precode = 0xC0;
	if( cpu < 2 )
	    cpu = 2;
	}
    else precode = 0;
    next = 0;
    }
else clornumberexpected();
if(precode != 0)
    {switch(htok)
	{case tk_reg:  op(precode+1);  op(128+64+basecode+hnumber);
		       break;
	case tk_reg32:  op(0x66); op(precode+1); op(128+64+basecode+hnumber);
			if( cpu < 3 )
			    cpu = 3;
			break;
	case tk_beg:  op(precode);      op(128+64+basecode+hnumber);
		      break;
	case tk_charvar:
	case tk_bytevar:  outseg(hsegm,hpost,2);
			  op(precode);
			  op(hrm+basecode);
			  outaddress(hrm,hnumber);
			  break;
	case tk_wordvar:
	case tk_intvar:   outseg(hsegm,hpost,2);
			  op(precode+1);
			  op(hrm+basecode);
			  outaddress(hrm,hnumber);
			  break;
	case tk_dwordvar:
	case tk_longvar:  outseg(hsegm,hpost,3);
			  op(0x66);  op(precode+1);
			  op(hrm+basecode);
			  outaddress(hrm,hnumber);
			  if( cpu < 3 )
			      cpu = 3;
			  break;
	default:  varexpected();
		  break;
	}
    if(holdbyte != 0)
	op(holdbyte);
    }
if(next)
    nexttok();
}


asmone2 (basecode)       // used for NEG NOT MUL IMUL DIV IDIV.
int basecode;
{nexttok();
switch(tok)
	{case tk_reg:  op(246+1);       op(128+64+basecode+(unsigned int)number);        break;
	case tk_reg32:  op(0x66);  op(246+1);  op(128+64+basecode+(unsigned int)number);
					if( cpu < 3 )
						cpu = 3;
					break;
	case tk_beg:  op(246);  op(128+64+basecode+(unsigned int)number);  break;
	case tk_charvar:
	case tk_bytevar:  outseg(segm,post,2);
					  op(246);
					  op(rm+basecode);
					  outaddress(rm,(unsigned int)number);
					  break;
	case tk_intvar:
	case tk_wordvar:  outseg(segm,post,2);
					  op(246+1);
					  op(rm+basecode);
					  outaddress(rm,(unsigned int)number);
					  break;
	case tk_longvar:
	case tk_dwordvar:  outseg(segm,post,3);
					   op(0x66);  op(246+1);
					   op(rm+basecode);
					   outaddress(rm,(unsigned int)number);
					   if( cpu < 3 )
						   cpu = 3;
					   break;
	default:  varexpected();  break;
	}
}


asmtwo1 (basecode,extendable)   // used for ADD ADC SUB SBB CMP AND OR XOR.
int basecode,extendable;
{unsigned int hnumber,holdnumber2;
int htok,hrm,hsegm,hpost;
long longholdnumber;
unsigned char next=1;
nexttok();
hnumber = (unsigned int)number;
htok = tok;
hrm = rm;
hsegm = segm;
hpost = post;
nexttok();
if( tok != tk_camma )
    expected(',');
nexttok();
switch(htok)
	{case tk_reg:  switch(tok)
					   {case tk_number:
						   holdnumber2 = doconstlongmath();
						   next = 0;
						   if(hnumber==AX)
							   {op(4+1+basecode);
							   outword(holdnumber2);}
						   else{if( extendable && short_ok(holdnumber2) )
								   {op(128+2+1);
								   op(128+64+hnumber+basecode);
								   op(holdnumber2);}
							   else{op(128+1);
								   op(128+64+hnumber+basecode);
								   outword(holdnumber2);}
							   }
						   break;
					   case tk_postnumber:
						   if(hnumber==AX)
							   op(4+1+basecode);
						   else{op(128+1);
							   op(128+64+hnumber+basecode);}
						   setwordpost();
						   outword((unsigned int)number);
						   break;
					   case tk_reg:
						   op(2+1+basecode);
						   op(128+64+(unsigned int)number+hnumber*8);
						   break;
					   case tk_wordvar:
					   case tk_intvar:
						   outseg(segm,post,2);
						   op(2+1+basecode);
						   op(rm+hnumber*8);
						   outaddress(rm,(unsigned int)number);
						   break;
					   default:
						   wordvalexpected();
						   break;
					   }
				   break;
	case tk_reg32:  switch(tok)
					   {case tk_number:
						   longholdnumber = doconstdwordmath();
						   next = 0;
						   op(0x66);
						   if(hnumber==EAX)
							   {op(4+1+basecode);
							   outdword(longholdnumber);}
						   else{if( extendable && short_ok(holdnumber2) )
								   {op(128+2+1);
								   op(128+64+hnumber+basecode);
								   op( (int) longholdnumber );}
							   else{op(128+1);
								   op(128+64+hnumber+basecode);
								   outdword(longholdnumber);}
							   }
						   break;
					   case tk_reg32:
						   op(0x66);
						   op(2+1+basecode);
						   op(128+64+(unsigned int)number+hnumber*8);
						   break;
					   case tk_dwordvar:
					   case tk_longvar:
						   outseg(segm,post,3);
						   op(0x66);
						   op(2+1+basecode);
						   op(rm+hnumber*8);
						   outaddress(rm,(unsigned int)number);
						   break;
					   default:
						   dwordvalexpected();
						   break;
					   }
				   if( cpu < 3 )
					   cpu = 3;
				   break;
	case tk_beg:  switch(tok)
					   {case tk_number:
						   holdnumber2 = doconstlongmath();
						   next = 0;
						   if(hnumber==AL)
							   {op(4+basecode);
							   op(holdnumber2);}
						   else{op(128);
							   op(128+64+hnumber+basecode);
							   op(holdnumber2);}
						   break;
					   case tk_beg:
						   op(2+basecode);
						   op(128+64+(unsigned int)number+hnumber*8);
						   break;
					   case tk_bytevar:
					   case tk_charvar:
						   outseg(segm,post,2);
						   op(2+basecode);
						   op(rm+hnumber*8);
						   outaddress(rm,(unsigned int)number);
						   break;
					   default:
						   bytevalexpected();
						   break;
					   }
				   break;
	case tk_intvar:
	case tk_wordvar:  switch(tok)
						  {case tk_number:
							  holdnumber2 = doconstlongmath();
							  next = 0;
							  if( extendable && short_ok(holdnumber2) )
								  {outseg(hsegm,hpost,2);
								  op(128+2+1);
								  op(hrm+basecode);
								  outaddress(hrm,hnumber);
								  op(holdnumber2);}
							  else{outseg(hsegm,hpost,2);
								  op(128+1);
								  op(hrm+basecode);
								  outaddress(hrm,hnumber);
								  outword(holdnumber2);}
							  break;
						  case tk_postnumber:
							  outseg(hsegm,hpost,2);
							  op(128+1);
							  op(hrm+basecode);
							  outaddress(hrm,hnumber);
							  setwordpost();
							  outword((unsigned int)number);
							  break;
						  case tk_reg:
							  outseg(hsegm,hpost,2);
							  op(1+basecode);
							  op(hrm+(unsigned int)number*8);
							  outaddress(hrm,hnumber);
							  break;
						  default:      wordvalexpected();
									break;
						  }
					  break;
	case tk_dwordvar:
	case tk_longvar:  switch(tok)
						  {case tk_number:
							  longholdnumber = doconstdwordmath();
							  next = 0;
							  if( extendable && short_ok(holdnumber2) )
								  {outseg(hsegm,hpost,3);
								  op(0x66);  op(128+2+1);
								  op(hrm+basecode);
								  outaddress(hrm,hnumber);
								  op((int) longholdnumber);}
							  else{outseg(hsegm,hpost,3);
								  op(0x66);  op(128+1);
								  op(hrm+basecode);
								  outaddress(hrm,hnumber);
								  outdword(longholdnumber);}
							  break;
						  case tk_reg32:
							  outseg(hsegm,hpost,3);
							  op(0x66);  op(1+basecode);
							  op(hrm+(unsigned int)number*8);
							  outaddress(hrm,hnumber);
							  break;
						  default:      dwordvalexpected();
									break;
						  }
					  if( cpu < 3 )
						  cpu = 3;
					  break;
	case tk_bytevar:
	case tk_charvar:  switch(tok)
						  {case tk_number:
							  holdnumber2 = doconstlongmath();
							  next = 0;
							  outseg(hsegm,hpost,2);
							  op(128);
							  op(hrm+basecode);
							  outaddress(hrm,hnumber);
							  op(holdnumber2);
							  break;
						  case tk_beg:
							  outseg(hsegm,hpost,2);
							  op(basecode);
							  op(hrm+(unsigned int)number*8);
							  outaddress(hrm,hnumber);
							  break;
						  default:      bytevalexpected();
									break;
						  }
					  break;
	default:  varexpected();  break;
	}
if(next)
	nexttok();
}


asmregmem (out1,out2)   // used for LEA LDS LES LFS LGS LSS.
int out1,out2;
{int holdreg;
nexttok();
if( tok != tk_reg )
    regexpected();
holdreg = (unsigned int)number;
nexttok();
if( tok != tk_camma )
    expected(',');
nexttok();
if( tok != tk_intvar && tok != tk_wordvar && tok != tk_charvar
	&& tok != tk_bytevar && tok != tk_long && tok != tk_dword )
    varexpected();
if(out2==0)
    outseg(segm,post,2);
else outseg(segm,post,3);
op(out1);
if(out2!=0)
    op(out2);
op(rm+holdreg*8);
outaddress(rm,(unsigned int)number);
}


doasmmov ()     // do MOV
{unsigned char next=1,possiblecpu=0;
int htok,hrm,hpost,hsegm;
unsigned int hnumber;
nexttok();
htok = tok;
hrm = rm;
hpost = post;
hnumber = (unsigned int)number;
hsegm = segm;
nexttok();
if(tok != tk_camma)
    expected(',');
nexttok();
switch(htok)
    {case tk_reg:  switch(tok)
		       {case tk_reg:  if(rand() > RAND_MAX/2)
					  {op(0x89);
					  op(128+64+(unsigned int)number*8+hnumber);}
				      else{op(0x8B);
					  op(128+64+hnumber*8+(unsigned int)number);}
				      break;
		       case tk_number:   op(0xB8+hnumber);
					 outword((unsigned int)doconstlongmath());
					 next=0;
					 break;
		       case tk_postnumber:      op(0xB8+hnumber);
					setwordpost();
					outword((unsigned int)number);
					break;
		       case tk_seg:  op(0x8C);
				 op(128+64+(unsigned int)number*8+hnumber);
				 if((unsigned int)number==FS || (unsigned int)number==GS)
					 possiblecpu=3;
				 else if((unsigned int)number==HS || (unsigned int)number==IS)
					 possiblecpu=5;
				 break;
		       case tk_wordvar:
		       case tk_intvar:  outseg(segm,post,2);
					op(0x8B);
					op(rm+hnumber*8);
					outaddress(rm,(unsigned int)number);
					break;
		       default: invalidoperand();  break;
		       }
		   break;
    case tk_beg:  switch(tok)
					   {case tk_beg:  if(rand() > RAND_MAX/2)
										  {op(0x88);
										  op(128+64+(unsigned int)number*8+hnumber);}
									  else{op(0x8A);
										  op(128+64+hnumber*8+(unsigned int)number);}
									  break;
					   case tk_number:      op(0xB0+hnumber);
										op((int)doconstlongmath());
										next=0;
										break;
					   case tk_bytevar:
					   case tk_charvar:      outseg(segm,post,2);
										 op(0x8A);
										 op(rm+hnumber*8);
										 outaddress(rm,(unsigned int)number);
										 break;
					   default: invalidoperand();  break;
					   }
				  break;
    case tk_seg:  switch(tok)
					  {case tk_reg:  op(0x8E);
									 op(128+64+hnumber*8+(unsigned int)number);
									 if(hnumber==FS || hnumber==GS)
										 possiblecpu=3;
									 else if(hnumber==HS || hnumber==IS)
										 possiblecpu=5;
									 break;
					  case tk_wordvar:
					  case tk_intvar:  outseg(segm,post,2);
									   op(0x8E);
									   op(rm+hnumber*8);
									   outaddress(rm,(unsigned int)number);
									   if(hnumber==FS || hnumber==GS)
										   possiblecpu=3;
									   else if(hnumber==HS || hnumber==IS)
										   possiblecpu=5;
									   break;
					  default: invalidoperand();  break;
					  }
				  break;
    case tk_wordvar:
    case tk_intvar:  switch(tok)
						 {case tk_reg:  outseg(hsegm,hpost,2);
										op(0x89);
										op(hrm+(unsigned int)number*8);
										outaddress(hrm,hnumber);
										break;
						 case tk_number:  outseg(hsegm,hpost,2);
										  op(0xC7);
										  op(hrm+(unsigned int)number*8);
										  outaddress(hrm,hnumber);
										  outword((unsigned int)doconstlongmath());
										  next = 0;
										  break;
						 case tk_postnumber:  outseg(hsegm,hpost,2);
											  op(0xC7);
											  op(hrm+(unsigned int)number*8);
											  outaddress(hrm,hnumber);
											  setwordpost();
											  outword((unsigned int)number);
											  break;
						 case tk_seg:  outseg(hsegm,hpost,2);
									   op(0x8C);
									   op(hrm+(unsigned int)number*8);
									   outaddress(hrm,hnumber);
									   if(hnumber==FS || hnumber==GS)
										   possiblecpu=3;
									   else if(hnumber==HS || hnumber==IS)
										   possiblecpu=5;
									   break;
						 default: invalidoperand();     break;
						 }
					 break;
    case tk_dwordvar:
    case tk_longvar:  switch(tok)
						 {case tk_reg32:  outseg(hsegm,hpost,3);
										  op(0x66);      op(0x89);
										  op(hrm+(unsigned int)number*8);
										  outaddress(hrm,hnumber);
										  break;
						 case tk_number:  outseg(hsegm,hpost,3);
										  op(0x66);  op(0xC7);
										  op(hrm+(unsigned int)number*8);
										  outaddress(hrm,hnumber);
										  outdword(doconstdwordmath());
										  next = 0;
										  break;
						 default: invalidoperand();     break;
						 }
					 if( cpu < 3 )
						 cpu = 3;
					 break;
    case tk_charvar:
    case tk_bytevar: switch(tok)
						 {case tk_beg:  outseg(hsegm,hpost,2);
										op(0x88);
										op(hrm+(unsigned int)number*8);
										outaddress(hrm,hnumber);
										break;
						 case tk_number:  outseg(hsegm,hpost,2);
										  op(0xC6);
										  op(hrm+(unsigned int)number*8);
										  outaddress(hrm,hnumber);
										  op((int)doconstlongmath());
										  break;
						 default:  invalidoperand();  break;
						 }
					 break;
    case tk_reg32:      possiblecpu=3;
					switch(tok)
						{case tk_reg32:  op(0x66);
										 if(rand() > RAND_MAX/2)
											 {op(0x89);
											 op(128+64+(unsigned int)number*8+hnumber);}
										 else{op(0x8B);
											 op(128+64+hnumber*8+(unsigned int)number);}
										 break;
						case tk_number:  op(0x66);      op(0xB8+hnumber);
										 outdword(doconstdwordmath());
										 next=0;
										 break;
						case tk_dwordvar:
						case tk_longvar:  outseg(segm,post,3);
										  op(0x66);
										  op(0x8B);
										  op(rm+hnumber*8);
										  outaddress(rm,(unsigned int)number);
										  break;
						case tk_debugreg:
							op(0xF);  op(0x21);
							op(128+64+(unsigned int)number*8+hnumber);
							possiblecpu=4;
							if( (unsigned int)number<=DR3 || (unsigned int)number==DR6 || (unsigned int)number==DR7 )
								possiblecpu=3;
							break;
						case tk_controlreg:
							op(0xF);  op(0x20);
							op(128+64+(unsigned int)number*8+hnumber);
							possiblecpu=4;
							if((unsigned int)number <= CR3)
								possiblecpu=3;
							break;
						case tk_testreg:
							op(0xF);  op(0x24);
							op(128+64+(unsigned int)number*8+hnumber);
							possiblecpu=4;
							if( (unsigned int)number==TR6 || (unsigned int)number==TR7 )
								possiblecpu=3;
							break;
						default:  invalidoperand();  possiblecpu = 0;  break;
						}
					break;
    case tk_debugreg:  if(tok==tk_reg32)
			   {op(0xF);  op(0x23);
			   op(128+64+hnumber*8+(unsigned int)number);
			   possiblecpu=4;
			   if( hnumber<=DR3 || hnumber==DR6 || hnumber==DR7 )
			       possiblecpu=3;
			   }
		       else invalidoperand();
		       break;
    case tk_controlreg:  if(tok==tk_reg32)
			     {op(0xF);  op(0x22);
			     op(128+64+hnumber*8+(unsigned int)number);
			     possiblecpu=4;
			     if(hnumber <= CR3)
				 possiblecpu=3;
			     }
			 else invalidoperand();
			 break;
    case tk_testreg:  if(tok==tk_reg32)
			  {op(0xF);     op(0x26);
			  op(128+64+hnumber*8+(unsigned int)number);
			  possiblecpu=4;
			  if( hnumber==TR6 || hnumber==TR7 )
			      possiblecpu=3;
			  }
		      else invalidoperand();
		      break;
    default:  invalidoperand();  break;
    }
if(possiblecpu > cpu)
    cpu = possiblecpu;
if(next)
    nexttok();
}


/* ***************** start of some quick codes ****************** */

int short_ok (thenumber)
int thenumber;
{if( thenumber <= SHORTMAX && thenumber >= SHORTMIN )
    return(1);
return(0);
}

shortjump (address)
unsigned int address;
{address -= outptr+2;
if( short_ok(address) )
    {op(0xEB);
    op(address);}
else shortjumptoolarge();
}

cbw ()
{op(0x98);
}

cwd ()
{op(0x99);
}

cbd ()
{outword(0x9866);
if( cpu < 3 )
    cpu = 3;
}

cdq ()
{outword(0x9966);
if( cpu < 3 )
    cpu = 3;
}

lodsb ()
{op(0xAC);
}

lodsw ()
{op(0xAD);
}

lodsd ()
{outword(0xAD66);
if(cpu<3)
    cpu = 3;
}

stosb ()
{op(0xAA);
}

stosw ()
{op(0xAB);
}

stosd ()
{outword(0xAB66);
if(cpu<3)
    cpu = 3;
}

movsb ()
{op(0xA4);
}

movsw ()
{op(0xA5);
}

movsd ()
{outword(0xA566);
if(cpu<3)
    cpu = 3;
}


pushds ()  /* produce PUSH DS */
{op(0x1E);
}

pushes ()
{op(0x06);      /* PUSH ES */
}

pushcs ()
{op(0x0E);      /* PUSH CS */
}

pushss ()
{op(0x16);      /* PUSH SS */
}

pushax ()
{op(0x50);      /* PUSH AX */
}

pushbx ()
{op(0x53);      /* PUSH BX */
}

pushcx ()
{op(0x51);      /* PUSH CX */
}

pushdx ()
{op(0x52);      /* PUSH DX */
}

pushdi ()
{op(0x57);      /* PUSH DI */
}

pushsi ()
{op(0x56);      /* PUSH SI */
}

pushbp ()
{op(0x55);      /* PUSH BP */
}

pushsp ()
{op(0x54);      /* PUSH BP */
}

pushf ()   /* produce PUSHF */
{op(0x9C);
}


popf ()    /* produce POPF */
{op(0x9D);
}

popds ()   /* produce POP DS */
{op(0x1F);
}

popes ()   /* produce POP ES */
{op(0x07);
}

popss ()   /* produce POP SS */
{op(0x17);
}

popax ()   /* produce POP AX */
{op(0x58);
}

popbx ()   /* produce POP BX */
{op(0x5B);
}

popcx ()   /* produce POP CX */
{op(0x59);
}

popdx ()   /* produce POP DX */
{op(0x5A);
}

popsi ()   /* produce POP SI */
{op(0x5E);
}

popdi ()   /* produce POP DI */
{op(0x5F);
}

popbp ()   /* produce POP BP */
{op(0x5D);
}

popsp ()   /* produce POP SP */
{op(0x5C);
}


ret()    /* produce RET */
{op(0xC3);
}

retnum (num)    /* produce RET # */
unsigned int num;
{op(0xC2);
outword(num);
}

retf()   /* produce RETF */
{op(0xCB);
}

retfnum (num)    /* produce RETF # */
unsigned int num;
{op(0xCA);
outword(num);
}

iret()   /* produce IRET */
{op(0xCF);
}

intnum(num)  /* produce INT num */
int num;
{if(num==3)
    op(0xCC);
else{op(0xCD);
    op( num & 255 );}
}

jumploc (loc)     /* produce JUMP # */
unsigned int loc;
{loc = loc-outptr-3;
op(0xE9);
outword(loc);
}

callloc (loc)   /* produce CALL # */
unsigned int loc;
{loc = loc-outptr-3;
op(0xE8);
outword(loc);
}

movalnum(num)   /* produce MOV AL,# */
unsigned int num;
{op(0xB0);
op(num);
}

movahnum(num)   /* produce MOV AH,# */
unsigned int num;
{op(0xB4);
op(num);
}

intdos (v1,v2)    /* produce INT 21h with update of required DOS version */
int v1,v2;
{dosrequired(v1,v2);
outword(0x21CD);  /* INT 21h */
}

dosrequired (v1,v2)
int v1,v2;
{if( (v1>dos1) || (v1==dos1&&v2>dos2) )
    {dos1 = v1;
    dos2 = v2;}
}

xorAHAH ()   /* produce XOR AH,AH */
{outword(0xE430);
}

xorAXAX ()   /* produce XOR AX,AX */
{outword(0xC031);
}

xorEAXEAX ()   /* produce XOR EAX,EAX */
{outword(0x3166); op(0xC0);
if( cpu < 3 )
    cpu = 3;
}

xorDXDX ()   /* produce XOR DX,DX */
{outword(0xD231);
}

xorEDXEDX ()   /* produce XOR EDX,EDX */
{op(0x66);  outword(0xD231);
if( cpu < 3 )
    cpu = 3;
}

/******************* compiler directives start ****************************/

int get_directive_value ()   // return the 0 or 1 value for directive
{nexttok();
if(tok==tk_number)
    {if( doconstlongmath() )
	return(1);
    return(0);
    }
numexpected();
nexttok();
return(0);
}


int directive()
{unsigned char next = 1;
char *holdinput;
unsigned int holdinptr,holdendinptr,holdlinenum;
unsigned char holdcha,holdendoffile,gotoendif=0;
unsigned char holdfilename[FILENAMESIZE];
char holdid[IDLENGTH];
long longhold,longhold2;

nexttok();
if(tok == tk_id)
    {
    if(strcmp("align",string)==0)
	{if( notdoneprestuff )
	    doprestuff();
	if( outptrdata%2 == 1)
	    {opd(aligner);
	    alignersize++;}
	}
    else if(strcmp("aligner",string)==0)
	{nexttok();
	if(tok==tk_number)
	    aligner = doconstlongmath();
	else{numexpected();
	    nexttok();}
	next = 0;}
    else if(strcmp("alignword",string)==0)
	{alignword = get_directive_value();
	next = 0;}
    else if(strcmp("assumeDSSS",string)==0)
	{assumeDSSS = get_directive_value();
	next = 0;}
    else if(strcmp("beep",string)==0)
	printf("\x07");
    else if(strcmp("codesize",string)==0)
	optimizespeed = 0;
    else if(strcmp("ctrl_c",string)==0)
	{killctrlc = get_directive_value();
	next = 0;}
    else if(strcmp("define",string)==0)
	{nexttok();
	if( tok==tk_id || tok==tk_ID )
	    {strcpy(holdid, string);
	    nexttok();
	    switch(tok)
		{case tk_eof:    unexpectedeof();  break;
		case tk_number: addconsttotree(holdid,doconstlongmath());
				 next = 0;
				 break;
		case tk_minus:  if(tok2 == tk_number)
				    {addconsttotree(holdid,doconstlongmath());
				    next = 0;
				    break;}
		case tk_undefregproc:   tok = tk_ID;
				       addtotree(holdid);
				       break;
		case tk_undefproc:  tok = tk_id;
		default:            addtotree(holdid);
				    break;
		}
	    }
	else idexpected();
	}
    else if(strcmp("DOSrequired",string)==0)
	{nexttok();
	if(tok==tk_number)
	    {longhold = doconstlongmath();
	    longhold2 = dos1*256 + dos2;
	    if( longhold > longhold2 )
		{dos1 = longhold / 256;
		dos2 = longhold % 256;}
	    }
	else{numexpected();
	    nexttok();}
	next = 0;
	}
    else if(strcmp("endif",string)==0)
	{if(endifcount==0)
	    preerror("?endif without preceeding ?if");
	else endifcount--;}
    else if(strcmp("ifdef",string)==0)
	{nexttok();
	if( tok == tk_id || tok == tk_ID )
	    gotoendif = 1;
	endifcount++;
	}
    else if(strcmp("ifndef",string)==0)
	{nexttok();
	if( tok != tk_id && tok != tk_ID )
	    gotoendif = 1;
	endifcount++;
	}
    else if(strcmp("include",string)==0)
	{nexttok();
	if(tok==tk_string)
	    {holdinput = input;
	    holdinptr = inptr2;
	    holdcha = cha2;
	    holdlinenum = linenum2;
	    holdendinptr = endinptr;
	    holdendoffile = endoffile;
	    strcpy(holdfilename,currentfilename);
	    compilefile(string,0);
	    strcpy(currentfilename,holdfilename);
	    endoffile = holdendoffile;
	    endinptr = holdendinptr;
	    input = holdinput;
	    inptr2 = holdinptr;
	    cha2 = holdcha;
	    linenum2 = holdlinenum;
	    }
	else stringexpected();
	}
    else if(strcmp("jumptomain",string)==0)
	{if( notdoneprestuff==0 )
	    preerror("Too late to change jumptomain option");
	if( comfile == file_exe )
	    preerror("EXE jump to main() type can not be changed");
	nexttok();
	switch( tok )
	    {case tk_number:  jumptomain = CALL_NEAR;
			      if( (unsigned int)number == 0 )
				  {jumptomain = CALL_NONE;
				  header = 0;}
			      break;
	    case tk_ID:
	    case tk_id:  if( stricmp(string,"NONE")==0 )
			     {jumptomain = CALL_NONE;
			     header = 0;}
			 else if( stricmp(string,"SHORT")==0 )
			     {jumptomain = CALL_SHORT;
			     header = 1;}
			 else if( stricmp(string,"NEAR")==0 )
			     {jumptomain = CALL_NEAR;
			     header = 1;}
			 else preerror("Unknown jumptomain option");
			 break;
	    default:  preerror("Unknown jumptomain option");
		      break;
	    }
	}
    else if(strcmp("maxerrors",string)==0)
	{nexttok();
	if(tok==tk_number)
	    {maxerrors = doconstlongmath();
	    if( error >= maxerrors )
		toomanyerrors();
	    }
	else{numexpected();
	    nexttok();}
	next = 0;
	}
    else if(strcmp("parsecommandline",string)==0)
	{if( notdoneprestuff==0 )
	    preerror("Too late to change parsecommandline option");
	if( comfile == file_exe )
	    preerror("parsecommandline option invalid for EXE run files");
	parsecommandline = get_directive_value();
	next = 0;}
    else if(strcmp("pause",string)==0)
	_getch();  // wait for a key to be pressed.
    else if(strcmp("print",string)==0)
	{nexttok();
	switch(tok)
	    {case tk_string:  printf("%s",string);
			      if(makemapfile)
				  fprintf(mapfile,"%s",string);
			      break;
	    case tk_number:  printf("%u",string);
			     if(makemapfile)
				 fprintf(mapfile,"%u",string);
			     break;
	    default:  preerror("unsupported token for ?print");  break;
	    }
	}
    else if(strcmp("printhex",string)==0)
	{nexttok();
	if( tok == tk_number )
	    {printf("%X",string);
	    if(makemapfile)
		fprintf(mapfile,"%X",string);
	    }
	else numexpected();
	}
    else if(strcmp("randombyte",string)==0)
	{if( notdoneprestuff )
	    doprestuff();
	op(rand());}
    else if(strcmp("resize",string)==0)
	{if( notdoneprestuff==0 )
	    preerror("Too late to change resize option");
	if( comfile == file_exe )
	    preerror("resize option invalid for EXE run files");
	resizemem = get_directive_value();
	next = 0;}
    else if(strcmp("resizemessage",string)==0)
	{if( notdoneprestuff==0 )
	    preerror("Too late to change the resize memory error message");
	nexttok();
	if(tok==tk_string)
	    {if(resizemessage != NULL)
		free(resizemessage);
	    resizemessage = (unsigned char *) malloc(strlen(string)+2);
	    if(resizemessage == NULL)
		preerror("Not enough memory to store resize memory error message");
	    else sprintf(resizemessage,"%s$",string);
	    }
	else stringexpected();
	nexttok();
	next = 0;}
    else if(strcmp("speed",string)==0)
	optimizespeed = 1;
    else if(strcmp("stack",string)==0)
	{if( notdoneprestuff==0 )
	    preerror("Too late to change stack size");
	nexttok();
	if(tok==tk_number)
	    {longhold = doconstlongmath();
	    if( longhold > 0xFEFF || longhold < 0 )
		preerror("Invalid size for stack");
	    else stacksize = longhold;
	    }
	else{numexpected();
	    nexttok();}
	next = 0;
	}
    else if(strcmp("startaddress",string)==0)
	{if( notdoneprestuff==0 )
	    preerror("Too late to change code start address");
	nexttok();
	if(comfile != file_exe)
	    {if(tok==tk_number)
		startptr = doconstlongmath();
	    else{numexpected();
		nexttok();}
	    outptr = startptr;
	    outptrdata = outptr;}
	else preerror("Start address cannot be changed for EXE output files");
	next = 0;
	}
    else if(strcmp("use8086",string)==0 || strcmp("use8088",string)==0)
	chip = 0;
    else if(strcmp("use80186",string)==0)
	chip = 1;
    else if(strcmp("use80286",string)==0)
	chip = 2;
    else if(strcmp("use80386",string)==0)
	chip = 3;
    else if(strcmp("use80486",string)==0)
	chip = 4;
    else if(strcmp("use80586",string)==0)
	chip = 5;
    else if(strcmp("use80686",string)==0)
	chip = 6;
    else if(strcmp("use80786",string)==0)
	chip = 7;
    else directiveexpected();
    }
else if( tok == tk_if )
    {nexttok();
    switch(tok)
	{
	default:  notyet();
		  break;
	}
    endifcount++;
    }
else directiveexpected();
if(next)
    nexttok();
while( gotoendif )
    {if( tok == tk_question )
	{nexttok();
	if( tok == tk_id )
	    {if(strcmp(string,"endif")==0)
		{gotoendif--;
		endifcount--;}
	    else if(strcmp(string,"ifdef")==0 || strcmp(string,"ifndef")==0)
		{gotoendif++;
		endifcount++;}
	    nexttok();
	    }
	else if( tok == tk_if )
	    {gotoendif++;
	    endifcount++;
	    nexttok();}
	}
    else if( tok == tk_eof )
	{unexpectedeof();
	gotoendif=0;}
    else nexttok();
    }
}


doenum()
{long int counter=0;
unsigned char next;
unsigned char holdid[IDLENGTH];
nexttok();
if( tok != tk_openbrace )
    expected('{');
do {next = 1;
    nexttok();
    if( tok == tk_ID || tok == tk_id )
	{strcpy(holdid, string);
	if( tok2 == tk_assign )
	    {nexttok();
	    nexttok();
	    if( tok == tk_number )
		{counter = doconstlongmath();
		next=0;}
	    else numexpected();
	    }
	addconsttotree(holdid,counter);
	counter++;
	}
    else idexpected();
    if(next)
	nexttok();
    } while( tok == tk_camma );
if( tok != tk_closebrace )
    expected('}');
nextseminext();
}


/* end of TOKR.C */
