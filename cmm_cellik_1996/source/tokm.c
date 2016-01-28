/* macros, REG and stack procedures for SPHINX C-- Compiler */
/* property of PETER CELLIK copyright (C) 1994 all rights reserved */
/* SPHINX Programming 1994 */
/* last modified 3 July 1994 */

/*
Things to do:
   STRTOWORD( ,stringloc);
   INSERTBYTE(bytevalue,array,length,position);
   INSERTWORD(wordvalue,array,length,position);
   SILENCEINT24H();
   STACKAVAILABLE();
   CLEARSCREEN(pagenumber);     for vid modes 0, 1, 2, 3, 0xE etc...
   STACKAVAILABLE();
   PRINTSTR(), PRINTPAS(), PRINTWORD() etc...   for writing to STDOUT
   WRITEPAS();
*/


#include <stdio.h>
#include <memory.h>
#include "tok.h"

extern unsigned int outptr,outptrdata;
extern unsigned char *output;
extern int dos1,dos2;
extern unsigned char cpu,alignword,aligner,parsecommandline,comfile;
extern unsigned int newcommandlineaddr,postsize;
extern unsigned char string[];
extern unsigned char assumeDSSS,optimizespeed;



outprocedure (array,length)
unsigned char *array;
unsigned int length;
{long longhold;
longhold = outptr;
if( longhold + length > MAXDATA )
	maxoutputerror();
memcpy( output+outptr, array, length );
outptr += length;
if(comfile != file_exe)
    outptrdata = outptr;
}


void movESDS ()     // get the value of DS into ES, AX may be destroyed
{if(optimizespeed)
    {outword(0xD88C);  /* MOV AX,DS */
    outword(0xC08E);}  /* MOV ES,AX */
else{pushds();
    popes();}
}


/******  REG proc and @ command including procedures start      ******/

// start of REG proc and @ array constants

unsigned char aaBYTETODIGITS[] = {
	0xB1,0x64,0xF6,0xF1,0x4,0x30,0x88,0x7,0x88,0xE0,
	0x30,0xE4,0xB1,0xA,0xF6,0xF1,0x4,0x30,0x88,0x47,
	0x1,0x80,0xC4,0x30,0x88,0x67,0x2,0xC6,0x47,0x3,
	0x0};
#define BYTETODIGITSLEN 31

unsigned char aaBYTETOHEX[] = {
	0x88,0xC4,0x80,0xE4,0xF,0x80,0xFC,0x9,0x76,0x3,
	0x80,0xC4,0x7,0x80,0xC4,0x30,0x88,0x67,0x1,0xB1,
	0x4,0xD2,0xE8,0x3C,0x9,0x76,0x2,0x4,0x7,0x4,
	0x30,0x88,0x7,0xC6,0x47,0x2,0x0};
#define BYTETOHEXLEN 37

unsigned char aaGETBORDERCOLOR[] = {
	0xB8,0x8,0x10,0xCD,0x10,0x88,0xF8};
#define GETBORDERCOLORLEN 7

unsigned char aaGETCPU[] = {
	0x9C,0xB2,0x0,0x54,0x58,0x39,0xC4,0x75,0x49,0xB2,
	0x2,0x9C,0x58,0xD,0x0,0x40,0x50,0x9D,0x9C,0x58,
	0xA9,0x0,0x40,0x74,0x39,0xB2,0x3,0x66,0x8B,0xDC,
	0x66,0x83,0xE4,0xFC,0x66,0x9C,0x66,0x58,0x66,0x8B,
	0xC8,0x66,0x35,0x0,0x0,0x4,0x0,0x66,0x50,0x66,
	0x9D,0x66,0x9C,0x66,0x58,0x66,0x25,0x0,0x0,0x4,
	0x0,0x66,0x81,0xE1,0x0,0x0,0x4,0x0,0x66,0x3B,
	0xC1,0x74,0x4,0xB2,0x4,0x66,0x51,0x66,0x9D,0x66,
	0x8B,0xE3,0x88,0xD0,0x9D};
#define GETCPULEN 85

unsigned char aaGETINTVECT[] = {
	0x1,0xDB,0x1,0xDB,0x31,0xD2,0x8E,0xC2,0x26,0x8B,
	0x17,0x26,0x8B,0x4F,0x2,0x89,0xC3,0x89,0x17,0x89,
	0x4F,0x2};
#define GETINTVECTLEN 22

unsigned char aaGETVGAPALETTE[] = {
	0x8C,0xD8,0x8E,0xC0,0xB8,0x17,0x10,0xCD,0x10};
#define GETVGAPALETTELEN 9

unsigned char aaKBHIT[] = {
	0x1E,0xB8,0x0,0x0,0x8E,0xD8,0xA1,0x1A,0x4,0x2B,
	0x6,0x1C,0x4,0x1F};
