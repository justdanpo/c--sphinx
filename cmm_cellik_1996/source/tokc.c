/*
    SPHINX C-- by SPHINX programming
    Property of Peter Cellik.
    Copyright Peter Cellik (C) 1995.  All rights reserved.
    Last updated 16 May 1995
*/
/*
14 Mar 94 notes:
    - fixed alignment of 32bit vars
    - fixed signed/unsigned trouble on 32bit vars (careless error)
19 Mar 94 notes version 0.192:
    - 32 bit fixed point numbers added for this file except where noted by
      FIXED32 messages.
12 June 94 notes version 0.193:
    - fixed a 32 bit dword/long compare bug.
12 June 94 notes version 0.194:
    - still trying to fix memory sizes > 64K silent overflow bug.
3 July 94 notes version 0.195:
    - Adding OBJ file stuff!  Hurra!
4 July 94 notes version 0.196:
    - OBJ almost done!
6 July 94 notes version 0.197:
    - added far keyword.
    - removed BETA message :-)
    - OVL finding spiffed up.
7 July 94 notes version 0.198:
    - fixed far bugs.
16 Aug 94 notes version 0.199:
    - added BSWAP, CMPXCHG, INVD, INVLPG, WBINVD, and XADD.
14 Sept 94 notes version 0.200:
    - if a reg32 long variable or dword variable is the first thing in a parameter
      expression, then the expression is now assumed to be of that type.
18 Sept 94 notes version 0.201:
    - REP alias for REPZ added.
3 Nov 94 notes version 0.202:
    - Fixed dword >< bug in TOKB.C.
16 May 95 notes version 0.203:
    - OBJ option now automatically sets jump to main to none.
    
stuff to do:
- add support for global data addressing in OBJ files.
- do COM and DAT file output for 64K * 2 program size
- do EXE run file output option (but mabe never) it sucks.  Could do this by making
  use of the OBJ output...
- rm values are only handled at 16-bit level, and probably always will be.
- do parameter number remembering and dynamic handling for procs
  done: number = address of proc or post call number.
  done: segm = 0+ for dynamic proc pointer index or NOT_DYNAMIC for all
	       other types if value is >= MAXDYNAMICS then it has already
	       been added statically, subtract value by MAXDYNAMICS to get
	       index.
	post = # of param words (this may not be done, for flexability)
  done: rm = return type (tk_void, tk_byte, etc.  tokens for unknown)
- restriction error messages for dynamics required:
      done: -function calls illegal.
      done: -constant strings illegal.
      done: -non-local labels illegal.
	    -non-local jumps illegal (messages may not be possible).
- self encryption is required.
- self virus checking.
- think about doing device driver creation. ?device "NAME5678" (nawww, DOS device
  drivers are dead).
- FINISH FIXED32 stuff!!!
*/

#include <stdio.h>   /* standard stuff */
#include <stdlib.h>  /* getenv and _splitpath stuff */
#include <io.h>      /* low level file stuff */
#include <fcntl.h>   /* O_ constant definitions */
#include <malloc.h>  /* memory allocation */
#include <string.h>  /* string functions */
#include "tok.h"


int tok,tok2; /* token holders current, next */
int rm,rm2;
int segm,segm2;
int post,post2;
unsigned char string[STRLEN],string2[STRLEN];
long number,number2;

#define MAXPOSTS 15000
unsigned char *posttype;   /* [MAXPOSTS]; */
unsigned int *postloc;     /* [MAXPOSTS]; */
unsigned int *postnum;     /* [MAXPOSTS]; */
unsigned int posts=0;      /* number of post entrys */
unsigned int secondcallnum=0;  /* # of different second calls and labels */
unsigned int postsize=0;       /* total size of all post vars */
unsigned int poststrptr=MAXDATA-1;  /* post string index in output */

FILE *ovlfile,*mapfile;         /* ovl and map file handles */
#define OVLFILENAME "C--.OVL"   /* ovl file name */
unsigned char ovlpath[FILENAMESIZE];   /* ovl filename with directory */
unsigned char rawdrive[_MAX_DRIVE],rawdir[_MAX_DIR];
unsigned char rawfname[_MAX_FNAME],rawext[_MAX_EXT];
unsigned char rawfilename[FILENAMESIZE];
unsigned char inputfile[FILENAMESIZE];
unsigned char currentfilename[FILENAMESIZE];
long runfilesize;

int error=0;     /* current error number holder */

int dos1=0,dos2=0;    /* DOS version required for program execution */
unsigned char cpu=0;  /* CPU required, 2 for 80286, 3 for 80386, ... */

unsigned int paramsize=0;  /* byte size of all procedure parameters */
unsigned int localsize=0;  /* byte size of all procedure local variables */
unsigned int procedure_start=0; /* address of start of procedure */

int current_proc_type;   /* current procedure type (cpt_near, cpt_far, cpt_interrupt) */
int returntype;          /* return type, (void, byte, word, ...) */

#define DYNAMIC_HEADER 5 /* size in bytes of header in each dynamic buffer:
		    byte 0 and 1:  size of dynamic proc.
			  byte 2:  cpu required.
			  byte 3:  high version number of DOS required.
			  byte 4:  low version number of DOS required.
			 */
unsigned char *dynamic_ptrs[MAXDYNAMICS];
			  /* ptrs to stored dynamic procedures */
int dynamic_flag=NOT_DYNAMIC;  /* flag set number of current dynamic proc */
int dynamic_count=0;  /* current number of dynamic procedures */

unsigned int exeheader[16];  // 0x20 byte header for EXE format

unsigned char me[12] = {'P'-1,'e'-2,'t'-3,'e'-4,'r'-5,' '-6,
			'C'-7,'e'-8,'l'-9,'l'-10,'i'-11,'k'-12};

/*+++++++++++++++++++++++ flexable compiler options +++++++++++++++++++++++*/
unsigned int  startptr = 0x100;       // start address
unsigned int  startptrdata = 0x100;   // data start address
unsigned char comfile = file_com;     // output file format
unsigned char makemapfile = 0;        // create map file
unsigned char jumptomain = CALL_NEAR; // jump to the main()
unsigned char optimizespeed = 1;      // optimize for size or speed flag
unsigned char parsecommandline = 0;   // parse command line flag
unsigned int  newcommandlineaddr = 0; // address of new command line
unsigned char alignword = 1;          // flag whether or not to align words
unsigned char aligner = 0;            // value used to align words
unsigned char header = 1;             // output SPHINX C-- Ver1 Ver2 header
unsigned char chip = 0;               // CPU optimization (286,386,486,...)
unsigned char killctrlc = 0;          // add disable CTRL-C code in header
unsigned char assumeDSSS = 0;         // assume SS is equal to DS
unsigned char stackcheck = 0;         // display stack available in tokscan
unsigned char resizemem = 1;          // set owned memory block to 64K
unsigned int  stacksize = 2048;       // stack size (2048 default)
unsigned char *resizemessage = NULL;  // resize memory error message
unsigned int  maxerrors = 16;         // number of errors to stop at
/*+++++++++++++++++++ end of flexable compiler options ++++++++++++++++++++*/
unsigned char notdoneprestuff = 1; // flag if initial stuff has been entered

unsigned int datasize=0,alignersize=0;  /* size of data and other */
unsigned int outptr=0x100,outptrdata=0x100;       /* ptr to output */
unsigned char *output;           /* [MAXDATA]; */
unsigned char *outputdata=NULL;  /* [MAXDATA]; */
unsigned int resizesizeaddress;  /* location of resize memory size descr. */
unsigned int stackstartaddress;  /* location of SP assignment */

unsigned int linenumber;
extern unsigned int endifcount;

unsigned char ver1=0,ver2=203;
unsigned char compilerstr[] = "SPHINX C-- 0.203";

unsigned char compilerdir[FILENAMESIZE]; /* value of "C--" environment var */

/******************* function predefinitions ********************/

unsigned int addpoststring ();  /* add a string to the post queue */


/****************************** main () *********************************/


void startsymbiosys (symfile)
unsigned char *symfile;
{unsigned int size=0,filehandle,addresshold;
long filesize;

outptr = startptr;
filehandle = open( symfile, O_BINARY | O_RDONLY );
if( filehandle == -1 )
    {puts("ERROR> unable to open symbio COM file.");
    exit(e_symbioerror);}
if( (filesize=filelength(filehandle)) != -1L )
    {if( filesize+outptr < MAXDATA )
	{size = filesize;
	if( read(filehandle,output+outptr,size) != size )
	    {close(filehandle);
	    puts("Error reading symbio COM file.");
	    exit(e_symbioerror);}
	outptr += size;
	}
    else{puts("Symbio COM file too large.");
	exit(e_symbioerror);}
    }
else{puts("Unable to determine symbio COM file size.");
    exit(e_symbioerror);
    }
close(filehandle);
outptrdata = outptr;
addresshold = outptr;
pushax();
pushbx();
pushcx();
pushdx();
pushdi();
pushsi();
pushbp();
pushes();
tok = tk_ID;
strcpy(string,"main");
tobedefined(CALL_NEAR,tk_void);  /* put main on the to be defined stack */
op(0xE8); outword(0);   /* CALL main() */
op(0xC7);  op(0x06);
outword(0x0100);
op(output[startptr]);
op(output[startptr+1]);  /* MOV WORD PTR [0100],(0x100)(0x101) */
op(0xC6); op(0x06);
outword(0x0102);
op(output[startptr+2]);  /* MOV BYTE PTR [0102],(0x102) */
popes();
popbp();
popsi();
popdi();
popdx();
popcx();
popbx();
popax();
op(0xE9);  outword(startptr-(outptr+2));   /* JMP startptr */

output[startptr]=0xE9;                  /* set     */
addresshold = addresshold-(startptr+3); /* inital  */
output[startptr+1]=addresshold&255;     /* code to */
output[startptr+2]=addresshold/256;     /* jump    */

notdoneprestuff = 0;
}


void compile ()
{long segments_required;
long longhold;

if(makemapfile)
    startmapfile();
printf("Compiling Commenced . . .\n");
sprintf(inputfile,"%s.C--",rawfilename);
currentfilename[0] = 0;       /* set current filename to nothing */
linenumber = 0;
compilefile(inputfile,1);
if( notdoneprestuff )
    doprestuff();
if( endifcount > 0 )
    preerror("?endif expected before end of file");
docalls();  /* attempt to declare remaining undeclared calls */
if( comfile == file_exe )           // if an EXE file
    {if( outptr%16 != 0 )    // paragraph align the end of the code seg
	outptr += 16 - outptr%16;
    }
else{
    longhold = outptr+postsize+stacksize;
    if( longhold > 65536L )
	preerror("Code, data and stack total exceeds 64k");
    }    

if(posts > 0)
    doposts();  /* update all post address locations */
if(resizemem)
    {segments_required = (outptr+postsize+stacksize+15) /16;
    output[resizesizeaddress] = segments_required % 256;
    output[resizesizeaddress+1] = segments_required / 256;
    output[stackstartaddress] = segments_required*16 % 256;
    output[stackstartaddress+1] = segments_required*16 / 256;
    }
if(error==0)
    {printf("COMPILING FINISHED.   Errors: %d   Lines: %u\n",error,linenumber);
    if(dos1 > 0)
	printf("DOS required: %d.%d or greater.\n",dos1,dos2);
    if(cpu >= 1)
	printf("CPU required: 80%d86 or greater.\n",cpu);
    runfilesize = outptr-startptr;
    if( comfile == file_exe )
	{runfilesize += outptrdata-startptrdata+0x20;
	postsize += postsize%2;
	stacksize = (stacksize+15)/16*16;
	}
    if(makemapfile)
	finishmapfile();
    printmemsizes(stdout);
    if(writeoutput(0)==0)
	printf("Run File Saved (%ld bytes).\n",runfilesize);
    }
else if(makemapfile)
    finishmapfile();
}



char *pptr;   /* used for pointing to current command line entry */

