/*****************************************************************************
 Sphinx C-- library (C) 2000.
 FILE:        DIR.H
 DESCRIPTION: Defines structures, macros, and functions for dealing with
              directories and pathnames.
 LAST MODIFIED: 28.10.01 22:12
 ----------------------------------------------------------------------------
 int chdir(word pathname)
 int findfirst(unsigned int pathname,buffer;short attrib)
 int findnext(unsigned int buf)
 void fnmerge(unsigned int path,drive,dir,name,ext)
 int fnsplit(unsigned int path,drive,dir,name,ext)
 int getcurdir(int drive;word buff)
 int getdisk()
 int mkdir(unsigned int path)
 int rmdir(unsigned int path)
 int setdisk(int drive)
 unsigned int mktemp(unsigned int template)
 unsigned int getcwd(unsigned int buf,word len)

To do create:
 searchpath

*****************************************************************************/

#ifndef __DIR_H
#define __DIR_H TRUE

#ifdef __check_error__
#include "errno.h"
#endif

#include "dir.hmm"
#include "dos.hmm"

#define WILDCARDS 0x01
#define EXTENSION 0x02
#define FILENAME  0x04
#define DIRECTORY 0x08
#define DRIVE     0x10

#define MAXDRIVE  3

#ifndef __FLAT__

#ifndef _FFBLK_DEF
#define _FFBLK_DEF TRUE
struct  ffblk   {
    char        ff_reserved[21];
    char        ff_attrib;
    unsigned    ff_ftime;
    unsigned    ff_fdate;
    long        ff_fsize;
    char        ff_name[13];
};
#endif

#else  /* defined __FLAT__ */

#ifndef _FFBLK_DEF
#define _FFBLK_DEF TRUE

struct  ffblk   {
    long            ff_reserved;
    long            ff_fsize;
    unsigned long   ff_attrib;
    unsigned short  ff_ftime;
    unsigned short  ff_fdate;
    char            ff_name[256];
};

#endif

#endif  /* __FLAT__  */

#ifdef __safe_reg_mode__
int chdir(unsigned int pathname);
int findfirst(unsigned int pathname,buffer;short attrib);
int findnext(unsigned int buf);
void fnmerge(unsigned int path,drive,dir,name,ext);
int fnsplit(unsigned int path,drive,dir,name,ext);
int getcurdir(int drive;unsigned int buff);
int mkdir(unsigned int path);
int rmdir(unsigned int path);
int setdisk(int drive);
unsigned int mktemp(unsigned int template);
unsigned int getcwd(unsigned int buf,word len);
#endif

