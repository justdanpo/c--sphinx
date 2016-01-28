/*****************************************************************************
 Sphinx C-- library (C) 2000.
 FILE:        STRING.H
 DESCRIPTION: Definitions for memory and string functions.

 LAST MODIFIED: 28.10.01 21:59
 ----------------------------------------------------------------------------
 total 29
 create 16
 ready 55.17%

 unsigned int strcpy(unsigned int dest,sours);
 unsigned int strcat(unsigned int dest,sours);
 unsigned int strchr(unsigned int string; int c);
 int strcmp(unsigned int str1,str2);
 int strcmpi(unsigned int str1,str2);
 int stricmp(unsigned int str1,str2);
 unsigned int strcoll(unsigned int dest,sours);
 int strlen(unsigned int str);
 unsigned int strlwr(unsigned int str);
 int strncmp(unsigned int str1,str2,n);
 int strnicmp(unsigned int str1,str2,n);
 int strncmpi(unsigned int str1,str2,n);
 int strncpy(unsigned int str1,str2,n);
 unsigned int strrchr(unsigned int str, int c);
 unsigned int strstr(unsigned int str1,str2);
 unsigned int strupr(unsigned int str);

To do create:
 strcspn
 strdup
 _strerror
 strerror
 strncat
 strnset
 strpbrk
 strrev
 strset
 strspn
 strtod
 strtok
 strxfrm

*****************************************************************************/
#include "string.hmm"

#ifdef __safe_reg_mode__

 unsigned int strcpy(unsigned int dest,sours);
 unsigned int strcat(unsigned int dest,sours);
 unsigned int strchr(unsigned int string; int c);
 int strcmp(unsigned int str1,str2);
 int strcmpi(unsigned int str1,str2);
 int stricmp(unsigned int str1,str2);
 int strlen(unsigned int str);
 unsigned int strlwr(unsigned int str);
 int strncmp(unsigned int str1,str2,n);
 int strnicmp(unsigned int str1,str2,n);
 int strncpy(unsigned int str1,str2,n);
 unsigned int strrchr(unsigned int str, int c);
 unsigned int strstr(unsigned int str1,str2);
 unsigned int strupr(unsigned int str);

#endif


/*****************************************************************************
* NAME  : unsigned int strcpy(unsigned int dest,sours)
* TITLE : Copy string (return pointer to last zero-byte)
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int strcpy(unsigned int dest,sours)
{
	$push (E)DI,(E)SI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	STRCPY(dest,sours);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)SI,(E)DI
	return dest;
}

#else	//__safe_reg_mode__

:unsigned int fastcall strcpy((E)DI,(E)SI)
{
	$push (E)DI
#ifndef __TLS__
	ES=DS;
#endif
	STRCPY();
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)AX
}

#endif	//__safe_reg_mode__


/*****************************************************************************
* NAME  : unsigned int strcat(unsigned int dest,sours)
* TITLE : Append a string (return pointer to dest string)
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int strcat(unsigned int dest,sours)
{
	$push (E)DI,(E)SI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@STRCAT(dest,sours);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)SI,(E)DI
	return dest;
}

#else	//__safe_reg_mode__

:unsigned int fastcall strcat((E)DI,(E)SI)
{
#ifndef __TLS__
	ES=DS;
#endif
	$push (E)DI
	@STRCAT();
	$pop (E)AX
}

#endif	//__safe_reg_mode__


/*****************************************************************************
* NAME  : unsigned int strchr(unsigned int string; int c )
* TITLE : Find a Character in a String
* PARAMS:
*****************************************************************************/

#ifdef __safe_reg_mode__

:unsigned int strchr(unsigned int string; int c )
{
#ifdef codesize
	$push (E)SI,(E)BX
#else
	$push (E)DI,(E)BX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
#endif
	@STRCHR(string,c);
#ifdef codesize
	$pop (E)BX,(E)SI
#else
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)BX,(E)DI
#endif
}

#else	//__safe_reg_mode__

:unsigned int fastcall strchr((E)SI,BL)
{
#ifdef speed
#ifndef __TLS__
	ES=DS;
#endif
#endif
	@STRCHR((E)SI,BL);
}

#endif	//__safe_reg_mode__

