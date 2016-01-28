/*****************************************************************************
 Sphinx C-- library (C) 2000.
 FILE:        IO.H
 DESCRIPTION: Definitions for low level I/O functions.
 LAST MODIFIED: 20.12.00 22:52
 -----------------------------------------------------------------------------
 FUNCTIONS:
int access(unsigned int name; word mode)
int  cdecl _chmod (word name; int func,...)
int  _close (int handl)
int  chmod (word name;int atr)
int  chsize (int hd;long sz)
int  _creat (word name,attr)
int  _open (word name,flag)
int  _read (word handl,buf,len)
long tell (int hndl)
int  _write (word handl,buf,len)
int  creatnew (word name,atr)
int  creattemp (word path,atr)
int  dup (word handl)
int  dup2 (word handl,handl2)
int  eof (word handl)
long filelength (word handle)
int  getftime (int handl;word buf)
int  setftime (int handl;word buf)
int  cdecl ioctl (int handl,cmd;...)
int  lock (int handl;long ofs,len)
int  unlock (int handl;long ofs,len)
int  locking (int handl,mode;long len)
long lseek (int handl;long ofs,int mode)
int  remove (word name);
int  unlink (word name);
int  rename (word old,new);

To do create:
	close
	creat
	isatty
	open
	read
	setmode
	sopen
	umask
	write

	_fmode
	_openfd

*****************************************************************************/

#include "io.hmm"

#ifdef __check_error__
#include "errno.h"
#endif

struct ftime
{
	byte ft_tsec;
	byte ft_min;
	byte ft_hour;
	byte ft_day;
	byte ft_month;
	short ft_year;
};

#define remove unlink

//declaration procedures
int chsize(int hd;long sz);
int cdecl _chmod(word name; int func; ...);
int cdecl ioctl(int handl,cmd;...);
int lock(int handl;long ofs,len);
int unlock(int handl;long ofs,len);
int locking(int handl,mode;long len);

#ifdef __safe_reg_mode__
int access(unsigned int name; word mode);
long tell (int hndl);
int  chmod (word name;int atr);
int  _close (int handl);
int  _creat (word name,attr);
int  _open (word name,flag);
int  _read (word handl,buf,len);
int  _write (word handl,buf,len);
int  creatnew (word name,atr);
int  creattemp (word path,atr);
int  dup (word handl);
int  dup2 (word handl,handl2);
int  eof (word handl);
long filelength (word handle);
int  getftime (int handl;word buf);
int  setftime (int handl;word buf);
long lseek (int handl;long ofs,int mode);
int  unlink (word name);
int  rename (word old,new);
#endif

/*****************************************************************************
* TITLE : int access(unsigned int name; word mode)
* DESCR : check access to file
* PARAMS: input:
*       :      DX - name
*       :      BL - mode check:
*       :          6 - read and write
*       :          4 - read
*       :          2 - write
*       :          0 - exist file
*       : output:
*       :          0 - access true
*       :         -1 - access false
*****************************************************************************/
#ifdef __safe_reg_mode__

:int access(unsigned int name; word mode)
{
	$push CX,(E)DX
	@READATR(name);
	IF(NOTCARRYFLAG){
		DX=mode&2;
		IF(NOTZEROFLAG){
			AX&=1;
			IF(NOTZEROFLAG)GOTO ERR;
		}
		(E)AX=0;
	}
	ELSE{
ERR:
#ifdef __check_error__
		errno=5;
#endif
		(E)AX=-1;
	}
	$pop (E)DX,CX
}

#else

:int fastcall access((E)DX, BX)
{
	@READATR();
	IF(NOTCARRYFLAG){
		BX&=2;
		IF(NOTZEROFLAG){
			AX&=1;
			IF(NOTZEROFLAG)GOTO ERR;
		}
		(E)AX=0;
	}
	ELSE{
ERR:
#ifdef __check_error__
		errno=5;
#endif
		(E)AX=-1;
	}
}