main (argc,argv)
int argc;
char *argv[];
{int count,pari = 0;
printf("\nSPHINX C-- Compiler   Version %d.%d\n",ver1,ver2);
printf("Created by ");
for(count=1;count<=12;count++)
    printf("%c",me[count-1]+count);
printf(". Copyright Peter Cellik (C) 1995. All rights reserved.\n");
printf("SPHINX Programming 1995\n\n");

getmem();   /* allocate buffers */

if( getenv("C--") )
    {strcpy(compilerdir,getenv("C--"));
    if( strlen(compilerdir) > 0 && compilerdir[strlen(compilerdir)-1]!='\\' )
	strcat(compilerdir,"\\");
    }
else{// if C-- environment var is not set, attempt to use program path to find OVL.
    strcpy(compilerdir,argv[0]);
    count = strlen(compilerdir);
    while( count > 0 && compilerdir[count] != '\\' )
	count--;
    if( compilerdir[count] == '\\' )
	count++;
    compilerdir[count] = 0;    
    }
sprintf(ovlpath,"%s%s",compilerdir,OVLFILENAME);
if(!(ovlfile=fopen(ovlpath,"rb")))
    {printf("ERROR >> unable to open %s\n",ovlpath);
    exit( e_noovl );}

for(count=1;count<argc;count++)    /* command line params handled here */
    {strupr(argv[count]);
    if(argv[count][0]=='/')
	{pptr = argv[count]+1;
	if( strcmp("-MAIN",pptr)==0 || strcmp("J0",pptr)==0 )
	    {jumptomain = CALL_NONE;
	    header = 0;}
	else if( strcmp("SHORTMAIN",pptr)==0 || strcmp("J1",pptr)==0 )
	    {jumptomain = CALL_SHORT;
	    header = 1;}
	else if( strcmp("+MAIN",pptr)==0 || strcmp("J2",pptr)==0 )
	    {jumptomain = CALL_NEAR;
	    header = 1;}
	else if(strcmp("-CTRLC",pptr)==0 || strcmp("-C",pptr)==0)
	    killctrlc = 0;
	else if(strcmp("+CTRLC",pptr)==0 || strcmp("+C",pptr)==0)
	    killctrlc = 1;
	else if(strcmp("SPEED",pptr)==0 || strcmp("OS",pptr)==0)
	    optimizespeed = 1;
	else if(strcmp("SIZE",pptr)==0 || strcmp("OC",pptr)==0)
	    optimizespeed = 0;
	else if(strcmp("-RESIZE",pptr)==0 || strcmp("-R",pptr)==0)
	    resizemem = 0;
	else if(strcmp("+RESIZE",pptr)==0 || strcmp("+R",pptr)==0)
	    resizemem = 1;
	else if(strcmp("-PARSE",pptr)==0 || strcmp("-P",pptr)==0)
	    parsecommandline = 0;
	else if(strcmp("+PARSE",pptr)==0 || strcmp("+P",pptr)==0)
	    parsecommandline = 1;
	else if(strcmp("-ALIGN",pptr)==0 || strcmp("-A",pptr)==0)
	    alignword = 0;
	else if(strcmp("+ALIGN",pptr)==0 || strcmp("+A",pptr)==0)
	    alignword = 1;
	else if(strcmp("-ASSUMEDSSS",pptr)==0 || strcmp("-D",pptr)==0)
	    assumeDSSS = 0;
	else if(strcmp("+ASSUMEDSSS",pptr)==0 || strcmp("+D",pptr)==0)
	    assumeDSSS = 1;
	else if(strcmp("STACK",pptr)==0)
	    stackcheck = 1;
	else if(strcmp("QUOTE",pptr)==0)
	    {printf("\nKill others only if you wish to be killed.\n");
	    exit( e_quote );}
	else if(strcmp("+MAP",pptr)==0 || strcmp("+M",pptr)==0)
	    makemapfile = 1;
	else if(strcmp("-MAP",pptr)==0 || strcmp("-M",pptr)==0)
	    makemapfile = 0;
	else if(strcmp("PROC",pptr)==0)
	    {outptr = startptr;
	    if(argv[count+1] != NULL)
		{strcpy(string,argv[count+1]);
		if(includeproc()==-1)
		    printf("%s not in Procedure library.\n",string);
		else{if(argv[count+2] != NULL)
			{strcpy(rawfilename,argv[count+2]);
			writeoutput(1);}
		    else printf("Output Filename expected.\n");
		    }
		}
	    else printf("Procedure name expected.\n");
		exit( e_extract );
	    }
	else if(strcmp("REGPROC",pptr)==0)
	    {outptr = startptr;
	    if(argv[count+1] != NULL)
		{strcpy(string,argv[count+1]);
		if(includeit(1)==-1)
		    printf("%s not in REG Procedure library.\n",string);
		else{if(argv[count+2] != NULL)
			{strcpy(rawfilename,argv[count+2]);
			writeoutput(1);}
		    else printf("Output Filename expected.\n");
		    }
		}
	    else printf("REG Procedure name expected.\n");
	    exit( e_extract );
	    }
	else if(strcmp("MACRO",pptr)==0)
	    {outptr = startptr;
	    if(argv[count+1] != NULL)
		{strcpy(string,argv[count+1]);
		if(includeit(0)==-1)
		    printf("%s not in REG Procedure library.\n",string);
		else{if(argv[count+2] != NULL)
			{strcpy(rawfilename,argv[count+2]);
			writeoutput(1);}
		    else printf("Output Filename expected.\n");
		    }
		}
	    else printf("Macro name expected.\n");
	    exit( e_extract );
	    }
	else if(strcmp("SYM",pptr)==0)
	    {startptr = 0x100;
	    comfile = file_com;
	    count++;
	    startsymbiosys(argv[count]);}
	else if(strcmp("8088",pptr)==0 || strcmp("8086",pptr)==0 || strcmp("0",pptr)==0)
	    chip = 0;
	else if(strcmp("80186",pptr)==0 || strcmp("1",pptr)==0)
	    chip = 1;
	else if(strcmp("80286",pptr)==0 || strcmp("2",pptr)==0)
	    chip = 2;
	else if(strcmp("80386",pptr)==0 || strcmp("3",pptr)==0)
	    chip = 3;
	else if(strcmp("80486",pptr)==0 || strcmp("4",pptr)==0)
	    chip = 4;
	else if(strcmp("80586",pptr)==0 || strcmp("5",pptr)==0)
	    chip = 5;
	else if(strcmp("80686",pptr)==0 || strcmp("6",pptr)==0)
	    chip = 6;
	else if(strcmp("80786",pptr)==0 || strcmp("7",pptr)==0)
	    chip = 7;
	else if(strcmp("X",pptr)==0)
	    header = 0;
	else if(strcmp("EXE",pptr)==0 || strcmp("E",pptr)==0)
	    {puts("WARNING!!!  EXE file output not yet complete.  Do not use!");
	    comfile = file_exe;
	    dos1=2;
	    dos2=0;
	    if( outputdata == NULL )
		{outputdata = (unsigned char *) malloc( (size_t) MAXDATA );
		if(outputdata == NULL)
		    {printf("ERROR > Not enough memory for output data buffer.\n");
		    exit(e_outofmemory);}
		}
	    }
	else if(strcmp("OBJ",pptr)==0)
	    {comfile = file_obj;
	    startptr = 0x0000;       // start address
	    startptrdata = 0x0000;   // data start address
	    jumptomain = CALL_NONE;
	    header = 0;
	    }
	else if(strcmp("HELP",pptr)==0 || strcmp("?",pptr)==0)
	    {printf("Read the document files (C--INFO.DOC, etc.).\n");
	    exit( e_ok );}
	else if(strcmp("ME",pptr)==0)
	    {puts("Creator Of SPHINX C--:");
	    puts("    Peter Cellik");
	    puts("    RR#2 Site 33 C11");
	    puts("    Gabriola Is.  B.C.");
	    puts("    V0R 1X0");
	    puts("    CANADA");
	    puts("\nDrop me a line.");
	    exit( e_ok );}
	else if(strcmp("KEYWORDS",pptr)==0)
	    {displaykeywords();
	    exit( e_ok );}
	else{
	    if( pptr[0] == 'S' && pptr[1] == '=' )
		{pptr += 2;
		stacksize = 0;
		while( *pptr >= '0' && *pptr <= '9' )
		    {stacksize = stacksize*10 + (*pptr - '0');
		    pptr++;
		    }
		if( *pptr )
		    {printf("ERROR > Bad stack size.\n");
		    exit( e_unknowncommandline );
		    }
		}
	    else{printf("ERROR > Unknown command line option '%s'.\n",argv[count]);
		exit( e_unknowncommandline );
		}
	   }
	}
    else{if(pari==0)
	    {_splitpath(argv[count],rawdrive,rawdir,rawfname,rawext);
	    if( strcmp(rawext,"")!=0 && strcmp(rawext,".C--")!=0 )
		{printf("ERROR > Bad input file extension '%s'.",rawext);
		exit( e_badinputfilename );}
	    sprintf(rawfilename,"%s%s%s",rawdrive,rawdir,rawfname);
	    pari = 1;}
	else{printf("ERROR > Unknown command line parameter '%s'.\n",argv[count]);
	    exit( e_unknowncommandline );}
	}
    }
if(pari != 1)
    {puts("USAGE >> C-- <SOURCE FILE NAME without extension> {options}");
    puts(" /80286       << enable 80286 code optimizations (/2)");
    puts(" /80386       << enable 80386 code optimizations (/3)");
    puts(" /-ALIGN      << disable even word address alignment (/-A)");
    puts(" /+ASSUMEDSSS << enable assumption of DS=SS optimization (/+D)");
    puts(" /+CTRLC      << insert CTRL<C> ignoring code (/+C)");
//  puts(" /EXE         << produce EXE run file (almost available) (/E)");
//  puts(" /HELP        << get a little help (not much) (/?)");
    puts(" /KEYWORDS    << display list of C-- reserved words");
//  puts(" /MACRO       << extract macro from internal library");
    puts(" /+MAP        << generate map file (/+M)");
    puts(" /ME          << display my name and my address");
    puts(" /-MAIN       << disable initial jump to main() (/J0)");
//  puts(" /OBJ         << produce OBJ output file");
    puts(" /-RESIZE     << do not insert resize memory block code (/-R)");
    puts(" /+PARSE      << insert parse command line code (/+P)");
    puts(" /PROC        << extract procedure from internal library");
//  puts(" /QUOTE       << quote of the program (QOTP)");
    puts(" /REGPROC     << extract REG-procedure from internal library");
    puts(" /+RESIZE     << insert resize memory block code (default) (/+R)");
    puts(" /SIZE        << optimize for code size (/OC)");
    puts(" /SHORTMAIN   << initial jump to main() short (/J1)");
    puts(" /SPEED       << optimize for speed (default) (/OS)");
    puts(" /S=#####     << set stack size to #####");
  printf(" /SYM         << COM file symbiosis");
//  puts(" /X           << disable SPHINX C-- header in output file");
    exit( e_noinputspecified );
    }
tokainitalize();
compile();
fclose( ovlfile );
if( error == 0 )
    exit( e_ok );
exit( e_someerrors );
}


doprestuff ()  // do initial code things, like resize mem, jump to main...
{struct tokenrec holdtoken;
storecurrenttoken(&holdtoken);

notdoneprestuff = 0;

if( comfile == file_com || comfile == file_obj )
    {outptr = startptr;
    if(resizemem)
	{popax();                   /* get return address off stack */
	op(0xBC);
	stackstartaddress = outptr;
	outword(0); /* MOV SP,# */  /* set new stack location */
	pushax();                   /* put return address on new stack */
	outword(0x4AB4);            /* MOV AH,4Ah */
	op(0xBB);
	resizesizeaddress = outptr;
	outword(0x1000);     /* MOV BX,0x1000 */
	outword(0x21CD);     /* INT 21h - Resize memory block */
	op(0x73);            /* JNB */
	if(resizemessage == NULL)
	    {op(0x01);}           /* JNB down 1 byte */
	else{op(0x08);            /* JNB down 8 bytes */
	    outword(0x09B4);      /* MOV AH,09h */
	    strcpy(string,resizemessage);
	    op(0xBA); outword(addpoststring());  /* MOV DX,resizemessage */
	    outword(0x21CD);}   /* INT 0x21 to display the string */
	ret();
	}
    if(killctrlc)
	{outword(0x01EB);   // JMP down 1 byte over IRET
	iret();
	xorAXAX();
	outword(0xC08E);  /* MOV ES,AX */
	op(0x26);         /* ES: MOV [0x8C],iret offset */
	outword(0x06C7);  outword(0x8C);  outword(outptr-8);
	op(0x26);         /* ES: */
	outword(0x0E8C);  outword(0x8E);   /* MOV [],CS */
	}
    if(parsecommandline)
	outparsecommandline();
    if(jumptomain != CALL_NONE)
	{tok = tk_ID;
	strcpy(string,"main");
	tobedefined(jumptomain,tk_void);  /* put main on the to be defined stack */
	if( jumptomain == CALL_NEAR )
	    {op(0xE9); outword(0x0000);}  /* JMP near */
	else{op(0xEB); op(0x00);}   /* JMP short */
	}
    }
else if( comfile == file_exe )
    {startptr = 0;         /* EXE file stuff */
    outptr = 0;
    outptrdata = 0;
    startptrdata = 0;
    pushss();
    popds();
    tok = tk_ID;
    strcpy(string,"main");
    tobedefined(CALL_NEAR,tk_void); /* put main on the to be defined stack */
    op(0xE8); outword(0x0000);  /* CALL main() */
    op(0xB8); outword(0x4C00);  /* MOV AX,0x4C00 */
    op(0xCD); op(0x21);         /* INT 0x21 >> exit with 0 return code */
    }

if(header)
    {outheader();
    alignersize += 12;}
restorecurrenttoken(&holdtoken);
if( jumptomain != CALL_NONE )
    if( (tok == tk_ID || tok == tk_id) && strcmp(string,"main")==0 )
	tok = tk_undefregproc;
}


compilefile (filename,firstflag)
char *filename;
int firstflag;
{int hold;
char trialfilename[FILENAMESIZE];

strcpy(trialfilename,filename);
hold = loadinputfile(trialfilename);
if( firstflag == 0 && hold == -2 && compilerdir[0] != 0 )
    {sprintf(trialfilename,"%s%s",compilerdir,filename);
    hold = loadinputfile(trialfilename);
    }
if( hold == -2 )
    unabletoopen(filename);
if( hold != 0 )
    exit( e_cannotopeninput );
strcpy(currentfilename,trialfilename);
if( makemapfile )
    fprintf(mapfile,"File %s included.\n",currentfilename);

starttok();
nexttok();
while( tok != tk_eof )
    {while( tok == tk_question )
	directive();
    if( notdoneprestuff )
	doprestuff();
    switch(tok)
	{case tk_id:
	case tk_ID:
	case tk_undefregproc:
	case tk_undefproc:  declare_procedure(tokens,cpt_near);  break;
	case tk_far:        nexttok();  declare_procedure(tokens,cpt_near);  break;
	case tk_at:         insertcode();               break;
	case tk_colon:      dynamic_proc();             break;
	case tk_fixed32s:
	case tk_fixed32u:
	case tk_long:
	case tk_dword:
	case tk_word:
	case tk_byte:
	case tk_char:
	case tk_int:       globalvar(tok);   break;
	case tk_interrupt: interruptproc();  break;
	case tk_void:      nexttok();
			   if( tok == tk_far )
			       {nexttok();
			       declare_procedure(tk_void,cpt_far);}
			   else declare_procedure(tk_void,cpt_near);
			   break;
	case tk_question:  directive();  break;
	case tk_enum:     doenum();  break;
	case tk_from:     nexttok();  dofrom(0);  nextseminext();  break;
	case tk_extract:  nexttok();  doextract(0);  seminext();  break;
	case tk_loop:
	case tk_while:
	case tk_do:
	case tk_else:
	case tk_ELSE:
	case tk_if:
	case tk_IF:
	case tk_interruptproc:
	case tk_proc:
	case tk_regproc:
	case tk_charvar:
	case tk_intvar:
	case tk_bytevar:
	case tk_longvar:
	case tk_dwordvar:
	case tk_fixed32svar:
	case tk_fixed32uvar:
	case tk_wordvar:  idalreadydefined();  break;
	case tk_reg32:
	case tk_debugreg:
	case tk_controlreg:
	case tk_testreg:
	case tk_reg:
	case tk_seg:
	case tk_beg:  regnameerror();
	case tk_eof:  break;
	case tk_locallabel:  internalerror("local label token found outside procedure block.");
	default:      unuseableinput();  break;
	}
    }
freeinput();
}