#define KBHITLEN 14

unsigned char aaLOAD8X8FONT[] = {
	0xB8,0x12,0x11,0xB3,0x00,0xCD,0x10};
#define LOAD8X8FONTLEN 7

unsigned char aaLOAD8X14FONT[] = {
	0xB8,0x11,0x11,0xB3,0x00,0xCD,0x10};
#define LOAD8X14FONTLEN 7

unsigned char aaLOAD8X16FONT[] = {
	0xB8,0x14,0x11,0xB3,0x00,0xCD,0x10};
#define LOAD8X16FONTLEN 7

unsigned char aaMAXKEYRATE[] = {
	0xB8,0x5,0x3,0x31,0xDB,0xCD,0x16};
#define MAXKEYRATELEN 7

unsigned char aaSETATIMASK[] = {
	0xB4,0x4A,0xBB,0x0,0x10,0xCD,0x21,0x73,0x1,0xC3,
	0xBA,0xCE,0x1,0xB0,0xB2,0x9C,0xFA,0xEE,0xFE,0xC2,
	0xEC,0x8A,0xE0,0x80,0xE4,0xE1,0xD0,0xE3,0xA,0xE3,
	0xB0,0xB2,0xFE,0xCA,0xEF,0x9D};
#define SETATIMASKLEN 36

unsigned char aaSETBORDERCOLOR[] = { 
	0x88,0xC7,0xB8,0x1,0x10,0xCD,0x10};
#define SETBORDERCOLORLEN 7

unsigned char aaSETCURRENTDIR[] = { 
	0x89,0xC2,0xB4,0x3B,0xCD,0x21,0x72,0x2,0x31,0xC0};
#define SETCURRENTDIRLEN 10

unsigned char aaSETCURRENTDRIVE[] = {
	0x88,0xC2,0xB4,0xE,0xCD,0x21,0x38,0xD0,0x7C,0x2,
	0x31,0xC0};
#define SETCURRENTDRIVELEN 12

unsigned char aaSETINTVECT[] = {
	0x1,0xDB,0x1,0xDB,0x31,0xC0,0x8E,0xC0,0xFA,0x26,
	0x89,0x17,0x26,0x89,0x4F,0x2,0xFB};
#define SETINTVECTLEN 17

unsigned char aaSETPANREG[] = {
	0xBA,0xDA,0x3,0xEC,0xBA,0xC0,0x3,0xB0,0x33,0xEE,
	0x88,0xD8,0xEE};
#define SETPANREGLEN 13

unsigned char aaSETREADMASK[] = {
	0xBA,0xCE,0x3,0xB0,0x4,0xEE,0x42,0x88,0xD8,0xEE};
#define SETREADMASKLEN 10

unsigned char aaSETSCREENWIDTH[] = {
	0xBA,0xD4,0x3,0xB0,0x13,0xEE,0x88,0xD8,0x42,0xEE};
#define SETSCREENWIDTHLEN 10

unsigned char aaSETSCREENOFFSET[] = {
	0xBA,0xD4,0x3,0xB0,0xC,0xFA,0xEE,0x42,0x88,0xF8,
	0xEE,0xB0,0xD,0x4A,0xEE,0x42,0x88,0xD8,0xEE,0xFB};
#define SETSCREENOFFSETLEN 20

unsigned char aaSETSCREENOFFSETM[] = {
	0xBA,0xB4,0x3,0xB0,0xC,0xFA,0xEE,0x42,0x88,0xF8,
	0xEE,0xB0,0xD,0x4A,0xEE,0x42,0x88,0xD8,0xEE,0xFB};
#define SETSCREENOFFSETMLEN 20

unsigned char aaSETTICKINTERVAL[] = {
	0xB0,0x36,0xFA,0xE6,0x43,0x88,0xD8,0xE6,0x40,0x88,
	0xF8,0xE6,0x40,0xFB};
#define SETTICKINTERVALLEN 14

unsigned char aaSETVGADAC[] = {
	0xBA,0xC8,0x3,0xEE,0xBA,0xC9,0x3,0xF3,0x6E};
#define SETVGADACLEN 9

unsigned char aaSETWRITEMASK[] = {
	0xBA,0xC4,0x3,0xB0,0x2,0xEE,0x42,0x88,0xD8,0xEE};
#define SETWRITEMASKLEN 10

unsigned char aaSHIFTSTATUS[] = {
	0x31,0xC0,0x8E,0xC0,0x26,0xA1,0x17,0x4};
#define SHIFTSTATUSLEN 8