#endif


/*****************************************************************************
* TITLE : int chsize(int hd; long sz)
* DESCR : change size file
* PARAMS: input:
*       :         hd - file handl
*       :         sz - new size file
*       : return: 0  - changed
*       :         -1 - filed
*****************************************************************************/

:int chsize(int hd;long sz)
word ls,ms,buf[0X40];
{
	$PUSH DS
#ifdef __safe_reg_mode__
	$PUSH (E)BX,ECX,EDX,(E)DI
#endif
	TELL(hd);	//текущая позиция
	IF(NOTCARRYFLAG){
		ls=DX;
		ms=AX;
		LFILELENGTH(BX);	//длина файла
		IF(NOTCARRYFLAG){
			EAX-=sz;
			IF(CARRYFLAG){	//увеличить длину
				-EAX;
				sz=EAX;
				ES=SS;	//подготовить буфер
				(E)DI=#buf;
				AX=0;
				CX=0X40;
				$REPE
				$STOSW
				DS=SS;
				ECX=0x80;
				do{
					IF(sz<ECX)ECX=sz;
					$PUSH (E)CX
					FWRITE((E)BX,#buf,(E)CX);
					$POP (E)CX
					IF((E)AX!=(E)CX)&&(CARRYFLAG)GOTO ER;
					sz-=ECX;
				}while(NOTZEROFLAG);
			}
			ELSE IF(NOTZEROFLAG){
#ifdef speed
				CX=SSWORD[#sz+2];
				DX=sz;
#else
				$push sz
				$pop DX
				$pop CX
#endif
				LSEEKSET((E)BX,(E)CX,(E)DX);	//set on position sz
				IF(CARRYFLAG)GOTO ER;
				FWRITE((E)BX,(E)DX,0);
				IF(CARRYFLAG)GOTO ER;
			}
			LSEEKSET((E)BX,ls,ms);
			(E)AX=0;
			GOTO EN;
		}
	}
ER:
#ifdef __check_error__
	errno=(E)AX;
#endif
	(E)AX=-1;
EN:
#ifdef __safe_reg_mode__
	$POP (E)DI,EDX,ECX,(E)BX
#endif
	$POP DS
}

/*****************************************************************************
* TITLE : long tell(int hndl)
* DESCR : get current position in file
* PARAMS: input:
*       :      BX or hndl - handl file
*       : output:
*       :      -1 if errors
*       :      other - position in file
*****************************************************************************/
#ifndef __safe_reg_mode__

:long fastcall tell(BX)
{
	LTELL(BX);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		EAX=-1;
	}
}

#else

:long tell(int hndl)
{
	$push BX,CX,DX
	LTELL(hndl);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		EAX=-1;
	}
	$pop DX,CX,BX
}

#endif

/*****************************************************************************
* TITLE : int chmod(word name;int atr)
* DESCR : Set File Attributes
* PARAMS: input: name - file name
*       :        atr  - new attributes
*       : output: -1 - if errors
*       :          0 - changed
*****************************************************************************/
#ifndef __safe_reg_mode__

:int fastcall chmod(DX,CX)
{
	CHMOD(DX,CX);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
}

#else

:int chmod(word name;int atr)
{
	$push CX,DX
	CHMOD(name,atr);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
	$pop DX,CX
}

#endif

/*****************************************************************************
* TITLE : int cdecl _chmod(word name; int func;(int atr))
* DESCR :
* PARAMS:
*****************************************************************************/
:int cdecl _chmod(unsigned int name,int func,...)
{
#ifdef __safe_reg_mode__
	$push CX,DX
#endif
	CX=SSWORD[BP+8];
	AL=func;
	DX=name;
	$int 0x21
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	ELSE
#ifdef speed
			AX=CX;
#else
			AX><CX;
#endif
#ifdef __safe_reg_mode__
	$pop DX,CX
#endif
}