printmemsizes (whereto)
FILE *whereto;
{long stacklong;
unsigned int stackword;
unsigned int postword;
postword = postsize;
if(comfile == file_com)
    {if( resizemem == 0 )
	{stacklong = 0xFFFE - outptr - postsize;
	stackword = stacklong;}
    else stackword = stacksize;
    fprintf(whereto,"Code: %u bytes, Data: %u bytes, Post: %u bytes, Stack: %u bytes\n"
		,outptr-startptr-datasize-alignersize,datasize,postword,stackword);
    }
else{fprintf(whereto,"Code: %u bytes, Data: %u bytes, Post: %u bytes, Stack: %u bytes\n"
		,outptr-startptr,datasize,postword,stacksize );
    }
}


/*================ OBJ file output stuff starts ===============*/


void obj_outLEDATA(FILE *fp,unsigned int offset,unsigned int recordlength,
		   unsigned char *data)
{
int checksum,i;
unsigned char recordtype = 0xA0;

fwrite(&recordtype,1,1,fp);
recordlength+=4;
checksum = recordtype;
checksum += (recordlength/256);
checksum += (recordlength&255);
fwrite(&recordlength,2,1,fp);
i = 0x01;
checksum += i;
fwrite(&i,1,1,fp);
checksum += (offset/256);
checksum += (offset&255);
fwrite(&offset,2,1,fp);
recordlength-=4;
for( i=0; i < recordlength; i++)
    checksum += data[i];
fwrite(data,recordlength,1,fp);
checksum = -checksum;
fwrite(&checksum,1,1,fp);
}



int obj_outrecord (FILE *fp,int recordtype,unsigned int recordlength,
		   unsigned char *data)
/*
   Outputs an OBJ record.
*/                   
{
int checksum,i;
recordlength++;
checksum = recordtype;
fwrite(&recordtype,1,1,fp);
checksum += (recordlength/256);
checksum += (recordlength&255);
fwrite(&recordlength,2,1,fp);
recordlength--;
for( i=0; i < recordlength; i++)
    checksum += data[i];
fwrite(data,recordlength,1,fp);
checksum = -checksum;
fwrite(&checksum,1,1,fp);
}


int out_objfile (char *objfilename,unsigned int length,char *data)
/*
add object name passed in later...
*/
{
FILE *fp;
unsigned char bytevalue;
unsigned char holdstr[100];
unsigned int i;

fp = fopen(objfilename,"wb");

// output the HEADER
obj_outrecord(fp,0x80,7,"\x06SPHINX");

// output the LNAMES
holdstr[0] = 0x00;
holdstr[1] = 0x04;
holdstr[2] = 0x43;
holdstr[3] = 0x4F;
holdstr[4] = 0x44;
holdstr[5] = 0x45;
holdstr[6] = 0x00;
obj_outrecord(fp,0x96,7,holdstr);

// output the SEGDEF
holdstr[0] = 0x28;
holdstr[1] = length & 255;    // Set the length of the segment of DATA or CODE
holdstr[2] = length / 256;    //
holdstr[3] = 0x02;
holdstr[4] = 0x01;
holdstr[5] = 0x01;
obj_outrecord(fp,0x98,6,holdstr);

// output the PUBDEF records for each exteral procedures (all procedures)
outputPUBDEF(fp);

// output the data (LEDATA) in 1K chunks as required!
i = 0;
while( i < length )
    {
    if( i+1024 > length )
	obj_outLEDATA(fp,i,length-i,data+i);
    else obj_outLEDATA(fp,i,1024,data+i);
    i += 1024;   
    }

// output end of OBJ notifier
bytevalue = 0x00;
obj_outrecord(fp,0x8A,1,&bytevalue);

// close the OBJ file
fclose(fp);
}


/*================ OBJ file output stuff ends ===============*/

/* ------------------- output procedures start ------------------- */

int writeoutput (fullname)
char fullname;
{FILE *diskout;
long paragraphsrequired;
unsigned int count;
unsigned char outputfile[FILENAMESIZE];

if(comfile == file_com)
    {if( fullname )
	strcpy(outputfile,rawfilename);
    else sprintf(outputfile,"%s.COM",rawfilename);
    if(!(diskout=fopen(outputfile,"wb")))
	{fprintf(stderr,"ERROR, unable to open output file %s.\n",outputfile);
	return(-1);}
    if( fwrite(output+startptr,outptr-startptr,1,diskout) != 1 )
	{fclose(diskout);
	fprintf(stderr,"ERROR, unable to write output file %s.\n",outputfile);
	return(-1);}
    fclose(diskout);
    }
else if(comfile == file_exe)
    {exeheader[0]=0x5A4D;         // MZ
    exeheader[1]=runfilesize%512;
    exeheader[2]=runfilesize/512;
    exeheader[3]=0;     // # of relocation items
    exeheader[4]=2;     // size of header in paragraphs (2 paragraphs)
    paragraphsrequired = (outptr+outptrdata+postsize+stacksize+15) /16;
    exeheader[5]= (unsigned int) paragraphsrequired;  // min-paragraphs
    exeheader[6]= (unsigned int) paragraphsrequired;  // max-paragraphs
    exeheader[7]=outptr/16;      // displacement of SS
    exeheader[8]=outptrdata+postsize+stacksize; // intial value of SP
    exeheader[9]=0;                      // word check sum WHO KNOWS? prob all file
    for(count=0; count<9; count++)
	exeheader[9]+=exeheader[count];
    exeheader[10]=0;             // IP at entry (0x0000)
    exeheader[11]=0;             // displacement of CS (0x0000)
    exeheader[12]=0;             // offset of first relocation item (0x0000)
    exeheader[13]=0;             // overlay # (0 for resident code)
    exeheader[14]=0; exeheader[15]=0;  // unused

    sprintf(outputfile,"%s.EXE",rawfilename);
    if(!(diskout=fopen(outputfile,"wb")))
	{fprintf(stderr,"ERROR, unable to open output file %s.\n",outputfile);
	return(-1);}
    if( fwrite(exeheader,0x20,1,diskout) != 1 )
	{fclose(diskout);
	fprintf(stderr,"ERROR, unable to write EXE header to file %s.\n",outputfile);
	return(-1);}
    if( fwrite(output+startptr,outptr-startptr,1,diskout) != 1 )
	{fclose(diskout);
	fprintf(stderr,"ERROR, unable to write CODE to file %s.\n",outputfile);
	return(-1);}
    if( outptrdata-startptrdata != 0 )
	if( fwrite(outputdata+startptrdata,outptrdata-startptrdata,1,diskout) != 1 )
	    {fclose(diskout);
	    fprintf(stderr,"ERROR, unable to write DATA to file %s.\n",outputfile);
	    return(-1);}
    fclose(diskout);
    }
else{// OBJ output file.
    sprintf(outputfile,"%s.OBJ",rawfilename);
    out_objfile(outputfile,outptr-startptr,output+startptr);
    }
return(0);
}


startmapfile ()
{unsigned char mapfilename[80];
sprintf(mapfilename,"%s.MAP",rawfilename);
if(!(mapfile=fopen(mapfilename,"w")))
    {fprintf(stderr,"ERROR, unable to open map file %s.\n",++error,mapfilename);
    mapfile = 0;
    exit(e_cannotopenmapfile);}
fprintf(mapfile,"MAP FILE FOR %s.",rawfilename);
if( comfile == file_com )
    fprintf(mapfile,"COM\n\n");
else if( comfile == file_obj )
    fprintf(mapfile,"OBJ\n\n");
else fprintf(mapfile,"EXE\n\n");
}


finishmapfile ()
{fprintf(mapfile,"\n");
displaytree();
fprintf(mapfile,"Component Sizes:\n");
printmemsizes(mapfile);
fprintf(mapfile,"RUN File Size: %ld bytes\n",runfilesize);
fprintf(mapfile,"\nEND OF MAP FILE FOR %s.",rawfilename);
if( comfile == file_com )
    fprintf(mapfile,"COM");
else if( comfile == file_obj )
    fprintf(mapfile,"OBJ");
else fprintf(mapfile,"EXE");
fclose(mapfile);
}

/* ------------------- output procedures start ------------------- */

op (byte)
int byte;
{if(outptr>=MAXDATA)
    maxoutputerror();
output[outptr++]=byte;
if(comfile != file_exe) 
    outptrdata = outptr;
}


void opd (byte)
int byte;
{if(comfile != file_exe )
    {if(outptr>=MAXDATA)
	maxoutputerror();
    output[outptr++]=byte;
    outptrdata = outptr;
    }
else{if(outptrdata>=MAXDATA)
	maxdataerror();
    outputdata[outptrdata++]=byte;
    }
}


void outword (num)
unsigned int num;
{if(outptr>=MAXDATA-1)
    maxoutputerror();
output[outptr++]=num;
output[outptr++]=num/256;
if(comfile != file_exe)
    outptrdata = outptr;
}


void outwordd (num)
unsigned int num;
{if(comfile != file_exe)
    {if(outptr>=MAXDATA-1)
	maxoutputerror();
    output[outptr++]=num;
    output[outptr++]=num/256;
    outptrdata = outptr;
    }
else{if(outptrdata>=MAXDATA-1)
	maxdataerror();
    outputdata[outptrdata++]=num;
    outputdata[outptrdata++]=num/256;
    }
}


void outdword (num)
unsigned long num;
{outword( (unsigned int) (num & 0xFFFFL) );
outword( (unsigned int) (num / 0x10000L) );
}


void outdwordd (num)
unsigned long num;
{outwordd( (unsigned int) (num & 0xFFFFL) );
outwordd( (unsigned int) (num / 0x10000L) );
}


void outaddress (outrm,outnumber)
unsigned int outrm,outnumber;
{if( outrm == rm_d16 )
    outword(outnumber);
else{outrm &= rm_mod11;
    if( outrm == rm_mod11 )
	internalerror("Bad outrm value in outaddress();");
    else if( outrm == rm_mod10 )
	outword(outnumber);
    else if( outrm != rm_mod00 )
	op(outnumber);
    }
}



void doblock ()
{unsigned char done=0;
expecting(tk_openbrace);
while( tok != tk_eof && done==0 )
    {if(tok == tk_closebrace)
	{done=1;
	nexttok();}
    else docommand();
    }
if(done==0)
    unexpectedeof();
}


docommand ()     /* do a single command */
{int useflag=0;
switch(tok)
    {case tk_ID:  useflag = 1;
    case tk_id:   if(doid(useflag,tk_void)!=tokens)
		      nextseminext();
		  break;
    case tk_undefregproc:  useflag=1;
    case tk_undefproc:     if(doanyundefproc(useflag,tk_void)!=tokens)
			       nextseminext();
			   break;
    case tk_regproc:  useflag = 1;
    case tk_proc:     doanyproc(useflag,tk_void);
		      nextseminext();
		      break;
    case tk_charvar:  useflag = 1;
    case tk_bytevar:  dobytevar(useflag);
		      break;
    case tk_intvar:   useflag = 1;
    case tk_wordvar:  dowordvar(useflag);
		      break;
    case tk_longvar:   useflag = 1;
    case tk_dwordvar:  dodwordvar(useflag);
		       break;
    case tk_fixed32svar:  useflag = 1;
    case tk_fixed32uvar:  dodwordvar(useflag);
			  break;
    case tk_return:    doreturn();    break;
    case tk_at:  nexttok();
		 macros(tk_void);
		 nextseminext();
		 break;
    case tk_if:        doif();        break;
    case tk_IF:        dobigif();     break;
    case tk_loop:      doloop();      break;
    case tk_while:     whileerror();  nexttok();  break;
    case tk_do:        dodo();        break;
    case tk_reg32:     doreg32((unsigned int)number);  break;
    case tk_reg:       doreg((unsigned int)number);  break;
    case tk_beg:       dobeg((unsigned int)number);  break;
    case tk_seg:       doseg((unsigned int)number);  break;
    case tk_dollar:    doasm();     break;
    case tk_openbrace:  doblock();  break;
    case tk_from:  nexttok();  dofrom(0);  nextseminext();  break;
    case tk_extract:  nexttok();  doextract(0);  seminext();  break;
    case tk_minus:  useflag = 1;
    case tk_not:    nexttok();
		    notnegit(useflag,tok,(unsigned int)number,rm,post,segm);
		    nextseminext();
		    break;
    case tk_locallabel:  define_locallabel();  break;
    case tk_question:  directive();  break;
    case tk_camma:
    case tk_semicolon:  nexttok();        break;
    case tk_else:       elseerror();      break;
    case tk_ELSE:       ELSEerror();      break;
    case tk_eof:        unexpectedeof();  break;
    case tk_void:
    case tk_long:
    case tk_dword:
    case tk_word:
    case tk_byte:
    case tk_int:
    case tk_char:  notwithinblock();  break;
    default:  unuseableinput();  break;
    }
}


/* ---------------------- Procedure Calling Starts -------------------- */


