/*****************************************************************************
 Sphinx C-- library (C) 2000.
 FILE:        MEM.H
 DESCRIPTION: Memory manipulation functions

 LAST MODIFIED: 17.10.01 21:42
 ----------------------------------------------------------------------------
 total  10
 create 10
 ready 100%

 int memcmp(unsigned int buf1,buf2,cnt);
 unsigned int memmove(unsigned int dest,sour,coun);
 void movmem(unsigned int dest,sour,coun);
 unsigned int memset(unsigned int dest,int val,unsigned int n);
 void setmem(unsigned int dest,n,char val);
 unsigned int memcpy(unsigned int dest,sour,n);
 void movedata(unsigned segsrc,offsrc,segdst,offdst,n);
 unsigned int memccpy(unsigned dest,sour,int c,unsigned int n);
 unsigned int memchr(unsigned int buf,int c,unsigned int n);
 int memicmp(unsigned int s1,s2,n);

*****************************************************************************/
#include "mem.hmm"

#ifdef __safe_reg_mode__

 int memcmp(unsigned int buf1,buf2,cnt);
 unsigned int memmove(unsigned int dest,sour,coun);
 void movmem(unsigned int dest,sour,coun);
 unsigned int memset(unsigned int dest,int val,unsigned int n);
 void setmem(unsigned int dest,n,char val);
 unsigned int memcpy(unsigned int dest,sour,n);
 void movedata(unsigned segsrc,offsrc,segdst,offdst,n);
 unsigned int memccpy(unsigned dest,sour,int c,unsigned int n);
 unsigned int memchr(unsigned int buf,int c,unsigned int n);
 int memicmp(unsigned int s1,s2,n);

#endif

/*****************************************************************************
* NAME  :int memcmp(unsigned int buf1,buf2,cnt)
* TITLE :Compare characters from two buffers
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int memcmp(unsigned int buf1,buf2,cnt)
{
	$push (E)DI,(E)SI,(E)CX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	(E)AX=@MEMCMP(buf1,buf2,cnt);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)CX,(E)SI,(E)DI
}

#else

:int fastcall memcmp((E)DI,(E)SI,(E)CX)
{
#ifndef __TLS__
	ES=DS;
#endif
	return @MEMCMP();
}

#endif

/*****************************************************************************
* NAME  :unsigned int memmove(unsigned int dest,sour,coun)
* TITLE :Move a block of bytes
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int memmove(unsigned int dest,sour,coun)
{
	$push (E)DI,(E)SI,(E)CX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	MEMMOV(dest,sour,coun);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)CX,(E)SI,(E)DI
	return dest;
}

#else

:unsigned int fastcall memmove((E)DI,(E)SI,(E)CX)
{
	$push (E)DI
#ifndef __TLS__
	ES=DS;
#endif
	MEMMOV(dest,sour,coun);
	$pop (E)AX
}

#endif

/*****************************************************************************
* NAME  :void movmem(unsigned int dest,sour,coun)
* TITLE :Move a block of bytes
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:void movmem(unsigned int dest,sour,coun)
{
	$push (E)DI,(E)SI,(E)CX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	MEMMOV(dest,sour,coun);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)CX,(E)SI,(E)DI
}

#else

:void fastcall movmem((E)DI,(E)SI,(E)CX)
{
#ifndef __TLS__
	ES=DS;
#endif
	MEMMOV(dest,sour,coun);
}

#endif

/*****************************************************************************
 * NAME  :unsigned int memset(unsigned int dest,int val,unsigned int n)
 * TITLE :Initialize buffer
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int memset(unsigned int dest,int val,unsigned int n)
{
	$push (E)DI,(E)CX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	MEMSET(dest,n,val);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)CX,(E)DI
	return dest;
}

#else

:unsigned int fastcall memset((E)DI,AL,(E)CX)
{
	$push (E)DI
#ifndef __TLS__
	$push DS
	$pop ES
#endif
	MEMSET();
	$pop (E)AX;
}

#endif

/*****************************************************************************
 * NAME  :Assing a value to memory
 * TITLE :void setmem(unsigned int dest,n,char val)
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:void setmem(unsigned int dest,n,char val)
{
	$push (E)DI,(E)CX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	MEMSET(dest,n,val);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)CX,(E)DI
}

#else

:void fastcall setmem((E)DI,(E)CX,AL)
{
#ifndef __TLS__
	$push DS
	$pop ES
#endif
	MEMSET();
}

#endif

/*****************************************************************************
 * NAME  :unsigned int memcpy(unsigned int dest,sour,n)
 * TITLE :Copy characters between buffers
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int memcpy(unsigned int dest,sour,n)
{
	$push (E)DI,(E)SI,(E)CX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	@MEMCPY(dest,sour,n);
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)CX,(E)SI,(E)DI
}

#else

:unsigned int fastcall memcpy((E)DI,(E)SI,(E)CX)
{
#ifndef __TLS__
	ES=DS;
#endif
	@MEMCPY();
}

#endif

/*****************************************************************************
 * NAME  :void movedata(unsigned segsrc,offsrc,segdst,offdst,n)
 * TITLE :Copy characters to a different segment
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:void movedata(unsigned segsrc,offsrc,segdst,offdst,n)
{
	$push (E)DI,(E)SI,(E)CX
#ifndef __TLS__
	$push ES
	ES=segdst;
	$push DS
	DS=segsrc;
#endif
	MEMMOV(offdst,offsrc,n);
#ifndef __TLS__
	$pop DS,ES
#endif
	$pop (E)CX,(E)SI,(E)DI
}

#else

:void movedata(unsigned segsrc,offsrc,segdst,offdst,n)
{
#ifndef __TLS__
	ES=segdst;
	$push DS
	DS=segsrc;
#endif
	MEMMOV(offdst,offsrc,n);
#ifndef __TLS__
	$pop DS
#endif
}

#endif

/*****************************************************************************
 * NAME  :unsigned int memccpy(unsigned dest,sour,int c,unsigned int n)
 * TITLE :copy characters from buffer
 * PARAMS:
 *****************************************************************************/