/*****************************************************************************
* TITLE : int _close(int handl)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifndef __safe_reg_mode__

:int fastcall _close(BX)
{
	CLOSE(BX);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
}

#else

:int _close(int handl)
{
	$push BX
	CLOSE(handl);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
	$pop BX
}

#endif

/*****************************************************************************
* TITLE : int _creat(word name,attr)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifndef __safe_reg_mode__

:int fastcall _creat(DX,CX)
{
	CREAT(DX,CX);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#else

:int _creat(word name,attr)
{
	$push CX,DX
	CREAT(name,attr);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop DX,CX
}

#endif

/*****************************************************************************
* TITLE : int _open(word name,flag)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int _open(word name,flag)
{
	$push DX
	OPEN(name,flag);
	IF(CARRYFLAG){
		AX=-1;
#ifdef __check_error__
		errno=AX;
#endif
	}
	$pop DX
}

#else

:int fastcall _open(DX,AL)
{
	OPEN(DX,AL);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int _read(word handl,buf,len)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int _read(word handl,buf,len)
{
	$push BX,CX,DX
	FREAD(handl,buf,len);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop DX,CX,BX
}

#else

:int fastcall _read(BX,DX,CX)
{
	FREAD(BX,DX,CX);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int _write(word handl,buf,len)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int _write(word handl,buf,len)
{
	$push BX,CX,DX
	FWRITE(handl,buf,len);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop DX,CX,BX
}

#else

:int fastcall _write(BX,DX,CX)
{
	FWRITE(BX,DX,CX);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int creatnew(word name,atr)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int creatnew(word name,atr)
{
	$push CX,DX
	CREATNEW(name,atr);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop DX,CX
}

#else

:int fastcall creatnew(DX,CX)
{
	CREATNEW(DX,CX);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int creattemp(word path,atr)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int creattemp(word path,atr)
{
	$push CX,DX
	CREATETEMP(name,atr);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop DX,CX
}

#else

:int fastcall creattemp(DX,CX)
{
	CREATETEMP(DX,CX);
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int dup(word handl)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int dup(word handl)
{
	$push BX
	BX=handl;
	AH=0x45;
	$int 0x21
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop BX
}

#else

:int fastcall dup(BX)
{
	AH=0x45;
	$int 0x21
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int dup2(word handl,handl2)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int dup2(word handl,handl2)
{
	$push BX,CX
	BX=handl;
	CX=handl2
	AH=0x46;
	$int 0x21
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop CX,BX
}

#else

:int fastcall dup2(BX,CX)
{
	AH=0x45;
	$int 0x21
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int eof(word handl)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int eof(word handl)
{
	$push BX,CX,DX
	TELL(hndl);
	IF(!CARRYFLAG){
		$push AX,DX
		LSEEKEND(BX);
		IF(CARRYFLAG)GOTO ERR;
		$pop CX
		IF(DX==CX){
			$pop DX
			IF(DX==AX)AX=1;
			ELSE AX=0;
		}
		ELSE{
			$pop DX
			AX=0;
		}
		$push AX
		LSEEKSET(BX,CX,DX);
		$pop AX
		IF(CARRYFLAG)GOTO ERR;
	}
	ELSE{
ERR:
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop DX,CX,BX
}

#else

:int fastcall eof(BX)
{
	TELL(BX);
	IF(!CARRYFLAG){
		$push AX,DX
		LSEEKEND(BX);
		IF(CARRYFLAG)GOTO ERR;
		$pop CX
		IF(DX==CX){
			$pop DX
			IF(DX==AX)AX=1;
			ELSE AX=0;
		}
		ELSE{
			$pop DX
			AX=0;
		}
		$push AX
		LSEEKSET(BX,CX,DX);
		$pop AX
		IF(CARRYFLAG)GOTO ERR;
	}
	ELSE{
ERR:
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : long filelength(word handle)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:long filelength(word handle)
{
	$push BX,CX,DX
	TELL(handle);
	IF(!CARRYFLAG){
		$PUSH DX,AX
		LFILELENGTH(BX)
		IF(CARRYFLAG)GOTO ERR;
		$pop DX,CX
		$push EAX;
		LSEEKSET(BX,CX,DX);
		IF(CARRYFLAG){
#ifdef __check_error__
			errno=AX;
#endif
			$pop EAX
			EAX=-1;
		}
		ELSE $pop EAX
	}
	ELSE{
ERR:
#ifdef __check_error__
		errno=AX;
#endif
		EAX=-1;
	}
	$pop DX,CX,BX
}

#else

:long fastcall filelength(BX)
{
	TELL(BX);
	IF(!CARRYFLAG){
		$PUSH DX,AX
		LFILELENGTH(BX)
		IF(CARRYFLAG)GOTO ERR;
		$pop DX,CX
		$push EAX;
		LSEEKSET(BX,CX,DX);
		IF(CARRYFLAG){
#ifdef __check_error__
			errno=AX;
#endif
			$pop EAX
			EAX=-1;
		}
		ELSE $pop EAX
	}
	ELSE{
ERR:
#ifdef __check_error__
		errno=AX;
#endif
		EAX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int getftime(int handl;word buf)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int getftime(int handl;word buf)
{
	$push BX,CX,DX,DI
	AX=0x5700;
	BX=handl;
	$int 0x21
	IF(!CARRYFLAG){
		DI=buf;
		AL=CL&0b00011111;
		$stosb
		AX=CX>>5&0b00111111;
		$stosb
		AL=CH>>3;
		$stosb;
		AL=DL&0x1F;
		$stosb;
		AX=DX>>5&0x1F;
		$stosb
		AX=DH>>1+1980;
		$stosw
		AX=0;
	}
	ELSE{
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	$pop DI,DX,CX,BX
}

#else

:int fastcall getftime(BX,DI)
{
	AX=0x5700;
	$int 0x21
	IF(!CARRYFLAG){
		AL=CL&0b00011111;
		$stosb
		AX=CX>>5&0b00111111;
		$stosb
		AL=CH>>3;
		$stosb;
		AL=DL&0x1F;
		$stosb;
		AX=DX>>5&0x1F;
		$stosb
		AX=DH>>1+1980;
		$stosw
		AX=0;
	}
	ELSE{
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
}

#endif

/*****************************************************************************
* TITLE : int setftime(int handl;word buf)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int setftime(int handl;word buf)
{
	$push BX,CX,DX,SI
	SI=buf;
	$lodsb
	CX=AL;
	$lodsb
	AH=0;
	AX<<=5;
	CX|=AX;
	$lodsb
	AL<<=3;
	CH|=AL;
	$lodsb;
	DX=AL;
	$lodsb
	AH=0;
	AX<<=5;
	DX|=AX;
	$lodsw
	AX-=1980;
	AL<<=1;
	DH|=AL;
	AX=0x5701;
	BX=handl;
	$int 0x21
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
	$pop SI,DX,CX,BX
}

#else

:int fastcall setftime(BX,SI)
{
	$lodsb
	CX=AL;
	$lodsb
	AH=0;
	AX<<=5;
	CX|=AX;
	$lodsb
	AL<<=3;
	CH|=AL;
	$lodsb;
	DX=AL;
	$lodsb
	AH=0;
	AX<<=5;
	DX|=AX;
	$lodsw
	AX-=1980;
	AL<<=1;
	DH|=AL;
	AX=0x5701;
	$int 0x21
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
}

#endif

/*****************************************************************************
* TITLE : int cdecl ioctl(int handl,cmd;...)
* DESCR :
* PARAMS:
*****************************************************************************/
:int cdecl ioctl(int handl,cmd;...)
{
#ifdef __safe_reg_mode__
	$push BX,CX,DX
#endif
	AL=cmd;
	IF(AL==11)||(AL>=2)&&(AL<=5){
		DX=SSWORD[BP+8];
		CX=SSWORD[BP+10];
	}
	BX=handl;
	$int 0x21
	IF(CARRYFLAG){
#ifdef __check_error__
		errno=AX;
#endif
		AX=-1;
	}
	ELSE{
		IF(cmd==0)||(cmd==1){
#ifdef speed
			AX=DX;
#else
			AX><DX;
#endif
		}
	}
#ifdef __safe_reg_mode__
	$pop DX,CX,BX
#endif
}