unsigned char aaTURNOFFCHAIN4[] = {
	0xBA,0xC4,0x3,0xB0,0x4,0xEE,0x42,0xEC,0x24,0xF7,
	0xC,0x4,0xEE,0xBA,0xCE,0x3,0xB0,0x5,0xEE,0x42,
	0xEC,0x24,0xEF,0xEE,0x4A,0xB0,0x6,0xEE,0x42,0xEC,
	0x24,0xFD,0xEE,0xBA,0xD4,0x3,0xB0,0x14,0xEE,0x42,
	0xEC,0x24,0xBF,0xEE,0x4A,0xB0,0x17,0xEE,0x42,0xEC,
	0xC,0x40,0xEE};
#define TURNOFFCHAIN4LEN 53

unsigned char aaWAITVSYNC[] = { 
	0xBA,0xDA,0x3,0xEC,0x24,0x8,0x75,0xFB,0xEC,0x24,
	0x8,0x74,0xFB};
#define WAITVSYNCLEN 13

unsigned char aaWORDTODIGITS[] = {
	0x31,0xD2,0xB9,0x10,0x27,0xF7,0xF1,0x4,0x30,0x88,
	0x7,0x89,0xD0,0x31,0xD2,0xB9,0xE8,0x3,0xF7,0xF1,
	0x4,0x30,0x88,0x47,0x1,0x89,0xD0,0xB1,0x64,0xF6,
	0xF1,0x4,0x30,0x88,0x47,0x2,0x88,0xE0,0x30,0xE4,
	0xB1,0xA,0xF6,0xF1,0x4,0x30,0x88,0x47,0x3,0x80,
	0xC4,0x30,0x88,0x67,0x4,0xC6,0x47,0x5,0x0};
#define WORDTODIGITSLEN 59



int includeit(type)
int type;
{int retvalue;
switch(string[0])
    {
    case 'B':  retvalue = iib();  break;
    case 'C':  retvalue = iic();  break;
    case 'F':  retvalue = iif();  break;
    case 'G':  retvalue = iig();  break;
    case 'K':  retvalue = iik();  break;
    case 'L':  retvalue = iil();  break;
    case 'M':  retvalue = iim();  break;
    case 'P':  retvalue = iip();  break;
    case 'R':  retvalue = iir();  break;
    case 'S':  retvalue = iis();  break;
    case 'T':  retvalue = iit();  break;
    case 'W':  retvalue = iiw();  break;
    default:  retvalue = -1;  break;
    }
if( retvalue==0 && type==1 )  /* if it is a REG Proc not a MACRO */
    ret();
return( retvalue );
}

int iib ()
{
if(strcmp("YTETODIGITS",string+1)==0)
	outprocedure(aaBYTETODIGITS,BYTETODIGITSLEN);
else if(strcmp("YTETOHEX",string+1)==0)
	outprocedure(aaBYTETOHEX,BYTETOHEXLEN);
else return(-1);
return(0);
}

int iic ()
{if(strcmp("LEARSCREEN0",string+1)==0 || strcmp("LEARSCREEN1",string+1)==0)
    mmclearscreen(0xB800,0x3E8,0x0200);
else if(strcmp("LEARSCREEN2",string+1)==0 || strcmp("LEARSCREEN3",string+1)==0)
    mmclearscreen(0xB800,0x7D0,0x0200);
else if(strcmp("LEARSCREEN4",string+1)==0 || strcmp("LEARSCREEN5",string+1)==0||
	strcmp("LEARSCREEN6",string+1)==0)
    mmclearscreen(0xB800,0x2000,0x0000);
else if(strcmp("LEARSCREEN7",string+1)==0)
    mmclearscreen(0xB000,0x7D0,0x0700);
else if(strcmp("LEARSCREEN17",string+1)==0)
    mmclearscreen(0xA000,0x4B00,0x0000);
else if(strcmp("LEARWINDOW",string+1)==0)
    {op(0xB8); op(0x00); op(0x06);  /* MOV AX,0600 */
    op(0x88); op(0xDF);             /* MOV BH,BL */
    intnum(0x10);}                 
else if(strcmp("LICKOFF",string+1)==0)
    {op(0xB8); outword(0x0400);
    intnum(0x16);}
else if(strcmp("LICKON",string+1)==0)
    {op(0xB8); outword(0x0401);
    intnum(0x16);}
else if(strcmp("OPYBACKWARDFAR",string+1)==0)
	mmcopymem(1,1,1);
else if(strcmp("OPYBACKWARDFARD",string+1)==0)
	mmcopymem(1,1,4);
else if(strcmp("OPYBACKWARDFARW",string+1)==0)
	mmcopymem(1,1,2);
else if(strcmp("OPYBACKWARDNEAR",string+1)==0)
	mmcopymem(0,1,1);
else if(strcmp("OPYBACKWARDNEARD",string+1)==0)
	mmcopymem(0,1,4);
else if(strcmp("OPYBACKWARDNEARW",string+1)==0)
	mmcopymem(0,1,2);
else if(strcmp("OPYFAR",string+1)==0)
	mmcopymem(1,0,1);
else if(strcmp("OPYFARD",string+1)==0)
	mmcopymem(1,0,4);
else if(strcmp("OPYFARW",string+1)==0)
	mmcopymem(1,0,2);
else if(strcmp("OPYNEAR",string+1)==0)
	mmcopymem(0,0,1);
else if(strcmp("OPYNEARD",string+1)==0)
	mmcopymem(0,0,4);
else if(strcmp("OPYNEARW",string+1)==0)
	mmcopymem(0,0,2);
else return(-1);
return(0);
}

