/*****************************************************************************
 Sphinx C-- library (C) 2000.
 FILE:        STDLIB.H
 DESCRIPTION: Definitions for common types, variables, and functions.

 LAST MODIFIED:28.10.01 22:01
 ----------------------------------------------------------------------------
 int atoi(unsigned int str)
 long atol(unsigned int str)
 void abort()
 int fastcall atexit(unsigned int AX)
 void fastcall exit(int AX)
 void fastcall _exit(int AX)

To do create:
 bsearch
 div
 ecvt
 fcvt
 gcvt
 getenv
 itoa
 labs
 ldiv
 lfind
 _lrotl
 _lrotr
 lsearch
 ltoa
 malloc
 max
 min
 putenv
 qsort
 rand
 random
 randomize
 _rotl
 _rotr
 srand
 strtol
 strtoul
 swab
 ultoa

*****************************************************************************/

#include "stdlib.hmm"
#include "write.h--"

void abort(void);

/*****************************************************************************
* TITLE : int atoi(unsigned int str)
* DESCR : Convert string to int
* PARAMS: input: str - address string to be converted
*       : output: int value
*****************************************************************************/
#ifdef __safe_reg_mode__

int atoi(unsigned int str);

#ifdef __FLAT__

#define atol atoi

#endif

:int atoi(unsigned int str)
{
	$push ECX,BX,(E)SI
	ATOL(str);
	$pop (E)SI,BX,ECX
}

#else //__safe_reg_mode__

:int fastcall atoi((E)SI)
{
	@ATOL();
}

#ifdef __FLAT__

#define atol atoi

#endif //__FLAT__

#endif


/*****************************************************************************
* TITLE : long atol(unsigned int str)
* DESCR : Convert string to long
* PARAMS: input: str - address string to be converted
* 			: output: long value
*****************************************************************************/
#ifdef __safe_reg_mode__

#ifndef __FLAT__

long atol(unsigned int str);

:long atol(unsigned int str)
{
	$push ECX,BX,SI
	ATOL(str);
	$pop SI,BX,CX
}

#endif //__FLAT__

#else //__safe_reg_mode__

#ifndef __FLAT__

:long fastcall atol((E)SI)
{
	@ATOL();
}

#endif //__FLAT__
#endif

/*****************************************************************************
* TITLE : void abort(void)
* DESCR : Abort process
* PARAMS:
*****************************************************************************/

:void abort(void)
inline
{
	WriteStr("Abnormal program termination\n");
	_exit(3);
}


/*****************************************************************************
* TITLE : int fastcall atexit(unsigned int AX)
* DESCR : registers the function, executed at completion of the program
* PARAMS: input: address function
*       : output: 0 if function registered
*****************************************************************************/
:int fastcall atexit((E)AX)
{
	@ATEXIT();
}

/*****************************************************************************
* TITLE : void fastcall exit(int AX)
* DESCR : Terminate process after cleanup
* PARAMS: input: exit status
*****************************************************************************/

:void fastcall exit((E)AX)
inline
{
	@EXIT();
}


/*****************************************************************************
* TITLE : void fastcall _exit(int AX)
* DESCR : Terminate process without cleanup
* PARAMS: input: exit status
*****************************************************************************/

inline void fastcall _exit((E)AX)
inline
{
#ifdef __COM__
	$int 0x20
#else
	#ifdef __TLS__
		ExitProcess(EAX);
	#else
		AH=0x4c;
		$int 0x21
	#endif
#endif
}