int doanyundefproc (regproc,expectedreturn)
int regproc,expectedreturn;
{unsigned int cnum;
int returnvalue;
if( tok2 == tk_colon )  // if a label
    {if( regproc )    /* local label that has been used, but not placed */
	{addlocalvar(string,tk_number,outptr);
	updatecall( (unsigned int)number,outptr,procedure_start);
	}
    else{if( dynamic_flag != NOT_DYNAMIC )
	    dynamiclabelerror();
	tok = tk_proc;
	number = outptr;
	updatecall( (unsigned int)updatetree(), (unsigned int)number,0);
	}
    nexttok();  // move past id
    nexttok();  // move past :
    return(tokens);
    }
if( tok2 == tk_openbracket )
    {cnum = (unsigned int)number;
    if( typesize(expectedreturn) > typesize(rm) )
	{rm = expectedreturn;
	updatetree();}
    returnvalue = rm;
    nexttok();
    if(regproc)
	doregparams();
    else doparams();
    addacall(cnum,CALL_NEAR);
    callloc(0x0000);    /* produce CALL [#] */
    return(returnvalue);
    }
undefinederror();
nexttok();
return(tk_long);
}


int doanyproc (regflag)
int regflag;
{unsigned int cloc;
int returnvalue,dynamicindex = NOT_DYNAMIC;
cloc = (unsigned int)number;   /* get address or handle */
returnvalue = rm;
if( segm != NOT_DYNAMIC && segm < MAXDYNAMICS ) /* if a nonstatic dynamic */
    dynamicindex = segm;
nexttok();
if(regflag)
    doregparams();
else doparams();
if( dynamicindex != NOT_DYNAMIC )
    {addacall(cloc,CALL_NEAR);
    cloc = 0x0000;
    }
callloc(cloc);
return(returnvalue);
}


int doid (uppercase,expectedreturn)
char uppercase;
int expectedreturn;
{int cnum;
if( tok2 == tk_colon )  // if a label
    {if( uppercase )
	addlocalvar(string,tk_number,outptr);
    else{if( dynamic_flag != NOT_DYNAMIC )
	    dynamiclabelerror();
	tok = tk_proc;
	rm = tk_void;
	number = outptr;
	segm = NOT_DYNAMIC;
	addtotree(string);
	}
    nexttok();  // move past id
    nexttok();  // move past :
    return(tokens);
    }
if( tok2 == tk_openbracket )
    {tobedefined(CALL_NEAR,expectedreturn);
    cnum = posts-1;
    nexttok();
    if(uppercase)
	doregparams();
    else doparams();
    postloc[cnum] = outptr + 1;
    callloc(0x0000);      /* produce CALL [#] */
    return(expectedreturn);
    }
undefinederror();
nexttok();
return(tk_long);
}


int typesize (vartype)
/* return the size in bytes of the given type */
int vartype;
{char holdstr[60];
switch( vartype )
    {case tk_void:  return(0);
    case tk_char:
    case tk_byte:  return(1);
    case tk_int:
    case tk_word:  return(2);
    case tk_fixed32u:
    case tk_fixed32s:
    case tk_dword:
    case tk_long:  return(4);
    default:  sprintf(holdstr,"vartype=%d in typesize()",vartype);
	      internalerror(holdstr);
	      break;
    }
}



void convert_returnvalue (expectedreturn,actualreturn)
int expectedreturn,actualreturn;
{
if( expectedreturn == tk_void )
    return;

if( actualreturn == tk_void )
    {preerror("Procedure has return type of void");
    return;
    }

switch( expectedreturn )
    {case tk_byte:
    case tk_char:  if( actualreturn == tk_fixed32s ||
			   actualreturn == tk_fixed32u )
		       {  /* SHR EAX,16 */
		       outword(0xC166);  outword(0x10E8);
		       if( cpu < 3 )
			   cpu = 3;
		       }
		   break;
    case tk_word:
    case tk_int:  if( actualreturn == tk_fixed32s ||
			 actualreturn == tk_fixed32u )
		      {  /* SHR EAX,16 */
		      outword(0xC166);  outword(0x10E8);
		      if( cpu < 3 )
			  cpu = 3;
		      }
		  else if( actualreturn == tk_char )
		      cbw();
		  else if( actualreturn == tk_byte )
		      xorAHAH();
		  break;
    case tk_long:
    case tk_dword:  switch( actualreturn )
			{case tk_char:  cbd();  break;
			case tk_byte:   /* MOVZX EAX,AL */
				       outword(0x0F66); outword(0xC0B6);
				       break;
			case tk_word:  /* MOVZX EAX,AX */
				       outword(0x0F66); outword(0xC0B7);
				       break;
			case tk_int:  /* MOVSX EAX,AX */
				      outword(0x0F66); outword(0xC0BF);
				      break;
			case tk_fixed32s:  /* SAR EAX,16 */
					   outword(0xC166);  outword(0x10F8);
					   break;
			case tk_fixed32u:  /* SHR EAX,16 */
					   outword(0xC166);  outword(0x10E8);
					   break;
			}
		    if( cpu < 3 )
			cpu = 3;
		    break;
    case tk_fixed32s:
    case tk_fixed32u:
	    switch( actualreturn )
		{case tk_char:  cbw();
				/* SHL EAX,16 */
				outword(0xC166);  outword(0x10E0);
				break;
		case tk_byte:  xorAHAH();
		case tk_word:
		case tk_int:
		case tk_dword:
		case tk_long:
			       /* SHL EAX,16 */
			       outword(0xC166);  outword(0x10E0);
			       break;
		}
	    if( cpu < 3 )
		cpu = 3;
	    break;
    }
}


procdo (expectedreturn)
int expectedreturn;
{int actualreturn;
int idflag=0;
if( dynamic_flag != NOT_DYNAMIC )
    preerror("Function calls within a dynamic procedure are illegal");
switch(tok)
    {case tk_ID:  idflag=1;
    case tk_id:   actualreturn = doid(idflag,expectedreturn);
		  break;
    case tk_regproc:  idflag=1;
    case tk_proc:     actualreturn = doanyproc(idflag);
		      break;
    case tk_undefregproc: idflag=1;
    case tk_undefproc:    actualreturn=doanyundefproc(idflag,expectedreturn);
			  break;
    default:  internalerror("Bad tok in procdo();");
	      break;
    }
convert_returnvalue(expectedreturn,actualreturn);
}



/* +++++++++++++++++++++++ loops and ifs start ++++++++++++++++++++++++ */

/*** FIXED32 stuff not done for following procedure!!! ***/

int outcmp (swapped,ctok,cnumber,crm,csegm,cpost,ctok2,cnumber2,crm2,csegm2,cpost2)
int swapped,ctok,ctok2,crm,crm2,cpost,csegm,csegm2,cpost2;
long cnumber,cnumber2;
{unsigned char err=0;
switch(ctok)
    {case tk_reg:
	switch(ctok2)
	    {case tk_reg:  op(0x39);
			   op(0xC0 +(unsigned int)cnumber +(unsigned int)cnumber2*8);
			   break;
	    case tk_postnumber:
	    case tk_number:  if(cnumber==AX)
				 op(0x3D);
			     else{op(0x81);
				 op(0xF8 +(unsigned int)cnumber);}
			     if( ctok2 == tk_postnumber )
				 setwordpost();
			     outword((unsigned int)cnumber2);
			     break;
	    default:  if(swapped)
			  err = 1;
		      else return( outcmp(1,ctok2,cnumber2,crm2,csegm2,cpost2,ctok,cnumber,crm,csegm,cpost) );
		      break;
	    }
	break;
    case tk_intvar:
    case tk_wordvar:
	switch(ctok2)
	    {case tk_reg:  outseg(csegm,cpost,2);
			   op(0x39);
			   op((unsigned int)cnumber2*8 +crm);
			   outaddress(crm,(unsigned int)cnumber);
			   break;
	    case tk_postnumber:
	    case tk_number:  outseg(csegm,cpost,2);
			     op(0x81);
			     op(0x38+crm);
			     outaddress(crm,(unsigned int)cnumber);
			     if( ctok2 == tk_postnumber )
				 setwordpost();
			     outword((unsigned int)cnumber2);
			     break;
	    case tk_charvar:
	    case tk_intvar:  getintoax(1,ctok2,(unsigned int)cnumber2,crm2,csegm2,cpost2);
			     outseg(csegm,cpost,2);
			     op(0x39);  /* CMP [word],AX */
			     op(crm);
			     outaddress(crm,(unsigned int)cnumber);
			     break;
	    default:  getintoax(0,ctok2,(unsigned int)cnumber2,crm2,csegm2,cpost2);
		      outseg(csegm,cpost,2);
		      op(0x39); /* CMP [word],AX */
		      op(crm);
		      outaddress(crm,(unsigned int)cnumber);
		      break;
	    }
	break;
    case tk_number:  if( ctok2==tk_number || ctok2==tk_postnumber )
			 {op(0xB8);    /* MOV AX,# */
			 outword((unsigned int)cnumber);
			 op(0x3D);     /* CMP AX,# */
			 if(ctok2==tk_postnumber)
			     setwordpost();
			 outword((unsigned int)cnumber2);}
		     else{if(swapped)
			     err = 1;
			 else return( outcmp(1,ctok2,cnumber2,crm2,csegm2,cpost2,ctok,cnumber,crm,csegm,cpost) );
			 }
		     break;
    case tk_postnumber:  if( ctok2==tk_number || ctok2==tk_postnumber )
			     {op(0xB8); /* MOV AX,# */
			     setwordpost();
			     outword((unsigned int)cnumber);
			     op(0x3D);  /* CMP AX,# */
			     if(ctok2==tk_postnumber)
				 setwordpost();
			     outword((unsigned int)cnumber2);}
			 else{if(swapped)
				 err = 1;
			     else return( outcmp(1,ctok2,cnumber2,crm2,csegm2,cpost2,ctok,cnumber,crm,csegm,cpost) );
			     }
			 break;
    case tk_charvar:
    case tk_bytevar:  switch(ctok2)
			  {case tk_number:  outseg(csegm,cpost,2);
					    op(0x80);  /* CMP [byte],# */
					    op(0x38+crm);
					    outaddress(crm,(unsigned int)cnumber);
					    op((unsigned int)cnumber2);
					    break;
			  case tk_beg:   outseg(csegm,cpost,2);
					 op(0x38);   /* CMP [byte],beg */
					 op((unsigned int)cnumber2*8 +crm);
					 outaddress(crm,(unsigned int)cnumber);
					 break;
			  default:  getintoal(ctok2,(unsigned int)cnumber2,crm2,csegm2,cpost2);
				    outseg(csegm,cpost,2);
				    op(0x38);   /* CMP [byte],AL */
				    op(crm);
				    outaddress(crm,(unsigned int)cnumber);
				    break;
			   }
		       break;
    case tk_beg:  switch(ctok2)
		      {case tk_number:  if((unsigned int)cnumber == AL)
					    op(0x3C);
					else{op(0x80);
					    op(0xF8+(unsigned int)cnumber);}
					op((unsigned int)cnumber2);
					break;
		      case tk_beg:  op(0x38);
				    op(0xC0+(unsigned int)cnumber+(unsigned int)cnumber2*8);
				    break;
		      case tk_reg:  if((unsigned int)cnumber2 <= BX)  /* CMP beg,beg */
					{op(0x38);
					op(0xC0+(unsigned int)cnumber+(unsigned int)cnumber2*8);}
				    else{op(0x89);          /* MOV AX,reg */
					op(0xC0+(unsigned int)cnumber2*8);
					op(0x38);       /* CMP beg,AL */
					op(0xC0+(unsigned int)cnumber);}
				    break;
		      default:  if(swapped)
				    err = 1;
				else return( outcmp(1,ctok2,cnumber2,crm2,csegm2,cpost2,ctok,cnumber,crm,csegm,cpost) );
				break;
		      }
		  break;
    case tk_reg32:
	switch(ctok2)
	    {case tk_reg32:  outword(0x3966);
			     op(0xC0 +(unsigned int)cnumber +(unsigned int)cnumber2*8);
			     break;
	    case tk_postnumber:
	    case tk_number:  if(cnumber==EAX)
				 outword(0x3D66);
			     else{outword(0x8166);
				 op(0xF8 +(unsigned int)cnumber);}
			     if( ctok2 == tk_postnumber )
				 setdwordpost();
			     outdword(cnumber2);
			     break;
	    default:  if(swapped)
			  err = 1;
		      else return( outcmp(1,ctok2,cnumber2,crm2,csegm2,cpost2,ctok,cnumber,crm,csegm,cpost) );
		      break;
	    }
	break;
    case tk_longvar:
    case tk_dwordvar:
	switch(ctok2)
	    {case tk_reg32:  outseg(csegm,cpost,3);
			     outword(0x3966);
			     op((unsigned int)cnumber2*8 +crm);
			     outaddress(crm,(unsigned int)cnumber);
			     break;
	    case tk_postnumber:
	    case tk_number:  outseg(csegm,cpost,3);
			     outword(0x8166);
			     op(0x38+crm);
			     outaddress(crm,(unsigned int)cnumber);
			     if( ctok2 == tk_postnumber )
				 setdwordpost();
			     outdword(cnumber2);
			     break;
	    case tk_charvar:
	    case tk_intvar:  getintoeax(1,ctok2,cnumber2,crm2,csegm2,cpost2);
			     outseg(csegm,cpost,3);
			     outword(0x3966);   /* CMP [dword],EAX */
			     op(crm);
			     outaddress(crm,(unsigned int)cnumber);
			     break;
	    default:  getintoeax(0,ctok2,cnumber2,crm2,csegm2,cpost2);
		      outseg(csegm,cpost,3);
		      outword(0x3966); /* CMP [dword],EAX */
		      op(crm);
		      outaddress(crm,(unsigned int)cnumber);
		      break;
	    }
	break;
    default:  err = 1;
	      break;
    }
if(err)
    preerror("unable to create comparison, check restrictions");
return(swapped);
}