/*****************************************************************************
* TITLE : int chdir(word pathname)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int chdir(unsigned int pathname)
{
	$push (E)DX
	@CHDIR(unsigned int pathname);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
	$pop (E)DX
}

#else

:int fastcall chdir((E)DX)
{
	@CHDIR((E)DX);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
}

#endif  //__safe_reg_mode__

/*****************************************************************************
* TITLE : int findfirst(unsigned int pathname,buffer;short attrib)
* DESCR : Find First Matching File
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int findfirst(unsigned int pathname,buffer;short attrib)
{
	$push (E)CX,(E)DX
	SETDTA(buffer);
	@FINDFIRST(pathname,attrib);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
	$pop (E)DX,(E)CX
}

#else  //__safe_reg_mode__

:int fastcall findfirst((E)BX,(E)DX,CX)
{
	SETDTA((E)DX);
	@FINDFIRST((E)BX,CX);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
}

#endif  //__safe_reg_mode__

/*****************************************************************************
* TITLE : int findnext(unsigned int buf)
* DESCR : Find Next Matching File
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int findnext(unsigned int buf)
{
	$push (E)BX,(E)DX,ES
	@GETDTA();
	IF(ES!=DS)||((E)AX!=buf)SETDTA(buf);
	@FINDNEXT();
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
	$pop ES,(E)DX,(E)BX
}

#else

:int fastcall findnext((E)DX)
{
	@GETDTA();
	IF(ES!=DS)||((E)AX!=(E)DX)SETDTA((E)DX);
	@FINDNEXT();
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
}

#endif

/*****************************************************************************
* TITLE : void fnmerge(unsigned int path,drive,dir,name,ext)
* DESCR : Make New File Name
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:void fnmerge(unsigned int path,drive,dir,name,ext)
{
	$push (E)BX,(E)CX,(E)DX,(E)DI,(E)SI,ES
	@FNMERGE(path,drive,dir,name,ext);
	$pop ES,(E)SI,(E)DI,(E)DX,(E)CX,(E)BX
}

#else

:void fastcall fnmerge((E)DI,(E)SI,(E)CX,(E)DX,(E)BX)
{
	@FNMERGE();
}

#endif

/*****************************************************************************
* TITLE : int fnsplit(unsigned int path,drive,dir,name,ext)
* DESCR : split full name
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int fnsplit(unsigned int path,drive,dir,name,ext)
{
	$PUSH (E)BX,(E)CX,(E)DI,(E)SI,ES
	ES=DS;
	(E)SI=path;
	(E)BX=0;
	IF(drive!=0){
		(E)DI=drive
		IF(DSBYTE[(E)SI+1]==':'){
			$MOVSW
		}
		AL=0;
		$STOSB
		(E)BX|=DRIVE;
	}
	IF(dir!=0){
		(E)DI=dir;
		IF(DSBYTE[(E)SI]=='\\'){
			$PUSH (E)SI
			(E)CX=0;
			do{
				$LODSB
				(E)CX++;
			}while(AL!=0);
			$STD
			(E)SI--;
			do{
				$LODSB
				(E)CX--;
			}while(AL!='\\');
			(E)CX++;
			$POP (E)SI
			$CLD
			$REPE
			$MOVSB
		}
		AL=0;
		$STOSB
		(E)BX|=DIRECTORY;
	}
	IF(name!=0){
		(E)DI=name;
		(E)CX=8;
		loop((E)CX){
			$LODSB
			IF(AL=='.')||(AL==0){
				(E)SI--;
				BREAK;
			}
			$STOSB
		}
		AL=0;
		$STOSB
		(E)BX|=FILENAME;
	}
	IF(ext!=0){
		(E)DI=ext;
		do{
			$LODSB
			$STOSB
		}while(AL!=0);
		(E)BX|=EXTENSION;
	}
#ifdef speed
	(E)AX=(E)BX;
#else
	(E)BAX><(E)BX
#endif
	$POP ES,(E)SI,(E)DI,(E)CX,(E)BX
}

#else //__safe_reg_mode__

:int fastcall fnsplit((E)SI,(E)DI,(E)CX,(E)DX,(E)BX)
{
	ES=DS;
	$PUSH (E)BX
	(E)BX=0
	IF((E)DI!=0){
		IF(DSBYTE[(E)SI+1]==':'){
			$MOVSW
		}
		AL=0;
		$STOSB
		(E)BX|=DRIVE;
	}
	IF((E)CX!=0){
		(E)DI=(E)CX;
		IF(DSBYTE[(E)SI]=='\\'){
			$PUSH (E)SI
			(E)CX=0;
			do{
				$LODSB
				(E)CX++;
			}while(AL!=0);
			$STD
			(E)SI--;
			do{
				$LODSB
				(E)CX--;
			}while(AL!='\\');
			(E)CX++;
			$POP (E)SI
			$CLD
			$REPE
			$MOVSB
		}
		AL=0;
		$STOSB
		(E)BX|=DIRECTORY;
	}
	IF((E)DX!=0){
		(E)DI=(E)DX;
		(E)CX=8;
		loop((E)CX){
			$LODSB
			IF(AL=='.')||(AL==0){
				(E)SI--;
				BREAK;
			}
			$STOSB
		}
		AL=0;
		$STOSB
		(E)BX|=FILENAME;
	}
	$POP (E)AX
	IF((E)AX!=0){
#ifdef speed
		(E)DI=(E)AX;
#else
		(E)DI><(E)AX;
#endif
		do{
			$LODSB
			$STOSB
		}while(AL!=0);
		(E)BX|=EXTENSION;
	}
#ifdef speed
	(E)AX=(E)BX;
#else
	(E)AX><(E)BX
#endif
}

#endif	//__safe_reg_mode__

/*****************************************************************************
* TITLE : int getcurdir(int drive;word buff)
* DESCR : Get Current Directory
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int getcurdir(int drive;unsigned int buff)
{
	$push (E)DX,(E)SI
	GETCURDIR(drive,buff);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
	$pop (E)SI,(E)DX
}

#else	//__safe_reg_mode__

:int fastcall getcurdir(DL,(E)SI)
{
	GETCURDIR(DL,(E)SI);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
}
#endif	//__safe_reg_mode__

/*****************************************************************************
* TITLE : int getdisk()
* DESCR : Get Current Default Drive
* PARAMS:
*****************************************************************************/
:int fastcall getdisk()
{
	GETDISK();
	(E)AX=AL;
}