int iif ()
{if(strcmp("CLOSE",string+1)==0)
	mmdos2ahcarry(0x3E,0);
else if(strcmp("CREATE",string+1)==0)
	mmdos2ahcarry(0x3C,1);
else if(strcmp("DELETE",string+1)==0)
	mmdos2ahcarry(0x41,0);
else if(strcmp("HANDLE",string+1)==0)
	mmdos2ahcarry(0x45,1);
else if(strcmp("INDFIRSTFILE",string+1)==0)
	mmdos2ahcarry(0x4E,0);
else if(strcmp("INDNEXTFILE",string+1)==0)
	mmdos2ahcarry(0x4F,0);
else if(strcmp("OPEN",string+1)==0)
	mmdos2ahcarry(0x3D,1);
else if(strcmp("READ",string+1)==0)
	mmdos2ahcarry(0x3F,1);
else if(strcmp("REEMEM",string+1)==0)
	{op(0x8E); op(0xC0);      /* MOV ES,AX */
	mmdos2ahcarry(0x49,0);}
else if(strcmp("WRITE",string+1)==0)
	mmdos2ahcarry(0x40,1);
else return(-1);
return(0);
}

int iig ()
{if(strcmp("ETBORDERCOLOR",string+1)==0)
	outprocedure(aaGETBORDERCOLOR,GETBORDERCOLORLEN);
else if(strcmp("ETCONFIG",string+1)==0)
	intnum(0x11);
else if(strcmp("ETCOLORPAGESTATE",string+1)==0)
	{op(0xB4);  op(0x10);
	op(0xB0);  op(0x1A);
	intnum(0x10);}
else if(strcmp("ETCPU",string+1)==0)
	outprocedure(aaGETCPU,GETCPULEN);
else if(strcmp("ETDEFAULTDRIVE",string+1)==0)   
	dos1function(0x19);
else if(strcmp("ETDEFAULTINFO",string+1)==0)
	dos1function(0x1B);
else if(strcmp("ETDOSVERSION",string+1)==0)
	{op(0xB8); op(0x00); op(0x30);
	intdos(1,0);}
else if(strcmp("ETINTVECT",string+1)==0)
	outprocedure(aaGETINTVECT,GETINTVECTLEN);
else if(strcmp("ETMEMSIZE",string+1)==0)
	intnum(0x12);
else if(strcmp("ETMEM",string+1)==0)
	{op(0x89);      op(0xC3);       /* MOV BX,AX */
	mmdos2ahcarry(0x48,1);}
else if(strcmp("ETPIXEL",string+1)==0)
	{movahnum(0x0D);
	intnum(0x10);}
else if(strcmp("ETRETURNCODE",string+1)==0)
	{op(0xB4);  op(0x4D);
	intdos(2,0);}
else if(strcmp("ETTEXTPOSITION",string+1)==0)
	{movahnum(0x03);
	intnum(0x10);}
else if(strcmp("ETVERIFYFLAG",string+1)==0)
	{op(0xB4);  op(0x54);
	intdos(2,0);}
else if(strcmp("ETVGAPALETTE",string+1)==0)
	outprocedure(aaGETVGAPALETTE,GETVGAPALETTELEN);
else return(-1);
return(0);
}

int iik ()
{if(strcmp("BHIT",string+1)==0)
    outprocedure(aaKBHIT,KBHITLEN);
else if(strcmp("EYCHECK",string+1)==0)
	dos1function(0x0B);
else return(-1);
return(0);
}

int iil ()
{if(strcmp("OAD8X8FONT",string+1)==0)
    outprocedure(aaLOAD8X8FONT,LOAD8X8FONTLEN);
else if(strcmp("OAD8X14FONT",string+1)==0)
    outprocedure(aaLOAD8X14FONT,LOAD8X14FONTLEN);
else if(strcmp("OAD8X16FONT",string+1)==0)
    outprocedure(aaLOAD8X16FONT,LOAD8X16FONTLEN);
else return(-1);
return(0);
}