outjxx (comparetok,flipit,signedtype)     /* produce JXX [#] */
int comparetok,flipit,signedtype;
{int code;
if(!flipit)
    switch(comparetok)
	{case tk_equalto:  code = 0x74;  break;
	case tk_notequal:  code = 0x75;  break;
	case tk_greater:   if(signedtype==0)
			       code = 0x77;
			   else code = 0x7F;
			   break;
	case tk_less:  if(signedtype==0)
			   code = 0x72;
		       else code = 0x7C;
		       break;
	case tk_greaterequal:  if(signedtype==0)
				   code = 0x73;
			       else code = 0x7D;
			       break;
	case tk_lessequal:  if(signedtype==0)
				code = 0x76;
			    else code = 0x7E;
			    break;
	default:  unknowncompop();
		  break;
	}
else switch(comparetok)     /* all are flipped types */
	{case tk_equalto:  code = 0x75;  break;
	case tk_notequal:  code = 0x74;  break;
	case tk_greater:  if(signedtype==0)
			      code = 0x76;
			  else code = 0x7E;
			  break;
	case tk_less:  if(signedtype==0)
			   code = 0x73;
		       else code = 0x7D;
		       break;
	case tk_greaterequal:  if(signedtype==0)
				   code = 0x72;
			       else code = 0x7C;
			       break;
	case tk_lessequal:  if(signedtype==0)
				code = 0x77;
			    else code = 0x7F;
			    break;
	default:  internalerror("Bad comparetok in outjxx();");
		  break;
	}
op(code);  /* output instruction code */
}


int getjumptype(gtok,gtok2,vartype)  /* determines if signed operand */
int gtok,gtok2,vartype;
{if( vartype == tk_char || vartype == tk_int || vartype == tk_long
	|| vartype == tk_fixed32s )
    return(1);
return(0);
}


int iscomparetok (ctok)
int ctok;
{if( ctok==tk_equalto || ctok==tk_notequal || ctok==tk_greater ||
	ctok==tk_less || ctok==tk_greaterequal || ctok==tk_lessequal )
    return(1);
else return(0);
}


int doflag (op1,op2,invertflag,startloc)
int op1,op2;
int invertflag;
unsigned int startloc;
{if(invertflag==2)
    {if( outptr+2-startloc > 127)       // long jump
	invertflag=1;
    else invertflag=0;
    }
if(invertflag)
    op(op2);
else op(op1);
nexttok();
expecting(tk_closebracket);
return(invertflag);
}


int constructcompare (invertflag,startloc)
/* build cmp for IF, if and do {} while */
int invertflag;
unsigned int startloc;
{int comparetok,jumptype,cmpreturn,vartype;
int itok,irm,isegm,ipost,itok2,irm2,isegm2,ipost2;
long inumber,inumber2;
nexttok();
expecting(tk_openbracket);
switch(tok)
    {case tk_fixed32s:
    case tk_fixed32u:
    case tk_int:
    case tk_word:
    case tk_char:
    case tk_byte:
    case tk_long:
    case tk_dword:  vartype = tok;  nexttok();  break;
    case tk_carryflag:        return(doflag(0x72,0x73,invertflag,startloc));
    case tk_notcarryflag:     return(doflag(0x73,0x72,invertflag,startloc));
    case tk_overflowflag:     return(doflag(0x70,0x71,invertflag,startloc));
    case tk_notoverflowflag:  return(doflag(0x71,0x70,invertflag,startloc));
    case tk_zeroflag:         return(doflag(0x74,0x75,invertflag,startloc));
    case tk_notzeroflag:      return(doflag(0x75,0x74,invertflag,startloc));
    case tk_regproc:
    case tk_proc:  vartype = rm;
		   if( vartype == tokens )
		       vartype = tk_word;
		   else if( vartype == tk_void )
		       {preerror("Procedure has a return type of void");
		       vartype = tk_word;}
		   break;
    case tk_beg:
    case tk_bytevar:  vartype = tk_byte;  break;
    case tk_charvar:  vartype = tk_char;  break;
    case tk_intvar:   vartype = tk_int;   break;
    case tk_reg32:
    case tk_dwordvar: vartype = tk_dword;  break;
    case tk_longvar:  vartype = tk_long;   break;
    case tk_fixed32svar:  vartype = tk_fixed32s;  break;
    case tk_fixed32uvar:  vartype = tk_fixed32u;  break;
    case tk_at:  nexttok();
		 if( tok == tk_regproc && segm != NOT_DYNAMIC )
		     vartype = rm;
		 else vartype = tk_word;
		 macros(vartype);
		 switch(vartype)
		     {case tk_byte:
		     case tk_char:  tok = tk_beg;  break;
		     case tk_int:
		     case tk_word:  tok = tk_reg;  break;
		     case tk_fixed32u:
		     case tk_fixed32s:
		     case tk_dword:
		     case tk_long:  tok = tk_reg32;     break;
		     default:  preerror("Macro has a return type of void");
			       tok = tk_reg;
			       break;
		     }
		 number = AX;  // or AL or EAX
		 break;
    default:  vartype = tk_word;  break;
    }
if(tok==tk_minus && tok2 == tk_number)
    {nexttok();
    number = -number;
    }
if( iscomparetok(tok2) == 0 )
    {switch(vartype)
	{case tk_int:  doaxmath(1);  itok = tk_reg;  break;
	case tk_word:  doaxmath(0);  itok = tk_reg;  break;
	case tk_char:  doalmath(1);  itok = tk_beg;  break;
	case tk_byte:  doalmath(0);  itok = tk_beg;  break;
	case tk_long:  doeaxmath(1);  itok = tk_reg32;  break;
	case tk_dword:  doeaxmath(0);  itok = tk_reg32;  break;
	case tk_fixed32s:  dofixed32math(1);  itok = tk_reg32;  break;
	case tk_fixed32u:  dofixed32math(0);  itok = tk_reg32;  break;
	default:  internalerror("Bad vartype value in constructcompare();");
		  tok = tk_reg;  break;
	}
    inumber = AX;       /* same value as AL and EAX */
    }
else{itok = tok;
    irm = rm;
    isegm = segm;
    ipost = post;
    inumber = number;
    nexttok();
    }
if( tok != tk_closebracket )
    {if( iscomparetok(tok) )
	comparetok = tok;
    else{unknowncompop();
	comparetok = tk_equalto;}
    nexttok();
    if(tok==tk_minus)
	{nexttok();
	if(tok != tk_number)
	    negcomperror();
	else number = -number;
	}
    if(tok == tk_number)
	{switch(vartype)
	    {case tk_int:
	    case tk_char:
	    case tk_long:  inumber2 = doconstlongmath();  break;
	    case tk_word:
	    case tk_byte:
	    case tk_dword:  inumber2 = doconstdwordmath();  break;
/*** FIXED32 stuff has to be done right here ***/
	    }
	itok2 = tk_number;
	}
    else{itok2 = tok;
	inumber2 = number;
	irm2 = rm;
	isegm2 = segm;
	ipost2 = post;
	nexttok();
	}
    }
else{comparetok = tk_notequal;
    itok2 = tk_number;
    inumber2 = 0;
    irm2 = rm_d16;
    isegm2 = DS;
    ipost2 = 0;
    }
jumptype = getjumptype(itok,itok2,vartype);
cmpreturn = outcmp(0,itok,inumber,irm,isegm,ipost,itok2,inumber2,irm2,isegm2,ipost2);
if(invertflag==2)
    {if( outptr+2-startloc > 127)        /* long jump */
	invertflag=1;
    else invertflag=0;
    }
if( cmpreturn != 0 )  // if operands have been swapped
    {switch(comparetok)
	{case tk_less:         comparetok = tk_greater;       break;
	case tk_lessequal:     comparetok = tk_greaterequal;  break;
	case tk_greater:       comparetok = tk_less;          break;
	case tk_greaterequal:  comparetok = tk_lessequal;     break;
	}
    }
outjxx(comparetok,invertflag,jumptype);
expecting(tk_closebracket);
return(invertflag);
}


doif ()
{unsigned int startloc,elseline;
constructcompare(0,outptr);
op(0x03);        /* jump over word jump instruction of 3 bytes */
op(0xE9);        /* JMP word */
op(0x00);
op(0x00);
startloc = outptr;
docommand();
if(tok==tk_else)
    {op(0xE9);
    op(0x00);
    op(0x00);
    output[startloc-2] = (outptr - startloc) %256;
    output[startloc-1] = (outptr - startloc) /256;
    startloc = outptr;
    nexttok();
    docommand();
    output[startloc-2] = (outptr - startloc) %256;
    output[startloc-1] = (outptr - startloc) /256;
    }
else if(tok==tk_ELSE)
    {elseline = linenumber;
    op(0xEB);
    op(0x00);
    output[startloc-2] = (outptr - startloc) %256;
    output[startloc-1] = (outptr - startloc) /256;
    startloc = outptr;
    nexttok();
    docommand();
    if( outptr-startloc > 127)
	ELSEjumperror(elseline);
    output[startloc-1] = outptr - startloc;
    }
else{output[startloc-2] = (outptr - startloc) %256;
    output[startloc-1] = (outptr - startloc) /256;
    }
}


int dobigif ()
{unsigned int startloc,elseline,ifline;
ifline = linenumber;
constructcompare(1,outptr);
op(0x00);
startloc = outptr;
docommand();
if(tok==tk_else)
    {op(0xE9);
    op(0x00);
    op(0x00);
    if( outptr-startloc > 127)
	IFjumperror(ifline);
    output[startloc-1] = outptr - startloc;
    startloc = outptr;
    nexttok();
    docommand();
    output[startloc-2] = (outptr - startloc) %256;
    output[startloc-1] = (outptr - startloc) /256;
    }
else if(tok==tk_ELSE)
    {elseline = linenumber;
    op(0xEB);
    op(0x00);
    if( outptr-startloc > 127)
	IFjumperror(ifline);
    output[startloc-1] = outptr - startloc;
    startloc = outptr;
    nexttok();
    docommand();
    if( outptr-startloc > 127)
	ELSEjumperror(elseline);
    output[startloc-1] = outptr - startloc;
    }
else{if( outptr-startloc > 127)
	IFjumperror(ifline);
    output[startloc-1] = outptr - startloc;
    }
}


dodo ()
{unsigned int startloc;
nexttok();
startloc = outptr;
docommand();
if( tok!=tk_while )
    preerror("'while' expected following 'do'");
if( constructcompare(2,startloc) )   /* long jump */
    {op(0x03);              /* jump over large jump */
    op(0xE9);
    outword(startloc - (outptr+2));   /* the large jump */
    }
else op(startloc - (outptr+1));      /* the small jump */
seminext();
}


void decit (dectok,decnumber,decrm,decsegm,decpost,errormessage)
/*
    outputs code to decrement the given variable one.
*/
int dectok,decrm,decsegm,decpost,errormessage;
unsigned int decnumber;
{int vop = 0;
switch(dectok)
    {
    case tk_fixed32u:
    case tk_fixed32s:  outseg(decsegm,decpost,2);
		       op(0xFF);
		       op(0x08+decrm);
		       outaddress(decrm,decnumber+2); /* dec high word only */
		       break;
    case tk_wordvar:
    case tk_intvar:   vop = 1;
    case tk_bytevar:
    case tk_charvar:  outseg(decsegm,decpost,2);
		      op(0xFE+vop);
		      op(0x08+decrm);
		      outaddress(decrm,decnumber);
		      break;
    case tk_dwordvar:
    case tk_longvar:  outseg(decsegm,decpost,3);
		      outword(0xFF66);  op(0x08+decrm);
		      outaddress(decrm,decnumber);
		      if( cpu < 3 )
			  cpu = 3;
		      break;
    case tk_reg:  op(0x48+decnumber);
		  break;
    case tk_beg:  op(0xFE);
		  op(0xC8+decnumber);
		  break;
    case tk_reg32:  op(0x66);  op(0x48+decnumber);
		    if( cpu < 3 )
			cpu = 3;
		    break;
    default:  if(errormessage)
		  preerror("invalid token to be decremented");
	      break;
    }
}


doloop ()               /* both short and long loops */
{unsigned int startloc,loopnumber;
int looptok,looprm,loopsegm,looppost;
signed char delta;
nexttok();
expecting(tk_openbracket);
looptok = tok;
loopnumber = (unsigned int)number;
looprm = rm;
looppost = post;
loopsegm = segm;
if( tok!=tk_bytevar && tok!=tk_charvar && tok!=tk_wordvar && tok!=tk_intvar
      && tok!=tk_dwordvar && tok!=tk_longvar && tok!=tk_reg && tok!=tk_reg32
      && tok!=tk_beg && tok!=tk_closebracket
      && tok!=tk_fixed32svar && tok!=tk_fixed32uvar )
    preerror("invalid token to be decremented");
if( tok != tk_closebracket )
    nexttok();
expecting(tk_closebracket);
startloc = outptr;
docommand();
if( looptok != tk_closebracket )
    {if( (outptr-startloc <= 127-2) && (chip < 3 || optimizespeed==0)
	    && looptok == tk_reg && loopnumber == CX )
	{delta = startloc - (outptr+2);
	op(0xE2);
	op(delta);  /* LOOP 'delta' */
	}
    else{
	decit( looptok,loopnumber,looprm,loopsegm,looppost,0 );
	if( outptr-startloc > 127-2 )   /* long jump */
	    {if( chip < 3 )
		{op(0x74);
		op(3);  /* JZ past jump up */
		op(0xE9);
		outword( startloc - (outptr+2) );}  /* JMP top of loop */
	    else{op(0x0F); op(0x85);
		outword( startloc - (outptr+2) );  /* JNZ <full disp.> */
		if( cpu < 3 )
		    cpu = 3;
		}
	    }
	else{delta = startloc - (outptr+2);     /* short jump */
	    op(0x75);
	    op(delta);} /* JNZ 'delta' */
	}
    }
else{if( (outptr+2)-startloc > 127)     /* long jump */
	{op(0xE9);
	outword( startloc - (outptr+2) );}      /* JMP top of loop */
    else{delta = startloc - (outptr+2); /* short jump */
	op(0xEB);
	op(delta);}     /* JNZ 'delta' */
    }
}


/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