/*****************************************************************************
* TITLE : int mkdir(unsigned int path)
* DESCR : Create Subdirectory
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int mkdir(unsigned int path)
{
	$push (E)DX
	@MKDIR(path);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
	$pop (E)DX
}

#else	//__safe_reg_mode__

:int fastcall mkdir((E)DX)
{
	@MKDIR((E)DX);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
}

#endif	//__safe_reg_mode__

/*****************************************************************************
* TITLE : int rmdir(unsigned int path)
* DESCR : Remove Subdirectory
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int rmdir(unsigned int path)
{
	$push (E)DX
	@RMDIR(path);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
	$pop (E)DX
}

#else	//__safe_reg_mode__

:int fastcall rmdir((E)DX)
{
	@RMDIR((E)DX);
#ifdef __check_error__
	IF(CARRYFLAG){
		errno=(E)AX;
		(E)AX=-1;
	}
	ELSE (E)AX=0;
#else
	$setnc AL
	(E)AX=AL;
	(E)AX--;
#endif
}

#endif	//__safe_reg_mode__

/*****************************************************************************
* TITLE : int setdisk(int drive)
* DESCR : Select Default Drive
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:int setdisk(int drive)
{
	$push (E)DX
	(E)AX=@SETDISK(drive);
	$pop (E)DX
}

#else	//__safe_reg_mode__

:int fastcall setdisk(DL)
{
	(E)AX=@SETDISK(DL);
}

#endif	//__safe_reg_mode__

/*****************************************************************************
* TITLE : unsigned int mktemp(unsigned int template)
* DESCR : Create a Unique File Name
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int mktemp(unsigned int template)
{
	$PUSH (E)BX,(E)CX,(E)SI
	(E)SI=template;
	(E)CX=0;
	(E)BX=0;
	do{
		$lodsb
		(E)CX++;
	}while(AL!=0);
	IF((E)CX>7){
		(E)SI-=7;
		$push (E)SI
		(E)CX=6;
		loop((E)CX){
			$lodsb
			IF(AL!='X')BREAK;
		}
		$pop (E)SI
		IF(ZEROFLAG){
			DSBYTE[(E)SI+2]='.';
			DSBYTE[(E)SI]='A';
			do{
 				DSBYTE[(E)SI+1]='A';
				do{
 					DSBYTE[(E)SI+3]='A';
					do{
	 					DSBYTE[(E)SI+4]='A';
						do{
		 					DSBYTE[(E)SI+5]='A';
							do{
								IF(access(template,0)!=0){
									(E)BX=template;
									BREAK 4;
								}
								DSBYTE[(E)SI+5]++;
							}while(DSBYTE[(E)SI+5]<='Z');
							DSBYTE[(E)SI+4]++;
						}while(DSBYTE[(E)SI+4]<='Z');
						DSBYTE[(E)SI+3]++;
					}while(DSBYTE[(E)SI+3]<='Z');
					DSBYTE[(E)SI+1]++;
				}while(DSBYTE[(E)SI+1]<='Z');
				DSBYTE[(E)SI]++;
			}while(DSBYTE[(E)SI]<='Z');
		}
	}
#ifdef speed
	(E)AX=(E)BX;
#else
	(E)AX><(E)BX;
#endif
	$POP (E)SI,(E)CX,(E)BX
}

#else	//__safe_reg_mode__

:unsigned int fastcall mktemp((E)DI)
{
	(E)SI=(E)DI;
	(E)CX=0;
	(E)BX=0;
	do{
		$lodsb
		(E)CX++;
	}while(AL!=0);
	IF((E)CX>7){
		(E)SI-=7;
		$push (E)SI
		(E)CX=6;
		loop((E)CX){
			$lodsb
			IF(AL!='X')BREAK;
		}
		$pop (E)SI
		IF(ZEROFLAG){
			DSBYTE[(E)SI+2]='.';
			DSBYTE[(E)SI]='A';
			do{
 				DSBYTE[(E)SI+1]='A';
				do{
 					DSBYTE[(E)SI+3]='A';
					do{
	 					DSBYTE[(E)SI+4]='A';
						do{
		 					DSBYTE[(E)SI+5]='A';
							do{
								IF(access((E)DI,0)!=0){
									(E)BX=(E)DI;
									BREAK 4;
								}
								DSBYTE[(E)SI+5]++;
							}while(DSBYTE[(E)SI+5]<='Z');
							DSBYTE[(E)SI+4]++;
						}while(DSBYTE[(E)SI+4]<='Z');
						DSBYTE[(E)SI+3]++;
					}while(DSBYTE[(E)SI+3]<='Z');
					DSBYTE[(E)SI+1]++;
				}while(DSBYTE[(E)SI+1]<='Z');
				DSBYTE[(E)SI]++;
			}while(DSBYTE[(E)SI]<='Z');
		}
	}
#ifdef speed
	(E)AX=(E)BX;
#else
	(E)AX><(E)BX;
#endif
}

#endif	//__safe_reg_mode__

/*****************************************************************************
* TITLE : unsigned int getcwd(unsigned int buf,word len)
* DESCR :
* PARAMS:
*****************************************************************************/
#ifdef __safe_reg_mode__