:unsigned int memccpy(unsigned dest,sour,int c,unsigned int n)
{
#ifdef __safe_reg_mode__
	$push (E)CX,(E)DX,(E)DI,(E)SI
#endif
	(E)DI=dest;
	(E)SI=sour;
	(E)CX=n;
	AH=c;
	(E)DX=0;
#ifndef __TLS__
#ifdef __safe_reg_mode__
	$push ES
#endif
	$push DS
	$pop ES
#endif
	LOOPNZ((E)CX){
		$lodsb
		$stosb
		IF(AL==AH){
			(E)DX=(E)DI;
			BREAK;
		}
	}
#ifdef __safe_reg_mode__
	$pop ES
#endif
#ifdef __safe_reg_mode__
	$pop (E)SI,(E)DI,(E)DX,(E)CX
#endif
#ifdef codesize
	(E)AX><(E)DX;
#else
	(E)AX=(E)DX;
#endif
}

/*****************************************************************************
 * NAME  :unsigned int memchr(unsigned int buf,int c,unsigned int n)
 * TITLE :Find character in buffer
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int memchr(unsigned int buf,int c,unsigned int n)
{
	$push (E)CX,(E)DI
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	(E)DI=buf;
	(E)CX=n;
	AL=c;
	$rep $scasb
	IF(ZEROFLAG)$lea (E)AX,DSDWORD[(E)DI-1]
	ELSE (E)AX=0;
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)DI,(E)CX
}

#else

:unsigned int fastcall memchr((E)DI,AL,(E)CX)
{
#ifndef __TLS__
	$push ES,DS
	$pop ES
#endif
	$rep $scasb
	IF(ZEROFLAG)$lea (E)AX,DSDWORD[(E)DI-1]
	ELSE (E)AX=0;
#ifndef __TLS__
	$pop ES
#endif
}

#endif

/*****************************************************************************
 * NAME  :int memicmp(unsigned int s1,s2,n)
 * TITLE :Compare characters in two buffers
 * PARAMS:
 *****************************************************************************/
#ifdef __safe_reg_mode__

:int memicmp(unsigned int s1,s2,n)
{
	$push (E)DI,(E)SI,(E)BX,(E)CX
#ifndef __TLS__
	$push ES
	ES=DS;
#endif
	(E)DI=s1;
	(E)SI=s2;
	(E)CX=n;
	LOOPNZ(E)CX){
#ifdef codesize
		$lodsb
#else
		AL=DSBYTE[(E)SI];
		(E)SI++;
#endif
		IF(AL>='a')&&(AL<='z')AL-=0x20;
		BL=DSBYTE[(E)DI];
		IF(BL>='a')&&(BL<='z')BL-=0x20;
		(E)DI++;
		AL-=BL;
		IF(!ZEROFLAG)BREAK;
	}
#ifndef __TLS__
	$pop ES
#endif
	$pop (E)CX,(E)BX,(E)SI,(E)DI
	return AL;
}

#else

:int fastcall memicmp((E)DI,(E)SI,(E)CX)
{
#ifndef __TLS__
	ES=DS;
#endif
	LOOPNZ(E)CX){
#ifdef codesize
		$lodsb
#else
		AL=DSBYTE[(E)SI];
		(E)SI++;
#endif
		IF(AL>='a')&&(AL<='z')AL-=0x20;
		BL=DSBYTE[(E)DI];
		IF(BL>='a')&&(BL<='z')BL-=0x20;
		(E)DI++;
		AL-=BL;
		IF(!ZEROFLAG)BREAK;
	}
	return AL;
}

#endif