globalvar (type)   /* both initialized and unitialized combined */
/*
    fixed32u and fixed32s are initialized with 32bit ints, for now...
*/
int type;
{long size,loop,i,longpostsize,elements=1;
char done=0;
char var_name[IDLENGTH];

nexttok();
if( tok == tk_far )
    {nexttok();
    declare_procedure(type,cpt_far);
    return(0);    
    }
if(tok2==tk_openbracket)
    {declare_procedure(type,cpt_near);
    return(0);
    }

size = typesize(type);

while(tok!=tk_eof && done==0)
    {switch(tok)
	{case tk_id:
	case tk_ID:
	    strcpy(var_name, string);
	    nexttok();
	    if(tok==tk_openblock)
		{nexttok();
		elements = doconstlongmath();
		expecting(tk_closeblock);}
	    else elements = 1;
	    if(tok == tk_assign)
		{switch(type)
		    {case tk_byte:      tok = tk_bytevar;       break;
		    case tk_word:       tok = tk_wordvar;       break;
		    case tk_char:       tok = tk_charvar;       break;
		    case tk_int:        tok = tk_intvar;        break;
		    case tk_dword:      tok = tk_dwordvar;      break;
		    case tk_long:       tok = tk_longvar;       break;
		    case tk_fixed32s:   tok = tk_fixed32svar;   break;
		    case tk_fixed32u:   tok = tk_fixed32uvar;   break;
		    default:  internalerror("Bad type in globalvar();");
			      break;
		    }
		if( size==2 )
		    {if( alignword && outptrdata%2 == 1)
			{opd(aligner);
			alignersize++;}
		    }
		else if( size == 4 )
		    {if( alignword )
			{switch( outptrdata%4 )
			    {case 1:  opd(aligner);
				      alignersize++;
			    case 2:   opd(aligner);
				      alignersize++;
			    case 3:   opd(aligner);
				      alignersize++;
				      break;
			    }
			}
		    }
		number = outptrdata;
		rm = rm_d16;
		segm = DS;
		addtotree(var_name);
		nexttok();
		switch(tok)
		    {case tk_minus:
		    case tk_number:  if( type==tk_byte || type==tk_word ||
					     type==tk_dword || type==tk_fixed32u )
					 i = doconstdwordmath();
				     else i = doconstlongmath();
				     for(loop=0;loop<elements;loop++)
					 {switch(size)
					     {case 1:  opd((unsigned int)i);    break;
					     case 2: outwordd((unsigned int)i); break;
					     case 4: outdwordd(i);              break;
					     }
					 datasize += size;
					 }
				     break;
		    case tk_string:  loop = 1;  /* because of opd(0); */
				     do {for(i=0;i<number;i++)
					    {opd(string[i]);
					    loop++;
					    datasize++;}
					 nexttok();
					 } while( tok == tk_string );
				     opd(0);
				     datasize++;
				     for(;loop<size*elements;loop++)
					 {opd(0);
					 datasize++;}
				     break;
		    case tk_from:  nexttok();
				   if( comfile == file_exe )
				       loop = dofrom(1);
				   else loop = dofrom(0);
				   datasize += loop;
				   for(;loop<size*elements;loop++)
				       {opd(0);
				       datasize++;}
				   nexttok();
				   break;
		    case tk_extract:  nexttok();
				      if( comfile == file_exe )
					  loop = doextract(1);
				      else loop = doextract(0);
				      datasize += loop;
				      for(;loop<size*elements;loop++)
					  {opd(0);
					  datasize++;}
				      break;
		    case tk_openbrace:  loop = 0;
					nexttok();
					i = 0;
					loop = 0;
					while( tok != tk_closebrace )
					    {if(i)
						expecting(tk_camma);
					    i = 1;
					    if(tok != tk_number && tok != tk_minus)
						{numexpected();
						nexttok();}
					    else{switch(type)
						    {case tk_byte:  opd((unsigned int)doconstdwordmath());
								    break;
						    case tk_word:  outwordd((unsigned int)doconstdwordmath());
								   break;
						    case tk_char:  opd((int)doconstlongmath());
								   break;
						    case tk_int:  outwordd((int)doconstlongmath());
								  break;
						    case tk_fixed32u:
						    case tk_dword:  outdwordd(doconstdwordmath());
								    break;
						    case tk_fixed32s:
						    case tk_long:  outdwordd(doconstlongmath());
								   break;
						    default:  internalerror("Bad type in globalvar();");
							      break;
						    }
						datasize += size;
						loop++;
						}
					    }
					for(;loop<elements;loop++)
					    {switch(size)
						{case 1: opd(0);  break;
						case 2: outwordd(0);  break;
						case 4: outdwordd(0);  break;
						}
					    datasize += size;}
					nexttok();
					break;
		    default:  numexpected();
			      nexttok();
			      break;
		    }
		}
	    else{switch(tok)
		    {default:       expected(';');
		    case tk_semicolon:  done=1;
		    case tk_camma:              /* post global type */
			switch(type)
			    {case tk_byte: tok=tk_bytevar;  break;
			    case tk_word:  tok=tk_wordvar;  break;
			    case tk_char:  tok=tk_charvar;  break;
			    case tk_int:   tok=tk_intvar;   break;
			    case tk_dword: tok=tk_dwordvar; break;
			    case tk_long:  tok=tk_longvar;  break;
			    case tk_fixed32s:  tok=tk_fixed32svar;  break;
			    case tk_fixed32u:  tok=tk_fixed32uvar;  break;
			    default:  internalerror("Bad type in globalvar();");
				      break;
			    }
			post = 1;
			segm = DS;
			if( alignword )
			    {if( size == 2 )
				{if( postsize%2 == 1 )
				    postsize++;
				}
			    else if( size == 4 )
				postsize += outptrdata%4;
			    }
			number = postsize;
			addtotree(var_name);
			longpostsize = elements * size + postsize;
			if( longpostsize > 0xFFFFL )
			    {preerror("maximum size of post variables exceeded");
			    postsize = 0xFFFF;}
			else postsize = longpostsize;
			nexttok();
			break;
		    }
		}
	    break;
	case tk_proc:
	case tk_regproc:
	case tk_fixed32svar:
	case tk_fixed32uvar:
	case tk_dwordvar:
	case tk_longvar:
	case tk_charvar:
	case tk_intvar:
	case tk_bytevar:
	case tk_wordvar:  idalreadydefined();
			  nexttok();
			  break;
	default:            expected(';');
	case tk_semicolon:  done = 1;
	case tk_camma:      nexttok();
			    break;
	}
    }
}


/* ======= procedure handling starts here ======== */


int setuprm (defflag)
/* set rm, segm and returntype for proper values required for adding
   or modifing the id tree entry of a undef to regproc or proc.
*/
int defflag;
{segm = dynamic_flag;
if( segm != NOT_DYNAMIC )       /* if dynamic proc */
    number = secondcallnum++;   /* get a call number reference */
if( defflag == 0 )
    rm = tk_void;
if( rm != tokens )
    {if( returntype == tokens )
	returntype = rm;
    else if( typesize(returntype) < typesize(rm) )
	{badreturntype();
	return(-1);}
    else rm = returntype;
    }
else if( returntype == tokens )
    {rm = tk_void;
    returntype = tk_void;
    }
else rm = returntype;
return(0);
}


void doregparams ()
{int signflag=0;
expecting(tk_openbracket);
if(tok == tk_closebracket)
    return;
if(tok != tk_camma)
    {switch(tok)
	{
	case tk_int:   signflag=1;
	case tk_word:  nexttok();  doaxmath(signflag);  break;
	case tk_char:  signflag=1;
	case tk_byte:  nexttok();  doalmath(signflag);  break;
	case tk_long:   signflag=1;
	case tk_dword:  nexttok();  doeaxmath(signflag); break;
	case tk_fixed32s:  signflag=1;
	case tk_fixed32u:  nexttok();  dofixed32math(signflag); break;
	case tk_string:  op(0xB8);
			 outword(addpoststring());
			 nexttok();
			 break;
	case tk_longvar:   signflag=1;
	case tk_reg32:
	case tk_dwordvar:  doeaxmath(signflag);  break;
	case tk_fixed32svar:  signflag=1;
	case tk_fixed32uvar:  dofixed32math(signflag); break;
	default:  doaxmath(0);  break;
	}
    }
if(tok == tk_camma)
    nexttok();
else{if(tok != tk_closebracket)
	expected(')');
    return;}
if(tok != tk_camma)
    {switch( tok )
	{case tk_char:
	case tk_byte:  nexttok();  getintobeg(BL);  dobegmath(BL);  break;
	case tk_dword:
	case tk_long:  nexttok();  getintoreg32(EBX);  doreg32math(EBX);  break;
	case tk_fixed32s:
	case tk_fixed32u:  nexttok();
			   getintofixedreg32(EBX);
			   dofixedreg32math(EBX);
			   break;
	case tk_string:  op(0xB8+BX);
			 outword(addpoststring());
			 nexttok();
			 break;
	case tk_dwordvar:
	case tk_reg32:
	case tk_longvar:  getintoreg32(EBX);  doreg32math(EBX);  break;
	case tk_fixed32svar:
	case tk_fixed32uvar:  getintofixedreg32(EBX);
			      dofixedreg32math(EBX);
			      break;
	case tk_int:
	case tk_word:  nexttok();
	default:       getintoreg(BX);
		       doregmath(BX);
		       break;
	}
    }
if(tok == tk_camma)
    nexttok();
else{if(tok != tk_closebracket)
	expected(')');
    return;}
if(tok != tk_camma)
    {switch( tok )
	{case tk_char:
	case tk_byte:  nexttok();  getintobeg(CL);  dobegmath(CL);  break;
	case tk_dword:
	case tk_long:  nexttok();  getintoreg32(ECX);  doreg32math(ECX);  break;
	case tk_fixed32s:
	case tk_fixed32u:  nexttok();
			   getintofixedreg32(ECX);
			   dofixedreg32math(ECX);
			   break;
	case tk_string:  op(0xB8+CX);
			 outword(addpoststring());
			 nexttok();
			 break;
	case tk_dwordvar:
	case tk_reg32:
	case tk_longvar:  getintoreg32(ECX);  doreg32math(ECX);  break;
	case tk_fixed32svar:
	case tk_fixed32uvar:  getintofixedreg32(ECX);
			      dofixedreg32math(ECX);
			      break;
	case tk_int:
	case tk_word:  nexttok();
	default:       getintoreg(CX);
		       doregmath(CX);
		       break;
	}
    }
if(tok == tk_camma)
    nexttok();
else{if(tok != tk_closebracket)
	expected(')');
    return;}
if(tok != tk_camma)
    {switch( tok )
	{case tk_char:
	case tk_byte:  nexttok();  getintobeg(DL);  dobegmath(DL);  break;
	case tk_dword:
	case tk_long:  nexttok();  getintoreg32(EDX);  doreg32math(EDX);  break;
	case tk_fixed32s:
	case tk_fixed32u:  nexttok();
			   getintofixedreg32(EDX);
			   dofixedreg32math(EDX);
			   break;
	case tk_string:  op(0xB8+DX);
			 outword(addpoststring());
			 nexttok();
			 break;
	case tk_dwordvar:
	case tk_reg32:
	case tk_longvar:  getintoreg32(EDX);  doreg32math(EDX);  break;
	case tk_fixed32svar:
	case tk_fixed32uvar:  getintofixedreg32(EDX);
			      dofixedreg32math(EDX);
			      break;
	case tk_int:
	case tk_word:  nexttok();
	default:       getintoreg(DX);
		       doregmath(DX);
		       break;
	}
    }
if(tok == tk_camma)
    nexttok();
else{if(tok != tk_closebracket)
	expected(')');
    return;}
if(tok != tk_camma)
    {switch( tok )
	{case tk_char:
	case tk_byte:  preerror("Only DI and EDI math valid");
		       break;
	case tk_dword:
	case tk_long:  nexttok();  getintoreg32(EDI);  doreg32math(EDI);  break;
	case tk_fixed32s:
	case tk_fixed32u:  nexttok();
			   getintofixedreg32(EDI);
			   dofixedreg32math(EDI);
			   break;
	case tk_string:  op(0xB8+DI);
			 outword(addpoststring());
			 nexttok();
			 break;
	case tk_dwordvar:
	case tk_reg32:
	case tk_longvar:  getintoreg32(EDI);  doreg32math(EDI);  break;
	case tk_fixed32svar:
	case tk_fixed32uvar:  getintofixedreg32(EDI);
			      dofixedreg32math(EDI);
			      break;
	case tk_int:
	case tk_word:  nexttok();
	default:       getintoreg(DI);
		       doregmath(DI);
		       break;
	}
    }
if(tok == tk_camma)
    nexttok();
else{if(tok != tk_closebracket)
	expected(')');
    return;}
if(tok != tk_camma)
    {switch( tok )
	{case tk_char:
	case tk_byte:  preerror("Only SI and ESI math valid");
		       break;
	case tk_dword:
	case tk_long:  nexttok();  getintoreg32(ESI);  doreg32math(ESI);  break;
	case tk_fixed32s:
	case tk_fixed32u:  nexttok();
			   getintofixedreg32(ESI);
			   dofixedreg32math(ESI);
			   break;
	case tk_string:  op(0xB8+SI);
			 outword(addpoststring());
			 nexttok();
			 break;
	case tk_dwordvar:
	case tk_reg32:
	case tk_longvar:  getintoreg32(ESI);  doreg32math(ESI);  break;
	case tk_fixed32svar:
	case tk_fixed32uvar:  getintofixedreg32(ESI);
			      dofixedreg32math(ESI);
			      break;
	case tk_int:
	case tk_word:  nexttok();
	default:       getintoreg(SI);
		       doregmath(SI);
		       break;
	}
    }
if(tok!=tk_closebracket)
    expected(')');
}


regproc (int defflag,int proc_type)
{int inlineflag = 0;  // flag for disabling entry and exit codes production
current_proc_type = proc_type;
tok = tk_regproc;
number = outptr;
if( setuprm(defflag) == -1 )
    return(-1);
if(defflag)
    updatecall( (unsigned int)updatetree(), (unsigned int)number, 0 );
else addtotree(string);
nexttok();
expecting(tk_openbracket);
expecting(tk_closebracket);
if( tok == tk_inline )
    {inlineflag = 1;
    nexttok();}
else if( tok != tk_openbrace )
    declarelocals();
doblock();
if( inlineflag == 0 )
    leaveproc();
killlocals();
}