/*****************************************************************************
* NAME  :int strcmp(unsigned int str1,str2)
* TITLE :Compare two strings, case sensitive
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int strcmp(unsigned int str1,str2)
{
	$push (E)SI,(E)DI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@STRCMP(str1,str2);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)DI,(E)SI
	return AL;
}

#else

:int fastcall strcmp((E)SI,(E)DI)
{
#ifndef __TLS__
	ES=DS;
#endif
	@STRCMP();
	return AL;
}

#endif

/*****************************************************************************
* NAME  :int strcmpi(unsigned int str1,str2)
* TITLE :Compare two strings, case insensitive
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int strcmpi(unsigned int str1,str2)
{
	$push (E)SI,(E)DI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@STRCMPI(str1,str2);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)DI,(E)SI
	return AL;
}

#else

:int fastcall strcmpi((E)SI,(E)DI)
{
#ifndef __TLS__
	ES=DS;
#endif
	@STRCMPI();
	return AL;
}

#endif

/*****************************************************************************
* NAME  :int strlen(unsigned int str)
* TITLE :Get string length
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int strlen(unsigned int str)
{
	$push (E)DI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@STRLEN(str);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)DI
}

#else

:int fastcall strlen((E)DI)
{
#ifndef __TLS__
	ES=DS;
#endif
	@STRLEN();
}

#endif

/*****************************************************************************
* NAME  : unsigned int strlwr(unsigned int str)
* TITLE : Convert string to lower case
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int strlwr(unsigned int str)
{
	$push (E)SI
	@STRLWR();
	$pop (E)SI
	return str;
}

#else

:unsigned int fastcall strlwr((E)SI)
{
	$push (E)SI
	@STRLWR();
	$pop (E)AX
}

#endif

/*****************************************************************************
 * NAME  :int strncmp(unsigned int str1,str2,n)
 * TITLE :Compare n Characters of Two Strings, Case Sensitive
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:int strncmp(unsigned int str1,str2,n)
{
	$push (E)CX,(E)DI,(E)SI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@STRNCMP(str1,str2,n);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)SI,(E)DI,(E)CX
	return AL;
}

#else

:int fastcall strncmp((E)SI,(E)DI,(E)CX)
{
#ifndef __TLS__
	ES=DS;
#endif
	@STRNCMP();
	return AL;
}

#endif

/*****************************************************************************
 * NAME  :int strnicmp(unsigned int str1,str2,n)
 * TITLE :Compare n Characters of Strings, Case Insensitive
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:int strnicmp(unsigned int str1,str2,n)
{
	$push (E)CX,(E)DI,(E)SI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@STRNCMPI(str1,str2,n);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)SI,(E)DI,(E)CX
	return AL;
}

#else

:int fastcall strnicmp((E)SI,(E)DI,(E)CX)
{
#ifndef __TLS__
	ES=DS;
#endif
	@STRNCMPI();
	return AL;
}

#endif

/*****************************************************************************
* NAME  :int strncpy(unsigned int str1,str2,n)
* TITLE :Copy a Specified Number of Characters
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int strncpy(unsigned int str1,str2,n)
{
	$push (E)CX,(E)DI,(E)SI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@STRNCPY(str1,str2,n);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)SI,(E)DI,(E)CX
	return str1;
}

#else

:int fastcall strncpy((E)DI,(E)SI,(E)CX)
{
	$push (E)DI
#ifndef __TLS__
	ES=DS;
#endif
	@STRNCPY();
	$pop (E)AX
}

#endif

/*****************************************************************************
 * NAME  :unsigned int strrchr(unsigned int str, int c)
 * TITLE :Scan String for Last Occurrence of Character
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int strrchr(unsigned int str, int c)
{
	$push (E)BX,(E)DI
#ifdef speed
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
#endif
	@STRRCHR(str,c);
#ifdef speed
#ifndef __TLS__
	$pop ES
#endif
#endif
	$pop (E)DI,(E)BX
}

#else

:unsigned int fastcall strrchr((E)DI,BL)
{
#ifdef speed
#ifndef __TLS__
	ES=DS;
#endif
#endif
	@STRRCHR();
}
#endif

/*****************************************************************************
 * NAME  :unsigned int strstr(unsigned int str1,str2)
 * TITLE :Find substring
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int strstr(unsigned int str1,str2)
{
	$push (E)BX,(E)DX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@STRSTR(str1,str2);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)DX,(E)BX
}

#else

:unsigned int fastcall strstr((E)BX,(E)DX)
{
#ifndef __TLS__
	ES=DS;
#endif
	@STRSTR();
}

#endif

/*****************************************************************************
 * NAME  :unsigned int strupr(unsigned int str)
 * TITLE :Convert string to uppercase
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int strupr(unsigned int str)
{
	$push str,(E)SI
	@STRUPR(str);
	$pop (E)SI,(E)AX
}

#else

:unsigned int fastcall strupr((E)SI)
{
	$push (E)SI
	@STRUPR();
	$pop (E)AX
}

#endif


#define stricmp strcmpi
#define strncmpi strnicmp
#define strcoll strcpy
