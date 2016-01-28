/*
    TOK.H
    Constant declarations for SPHINX C--
    property of Peter Cellik, Copyright Peter Cellik (C) 1994.
    All Rights Reserved.
    Last modified 6 July 1994
*/


#define DATATYPES 8   // number of data types

enum { tk_eof,
       tk_number,tk_string,
       tk_id,tk_ID,
/* 5 */
       tk_assign,tk_swap,
       tk_minus,tk_plus,
       tk_minusminus,tk_plusplus,
       tk_mult,tk_div,tk_mod,
       tk_multminus,tk_divminus,tk_modminus,
       tk_rr,tk_ll,tk_rrminus,tk_llminus,
       tk_minusequals,tk_plusequals,tk_rrequals,tk_llequals,
/* 24 */
       tk_or,tk_and,tk_xor,tk_not,
       tk_orminus,tk_andminus,tk_xorminus,
       tk_orequals,tk_andequals,tk_xorequals,
/* 34 */
       tk_equalto,tk_notequal,
       tk_greater,tk_greaterequal,tk_less,tk_lessequal,
       tk_oror,tk_andand,
/* 42 */
       tk_openbrace,tk_closebrace,
       tk_openbracket,tk_closebracket,
       tk_openblock,tk_closeblock,
       tk_colon,tk_semicolon,
       tk_camma,tk_period,
       tk_at,tk_numsign,
       tk_dollar,tk_question,
       tk_tilda,
/* 57 */
       tk_byte,tk_word,tk_char,tk_int,tk_dword,tk_long,
       tk_fixed32s,tk_fixed32u,
       // must be in order
/* 63+2 */
       tk_if,tk_IF,tk_else,tk_ELSE,
       tk_loop,tk_do,tk_while,
       tk_return,tk_from,tk_extract,tk_interrupt,tk_far,tk_void,tk_enum,
/* 76+3 */
       tk_reg,tk_seg,tk_beg,tk_reg32,tk_debugreg,tk_controlreg,tk_testreg,
/* 83+3 */
       tk_undefproc,tk_undefregproc,tk_regproc,tk_proc,tk_interruptproc,
/* 88+3 */
       tk_bytevar,tk_wordvar,tk_charvar,tk_intvar,tk_dwordvar,tk_longvar,
       tk_fixed32svar,tk_fixed32uvar,
       // above must be in this order
/* 94+5 */
       tk_rmnumber,tk_postnumber,
       tk_localint,tk_localword,tk_localchar,tk_localbyte,
       tk_localdword,tk_locallong,
       tk_localfixed32s,tk_localfixed32u,
       tk_locallabel,
       tk_paramint,tk_paramword,tk_paramchar,tk_parambyte,
       tk_paramdword,tk_paramlong,
       tk_paramfixed32s,tk_paramfixed32u,
/* 119+9 */
       tk_carryflag,tk_notcarryflag,
       tk_overflowflag,tk_notoverflowflag,
       tk_zeroflag,tk_notzeroflag,
/* 125+9 */
       tk_TRUE,tk_FALSE,
       tk_datasize,tk_codesize,tk_stacksize,tk_postsize,
       tk_dataptr,tk_codeptr,tk_postptr,
       tk_progstart,tk_progend,tk_progsize,
       tk_stackstart,tk_stackend,
       tk_datestr,tk_compiler,
       tk_second,tk_minute,tk_hour,tk_day,tk_month,tk_year,tk_weekday,
       tk_ver1,tk_ver2,
/* 150+9 */
       tk_inline,

       tokens };

enum { AX,CX,DX,BX,SP,BP,SI,DI };	   // 16-bit word regs
enum { AL,CL,DL,BL,AH,CH,DH,BH };	   // 8-bit byte regs
enum { EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI };  // 386+ 32-bit regs
enum { ES,CS,SS,DS, FS,GS, HS,IS };	   // FS,GS 386+; HS,IS ?86+
enum { CR0,CR1,CR2,CR3,CR4,CR5,CR6,CR7 };  // 386+ control regs
enum { TR0,TR1,TR2,TR3,TR4,TR5,TR6,TR7 };  // 386+ test regs
enum { DR0,DR1,DR2,DR3,DR4,DR5,DR6,DR7 };  // 386+ debug regs

#define IDLENGTH 33  /* length of significance of IDS + NULL, therefore 32 */
#define STRLEN 1000  /* length of string token holder */

/* define exit codes */
enum { e_ok, e_outofmemory, e_cannotopeninput, e_toomanyerrors,
       e_internalerror, e_noinputspecified, e_unknowncommandline,
       e_inputtoobig, e_outputtoobig, e_noovl, e_quote, e_extract,
       e_noexe, e_cannotopenmapfile, e_someerrors, e_badinputfilename,
       e_symbioerror };

#define MAXDATA     65500 /* output run file buffer 65500 bytes */
#define MAXINPUT    65500 /* maximum size of input file */
#define MAXDYNAMICS   500 /* maximum number of dynamic procedures */
#define NOT_DYNAMIC 12345 /* flag value specifing a non-dynamic proc */

enum { rm_mod00=0, rm_mod01=64, rm_mod10=128, rm_mod11=192, rm_rm=8,
       rm_d16=6, rm_BXSI=0, rm_BXDI=1, rm_BPSI=2, rm_BPDI=3, rm_SI=4,
	   rm_DI=5, rm_BP=6, rm_BX=7 };

#define FILENAMESIZE 80

// posttype values for call or jump types or post variables
enum { CALL_NONE, CALL_SHORT, CALL_NEAR, POST_STRING, POST_VAR, POST_DWORD };

#define SHORTMAX  127 // largest 8 bit signed value
#define SHORTMIN -128 // smallest 8 bit signed value

// format of output file
enum {file_exe,file_com,file_obj};

// current procedure type constants
enum{ cpt_near, cpt_far, cpt_interrupt };

// structure for holding all info of a single token
struct tokenrec { unsigned char tr_string[STRLEN];
			    int tr_tok;
			    int tr_rm;
			    int tr_segm;
			    int tr_post;
			   long tr_number;
		};

// start of procedure pre-definitions
unsigned long doconstdwordmath (void);
signed	 long doconstlongmath (void);

void outdwordd (unsigned long);
void outdword (unsigned long);
void outwordd (unsigned int);
void outword (unsigned int);

/*
void opd (unsigned char);
void op (unsigned char);
*/

unsigned int dofrom (int);
unsigned int doextract (int);
void doregparams (void);
void startsymbiosys (unsigned char *);

long updatetree (void);

/* end of TOK.H */