int iim ()
{if(strcmp("AXKEYRATE",string+1)==0)
    outprocedure(aaMAXKEYRATE,MAXKEYRATELEN);
else return(-1);
return(0);
}

int iip ()
{if(strcmp("ARAMCOUNT",string+1)==0)
    {checkparam();
    op(0xA1); setwordpost();
    outword(newcommandlineaddr);}       /* MOV AX,[newcommandlineaddr] */
else if(strcmp("ARAMSTR",string+1)==0)
    mmparamstr();
else if(strcmp("UTPIXEL",string+1)==0)
	{movahnum(0x0C);
	intnum(0x10);}
else return(-1);
return(0);
}

int iir ()
{if(strcmp("EADSTRING",string+1)==0)
    dos1function(0x0A);
else if(strcmp("ESIZEMEM",string+1)==0)
    {outword(0xC08E);   /* MOV ES,AX */
    mmdos2ahcarry(0x4A,0);}
else return(-1);
return(0);
}

int iis ()
{if(strcmp("ETATIMASK",string+1)==0)
	outprocedure(aaSETATIMASK,SETATIMASKLEN);
else if(strcmp("ETBORDERCOLOR",string+1)==0)
	outprocedure(aaSETBORDERCOLOR,SETBORDERCOLORLEN);
else if(strcmp("ETCURRENTDIR",string+1)==0)
	{outprocedure(aaSETCURRENTDIR,SETCURRENTDIRLEN);
	dosrequired(2,0);}
else if(strcmp("ETCURRENTDRIVE",string+1)==0)
	{outprocedure(aaSETCURRENTDRIVE,SETCURRENTDRIVELEN);
	dosrequired(1,0);}
else if(strcmp("ETDISPLAYPAGE",string+1)==0)
	{op(0xB4); op(0x05); /* MOV AH,0x5 */
	op(0xCD); op(0x10);}
else if(strcmp("ETINTVECT",string+1)==0)
	outprocedure(aaSETINTVECT,SETINTVECTLEN);
else if(strcmp("ETPANREG",string+1)==0)
	outprocedure(aaSETPANREG,SETPANREGLEN);
else if(strcmp("ETREADMASK",string+1)==0)
	outprocedure(aaSETREADMASK,SETREADMASKLEN);
else if(strcmp("ETSCREENOFFSET",string+1)==0)
	outprocedure(aaSETSCREENOFFSET,SETSCREENOFFSETLEN);
else if(strcmp("ETSCREENOFFSETM",string+1)==0)
	outprocedure(aaSETSCREENOFFSETM,SETSCREENOFFSETMLEN);
else if(strcmp("ETSCREENWIDTH",string+1)==0)
	outprocedure(aaSETSCREENWIDTH,SETSCREENWIDTHLEN);
else if(strcmp("ETTEXTPOSITION",string+1)==0)
	{movahnum(0x02);
	intnum(0x10);}
else if(strcmp("ETTICKINTERVAL",string+1)==0)
	outprocedure(aaSETTICKINTERVAL,SETTICKINTERVALLEN);
else if(strcmp("ETVGADAC",string+1)==0)
	{outprocedure(aaSETVGADAC,SETVGADACLEN);
	if(cpu < 2)
	    cpu = 2;
	}
else if(strcmp("ETWRITEMASK",string+1)==0)
	outprocedure(aaSETWRITEMASK,SETWRITEMASKLEN);
else if(strcmp("HIFTSTATUS",string+1)==0)
	outprocedure(aaSHIFTSTATUS,SHIFTSTATUSLEN);
else return(-1);
return(0);
}

int iit ()
{if(strcmp("EXTROWS",string+1)==0)
	mmtextrows();
else if(strcmp("URNOFFCHAIN4",string+1)==0)
	outprocedure(aaTURNOFFCHAIN4,TURNOFFCHAIN4LEN);
else return(-1);
return(0);
}

int iiw ()
{if(strcmp("AITVSYNC",string+1)==0)
	outprocedure(aaWAITVSYNC,WAITVSYNCLEN);
else if(strcmp("ORDTODIGITS",string+1)==0)
	outprocedure(aaWORDTODIGITS,WORDTODIGITSLEN);
else return(-1);
return(0);
}


dos1function(ahvalue)
int ahvalue;
{op(0xB4); op(ahvalue);
intdos(1,0);
}