proc (int defflag,int proc_type)
{int inlineflag = 0;  // flag for disabling entry and exit codes production
current_proc_type = proc_type;
tok = tk_proc;
number = outptr;
if( setuprm(defflag) == -1 )
    return(-1);
if(defflag)
    updatecall( (unsigned int)updatetree(), (unsigned int)number, 0 );
else addtotree(string);
nexttok();
expecting(tk_openbracket);
if(tok!=tk_closebracket)
    declareparams();
nexttok();
if( tok == tk_inline )
    {inlineflag = 1;
    nexttok();}
else{if(paramsize > 0)
	{op(0x55);         /* PUSH BP */
	outword(0xE589);}  /* MOV BP,SP */
    if( tok != tk_openbrace )
	declarelocals();
    }
doblock();              /* do proc */
if( inlineflag == 0 )
    leaveproc();
killlocals();
}


declareparams ()                   /* declare procedure parameters */
{int paramtok;
char flag = 33;

do {if(flag != 33)
	nexttok();
    flag = 1;
    switch(tok)
	{case tk_int:  paramtok = tk_paramint;   break;
	case tk_word:  paramtok = tk_paramword;  break;
	case tk_char:  paramtok = tk_paramchar;  break;
	case tk_byte:  paramtok = tk_parambyte;  break;
	case tk_long:   paramtok = tk_paramlong;  break;
	case tk_dword:  paramtok = tk_paramdword;  break;
	case tk_fixed32s:  paramtok = tk_paramfixed32s;  break;
	case tk_fixed32u:  paramtok = tk_paramfixed32u;  break;
	default:  datatype_expected();
		  flag = 0;
		  nexttok();
		  break;
	}
    if( flag )
	{do {nexttok();
	    if( tok != tk_ID && tok != tk_id )
		idexpected();
	    else{paramsize += 2;
		if( paramtok == tk_paramlong || paramtok == tk_paramdword
			|| paramtok == tk_paramfixed32s
			|| paramtok == tk_paramfixed32u )
		    paramsize += 2;   // add 2 more bytes
		addlocalvar(string,paramtok,paramsize);
		}
	    nexttok();
	    } while ( tok == tk_camma );
	}
    } while( tok == tk_semicolon );
}


declarelocals ()                           /* declare locals */
{unsigned int size;
int localtok;

do {switch(tok)
	{case tk_int:  localtok = tk_localint;   size = 2;  break;
	case tk_word:  localtok = tk_localword;  size = 2;  break;
	case tk_char:  localtok = tk_localchar;  size = 1;  break;
	case tk_byte:  localtok = tk_localbyte;  size = 1;  break;
	case tk_long:  localtok = tk_locallong;  size = 4;  break;
	case tk_dword:  localtok = tk_localdword;  size = 4;  break;
	case tk_fixed32s:  localtok = tk_localfixed32s;  size = 4;  break;
	case tk_fixed32u:  localtok = tk_localfixed32u;  size = 4;  break;
	default:  datatype_expected();
		  nexttok();
		  size = 0;
		  break;
	}
    if( size != 0 )
	{do {nexttok();
	    if( tok != tk_ID && tok != tk_id )
		idexpected();
	    else{addlocalvar(string,localtok,localsize);
		nexttok();
		if( tok == tk_openblock )
		    {nexttok();
		    localsize += doconstlongmath() * size;
		    expecting(tk_closeblock);}
		else localsize += size;
		}
	    } while ( tok == tk_camma );
	expecting(tk_semicolon);
	}
    } while( tok != tk_openbrace && tok != tk_eof );
if( optimizespeed || chip < 2 )
    {if(paramsize == 0) /* else this has already been done */
	{op(0x55);            /* PUSH BP */
	op(0x89);  op(0xE5);  /* MOV BP,SP */
	}
    op(0x81);  op(0xEC);
    outword(localsize);}          /* sub SP, # of locals */
else{if(paramsize != 0)
	outptr -= 3;          /* remove PUSH BP and MOV BP,SP */
    op(0xC8);             /* ENTER */
    outword(localsize);   /* # of locals */
    op(0x00);             /* level = 0 */
    if( cpu < 2 )
	cpu = 2;
    }
}


declare_procedure (int rtype,int proc_type)
{returntype = rtype;
procedure_start = outptr;
switch(tok)
    {case tk_id:           proc(0,proc_type);     break;
    case tk_ID:            regproc(0,proc_type);  break;
    case tk_undefregproc:  regproc(1,proc_type);  break;
    case tk_undefproc:     proc(1,proc_type);     break;
    default:               idexpected();          break;
    }
dopoststrings();
}


interruptproc ()
{procedure_start = outptr;
returntype = tk_void;
current_proc_type = cpt_interrupt;
paramsize = 0;
localsize = 0;
nexttok();
if(tok == tk_ID || tok == tk_id)
    {tok = tk_interruptproc;
    number = outptr;
    segm = NOT_DYNAMIC;
    post = 0;
    rm = tk_void;
    addtotree(string);}
else idexpected();
nexttok();
expecting(tk_openbracket);
expecting(tk_closebracket);
doblock();
leaveproc();
paramsize = 0;
localsize = 0;
dopoststrings();
}


doparams ()      /* do stack procedure parameter pushing */
{char done=0,next;
int vartype;
int count;
expecting(tk_openbracket);
if(tok!=tk_closebracket)
    {while(tok!=tk_eof && done==0)
	{if(tok==tk_string)
	    {if(chip<2)
		{op(0xB8);
		outword(addpoststring());  /* MOV AX,imm16 */
		op(0x50);}      /* PUSH AX */
	    else{op(0x68);
		outword(addpoststring());  /* PUSH imm16 */
		if(cpu < 2)
		    cpu=2;
		}
	    nexttok();
	    }
	else{switch(tok)
		{case tk_int:
		case tk_word:
		case tk_char:
		case tk_byte:
		case tk_long:
		case tk_fixed32s:
		case tk_fixed32u:
		case tk_dword:  vartype = tok;
				nexttok();
				break;
		case tk_longvar:      vartype = tk_long;      break;
		case tk_fixed32svar:  vartype = tk_fixed32s;  break;
		case tk_fixed32uvar:  vartype = tk_fixed32u;  break;
		case tk_dwordvar:     vartype = tk_dword;     break;
		default:        vartype = tk_word;      break;
		}
	    if( tok2isopperand() )
		{switch(vartype)
		    {case tk_int:   doaxmath(1);        break;
		    case tk_word:   doaxmath(0);        break;
		    case tk_char:   doalmath(1);        break;
		    case tk_byte:   doalmath(0);        break;
		    case tk_dword:  doeaxmath(0);  op(0x66);
				    if( cpu < 3 )
					cpu = 3;
				    break;
		    case tk_long:  doeaxmath(1);  op(0x66);
				   if( cpu < 3 )
				       cpu = 3;
				   break;
		    case tk_fixed32s:  dofixed32math(1);
				       op(0x66);
				       if( cpu < 3 )
					   cpu = 3;
				       break;
		    case tk_fixed32u:  dofixed32math(0);
				       op(0x66);
				       if( cpu < 3 )
					   cpu = 3;
				       break;
		    default:  beep();  break;
		    }
		op(0x50);}      /* PUSH AX or EAX */
	    else{next = 1;
		if(vartype != tk_long && vartype != tk_dword
		       && vartype != tk_fixed32s && vartype != tk_fixed32u )
		    {switch(tok)
			{case tk_reg:   op(0x50+(unsigned int)number);
					break;
			case tk_seg:  if( (unsigned int)number < FS )
					  op(0x06+(unsigned int)number*8);
				      else{op(0xF);  op(0x80+(unsigned int)number*8);
				      if( (unsigned int)number <= GS )
					  {if( cpu < 3 )
					      cpu = 3;
					  }
				      else if( cpu < 5 )
					      cpu = 5;
				      }
				      break;
			case tk_intvar:
			case tk_wordvar:  outseg(segm,post,2);
					  op(0xFF);       /* PUSH [word] */
					  op(0x30+rm);
					  outaddress(rm,(unsigned int)number);
					  break;
			case tk_number: if( chip >= 2 )
					    {if( number<128 && number>=-128 )
						{op(0x6A);  /* PUSH const */
						op((unsigned int)number);}
					    else{op(0x68);   /* PUSH const */
						outword((unsigned int)number);}
					    if( cpu < 2 )
						cpu = 2;
					    break;}
			default:  switch(vartype)
				      {case tk_int:  doaxmath(1);       break;
				      case tk_word:  doaxmath(0);       break;
				      case tk_char:  doalmath(1);       break;
				      case tk_byte:  doalmath(0);       break;
				      default:  beep();  break;
				      }
				  op(0x50);   /* PUSH AX */
				  next = 0;
				  break;
			}
		    }
		else{switch(tok)        // long or dword or fixed32
			{case tk_reg32:  op(0x66);
					 op(0x50+(unsigned int)number);
					 break;
			case tk_fixed32s:
			case tk_fixed32u:
			case tk_dwordvar:
			case tk_longvar:  outseg(segm,post,3);
					  op(0x66);
					  op(0xFF);  /* PUSH [dword] */
					  op(0x30+rm);
					  outaddress(rm,(unsigned int)number);
					  break;
			case tk_number: if( number<128 && number>=-128 )
					    {op(0x66);
					    op(0x6A); /* PUSH 8 extend to 32 bit */
					    op((unsigned int)number);}
					 else{op(0x66); op(0x68); /* PUSH const */
					     outdword(number);}
					 break;
			default:  if( vartype == tk_long )
				      doeaxmath(1);
				  else doeaxmath(0);
				  op(0x66);  op(0x50);   /* PUSH EAX */
				  next = 0;
				  break;
			}
		    if( cpu < 3 )
			cpu = 3;
		    }
		if(next)
		    nexttok();
		}
	    }
	if(tok==tk_camma)
	    nexttok();
	else if(tok==tk_closebracket)
	    done = 1;
	else{expected(')');
	    done = 1;}
	}
    }
}


leaveproc ()
{if( current_proc_type == cpt_interrupt )   /* interrupt procedure */
    iret();                    /* IRET */
else{if(localsize > 0)
	{if(chip < 2)
	    {op(0x89);  op(0xEC);  /* MOV SP,BP */
	    op(0x5D);}             /* POP BP */
	else{op(0xC9);       /* LEAVE   (80286+) */
	    if( cpu < 2 )
		cpu = 2;
	    }
	}
    else if(paramsize > 0)
	op(0x5D);          /* POP BP */
    if( current_proc_type == cpt_far )
	{if(paramsize == 0)
	    retf();               /* RETF */
	else retfnum(paramsize);  /* RETF # */
	}
    else{if(paramsize == 0)
	    ret();               /* RET */
	else retnum(paramsize);  /* RET # */
	}
    }
}


dynamic_proc ()
{unsigned int dynamicsize;
int regprocflag=0,cpu_hold,dos1_hold,dos2_hold;
int proc_type;
if( dynamic_count >= MAXDYNAMICS )
    preerror("Maximum number of dynamic procedures exceeded.\n");
procedure_start = outptr;
cpu_hold = cpu;
cpu = 0;
dos1_hold = dos1;
dos2_hold = dos2;
dos1 = 0;
dos2 = 0;
nexttok();
switch(tok)
    {case tk_byte:
    case tk_word:
    case tk_int:
    case tk_char:
    case tk_fixed32s:
    case tk_fixed32u:
    case tk_dword:
    case tk_long:
    case tk_void:  returntype = tok;
		   nexttok();
		   break;
    default:               preerror("Return type expected");
			   nexttok();
    case tk_id:
    case tk_ID:
    case tk_undefregproc:
    case tk_undefproc:     returntype = tokens;
			   break;
    }
if( tok == tk_far )
    {proc_type = cpt_far;
    nexttok();
    }
else proc_type = cpt_near;
dynamic_flag = dynamic_count;
switch(tok)
    {case tk_id:           proc(0,proc_type);   break;
    case tk_ID:            regproc(0,proc_type);        regprocflag = 1;  break;
    case tk_undefregproc:  regproc(1,proc_type);        regprocflag = 1;  break;
    case tk_undefproc:     proc(1,proc_type);   break;
    default:  idexpected();  break;
    }

if( dynamic_count < MAXDYNAMICS )   /* store dynamic procedure */
    {dynamicsize = outptr-procedure_start;
    dynamic_ptrs[dynamic_count] = (unsigned char *) malloc( dynamicsize+DYNAMIC_HEADER );
    if( dynamic_ptrs[dynamic_count] == NULL )
	{preerror("Compiler out of memory for dynamic procedure storage");
	exit( e_outofmemory );}
    dynamic_ptrs[dynamic_count][0] = dynamicsize&255;
    dynamic_ptrs[dynamic_count][1] = dynamicsize/256;
    dynamic_ptrs[dynamic_count][2] = cpu;
    dynamic_ptrs[dynamic_count][3] = dos1;
    dynamic_ptrs[dynamic_count][4] = dos2;
    if( dynamicsize > 0 )
	memcpy(dynamic_ptrs[dynamic_count]+DYNAMIC_HEADER,output+procedure_start,dynamicsize);
    dynamic_count++;
    }
cpu = cpu_hold;
dos1 = dos1_hold;
dos2 = dos2_hold;
outptr = procedure_start;
if(comfile != file_exe)
    outptrdata = outptr;
dynamic_flag = NOT_DYNAMIC;
}


/* ======= procedure handling ends here ======== */

macros (expectedreturn)
int expectedreturn;
{int count,dynamicindex,actualreturn;
char heldid[IDLENGTH];
if(tok!=tk_ID && tok!=tk_id && tok!=tk_regproc && tok!=tk_undefregproc
	&& tok!=tk_proc && tok!=tk_undefproc)
    {idexpected();
    nexttok();
    return(-1);}
if( tok == tk_regproc && segm != NOT_DYNAMIC )
    {dynamicindex = segm;
    if( dynamicindex >= MAXDYNAMICS )
	dynamicindex -= MAXDYNAMICS;
    actualreturn = rm;
    }
else dynamicindex = NOT_DYNAMIC;
strcpy(heldid,string);
nexttok();
doregparams();
strcpy(string,heldid);
if( dynamicindex == NOT_DYNAMIC )
    {if(includeit(0)==-1)
	unknownmacro();
    }
else{insert_dynamic(dynamicindex,0);
    convert_returnvalue(expectedreturn,actualreturn);}
}