/*****************************************************************************
* TITLE : int lock(int handl;long ofs,len)
* DESCR :
* PARAMS:
*****************************************************************************/
:int lock(int handl;long ofs,len)
{
#ifdef __safe_reg_mode__
	$push BX,CX,DX,DI,SI
#endif
	DX=ofs;
	CX=SSWORD[#ofs+2];
	DI=len;
	SI=SSWORD[#len+2];
	BX=handl;
	AX=0x5C00;
	$int 0x21
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
#ifdef __safe_reg_mode__
	$pop SI,DI,DX,CX,BX
#endif
}

/*****************************************************************************
* TITLE : int unlock(int handl;long ofs,len)
* DESCR :
* PARAMS:
*****************************************************************************/
:int unlock(int handl;long ofs,len)
{
#ifdef __safe_reg_mode__
	$push BX,CX,DX,DI,SI
#endif
	DX=ofs;
	CX=SSWORD[#ofs+2];
	DI=len;
	SI=SSWORD[#len+2];
	BX=handl;
	AX=0x5C01;
	$int 0x21
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
#ifdef __safe_reg_mode__
	$pop SI,DI,DX,CX,BX
#endif
}

/*****************************************************************************
* TITLE : int locking(int handl,mode;long len)
* DESCR :
* PARAMS:
*****************************************************************************/
:int locking(int handl,mode;long len)
{
#ifdef __safe_reg_mode__
	$push BX,CX,DX,DI,SI
#endif
	TELL(handl);
	CX=DX;
#ifdef speed
	DX=AX;
#else
	DX><AX;
#endif
	DI=len;
	SI=SSWORD[#len+2];
	AL=mode;
	AH=0x5c;
	$int 0x21
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
#ifdef __safe_reg_mode__
	$pop SI,DI,DX,CX,BX
#endif
}

/*****************************************************************************
* TITLE : long lseek(int handl;long ofs,int mode)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:long lseek(int handl;long ofs,int mode)
{
	$push BX,CX,DX
	LSEEK(handl,SSWORD[#ofs+2],ofs,mode);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		EAX=-1;
	}
	ELSE{
		$push DX,AX
		$pop EAX
	}
#else
	IF(CARRYFLAG)$push long -1;
	ELSE $push DX,AX
	$pop EAX
#endif
	$pop DX,CX,BX
}

#else

:long fastcall lseek(BX,EDX,AL)
{
	LLSEEK(BX,EDX,AL);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		EAX=-1;
	}
	ELSE{
		$push DX,AX
		$pop EAX
	}
#else
	IF(CARRYFLAG)$push long -1;
	ELSE $push DX,AX
	$pop EAX
#endif
}

#endif

/*****************************************************************************
* TITLE : int unlink(word name)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int unlink(word name)
{
	$push DX
	UNLINK(name);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
	$pop DX
}

#else

:int fastcall unlink(DX)
{
	UNLINK(DX);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
}

#endif

/*****************************************************************************
* TITLE : int rename(word old,new)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int rename(word old,new)
{
	$push DX,DI,ES
	RENAME(old,new);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
	$pop ES,DI,DX
}

#else

:int fastcall rename(DX,DI)
{
	RENAME(DX,DI);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=AX;
		AX=-1;
	}
	ELSE AX=0;
#else
#ifdef cpu<3
	IF(CARRYFLAG)AX=-1;
	ELSE AX=0;
#else
	$setnc AL
	AH=0;
	AX--;
#endif
#endif
}

#endif

/* end of IO.H */