outparsecommandline ()
{if(optimizespeed)
    {op(0x8C); op(0xC8);  /* MOV AX,CS */
    op(0x8E); op(0xC0);}  /* MOV ES,AX */
else{pushcs();
    popes();}
newcommandlineaddr = postsize;
postsize += 130;
op(0xBF); setwordpost();
outword(newcommandlineaddr+2);  /* MOV DI,newcommandlineaddr+2 */
op(0xBE); outword(0x81);        /* MOV SI,0x81 */
op(0x31); op(0xD2);             /* XOR DX,DX */
op(0xBB); outword(1);           /* MOV BX,1 */
lodsb();              /* here: LODSB */
op(0x3C); op(0x20);   /* CMP AL,32 */
op(0x74); op(0xFB);   /* JE here: */
op(0x3C); op(0x0D);   /* here2: CMP AL,13 */
op(0x74); op(0x19);   /* JE done: */
op(0x3C); op(0x20);   /* CMP AL,32 */
op(0x74); op(0x08);   /* JE next: */
stosb();
lodsb();
op(0x01); op(0xDA);   /* ADD DX,BX */
op(0x31); op(0xDB);   /* XOR BX,BX */
op(0xEB); op(0xF0);   /* JMP here2: */
op(0xBB); outword(1); /* next: MOV BX,1 */
op(0xB0); op(0x00);   /* MOV AL,0 */
stosb();
lodsb();                   /* here3: LODSB */
op(0x3C); op(0x20);        /* CMP AL,32 */
op(0x74); op(0xFB);        /* JE next3: */
op(0xEB); op(0xE3);        /* JMP here2: */
op(0x89); op(0x16);  setwordpost();
outword(newcommandlineaddr); /* done: MOV [newcommandlineaddr],DX */
op(0x31); op(0xC0);        /* XOR AX,AX */
stosw();
}


checkparam ()
{if(!parsecommandline)
	preerror("?parsecommandline must be set to TRUE for PARAMCOUNT or PARAMSTR");
}


mmparamstr ()
{checkparam();
op(0xBB); setwordpost();
outword(newcommandlineaddr+2);  /* MOV BX,newcommandlineaddr+2 */
op(0x3D); outword(0);           /* CMP AX,0 */
op(0x74); op(0x10);             /* JE finshed: */
op(0x89); op(0xC1);             /* MOV CX,AX */
op(0x80); op(0x3F); op(0x00);   /* top: CMP byte ptr [BX],0 */
op(0x74); op(0x09);             /* JE finished: */
op(0x43);                       /* INC BX */
op(0x80); op(0x3F); op(0x00);   /* up: CMP byte ptr [BX],0 */
op(0x75); op(0xFA);             /* JNE up: */
op(0x43);                       /* INC BX */
op(0xE2); op(0xF2);             /* LOOP top: */
op(0x89); op(0xD8);             /* finish: MOV AX,BX */
}


mmdos2ahcarry(ahvalue,carry)
int ahvalue,carry;
{op(0xB4);  op(ahvalue);  /* MOV AH,ahvalue */
intdos(2,0);         /* INT 21h    and set dos required to 2.0+ */
if(carry)
    op(0x73);        /* JNC */
else op(0x72);       /* JC */
op(0x02);            /* down2bytes */
op(0x31); op(0xC0);  /* XOR AX,AX */
}

mmclearscreen(segm,size,valu)
unsigned int segm,size,valu;
{op(0xB9);
outword(size);          /* MOV CX,size */
op(0x31);  op(0xFF);    /* XOR DI,DI   */
op(0xB8);
outword(segm);          /* MOV AX,segm */
op(0x8E);  op(0xC0);    /* MOV ES,AX   */
op(0xB8);
outword(valu);          /* MOV AX,valu */
op(0xF3);               /* REPZ */
stosw();
}


mmcopymem (farflag,backward,word)
int farflag,backward,word;
{
if(farflag)
    {op(0x89); op(0xD6);     /* MOV SI,DX */
    op(0x8E); op(0xC0);      /* MOV ES,AX */
    if(optimizespeed)
	{op(0x8C); op(0xD8);}   /* MOV AX,DS */
    else pushds();
    op(0x8E); op(0xD9);   /* MOV DS,CX */
    op(0x89); op(0xF9);   /* MOV CX,DI */
    op(0x89); op(0xDF);   /* MOV DI,BX */
    }  
else{op(0x89); op(0xDE);  /* MOV SI,BX */
    op(0x89); op(0xC7);   /* MOV DI,AX */
    movESDS();
    }
if(backward)
    {op(0x01); op(0xCF);  /* ADD DI,CX */
    op(0x01); op(0xCE);   /* ADD SI,CX */
    if(word==1)
	{op(0x4F);  /* DEC DI */
	op(0x4E);}  /* DEC SI */   
    else{op(0x81); op(0xEF); outword(word);  /* SUB DI,word */
	op(0x81); op(0xEE); outword(word);}  /* SUB SI,word */   
    pushf();
    op(0xFD);  /* STD */
    }
op(0xF3);  /* REPZ */
if(word==1)
    movsb();
else if(word==2)
    movsw();
else movsd();
if(backward)
    popf();
if(farflag)
    {if(optimizespeed)
	{op(0x8E); op(0xD8);}  /* MOV DS,AX */         
    else popds();}
}