int updatecall (which,where,top)
/* update output with newly defined location, but only for addresses after
   and including top.
   return the number of addresses updated.
*/
unsigned int which,where,top;
{unsigned int count=0,hold;
int updates=0;
while(count<posts)
    {if( (posttype[count]==CALL_NEAR || posttype[count]==CALL_SHORT)
	    && postnum[count]==which && postloc[count] >= top )
	{if( posttype[count] == CALL_NEAR )
	    {hold = where - (postloc[count] + 2);
	    output[postloc[count]] = hold%256;
	    output[postloc[count]+1]= hold/256;}
	else{hold = where - (postloc[count] + 1);   /* CALL_SHORT */
	    if( short_ok(hold) )
		output[postloc[count]] = hold;
	    else shortjumptoolarge();}
	posts--;
	postloc[count] = postloc[posts];
	posttype[count] = posttype[posts];
	postnum[count] = postnum[posts];
	updates++;
	}
    else count++;
    }
return(updates);
}


tobedefined (callkind,expectedreturn)
int callkind,expectedreturn;
{if(tok == tk_ID)
    tok = tk_undefregproc;
else tok = tk_undefproc;
number = secondcallnum;
segm = NOT_DYNAMIC;
rm = expectedreturn;
post = 0;
addtotree(string);
addacall(secondcallnum,callkind);
secondcallnum++;
}


addlocaljump (callkind)
int callkind;
{addlocalvar(string,tk_locallabel,secondcallnum);
addacall(secondcallnum,callkind);
secondcallnum++;
}


define_locallabel ()
{updatecall( (unsigned int)updatelocalvar(string,tk_number,outptr), outptr, procedure_start );
nexttok();
expecting(tk_colon);
}


addacall (idnum,callkind)
unsigned int idnum;
unsigned char callkind;
{if(posts>=MAXPOSTS)
    {preerror("maximum number of second pass procedure calls exceeded");
    return(-1);}
postnum[posts] = idnum;
postloc[posts] = outptr + 1;
posttype[posts] = callkind;
posts++;
}


unsigned int dofrom (dataflag) // returns number of bytes read from FROM file
int dataflag;
{unsigned int size=0,filehandle;
long filesize;
if(tok!=tk_string)
    {stringexpected();
    return(0);}
filehandle = open( string, O_BINARY | O_RDONLY );
if( filehandle == -1 )
    {unableopenfile();
    return(0);}
if( (filesize=filelength(filehandle)) != -1L )
    {if( filesize < 0xFFFFL )
	{size = filesize;
	if(dataflag)
	    {filesize += outptrdata;
	    if( filesize < MAXDATA )
		{if( read(filehandle,outputdata+outptrdata,size) != size )
		    errorreadingfile();
		outptrdata += size;}
	    else maxdataerror();}
	else{filesize += outptr;
	    if( filesize < MAXDATA )
		{if( read(filehandle,output+outptr,size) != size )
		    errorreadingfile();
		outptr += size;}
	    else maxoutputerror();
	    }
	}
    else preerror("FROM file too large");
    }
else preerror("Unable to determine FROM file size");
close(filehandle);
if(comfile != file_exe)
    outptrdata = outptr;
return(size);
}


unsigned int doextract (dataflag) // returns number of bytes EXTRACTed
int dataflag;
{unsigned int size=0,filehandle,sizetoread;
long filesize,startpos;
if(tok!=tk_string)
    {stringexpected();
    return(0);}
filehandle = open( string, O_BINARY | O_RDONLY );
if( filehandle == -1 )
    {unableopenfile();
    return(0);}
nexttok();
expecting(tk_camma);
if( tok != tk_number )
    {numexpected();
    return(0);}
startpos = doconstlongmath();
expecting(tk_camma);
if( tok != tk_number )
    {numexpected();
    return(0);}
sizetoread = doconstlongmath();
if( (filesize=filelength(filehandle)) == -1L )
    {preerror("Unable to determine EXTRACT file size");
    close(filehandle);
    return(0);}
if( filesize <= startpos )
    {preerror("EXTRACT offset exceeds the length of the file");
    close(filehandle);
    return(0);}
if( sizetoread == 0)
    sizetoread = filesize-startpos;
if(sizetoread >= 0xFFFFL)
    {preerror("Block to EXTRACT exceeds 64K");
    close(filehandle);
    return(0);}
lseek(filehandle,startpos,0);   // error checking required on this
filesize = sizetoread;
size = sizetoread;
if(dataflag)
    {filesize += outptrdata;
    if( filesize < MAXDATA )
	{if( read(filehandle,outputdata+outptrdata,size) != size )
	    errorreadingfile();
	outptrdata += size;}
    else maxdataerror();}
else{filesize += outptr;
    if( filesize < MAXDATA )
	{if( read(filehandle,output+outptr,size) != size )
	    errorreadingfile();
	outptr += size;}
    else maxoutputerror();
    }
close(filehandle);
if(comfile != file_exe)
    outptrdata = outptr;
return(size);
}


outseg (segment,postnumber,locadd)
int segment,postnumber;
unsigned int locadd;
{switch(segment)
    {case ES: op(0x26); break; /* ES: */
    case SS:  if(!assumeDSSS)
		  op(0x36);    /* SS: */
	      break;
    case CS:  op(0x2E);        /* CS: */
    case DS:  break;           /* DS: not needed */
    case FS:  op(0x64);        /* FS: */
	      if( cpu < 3 )
		  cpu = 3;
	      break;
    case GS:  op(0x65);        /* GS: */
	      if( cpu < 3 )
		  cpu = 3;
	      break;
    case HS:
    case IS:  preerror("Sorry I do not yet know the instruction code");
	      break;
    default:  internalerror("Bad segment value in outset()");
	      break;
    }
if(postnumber)
    {if(posts >= MAXPOSTS)
	{maxwordpostserror();
	return(-1);}
    posttype[posts] = POST_VAR;
    postloc[posts] = outptr + locadd;
    posts++;
    }
}


setwordpost ()            /* for post word num setting */
{if(posts >= MAXPOSTS)
    {maxwordpostserror();
    return(-1);}
posttype[posts] = POST_VAR;
postloc[posts] = outptr;
posts++;
}


setdwordpost ()           /* for post dword num setting */
{if(posts >= MAXPOSTS)
    {maxwordpostserror();
    return(-1);}
posttype[posts] = POST_DWORD;
postloc[posts] = outptr;
posts++;
}


doposts ()
{unsigned int addvalue,addhold,addhold2,i;
addvalue = outptrdata;
if( outptrdata%2==1 )  /* alignment of entire post data block manditory */
    {addvalue++;
    postsize++;}
if(MAXDATA-outptrdata < postsize)
    {preerror2("post variable size exceeds size left in data segment");
    return(-1);}
for(i=0;i<posts;i++)
    {if( posttype[i]==POST_VAR )
	{addhold = output[postloc[i]] + (output[postloc[i]+1]<<8) + addvalue;
	output[postloc[i]] = addhold & 255;   /* LOW byte */
	output[postloc[i]+1] = addhold >> 8;  /* HIGH byte */
	}
    else if( posttype[i]==POST_DWORD )
	{addhold2 = output[postloc[i]] + (output[postloc[i]+1] << 8);
	addhold = addhold2 + addvalue;
	output[postloc[i]] = addhold & 255;   /* LOW byte */
	output[postloc[i]+1] = addhold >> 8;  /* HIGH byte */
	if( addhold2 > addhold )  /* if overflow occured, inc high word */
	    {addhold = output[postloc[i]+2] + (output[postloc[i]+3]<<8) + 1;
	    output[postloc[i]+2] = addhold & 255; /* LOW byte */
	    output[postloc[i]+3] = addhold >> 8;  /* HIGH byte */
	    }
	}
    }
}


unsigned int addpoststring ()    /* add a string to the post queue */
{unsigned int i,returnvalue;
if( dynamic_flag != NOT_DYNAMIC )
    preerror("String constants are illegal within dynamic procedures");
if(posts >= MAXPOSTS)
    {preerror("cannot add post string, post queue full");
    return(-1);}
posttype[posts] = POST_STRING;
postloc[posts] = outptr;
posts++;
returnvalue = MAXDATA-1-poststrptr;
i=0;
while( string[i]!=0 && poststrptr>0 )
    outputdata[poststrptr--]=string[i++];
if(poststrptr>0)
    outputdata[poststrptr--]=0;
else preerror("Data segment full");
return(returnvalue);
}


dopoststrings ()
{unsigned int addvalue,addhold,i;
addvalue = outptrdata;
if(outptrdata > poststrptr)
    {preerror("post string size exceeds size left in data segment");
    return(-1);}
datasize += MAXDATA-1-poststrptr;
for(i=MAXDATA-1; i>poststrptr; i--)
    outputdata[outptrdata++]=outputdata[i];
if(comfile != file_exe)
    outptr=outptrdata;
for(i=0;i<posts;i++)
    if( posttype[i]==POST_STRING )
	{addhold = outputdata[postloc[i]]
			+ (outputdata[postloc[i]+1] << 8) + addvalue;
	outputdata[postloc[i]] = addhold & 255;   /* LOW byte */
	outputdata[postloc[i]+1] = addhold >> 8;        /* HIGH byte */
	posts--;
	postloc[i]=postloc[posts];
	posttype[i]=posttype[posts];
	postnum[i]=postnum[posts];
	i--;
	}
poststrptr = MAXDATA-1;    /* reset the poststrptr */
}


insertcode ()     // force code procedure at specified location
{nexttok();
switch(tok)
    {case tk_void:
    case tk_word:
    case tk_int:
    case tk_char:
    case tk_fixed32s:
    case tk_fixed32u:
    case tk_byte:  returntype = tok;
		   nexttok();
		   break;
    default:  returntype = tk_word;
	      break;
    }
if( tok == tk_id || tok == tk_ID )
    {number = outptr;
    if( setuprm(0) == -1 )
	return(-1);
    }
else if( tok == tk_undefregproc || tok == tk_undefproc )
    {number = outptr;
    if( setuprm(1) == -1 )
	return(-1);
    }
switch(tok)
    {case tk_undefregproc:  tok = tk_regproc;
			    updatecall( (unsigned int)updatetree(), (unsigned int)number,0 );
			    if(includeit(1)==-1)
				thisundefined(string);
			    break;
    case tk_undefproc:  tok = tk_proc;
			updatecall( (unsigned int)updatetree(), (unsigned int)number,0 );
			if(includeproc()==-1)
			    thisundefined(string);
			break;
    case tk_id:  tok = tk_proc;
		 addtotree(string);
		 if(includeproc()==-1)
		     thisundefined(string);
		 break;
    case tk_ID:  tok = tk_regproc;
		 addtotree(string);
		 if(includeit(1)==-1)
		     thisundefined(string);
		 break;
    case tk_regproc:
    case tk_proc:  if( segm != NOT_DYNAMIC && segm < MAXDYNAMICS )
		       {number = outptr;
		       segm += MAXDYNAMICS;
		       updatecall( (unsigned int)updatetree(), (unsigned int)number,0 );
		       insert_dynamic(segm-MAXDYNAMICS,1);
		       }
		   else preerror("Procedure already inserted in code");
		   break;
    default:  idexpected();
	      break;
    }
nexttok();
expecting(tk_openbracket);
while( tok != tk_eof && tok != tk_closebracket )
    nexttok();
if( tok == tk_eof )
    unexpectedeof();
else nextseminext();
}


/************ some of the dynamic procedure support functions *************/


int insert_dynamic (index,retflag)
int index,retflag;
{unsigned int dynamicsize;
if( index >= dynamic_count )
    {internalerror("index value in insert_dynamic too high");
    return(-1);}
dynamicsize = dynamic_ptrs[index][1]*256 + dynamic_ptrs[index][0];
if( retflag==0 && dynamicsize > 0 )
    dynamicsize--;
if( dynamicsize > 0 )
    outprocedure(dynamic_ptrs[index]+DYNAMIC_HEADER,dynamicsize);
if( dynamic_ptrs[index][2] > cpu )
    cpu = dynamic_ptrs[index][2];
if( dynamic_ptrs[index][3] > dos1 )
    dos1 = dynamic_ptrs[index][3];
if( dynamic_ptrs[index][4] > dos2 )
    dos2 = dynamic_ptrs[index][4];
return(0);
}


unsigned int getdynamicsize (index)
int index;
{return( dynamic_ptrs[index][1]*256 + dynamic_ptrs[index][0] );
}


int getdynamiccpu (index)
int index;
{return( dynamic_ptrs[index][2] );
}


int getdynamicdos1 (index)
int index;
{return( dynamic_ptrs[index][3] );
}


int getdynamicdos2 (index)
int index;
{return( dynamic_ptrs[index][4] );
}


/* ==============  allocation of memory for arrays begins ============== */

int getmem ()
/* attempts to allocate memory required for compiling.
   if fails then displays error message and exits to DOS.
*/
{output = (unsigned char *) malloc( (size_t) MAXDATA );
if(output == NULL)
    outofmemory2();
if(comfile != file_exe)
    outputdata = output;
else{outputdata = (unsigned char *) malloc( (size_t) MAXDATA );
    if(outputdata == NULL)
	outofmemory2();
    }
postloc = (unsigned int *) malloc( MAXPOSTS*2 );
if(postloc == NULL)
    outofmemory2();
postnum = (unsigned int *) malloc( MAXPOSTS*2 );
if(postnum == NULL)
    outofmemory2();
posttype = (unsigned char *) malloc( MAXPOSTS );
if(posttype == NULL)
    outofmemory2();
}


/* end of TOKC.C */