:unsigned int getcwd(unsigned int buf,word len)
{
byte tbuf[MAXDIR+MAXDRIVE];
	$push (E)CX,(E)DX,(E)DI,(E)SI,ES
	(E)SI=#tbuf;
	tbuf[0]=GETDISK()+'A';
	SSWORD[#tbuf+1]=0x5C3A;
	$push (E)SI
	GETCURDIR(0,(E)SI+3);
	$pop (E)SI
	IF(NOTCARRYFLAG){
		$push DS
		DS=SS;
		(E)CX=0;
		$push (E)SI
		do{
			$lodsb
			(E)CX++;
		}while(AL!=0);
		$pop (E)SI
		IF(CX>len){
#ifdef __check_error__
			errno=0x22;
#endif
			(E)AX=0;
		}
		ELSE{
			(E)DI=buf;
			IF((E)DI!=0){
				ES=DS;
				$push (E)DI
				do{
					$lodsb
					$stosb
				}while(AL!=0);
				$pop (E)DI
			}
#ifdef __check_error__
			ELSE{
//надо добавить выделение памяти
				errno=8;
			}
#endif
#ifdef speed
			(E)AX=(E)DI;
#else
			(E)AX><(E)DI;
#endif
		}
		$pop DS
	}
	ELSE{
#ifdef __check_error__
		errno=(E)AX;
#endif
		(E)AX=0;
	}
	$pop ES,(E)SI,(E)DI,(E)DX,(E)CX
}

#else

:unsigned int fastcall getcwd(unsigned int (E)DI,BX)
{
byte tbuf[MAXDIR+MAXDRIVE];
	(E)SI=#tbuf;
	tbuf[0]=GETDISK()+'A';
	SSWORD[#tbuf+1]=0x5C3A;
	$push (E)SI
	GETCURDIR(0,(E)SI+3);
	$pop (E)SI
	IF(NOTCARRYFLAG){
		$push DS
		DS=SS;
		CX=0;
		$push (E)SI
		do{
			$lodsb
			CX++;
		}while(AL!=0);
		$pop (E)SI
		IF(CX>BX){
#ifdef __check_error__
			errno=0x22;
#endif
			(E)AX=0;
		}
		ELSE{
			IF((E)DI!=0){
				ES=DS;
				$push (E)DI
				do{
					$lodsb
					$stosb
				}while(AL!=0);
				$pop (E)DI
			}
#ifdef __check_error__
			ELSE{
//надо добавить выделение памяти
				errno=8;
			}
#endif
#ifdef speed
			(E)AX=(E)DI;
#else
			(E)AX><(E)DI;
#endif
		}
		$pop DS
	}
	ELSE{
#ifdef __check_error__
		errno=(E)AX;
#endif
		(E)AX=0;
	}
}

#endif

#endif  /* __DIR_H */
/* end of DIR.H */