mmtextrows()
{op(0x31); op(0xC0);           /* XOR AX,AX */
op(0x8E); op(0xC0);            /* MOV ES,AX */
op(0x26);                      /* ES: */
op(0xA0); op(0x84); op(0x04);  /* MOV AL,[484h] */
}


/*********************************************************************** 
	     Stack Procedures for SPHINX C-- Compiler 
 ***********************************************************************/


int includeproc ()
{if(string[0] == 'c')
    {if(string[1]=='o')
	if(string[2]=='p')
	    if(string[3]=='y')
		return( llcopy() );
    }
else if(string[0] == 'X')
    {if(string[1]=='M')
	if(string[2]=='S')
	    return( llXMS() );
    }
return( -1 );
}


int llcopy ()
{if(strcmp("backwardfar",string+4)==0)
    ppcopymem(1,1,1);
else if(strcmp("backwardfard",string+4)==0)
    ppcopymem(1,1,4);
else if(strcmp("backwardfarw",string+4)==0)
    ppcopymem(1,1,2);
else if(strcmp("backwardnear",string+4)==0)
    ppcopymem(0,1,1);
else if(strcmp("backwardneard",string+4)==0)
    ppcopymem(0,1,4);
else if(strcmp("backwardnearw",string+4)==0)
    ppcopymem(0,1,2);
else if(strcmp("far",string+4)==0)
    ppcopymem(1,0,1);
else if(strcmp("fard",string+4)==0)
    ppcopymem(1,0,4);
else if(strcmp("farw",string+4)==0)
    ppcopymem(1,0,2);
else if(strcmp("near",string+4)==0)
    ppcopymem(0,0,1);
else if(strcmp("neard",string+4)==0)
    ppcopymem(0,0,4);
else if(strcmp("nearw",string+4)==0)
    ppcopymem(0,0,2);
else return(-1);
return(0);
}

int llXMS ()
{if(strcmp("allocateEMB",string+3)==0)
    ppXMSgeneric2(9);
else if(strcmp("freeEMB",string+3)==0)
    ppXMSgeneric2(0xA);
else if(strcmp("getEMBinfo",string+3)==0)
    ppXMSgeneric2(0xE);
else if(strcmp("getversion",string+3)==0)
    ppXMSgetversion();
else if(strcmp("globaldisableA20",string+3)==0)
    ppXMSgeneric1(4);
else if(strcmp("globalenableA20",string+3)==0)
    ppXMSgeneric1(3);
else if(strcmp("localdisableA20",string+3)==0)
    ppXMSgeneric1(6);
else if(strcmp("localenableA20",string+3)==0)
    ppXMSgeneric1(5);
else if(strcmp("lockEMB",string+3)==0)
    ppXMSgeneric2(0xC);
else if(strcmp("moveEMB",string+3)==0)
    ppXMSmoveEMB();
else if(strcmp("queryA20",string+3)==0)
    ppXMSgeneric1(7);
else if(strcmp("queryfreeEMB",string+3)==0)
    ppXMSgeneric1(8);
else if(strcmp("reallocateEMB",string+3)==0)
    ppXMSreallocateEMB();
else if(strcmp("releaseHMA",string+3)==0)
    ppXMSgeneric1(2);
else if(strcmp("releaseUMB",string+3)==0)
    ppXMSgeneric2(0x11);
else if(strcmp("requestHMA",string+3)==0)
    ppXMSgeneric2(1);
else if(strcmp("requestUMB",string+3)==0)
    ppXMSgeneric2(0x10);
else if(strcmp("unlockEMB",string+3)==0)
    ppXMSgeneric2(0xD);
else return(-1);
return(0);
}


ppcopymem (farflag,backward,word)
int farflag,backward,word;
{popbx();
popcx();
popsi();
if(farflag)
    {outword(0xD88C);  /* MOV AX,DS */
    popds();}
popdi();
if(farflag)
    popes();
else{outword(0xDA8C);  /* MOV DX,DS */
    outword(0xC28E);}  /* MOV ES,DX */
if(backward)
    {outword(0xCF01);  /* ADD DI,CX */
    outword(0xCE01);   /* ADD SI,CX */
    if(word==1)
	outword(0x4E4F);  /* DEC DI, DEC SI */
    else{op(0x81); op(0xEF); outword(word);  /* SUB DI,word */
	op(0x81); op(0xEE); outword(word);}  /* SUB SI,word */   
    pushf();
    op(0xFD);  /* STD */
    }
op(0xF3);  /* REPZ */
if(word==1)
    movsb();
else if(word==2)
    movsw();
else movsd();
if(backward)
    popf();
if(farflag)
    outword(0xD88E);  /* MOV DS,AX */         
outword(0xE3FF);      /* JMP BX */
}


/************* start of XMS library procedures *****************/

unsigned int xmscallloc=0;        // address of a double word address


int checkxmscallvar (sizeadd)
unsigned int sizeadd;
{if(xmscallloc==0)
    {xmscallloc = outptr + sizeadd;
    if( xmscallloc %2 == 1 && alignword )
	{xmscallloc++;
	return(2);}
    return(1);}
return(0);
}


void outxmscallvar (thedoitflag)
{if( thedoitflag == 0 )
    return;
if( thedoitflag == 2 )
    op(aligner);
outword(0x1234);
outword(0x5678);
}


ppXMSgetversion ()
{int makeflag;
makeflag = checkxmscallvar(35);
op(0xB8); outword(0x4300);                 /* MOV AX,0x4300 */
op(0xCD); op(0x2F);                        /* INT 0x2F */
op(0x3C); op(0x80);                        /* CMP AL,0x80 */
op(0x74); op(0x3);                         /* JE down3 */
op(0x31); op(0xC0);                        /* XOR AX,AX */
op(0xC3);                                  /* RET */
op(0xB8); outword(0x4310);                 /* MOV AX,0x4310 */
op(0xCD); op(0x2F);                        /* INT 0x2F */
op(0x2E);                                  /* CS: */
op(0x8C); op(0x06); outword(xmscallloc+2); /* MOV [xmscalladdress+2],ES */
op(0x2E);                                  /* CS: */
op(0x89); op(0x1E); outword(xmscallloc);   /* MOV [xmscalladdress],BX */
op(0xB4); op(0x00);                        /* MOV AH,0 */
op(0x2E);                                  /* CS: */
op(0xFF); op(0x1E); outword(xmscallloc);   /* CALL far [xmscalladdress] */
op(0xC3);                                  /* RET */
outxmscallvar(makeflag);
}


ppXMSgeneric1 (functionnumber)
int functionnumber;
{int makeflag;
makeflag = checkxmscallvar(8);
op(0xB4); op(functionnumber);             /* MOV AH,functionnumber */
op(0x2E);                                 /* CS: */
op(0xFF); op(0x1E); outword(xmscallloc);  /* CALL far [xmscalladdress] */
op(0xC3);                                 /* RET */
outxmscallvar(makeflag);
}


ppXMSgeneric2 (functionnumber)
int functionnumber;
{int makeflag;
makeflag = checkxmscallvar(11);
op(0x58);                                 /* POP AX */
op(0x5A);                                 /* POP DX */
op(0x50);                                 /* PUSH AX */
op(0xB4); op(functionnumber);             /* MOV AH,functionnumber */
op(0x2E);                                 /* CS: */
op(0xFF); op(0x1E); outword(xmscallloc);  /* CALL far [xmscalladdress] */
op(0xC3);                                 /* RET */
outxmscallvar(makeflag);
}


ppXMSreallocateEMB ()
{int makeflag;
makeflag = checkxmscallvar(12);
op(0x58);                                 /* POP AX */
op(0x5B);                                 /* POP BX */
op(0x5A);                                 /* POP DX */
op(0x50);                                 /* PUSH AX */
op(0xB4); op(0x0F);                       /* MOV AH,0x0F */
op(0x2E);                                 /* CS: */
op(0xFF); op(0x1E); outword(xmscallloc);  /* CALL far [xmscalladdress] */
op(0xC3);                                 /* RET */
outxmscallvar(makeflag);
}


ppXMSmoveEMB ()
{int makeflag;
makeflag = checkxmscallvar(25);
op(0x1E);                                 /* PUSH DS */
op(0x56);                                 /* PUSH SI */
op(0x89); op(0xE0);                       /* MOV AX,SP */
op(0x05); outword(0x0006);                /* ADD AX,0x0006 */
op(0x89); op(0xC6);                       /* MOV SI,AX */
op(0x8C); op(0xD0);                       /* MOV AX,SS */
op(0x8E); op(0xD8);                       /* MOV DS,AX */
op(0xB4); op(0x0B);                       /* MOV AH,0x0B */
op(0x2E);                                 /* CS: */
op(0xFF); op(0x1E); outword(xmscallloc);  /* CALL far [xmscalladdress] */
op(0x5E);                                 /* POP SI */
op(0x1F);                                 /* POP DS */
op(0xC2); outword(0x0010);                /* RET 0x0010 */
outxmscallvar(makeflag);
}

/********************* end of XMS procedures *********************/

/* end of TOKM.C */
