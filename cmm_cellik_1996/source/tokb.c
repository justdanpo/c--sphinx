/*
    SPHINX C-- by SPHINX programming
    Property of Peter Cellik.
    Copyright Peter Cellik (C) 1994.  All rights reserved.
    Last updated 3 Nov 1994

14 Mar 94 notes:
    - fixed 32 bit return values bug.
18 Mar 94 notes:
    - fixed32u and s are not yet added to this file.
3 Nov 94 notes:
    - fixed dword >< bug.

todo:
    - check MOVSX and MOVZX and dword modulation.  They should now be fixed...
*/


dofixedreg32math ()
{
}


dofixed32math ()
{
}


getintofixedreg32 ()
{
}



#include "tok.h"


extern int tok,tok2; /* token holders current, next */
extern int rm,rm2;
extern int segm,segm2;
extern int post,post2;
extern unsigned char string[STRLEN],string2[STRLEN];
extern long number,number2;
extern unsigned char cpu,chip;
extern char returntype;	   /* return type, void, byte, word, ... */



notnegit (notneg,ntok,nnumber,nrm,npost,nsegm)
/* produce NOT .. or NEG .. */
int notneg,ntok,nrm,npost,nsegm;
unsigned int nnumber;
{int wordadd = 0;
if(notneg)       /* if NEG is the operation */
    notneg = 8;
switch(ntok)
    {case tk_reg:	wordadd = 1;
    case tk_beg:	op(0xF6+wordadd);  op(0xD0+notneg+nnumber);  break;
    case tk_wordvar:
    case tk_intvar:   wordadd = 1;
    case tk_bytevar:
    case tk_charvar:  outseg(nsegm,npost,2);
		      op(0xF6+wordadd);	op(0x10+notneg+nrm);
		      outaddress(nrm,nnumber);
		      break;
    case tk_reg32:    op(0x66); op(0xF7);  op(0xD0+notneg+nnumber);
		      if(cpu < 3)
			  cpu = 3;
		      break;
    case tk_longvar:
    case tk_dwordvar:  outseg(nsegm,npost,3);
		       op(0x66);
		       op(0xF7);  op(0x10+notneg+nrm);
		       outaddress(nrm,nnumber);
		       if( cpu < 3 )
			   cpu = 3;
		       break;
    default:  varexpected();  break;
    }
}


doreturn ()    /* do return(...); */
{int sign = 0;
nexttok();
if(tok == tk_openbracket)
    {nexttok();
    if(tok != tk_closebracket)
	{switch(returntype)
	    {case tk_int:  sign = 1;
	    case tk_word:  doaxmath(sign);
			   break;
	    case tk_char:  sign = 1;
	    case tk_byte:  doalmath(sign);
			   break;
	    case tk_long:   sign = 1;
	    case tk_dword:  doeaxmath(sign);
	    case tk_void:   break;
	    default:  beep();  break;
	    }
	}
    expecting(tk_closebracket);
    }
leaveproc();
seminext();
}


/* --------------- byte, char, word, int math starts ----------------- */


int tok2notstopper ()
{if( tok2 == tk_semicolon || tok2 == tk_camma || tok2 == tk_closebracket )
    return(0);
return(1);
}


int tok2isopperand ()
{if( tok2 == tk_plus || tok2 == tk_minus || tok2 == tk_mult || tok2 == tk_div
     || tok2 == tk_mod || tok2 == tk_rr || tok2 == tk_ll || tok2 == tk_or
	|| tok2 == tk_and || tok2 == tk_xor
	|| tok2 == tk_divminus || tok2 == tk_modminus || tok2 == tk_multminus
	|| tok2 == tk_xorminus || tok2 == tk_orminus || tok2 == tk_andminus
	|| tok2 == tk_llminus || tok2 == tk_rrminus )
   return(1);
return(0);
}


dodwordvar (sign)     /* signed or unsigned 32 bit memory variable */
int sign;
{unsigned char next = 1, getfromEAX = 0;
unsigned int wloc,vop=0;
int wrm,wsegm,wpost;
wloc = number;
wrm = rm;
wpost = post;
wsegm = segm;
nexttok();
switch(tok)
    {case tk_assign:  nexttok();
		      if( tok2isopperand() )
			   {doeaxmath(sign);
			   getfromEAX = 1;
			   next=0;}
		      else{switch(tok)
			      {case tk_number:
				  outseg(wsegm,wpost,3);
				  outword(0xC766);
				  op(wrm);
				  outaddress(wrm,wloc);
				  outdword(number);
				  break;
			      case tk_postnumber:
				  outseg(wsegm,wpost,3);
				  outword(0xC766);
				  op(wrm);
				  outaddress(wrm,wloc);
				  setdwordpost();
				  outdword(number);
				  break;
			      case tk_reg32:
				  if((unsigned int)number==0 && wrm == rm_d16 )
				      {outseg(wsegm,wpost,2);
				      outword(0xA366);
				      outword(wloc);}
				  else{outseg(wsegm,wpost,3);
				      outword(0x8966);
				      op((unsigned int)number*8 +wrm);
				      outaddress(wrm,wloc);
				      }
				  break;
		  /* check into this, see if MOVZX works...
			      case tk_seg:
				  outseg(wsegm,wpost,2);
				  op(0x8C);
				  op((unsigned int)number*8 +wrm);
				  outaddress(wrm,wloc);
				  break;		   */
			      default:
				  doeaxmath(sign);
				  getfromEAX = 1;
				  next=0;
				  break;
			      }
			 }
		    if( getfromEAX )
			{if( wrm == rm_d16 )
			    {outseg(wsegm,wpost,2);
			    outword(0xA366);	/* MOV [dword],EAX */
			    outword(wloc);}
			else{outseg(wsegm,wpost,3);
			    outword(0x8966);  op(wrm); /* MOV [rmdword],EAX */
			    outaddress(wrm,wloc);}
			}
		    break;
    case tk_minusminus:	vop = 0x8;
    case tk_plusplus:	outseg(wsegm,wpost,3);
			outword(0xFF66);  op(vop+wrm);
			outaddress(wrm,wloc);
			break;
    case tk_xorequals:	vop += 0x08;
    case tk_minusequals: vop += 0x08;
    case tk_andequals:	vop += 0x18;
    case tk_orequals:	vop += 0x08;
    case tk_plusequals:	nexttok();
			if( tok2isopperand() )
			    {doeaxmath(sign);
			    outseg(wsegm,wpost,3);  /* ADD [anydword],EAX */
			    op(0x66); op(0x01+vop);
			    op(wrm);
			    outaddress(wrm,wloc);
			    next=0;}
			else{switch(tok)
				{case tk_number:
				    outseg(wsegm,wpost,3);
				    if( number<128L && number>=-128L )
					outword(0x8366);
				    else outword(0x8166);
				    op(vop+wrm);
				    outaddress(wrm,wloc);
				    if( number<128 && number>=-128 )
					op((unsigned int)number);
				    else outdword(number);
				    break;
		      /* uncomment this once POST_DWORD is done
				case tk_postnumber:
				    outseg(wsegm,wpost,3);
				    op(0x81); op(vop+wrm);
				    outaddress(wrm,wloc);
				    setdwordpost();
				    outdword(number);
				    break;  */
				case tk_reg32:
				    outseg(wsegm,wpost,3);
				    op(0x66);  op(0x01+vop);
				    op((unsigned int)number*8+wrm);
				    outaddress(wrm,wloc);
				    break;
				default:
				    doeaxmath(sign);
				    /* ADD [anydword],EAX */
				    outseg(wsegm,wpost,3);
				    op(0x66); op(0x01+vop);
				    op(wrm); outaddress(wrm,wloc);
				    next=0;
				    break;
				}
			    }
			break;
    case tk_swap:  nexttok();
		   switch(tok)
		       {case tk_reg32:	outseg(wsegm,wpost,3);
					outword(0x8766);
					op((unsigned int)number*8 +wrm);
					outaddress(wrm,wloc);
					break;
		       case tk_longvar:
		       case tk_dwordvar:  if( wrm == rm_d16 )
					      {/* MOV EAX,[dword] */
					      outseg(segm,post,2);
					      outword(0xA166);
					      outword((unsigned int)number);}
					  else{/* MOV EAX,[rmdword] */
					      outseg(segm,post,3);
					      outword(0x8B66);
					      op(rm);
					      outaddress(rm,(unsigned int)number);}
					  /* XCHG EAX,[dword] */
					  outseg(wsegm,wpost,3);
					  outword(0x8766);  op(wrm);
					  outaddress(wrm,wloc);
					  if( wrm == rm_d16 )
					      {/* MOV [dword],EAX */
					      outseg(segm,post,2);
					      outword(0xA366);
					      outword((unsigned int)number);}
					  else{/* MOV [rmdword],EAX */
					      outseg(segm,post,3);
					      outword(0x8966);  op(rm);
					      outaddress(rm,(unsigned int)number);}
					  break;
		       default:	 swaperror();	break;
		       }
		   break;
    case tk_llequals:  nexttok();
		       if( tok2notstopper() )
			   {doalmath(0);    /* all shifts unsigned byte */
			   op(0x88);  op(0xC1);      /* MOV CL,AL */
			   /* SHL [rmdword],CL */
			   outseg(wsegm,wpost,3);
			   outword(0xD366);	op(0x20+wrm);
			   outaddress(wrm,wloc);
			   next=0;}
		       else if(tok == tk_number)
			   {if( (unsigned int)number == 1 )
			       {/* SHL [rmword],1 */
			       outseg(wsegm,wpost,3);
			       outword(0xD166); op(0x20+wrm);
			       outaddress(wrm,wloc);}
			   else if( (unsigned int)number != 0 )
			       {/* SHL [rmdword],imm8 */
			       outseg(wsegm,wpost,3);
			       outword(0xC166);	op(0x20+wrm);
			       outaddress(wrm,wloc);
			       }
			   }
		       else{
			   if(tok != tk_beg || (unsigned int)number != CL)
			       {getintobeg(CL);
			       next=0;}
			   /* SHL [rmdword],CL */
			   outseg(wsegm,wpost,3);
			   outword(0xD366);	op(0x20+wrm);
			   outaddress(wrm,wloc);}
		       break;
    case tk_rrequals: if( sign )
			  vop = 0x10;
		      nexttok();
		      if( tok2notstopper() )
			  {doalmath(0);	/* all shifts unsigned byte */
			  /* MOV CL,AL */
			  op(0x88);  op(0xC1);
			  /* SR [dword],CL */
			  outseg(wsegm,wpost,3);
			  outword(0xD366);  op(0x28+vop+wrm);
			  outaddress(wrm,wloc);
			  next=0;}
		      else if(tok == tk_number)
			  {if( (unsigned int)number == 1 )
			      {/* SR [dword],1 */
			      outseg(wsegm,wpost,3);
			      outword(0xD166);	op(0x28+vop+wrm);
			      outaddress(wrm,wloc);}
			  else if( (unsigned int)number != 0 )
			      {/* SR [dword],imm8 */
			      outseg(wsegm,wpost,3);
			      outword(0xC166); op(0x28+vop+wrm);
			      outaddress(wrm,wloc);
			      }
			  }
		      else{
			  if(tok != tk_beg || (unsigned int)number != CL)
			      {getintobeg(CL);
			      next=0;}
			  outseg(wsegm,wpost,3);  /* SR [dword],CL */
			  outword(0xD366);  op(0x28+vop+wrm);
			  outaddress(wrm,wloc);}
		      break;
    default:  operatorexpected();
	      break;
    }
if(next)
    nextseminext();
else seminext();
if( cpu < 3 )
    cpu = 3;
}


dowordvar (sign)      /* signed or unsigned 16 bit memory variable */
int sign;
{unsigned char next = 1, getfromAX = 0;
unsigned int wloc,vop=0;
int wrm,wsegm,wpost;
wloc = number;
wrm = rm;
wpost = post;
wsegm = segm;
nexttok();
switch(tok)
    {case tk_assign:  nexttok();
		      if( tok2isopperand() )
			   {doaxmath(sign);
			   getfromAX = 1;
			   next=0;}
		      else{switch(tok)
			      {case tk_number:
				  outseg(wsegm,wpost,2);
				  op(0xC7);
				  op(wrm);
				  outaddress(wrm,wloc);
				  outword((unsigned int)number);
				  break;
			      case tk_postnumber:
				  outseg(wsegm,wpost,2);
				  op(0xC7);
				  op(wrm);
				  outaddress(wrm,wloc);
				  setwordpost();
				  outword((unsigned int)number);
				  break;
			      case tk_reg:
				  if((unsigned int)number==0 && wrm == rm_d16 )
				      {outseg(wsegm,wpost,1);
				      op(0xA3);
				      outword(wloc);}
				  else{outseg(wsegm,wpost,2);
				      op(0x89);
				      op((unsigned int)number*8 +wrm);
				      outaddress(wrm,wloc);
				      }
				  break;
			      case tk_seg:
				  outseg(wsegm,wpost,2);
				  op(0x8C);
				  op((unsigned int)number*8 +wrm);
				  outaddress(wrm,wloc);
				  if( (unsigned int)number == FS || (unsigned int)number == GS )
				      if( cpu < 3 )
					  cpu = 3;
				  break;
			      default:
				  doaxmath(sign);
				  getfromAX = 1;
				  next=0;
				  break;
			      }
			 }
		    if( getfromAX )
			{if( wrm == rm_d16 )
			    {outseg(wsegm,wpost,1);
			    op(0xA3);	/* MOV [word],AX */
			    outword(wloc);}
			else{outseg(wsegm,wpost,2);
			    op(0x89);  op(wrm); /* MOV [rmword],AX */
			    outaddress(wrm,wloc);}
			}
		    break;
    case tk_minusminus:	vop = 0x8;
    case tk_plusplus:	outseg(wsegm,wpost,2);
			op(0xFF);  op(vop+wrm);
			outaddress(wrm,wloc);
			break;
    case tk_xorequals:	vop += 0x08;
    case tk_minusequals: vop += 0x08;
    case tk_andequals:	vop += 0x18;
    case tk_orequals:	vop += 0x08;
    case tk_plusequals:	nexttok();
			if( tok2isopperand() )
			    {doaxmath(sign);
			    outseg(wsegm,wpost,2);
			    op(0x01+vop);	op(wrm);  /* ADD [anyword],AX */
			    outaddress(wrm,wloc);
			    next=0;}
			else{switch(tok)
				{case tk_number:
				    outseg(wsegm,wpost,2);
				    if( number<128 && number>=-128L )
					op(0x83);
				    else op(0x81);
				    op(vop+wrm);
				    outaddress(wrm,wloc);
				    if( number<128 && number>=-128 )
					op((unsigned int)number);
				    else outword((unsigned int)number);
				    break;
				case tk_postnumber:
				    outseg(wsegm,wpost,2);
				    op(0x81); op(vop+wrm);
				    outaddress(wrm,wloc);
				    setwordpost();
				    outword((unsigned int)number);
				    break;
				case tk_reg:
				    outseg(wsegm,wpost,2);
				    op(0x01+vop); op((unsigned int)number*8+wrm);
				    outaddress(wrm,wloc);
				    break;
				default:
				    doaxmath(sign);
				    outseg(wsegm,wpost,2);
				    op(0x01+vop);  op(wrm);  /* ADD [anyword],AX */
				    outaddress(wrm,wloc);
				    next=0;
				    break;
				}
			    }
			break;
    case tk_swap:  nexttok();
		   switch(tok)
		       {case tk_reg:  outseg(wsegm,wpost,2);
				      op(0x87);
				      op((unsigned int)number*8 +wrm);
				      outaddress(wrm,wloc);
				      break;
		       case tk_intvar:
		       case tk_wordvar:	if( wrm == rm_d16 )
					    {outseg(segm,post,1);
					    op(0xA1);	/* MOV AX,[number] */
					    outword((unsigned int)number);}
					else{outseg(segm,post,2);
					    op(0x8B);  /* MOV AX,[rmword] */
					    op(rm);
					    outaddress(rm,(unsigned int)number);}
					outseg(wsegm,wpost,2);
					op(0x87);  /* XCHG AX,[wloc] */
					op(wrm);
					outaddress(wrm,wloc);
					if( wrm == rm_d16 )
					    {outseg(segm,post,1);
					    op(0xA3);  /* MOV [number],AX */
					    outword((unsigned int)number);}
					else{outseg(segm,post,2);
					    op(0x89);  /* MOV [rmword],AX */
					    op(rm);
					    outaddress(rm,(unsigned int)number);}
					break;
		       case tk_seg: op(0x8C);	/* MOV AX,seg */
				    op(0xC0+(unsigned int)number*8);
				    outseg(wsegm,wpost,2);
				    op(0x87);  /* XCHG AX,[wloc] */
				    op(wrm);
				    outaddress(wrm,wloc);
				    op(0x8E);	/* MOV seg,AX */
				    op(0xC0+(unsigned int)number*8);
				    break;
		       default:	 swaperror();	break;
		       }
		   break;
    case tk_llequals:  nexttok();
		       if( tok2notstopper() )
			   {doalmath(0);    /* all shifts unsigned byte */
			   op(0x88);  op(0xC1);      /* MOV CL,AL */
			   outseg(wsegm,wpost,2);
			   op(0xD3);  op(0x20+wrm);  /* SHL [rmword],CL */
			   outaddress(wrm,wloc);
			   next=0;}
		       else if(tok == tk_number)
			   {if( (unsigned int)number == 1 )
			       {outseg(wsegm,wpost,2);
			       op(0xD1); op(0x20+wrm);	/* SHL [rmword],1 */
			       outaddress(wrm,wloc);}
			   else if( (unsigned int)number != 0 )
			       {if( chip < 2 )
				   {getintobeg(CL);
				   outseg(wsegm,wpost,2);
				   op(0xD3);  op(0x20+wrm);  /* SHL [rmword],CL */
				   outaddress(wrm,wloc);
				   next=0;}
			       else{outseg(wsegm,wpost,2);
				   op(0xC1);  op(0x20+wrm);  /* SHL [rmword],imm8 */
				   outaddress(wrm,wloc);
				   if( cpu < 2 )
				       cpu = 2;
				   }
			       }
			   }
		       else{
			   if(tok != tk_beg || (unsigned int)number != CL)
			       {getintobeg(CL);
			       next=0;}
			   outseg(wsegm,wpost,2);
			   op(0xD3);	op(0x20+wrm);  /* SHL [rmword],CL */
			   outaddress(wrm,wloc);}
		       break;
    case tk_rrequals: if( sign )
			  vop = 0x10;
		      nexttok();
		      if( tok2notstopper() )
			  {doalmath(0);	/* all shifts unsigned byte */
			  op(0x88);  op(0xC1);	    /* MOV CL,AL */
			  outseg(wsegm,wpost,2);
			  op(0xD3);  op(0x28+vop+wrm);	/* SR [word],CL */
			  outaddress(wrm,wloc);
			  next=0;}
		      else if(tok == tk_number)
			  {if( (unsigned int)number == 1 )
			      {outseg(wsegm,wpost,2);
			      op(0xD1);	op(0x28+vop+wrm); /* SR [word],1 */
			      outaddress(wrm,wloc);}
			  else if( (unsigned int)number != 0 )
			      {if( chip < 2 )
				  {getintobeg(CL);
				  outseg(wsegm,wpost,2);
				  op(0xD3);  op(0x28+vop+wrm); /* SR [word],CL */
				  outaddress(wrm,wloc);
				  next=0;}
			      else{outseg(wsegm,wpost,2);
				  op(0xC1); op(0x28+vop+wrm); /* SR [word],imm8 */
				  outaddress(wrm,wloc);
				  if( cpu < 2 )
				      cpu = 2;
				  }
			      }
			  }
		      else{
			  if(tok != tk_beg || (unsigned int)number != CL)
			       {getintobeg(CL);
			       next=0;}
			  outseg(wsegm,wpost,2);
			  op(0xD3);  op(0x28+vop+wrm);	/* SR [word],CL */
			  outaddress(wrm,wloc);}
		      break;
    default:  operatorexpected();
	      break;
    }
if(next)
    nextseminext();
else seminext();
}



dobytevar (sign)   // byte, char
int sign;
{unsigned char next = 1, getfromAX = 0;
unsigned int bloc,vop = 0;
int brm,bsegm,bpost;
bloc = (unsigned int)number;
bsegm = segm;
bpost = post;
brm = rm;
nexttok();
switch(tok)
    {case tk_assign:  nexttok();
					  if( tok2isopperand() )
						  {doalmath(sign);
						  getfromAX = 1;
						  next = 0;}
					  else{switch(tok)
							  {case tk_number:
								  outseg(bsegm,bpost,2);
								  op(0xC6);
								  op(brm);
								  outaddress(brm,bloc);
								  op((unsigned int)number);
								  break;
							  case tk_beg:
								  if( (unsigned int)number==AL && brm==rm_d16 )
									  {outseg(bsegm,bpost,1);
									  op(0xA2);
									  outword(bloc);}
								  else{outseg(bsegm,bpost,2);
									  op(0x88);
									  op((unsigned int)number*8 +brm);
									  outaddress(brm,bloc);}
								  break;
							  case tk_seg:  segbyteerror();  break;
							  default:  doalmath(sign);
										getfromAX = 1;
										next = 0;
										break;
							  }
						  }
					  if( getfromAX )
						  {if( brm == rm_d16 )
							  {outseg(bsegm,bpost,1);
							  op(0xA2);     /* MOV [byte],AL */
							  outword(bloc);}
						  else{outseg(bsegm,bpost,2);  /* MOV [rmbyte],AL */
							  op(0x88);
							  op(brm);
							  outaddress(brm,bloc);}
						  }
					  break;
    case tk_minusminus:	vop = 0x8;
    case tk_plusplus:	outseg(bsegm,bpost,2);
						 op(0xFE);
						 op(vop+brm);
						 outaddress(brm,bloc);
						 break;
    case tk_xorequals:	vop += 0x08;
    case tk_minusequals: vop += 0x08;
    case tk_andequals:	vop += 0x18;
    case tk_orequals:	vop += 0x08;
    case tk_plusequals:	nexttok();
						 if( tok2isopperand() )
							 {doalmath(sign);
							 outseg(bsegm,bpost,2);
							 op(vop);  op(brm);  /* ADD [anybyte],AL */
							 outaddress(brm,bloc);
							 next=0;}
						 else{switch(tok)
								 {case tk_number:
									 outseg(bsegm,bpost,2);
									 op(0x80);
									 op(vop+brm);
									 outaddress(brm,bloc);
									 op((unsigned int)number);
									 break;
								 case tk_beg:
									 outseg(bsegm,bpost,2);
									 op(vop);
									 op((unsigned int)number*8 +brm);
									 outaddress(brm,bloc);
									 break;
								 case tk_seg:  segbyteerror();  break;
								 default:
									 doalmath(sign);
									 outseg(bsegm,bpost,2);
									 op(vop);  op(brm);  /* ADD [anybyte],AL */
									 outaddress(brm,bloc);
									 next=0;
									 break;
								 }
							 }
						 break;
    case tk_swap:	nexttok();
				   switch(tok)
					   {case tk_beg:  outseg(bsegm,bpost,2);
									  op(0x86);   /* XCHG beg,[anybloc] */
									  op((unsigned int)number*8+brm);
									  outaddress(brm,bloc);
									  break;
					   case tk_bytevar:
					   case tk_charvar:  if( rm == rm_d16 )
											 {outseg(segm,post,1);
											 op(0xA0);   /* MOV AL,[var] */
											 outword((unsigned int)number);}
										 else{outseg(segm,post,2);
											 op(0x8A);  /* MOV AL,[rmvar] */
											 op(rm);
											 outaddress(rm,(unsigned int)number);}
										 outseg(bsegm,bpost,2);
										 op(0x86);   /* XCHG AL,[bloc] */
										 op(brm);
										 outaddress(brm,bloc);
										 if( rm == rm_d16 )
											 {outseg(segm,post,1);
											 op(0xA2);     /* MOV [var],AL */
											 outword((unsigned int)number);}
										 else{outseg(segm,post,2);
											 op(0x88);   /* MOV [rmvar],AL */
											 op(rm);
											 outaddress(rm,(unsigned int)number);}
										 break;
					   default:  swaperror();  break;
					   }
				   break;
    case tk_llequals:  nexttok();
					   if( tok2notstopper() )
						   {doalmath(0);    /* all shifts unsigned byte */
						   op(0x88);  op(0xC1);      /* MOV CL,AL */
						   outseg(bsegm,bpost,2);
						   op(0xD2);  op(0x20+brm);  /* SHL [byte],CL */
						   outaddress(brm,bloc);
						   next=0;}
					   else if(tok == tk_number)
						   {if( (unsigned int)number == 1 )
							   {outseg(bsegm,bpost,2);
							   op(0xD0);  op(0x20+brm);  /* SHL [byte],1 */
							   outaddress(brm,bloc);}
						   else if( (unsigned int)number != 0 )
							   {if( chip < 2 )
								   {getintobeg(CL);
								   outseg(bsegm,bpost,2);
								   op(0xD2);  op(0x20+brm);  /* SHL [byte],CL */
								   outaddress(brm,bloc);
								   next=0;}
							   else{outseg(bsegm,bpost,2);
								   op(0xC0);  op(0x20+brm);  /* SHL [byte],imm8 */
								   outaddress(brm,bloc);
								   if( cpu < 2 )
									   cpu = 2;
								   }
							   }
						   }
					   else{
					       if(tok != tk_beg || (unsigned int)number != CL)
						   {getintobeg(CL);
						   next=0;}
					       outseg(bsegm,bpost,2);
					       op(0xD2);	op(0x20+brm);  /* SHL [byte],CL */
					       outaddress(brm,bloc);}
					   break;
    case tk_rrequals:  if( sign )
						   vop = 0x10;
					   nexttok();
					   if( tok2notstopper() )
						   {doalmath(0);    /* all shifts unsigned byte */
						   op(0x88);  op(0xC1);      /* MOV CL,AL */
						   outseg(bsegm,bpost,2);
						   op(0xD2);  op(0x28+vop+brm);  /* SR [byte],CL */
						   outaddress(brm,bloc);
						   next=0;}
					   else if(tok == tk_number)
						   {if( (unsigned int)number == 1 )
							   {outseg(bsegm,bpost,2);
							   op(0xD0); op(0x28+vop+brm);  /* SR [byte],1 */
							   outaddress(brm,bloc);}
						   else if( (unsigned int)number != 0 )
							   {if( chip < 2 )
								   {getintobeg(CL);
								   outseg(bsegm,bpost,2);
								   op(0xD2);  op(0x28+vop+brm); /* SR [byte],CL */
								   outaddress(brm,bloc);
								   next=0;}
							   else{outseg(bsegm,bpost,2);
								   op(0xC0);  op(0x28+vop+brm); /* SR [byte],imm8 */
								   outaddress(brm,bloc);
								   if( cpu < 2 )
									   cpu = 2;
								   }
							   }
						   }
					   else{
					       if(tok != tk_beg || (unsigned int)number != CL)
						   {getintobeg(CL);
						   next=0;}
					       outseg(bsegm,bpost,2);
					       op(0xD2);	op(0x28+vop+brm);  /* SR [byte],CL */
					       outaddress(brm,bloc);}
					   break;
    default:  operatorexpected();
	      break;
    }
if(next)
    nextseminext();
else seminext();
}


getintoeax (sign,gtok,gnumber,grm,gsegm,gpost)
int sign,gtok,grm,gsegm;
long gnumber;
{switch(gtok)
    {case tk_number:  if( gnumber == 0 )
			  xorEAXEAX();
		      else{outword(0xB866);	/* MOV EAX,# */
			  outdword(gnumber);}
		      break;
    case tk_rmnumber:  notyet();
		       op(0x8D); op(grm);
		       if(gpost != 0)
			   setwordpost();
		       outaddress(grm,gnumber);  /* LEA AX,[rm] */
		       break;
    case tk_postnumber:  outword(0xB866);    /* MOV EAX,# */
			 setdwordpost();
			 outdword(gnumber);
			 break;
    case tk_dwordvar:
    case tk_longvar:  if( grm == rm_d16 )
			  {outseg(gsegm,gpost,2);
			  outword(0xA166);
			  outword((unsigned int)gnumber);}
		      else{outseg(gsegm,gpost,3);
			  outword(0x8B66);
			  op(grm);
			  outaddress(grm,gnumber);}
		      break;
    case tk_intvar:  outseg(gsegm,gpost,4);  /* MOVSX EAX,[intvar] */
		     outword(0x0F66);  op(0xBF);  op(grm);
		     outaddress(grm,gnumber);
		     break;
    case tk_wordvar:  outseg(gsegm,gpost,4);  /* MOVZX EAX,[wordvar] */
		      outword(0x0F66);	op(0xB7);  op(grm);
		      outaddress(grm,gnumber);
		      break;
    case tk_charvar:  outseg(gsegm,gpost,4);	/* MOVSX EAX,[charvar] */
		      outword(0x0F66);	op(0xBE);  op(grm);
		      outaddress(grm,gnumber);
		      break;
    case tk_bytevar:  outseg(gsegm,gpost,4);	/* MOVZX EAX,[bytevar] */
		      outword(0x0F66);	op(0xB6);  op(grm);
		      outaddress(grm,gnumber);
		      break;
    case tk_reg32:  if(gnumber != EAX)	 /* MOV EAX,reg32 */
			{outword(0x8966);  op(0xC0 +gnumber*8);}
		    break;
    case tk_reg:  outword(0x0F66);  /* MOVSX or MOVZX EAX,reg */
		  if(sign)
		      op(0xBF);
		  else op(0xB7);
		  op(0xC0 +gnumber);
		  break;
    case tk_beg:  outword(0x0F66);  /* MOVSX or MOVZX EAX,beg */
		  if(sign)
		      op(0xBE);
		  else op(0xB6);
		  op(0xC0 +gnumber);
		  break;
    case tk_seg:  outword(0x8C66);   /* MOV EAX,seg */
		  op(0xC0 +gnumber*8);
		  break;
    default:  valueexpected();	break;
    }
if( cpu < 3 )
    cpu = 3;
}


doeaxmath ( sign )
int sign;
{int vop,negflag=0,next;
long holdnumber;

if(tok==tk_minus)
    {nexttok();
    if(tok==tk_number)
	number = -number;
    else negflag = 1;
    }
switch(tok)
    {case tk_number:  if(sign)
			  holdnumber = doconstlongmath();
		      else holdnumber = doconstdwordmath();
		      if( holdnumber == 0 )
			  xorEAXEAX();
		      else{outword(0xB866);   /* MOV EAX,# */
			  outdword(holdnumber);}
		      break;
    case tk_ID:
    case tk_id:
    case tk_proc:
    case tk_regproc:
    case tk_undefregproc:
    case tk_undefproc:	if(sign)
			    procdo(tk_long);
			else procdo(tk_dword);
			nexttok();
			break;
    case tk_at:  nexttok();
		 if(sign)
		     macros(tk_long);
		 else macros(tk_dword);
		 nexttok();
		 break;
    default:  getintoeax(sign,tok,number,rm,segm,post);
	      nexttok();
	      break;
    }
if(negflag)
    {op(0x66);
    outword(0xD8F7);	/* NEG EAX */
    negflag = 0;
    }

while( tok!=tk_semicolon && tok!=tk_camma && tok!=tk_closebracket
	&& tok!=tk_eof && iscomparetok(tok)==0 )
    {next=1;
    vop=0;
    switch(tok)
	{case tk_xor:  vop += 0x08;
	case tk_minus: vop += 0x08;
	case tk_and:   vop += 0x18;
	case tk_or:    vop += 0x08;
	case tk_plus:  nexttok();
		       switch(tok)
			   {case tk_number:  op(0x66);
					     op(0x05+vop);
					     outdword(number);
					     break;
			   case tk_longvar:
			   case tk_dwordvar:  outseg(segm,post,3);
					      op(0x66);
					      op(0x03+vop);
					      op(rm);
					      outaddress(rm,(unsigned int)number);
					      break;
			   case tk_reg32:  op(0x66);
					   op(0x01+vop);
					   op(0xC0+(unsigned int)number*8);
					   break;
			   case tk_intvar:
			   case tk_wordvar:
			   case tk_reg:
			   case tk_rmnumber:
			   case tk_postnumber:
			   case tk_charvar:
			   case tk_beg:
			   case tk_bytevar:  getintoreg32(ECX);
					     op(0x66);	/* OPT EAX,ECX */
					     op(0x01+vop);
					     op(0xC8);
					     next=0;
					     break;
			   default:  valueexpected();  break;
			   }
		       break;
	case tk_modminus:  negflag = 1;
	case tk_mod:	   negflag = 1-negflag;
			   vop=1;
	case tk_divminus:  negflag = 1-negflag;
	case tk_div:  nexttok();
		      if( tok == tk_number )
			  {if(negflag)
			      {number = -number;
			      negflag = 0;}
			  if(vop)
			      {switch(number)
				  {case 0:
				      preerror("Hey Joe!  What yer trying to MODULATE BY 0 fur?");
				      break;
				  case 1:
				  case 2:
				  case 4:
				  case 8:
				  case 16:
				  case 32:
				  case 64:
				  case 128:
				  case 256:
				  case 512:
				  case 1024:
				  case 2048:
				  case 4096:
				  case 8192:
				  case 16384:
				  case 32768:
				  case 65536:
				  case 131072:
				  case 262144:
				  case 524288:
				  case 1048576:
				  case 2097152:
				  case 4194304:
				  case 8388608:
				  case 16777216:
				  case 33554432:
				  case 67108864:
				  case 134217728:
				  case 268435456:
				  case 536870912:
				  case 1073741824:
				      outword(0x2566); /* AND EAX,number-1 */
				      outdword(number-1);
				      break;
				  default:
				      if(sign)
					  cdq();
				      else xorEDXEDX();
				      outword(0xB966);	/* MOV ECX,# */
				      outdword(number);
				      outword(0xF766);
				      if(sign)
					  op(0xF9);  /* IDIV ECX */
				      else op(0xF1); /* DIV ECX */
				      op(0x66);      /* MOV EAX,EDX */
				      outword(0xD089);
				      break;
				  }
			      }
			  else{  /*  vop == 0 therefore divide */
			      switch(number)
				  {case 0:
				      preerror("Hey Boy!  What ya trying to DIVIDE BY 0 fur?");
				      break;
				  case 1:  break;
				  case 2:  if(sign)
					       {outword(0xD166);
					       op(0xF8);} /* SAR EAX,1 */
					   else{outword(0xD166);
					       op(0xE8);} /* SHR EAX,1 */
					   break;
				  case 1073741824:  vop++;
				  case 536870912:  vop++;
				  case 268435456:  vop++;
				  case 134217728:  vop++;
				  case 67108864:  vop++;
				  case 33554432:  vop++;
				  case 16777216:  vop++;
				  case 8388608:  vop++;
				  case 4194304:  vop++;
				  case 2097152:  vop++;
				  case 1048576:  vop++;
				  case 524288:	vop++;
				  case 262144:	vop++;
				  case 131072:	vop++;
				  case 65536:  vop++;
				  case 32768:  vop++;
				  case 16384:  vop++;
				  case 8192:  vop++;
				  case 4096:  vop++;
				  case 2048:  vop++;
				  case 1024:  vop++;
				  case 512:  vop++;
				  case 256:  vop++;
				  case 128:  vop++;
				  case 64:  vop++;
				  case 32:  vop++;
				  case 16:  vop++;
				  case 8:  vop++;
	  /* EAX/4 = EAX >> 2 */  case 4:  vop+=2;
					   outword(0xC166);
					   if(sign)
					       op(0xF8); /* SAR EAX,num */
					   else op(0xE8); /* SHR EAX,num */
					   op(vop);
					   break;
				  default:
				      if(sign)
					  cdq();
				      else xorEDXEDX();
				      outword(0xB966);	/* MOV ECX,# */
				      outdword(number);
				      outword(0xF766);
				      if(sign)
					  op(0xF9);  /* IDIV ECX */
				      else op(0xF1); /* DIV ECX */
				      break;
				  }
			      }
			  }
		      else{if(sign)
			      cdq();
			  else xorEDXEDX();
			  switch(tok)
			      {case tk_number:	outword(0xB966);  /* MOV ECX,# */
						outdword(number);
						outword(0xF766);
						if(sign)
						    op(0xF9);  /* IDIV ECX */
						else op(0xF1); /* DIV ECX */
						break;
			      case tk_longvar:
			      case tk_dwordvar:  outseg(segm,post,3);
						 outword(0xF766);
						 if(sign)
						     op(0x38+rm); /* IDIV [dword] */
						 else op(0x30+rm); /* DIV [dword] */
						 outaddress(rm,(unsigned int)number);
						 break;
			      case tk_reg32:  outword(0xF766);
					      if(sign)
						  op(0xF8+(unsigned int)number);
					      else op(0xF0+(unsigned int)number);
					      break;
			      case tk_postnumber:
			      case tk_reg:
			      case tk_intvar:
			      case tk_wordvar:
			      case tk_rmnumber:
			      case tk_seg:
			      case tk_charvar:
			      case tk_beg:
			      case tk_bytevar:	getintoreg32(ECX);
						outword(0xF766);
						if(sign)
						    op(0xF9);  /* IDIV ECX */
						else op(0xF1); /* DIV ECX */
						next=0;
						break;
			      default:	valueexpected();  break;
			      }
			  if(vop)
			      {outword(0x8966);  /* MOV EAX,EDX */
			      op(0xD0);}
			  }
		      break;
	case tk_multminus:  negflag = 1;
	case tk_mult: nexttok();
		      if( negflag && tok == tk_number )
			  {number = -number;
			  negflag = 0;}
		      switch(tok)
			  {case tk_number:  switch(number)
						{case 0: /* EAX * 0 = XOR EAX,EAX */
						    xorEAXEAX();
						case 1:  /* EAX * 1 = EAX */
						    break;
						case 2:  /* EAX * 2 = ADD EAX,EAX */
						    op(0x66);
						    outword(0xC001);
						    break;
						case 1073741824: vop++;
						case 536870912:	vop++;
						case 268435456:	vop++;
						case 134217728:	vop++;
						case 67108864:	vop++;
						case 33554432:	vop++;
						case 16777216:	vop++;
						case 8388608:	vop++;
						case 4194304:	vop++;
						case 2097152:	vop++;
						case 1048576:	vop++;
						case 524288:	vop++;
						case 262144:	vop++;
						case 131072:	vop++;
						case 65536:	vop++;
						case 32768:  vop++;
						case 16384:  vop++;
						case 8192:  vop++;
						case 4096:  vop++;
						case 2048:  vop++;
						case 1024:  vop++;
						case 512:  vop++;
						case 256:  vop++;
						case 128:  vop++;
						case 64:  vop++;
						case 32:  vop++;
						case 16:  vop++;
						case 8:  vop++;
			/* EAX*4 = EAX << 2 */	case 4:	vop+=2;
							/* SHL EAX,num */
							op(0x66);
							outword(0xE0C1);
							op(vop);
							break;
						default:  outword(0xB966); /* MOV ECX,# */
							  outdword(number);
							  outword(0xF766);
							  if(sign)
							      op(0xE9);	/* IMUL ECX */
							  else op(0xE1); /* MUL ECX */
							  break;
						}
					    break;
			  case tk_longvar:
			  case tk_dwordvar:  outseg(segm,post,3);
					     outword(0xF766);
					     if(sign)
						 op(0x28+rm);
					     else op(0x20+rm);
					     outaddress(rm,(unsigned int)number);
					     break;
			  case tk_reg32:  outword(0xF766);
					  if(sign)
					      op(0xE8+(unsigned int)number);
					  else op(0xE0+(unsigned int)number);
					  break;
			  case tk_reg:
			  case tk_intvar:
			  case tk_wordvar:
			  case tk_rmnumber:
			  case tk_postnumber:
			  case tk_seg:
			  case tk_charvar:
			  case tk_beg:
			  case tk_bytevar:  getintoreg32(ECX);
					    outword(0xF766);
					    if(sign)
						op(0xE9);  /* IMUL ECX */
					    else op(0xE1); /* MUL ECX */
					    next=0;
					    break;
			  default:  valueexpected();  break;
			  }
		      break;
	case tk_xorminus:  vop += 0x08;
			   vop += 0x08;
	case tk_andminus:  vop += 0x18;
	case tk_orminus:   vop += 0x08;
			   nexttok();
			   if(tok == tk_number)
			       {number = -number;
			       op(0x66);  op(0x05+vop);
			       outdword(number);}
			   else{getintoreg32(ECX);
			       outword(0xF766); /* NEG ECX */
			       op(0xD9);
			       op(0x66);	/* opt EAX,ECX */
			       op(0x01+vop);
			       op(0xC8);
			       next=0;}
			   break;
	case tk_ll:  nexttok();
		     if(tok==tk_number)
			 {if( (unsigned int)number == 1 )
			     {op(0x66);   /* SHL EAX,1 */
			     outword(0xE0D1);}
			 else if( (unsigned int)number != 0 )
			     {op(0x66);
			     outword(0xE0C1); /* SHL EAX,imm8 */
			     op((unsigned int)number);
			     }
			 }
		     else{getintobeg(CL);
			 op(0x66);	/* SHL EAX,CL */
			 outword(0xE0D3);
			 next=0;}
		     break;
	case tk_llminus:  tok = tk_minus;   // do optimization 286+ here later
			  getintobeg(CL);
			  op(0x66);	  /* SHL EAX,CL */
			  outword(0xE0D3);
			  next=0;
			  break;
	case tk_rr:  if( sign )
			 vop = 0x10;
		     nexttok();
		     if(tok==tk_number)
			 {if( (unsigned int)number == 1 )
			     {outword(0xD166);	/* SR? EAX,1 */
			     op(0xE8+vop);}
			 else if( (unsigned int)number != 0 )
			     {/* SR? EAX,imm8 */
			     outword(0xC166);	op(0xE8+vop);
			     op((unsigned int)number);
			     }
			 }
		     else{getintobeg(CL);
			 outword(0xD366);  /* SR? EAX,CL */
			 op(0xE8+vop);
			 next=0;}
		     break;
	case tk_rrminus:  tok = tk_minus;  // do optimization 286+ here later
			  getintobeg(CL);
			  outword(0xD366);
			  if(sign)
			      op(0xF8);	  /* SAR EAX,CL */
			  else op(0xE8);  /* SHR EAX,CL */
			  next=0;
			  break;
	default:  operatorexpected();  break;
	}
    if(negflag)
	{op(0x66);    /* NEG EAX */
	outword(0xD8F7);
	negflag = 0;
	}
    if(next)
	nexttok();
    }
if(tok==tk_eof)
    unexpectedeof();
}


getintoax (sign,gtok,gnumber,grm,gsegm,gpost)
int sign,gtok,grm,gsegm;
unsigned int gnumber;
{switch(gtok)
    {case tk_number:  if( gnumber == 0 )
			  xorAXAX();
		      else{op(0xB8);	/* MOV AX,# */
			  outword(gnumber);}
		      break;
    case tk_rmnumber:  op(0x8D); op(grm);
		       if(gpost != 0)
			   setwordpost();
		       outaddress(grm,gnumber);	/* LEA AX,[rm] */
		       break;
    case tk_postnumber:	 op(0xB8);	    /* MOV AX,# */
			 setwordpost();
			 outword(gnumber);
			 break;
    case tk_dwordvar:
    case tk_longvar:
    case tk_intvar:
    case tk_wordvar:  if( grm == rm_d16 )
			  {outseg(gsegm,gpost,1);
			  op(0xA1);
			  outword(gnumber);}
		      else{outseg(gsegm,gpost,2);
			  op(0x8B);
			  op(grm);
			  outaddress(grm,gnumber);}
		      break;
    case tk_reg32:
    case tk_reg:  if(gnumber != AX)
		      {op(0x89);
		      op(0xC0 +gnumber*8);}
		  break;
    case tk_seg:  op(0x8C);
		  op(0xC0 +gnumber*8);
		  break;
    case tk_beg:  if(gnumber!=AL)
		      {op(0x88);
		  op(0xC0 +gnumber*8);}
		  xorAHAH();
		  break;
    case tk_charvar:  if( grm == rm_d16 )
			  {outseg(gsegm,gpost,1);
			  op(0xA0);
			  outword(gnumber);}
		      else{outseg(gsegm,gpost,2);
			  op(0x8A);  op(rm);
			  outaddress(grm,gnumber);}
		      cbw();
		      break;
    case tk_bytevar:  if( grm == rm_d16 )
			  {outseg(gsegm,gpost,1);
			  op(0xA0);
			  outword(gnumber);}
		      else{outseg(gsegm,gpost,2);
			  op(0x8A);  op(rm);
			  outaddress(grm,gnumber);}
		      xorAHAH();
		      break;
    default:  valueexpected();	break;
    }
}


doaxmath (sign)
int sign;
{int vop,negflag=0,next;
unsigned int holdnumber;

if(tok==tk_minus)
    {nexttok();
    if(tok==tk_number)
	number = -number;
    else negflag = 1;
    }
switch(tok)
    {case tk_number:  if(sign)
			  holdnumber = doconstlongmath();
		      else holdnumber = doconstdwordmath();
		      if( holdnumber == 0 )
			  xorAXAX();
		      else{op(0xB8);
			  outword(holdnumber);}
		      break;
    case tk_ID:
    case tk_id:
    case tk_proc:
    case tk_regproc:
    case tk_undefregproc:
    case tk_undefproc:	if(sign)
			    procdo(tk_int);
			else procdo(tk_word);
			nexttok();
			break;
    case tk_at:  nexttok();
		 if(sign)
		     macros(tk_int);
		 else macros(tk_word);
		 nexttok();
		 break;
    default:  getintoax(sign,tok,(unsigned int)number,rm,segm,post);
	      nexttok();
	      break;
    }
if(negflag)
    {outword(0xD8F7);	 /* NEG AX */
    negflag = 0;
    }

while( tok!=tk_semicolon && tok!=tk_camma && tok!=tk_closebracket
	&& tok!=tk_eof && iscomparetok(tok)==0 )
    {next=1;
    vop=0;
    switch(tok)
	{case tk_xor:  vop += 0x08;
	case tk_minus: vop += 0x08;
	case tk_and:   vop += 0x18;
	case tk_or:    vop += 0x08;
	case tk_plus:  nexttok();
		       switch(tok)
			   {case tk_number:  op(0x05+vop);
					     outword((unsigned int)number);
					     break;
			   case tk_rmnumber:  op(0x8D);
					      op(0x8+rm);
					      if(post != 0)
						  setwordpost();
					      outaddress(rm,(unsigned int)number);
					      /* LEA CX,[rm] */
					      op(0x01+vop); /* OPT AX,CX */
					      op(0xC8);
					      break;
			   case tk_postnumber:	op(0x05+vop);
						setwordpost();
						outword((unsigned int)number);
						break;
			   case tk_longvar:
			   case tk_dwordvar:
			   case tk_intvar:
			   case tk_wordvar:  outseg(segm,post,2);
					     op(0x03+vop);
					     op(rm);
					     outaddress(rm,(unsigned int)number);
					     break;
			   case tk_reg32:
			   case tk_reg:	 op(0x01+vop);
					 op(0xC0+(unsigned int)number*8);
					 break;
			   case tk_charvar:
			   case tk_beg:
			   case tk_bytevar:  getintoreg(CX);
					     op(0x01+vop);
					     op(0xC8);	/* OPT AX,CX */
					     next=0;
					     break;
			   default:  valueexpected();  break;
			   }
		       break;
	case tk_modminus:  negflag = 1;
	case tk_mod:	   negflag = 1-negflag;
			   vop=1;
	case tk_divminus:  negflag = 1-negflag;
	case tk_div:  nexttok();
		      if( tok == tk_number )
			  {if(negflag)
			      {number = -number;
			      negflag = 0;}
			  if(vop)
			      {switch((unsigned int)number)
				  {case 0:
				      preerror("Hey Joe!  What yer trying to MODULATE BY 0 far?");
				      break;
				  case 1:
				  case 2:
				  case 4:
				  case 8:
				  case 16:
				  case 32:
				  case 64:
				  case 128:
				  case 256:
				  case 512:
				  case 1024:
				  case 2048:
				  case 4096:
				  case 8192:
				  case 16384:
				  case 32768:
				      op(0x25);   /* AND AX,number-1 */
				      outword((unsigned int)number-1);
				      break;
				  default:
				      if(sign)
					  cwd();
				      else xorDXDX();
				      op(0xB9);	/* MOV CX,# */
				      outword((unsigned int)number);
				      op(0xF7);
				      if(sign)
					  op(0xF9);  /* IDIV CX */
				      else op(0xF1); /* DIV CX */
				      op(0x89);	op(0xD0);	/* MOV AX,DX */
				      break;
				  }
			      }
			  else{vop = 0;
			      switch((unsigned int)number)
				  {case 0:
				      preerror("Hey Boy!  What ya trying to DIVIDE BY 0 fur?");
				      break;
				  case 1:  break;
				  case 2:  if(sign)
					       {op(0xD1);
					       op(0xF8);} /* SAR AX,1 */
					   else{op(0xD1);
					       op(0xE8);} /* SHR AX,1 */
					   break;
				  case 32768:  vop++;
				  case 16384:  vop++;
				  case 8192:  vop++;
				  case 4096:  vop++;
				  case 2048:  vop++;
				  case 1024:  vop++;
				  case 512:  vop++;
				  case 256:  vop++;
				  case 128:  vop++;
				  case 64:  vop++;
				  case 32:  vop++;
				  case 16:  vop++;
				  case 8:  vop++;
	   /* AX/4 = AX >> 2 */   case 4:  vop+=2;
					   if( chip < 2 )
					       {op(0xB1);  op(vop); /* MOV CL,num */
					       if(sign)
						   {op(0xD3);
						   op(0xF8);} /* SAR AX,CL */
					       else{op(0xD3);
						   op(0xE8);} /* SHR AX,CL */
					       }
					   else{if(sign)
						   {op(0xC1);
						   op(0xF8);
						   op(vop);} /* SAR AX,num */
					       else{op(0xC1);
						   op(0xE8);
						   op(vop);} /* SHR AX,num */
					       if( cpu < 2 )
						   cpu = 2;
					       }
					   break;
				  default:
				      if(sign)
					  cwd();
				      else xorDXDX();
				      op(0xB9);	/* MOV CX,# */
				      outword((unsigned int)number);
				      op(0xF7);
				      if(sign)
					  op(0xF9);  /* IDIV CX */
				      else op(0xF1); /* DIV CX */
				      break;
				  }
			      }
			  }
		      else{if(sign)
			      cwd();
			  else xorDXDX();
			  switch(tok)
			      {case tk_number:	op(0xB9);	/* MOV CX,# */
						outword((unsigned int)number);
						op(0xF7);
						if(sign)
						    op(0xF9);  /* IDIV CX */
						else op(0xF1); /* DIV CX */
						break;
			      case tk_rmnumber:	 op(0x8D);  /* LEA CX,[rm] */
						 op(0x8+rm);
						 if(post != 0)
						     setwordpost();
						 outaddress(rm,(unsigned int)number);
						 op(0xF7);
						 if(sign)
						     op(0xF9);	/* IDIV CX */
						 else op(0xF1); /* DIV CX */
						 break;
			      case tk_postnumber:  op(0xB9);	/* MOV CX,# */
						   setwordpost();
						   outword((unsigned int)number);
						   op(0xF7);
						   if(sign)
						       op(0xF9);	/* IDIV CX */
						   else op(0xF1); /* DIV CX */
						   break;
			      case tk_longvar:
			      case tk_dwordvar:
			      case tk_intvar:
			      case tk_wordvar:	outseg(segm,post,2);
						op(0xF7);
						if(sign)
						    op(0x38+rm); /* IDIV word ptr [#] */
						else op(0x30+rm); /* DIV word ptr [#] */
						outaddress(rm,(unsigned int)number);
						break;
			      case tk_reg32:
			      case tk_reg:  op(0xF7);
					    if(sign)
						op(0xF8+(unsigned int)number);
					    else op(0xF0+(unsigned int)number);
					    break;
			      case tk_seg:
			      case tk_charvar:
			      case tk_beg:
			      case tk_bytevar:	getintoreg(CX);
						op(0xF7);
						if(sign)
						    op(0xF9);  /* IDIV CX */
						else op(0xF1); /* DIV CX */
						next=0;
						break;
			      default:	valueexpected();  break;
			      }
			  if(vop)
			      {op(0x89); op(0xD0);}  /* MOV AX,DX */
			  }
		      break;
	case tk_multminus:  negflag = 1;
	case tk_mult: nexttok();
		      if( negflag && tok == tk_number )
			  {number = -number;
			  negflag = 0;}
		      switch(tok)
			  {case tk_number:  switch((unsigned int)number)
						{case 0: /* AX * 0 = XOR AX,AX */
						    xorAXAX();
						case 1:  /* AX * 1 = AX */
						    break;
						case 2:  /* AX * 2 = ADD AX,AX */
						    outword(0xC001);
						    break;
						case 32768:  vop++;
						case 16384:  vop++;
						case 8192:  vop++;
						case 4096:  vop++;
						case 2048:  vop++;
						case 1024:  vop++;
						case 512:  vop++;
						case 256:  vop++;
						case 128:  vop++;
						case 64:  vop++;
						case 32:  vop++;
						case 16:  vop++;
						case 8:  vop++;
			/* AX*4 = AX << 2 */	case 4:	vop+=2;
							if( chip < 2 )
							    {op(0xB1);	op(vop); /* MOV CL,num */
							    outword(0xE0D3);}	 /* SHL AX,CL */
							else{/* SHL AX,num */
							    outword(0xE0C1);
							    op(vop);
							    if( cpu < 2 )
								cpu = 2;
							    }
							break;
						default:  op(0xB9);  /* MOV CX,# */
							  outword((unsigned int)number);
							  op(0xF7);
							  if(sign)
							      op(0xE9);	/* IMUL CX */
							  else op(0xE1); /* MUL CX */
							  break;
						}
					    break;
			  case tk_rmnumber:  op(0x8D);	/* LEA CX,[rm] */
					     op(0x8+rm);
					     if(post != 0)
						 setwordpost();
					     outaddress(rm,(unsigned int)number);
					     op(0xF7);
					     if(sign)
						 op(0xE9);  /* IMUL CX */
					     else op(0xE1); /* MUL CX */
					     break;
			  case tk_postnumber:  op(0xB9);  /* MOV CX,# */
					       setwordpost();
					       outword((unsigned int)number);
					       op(0xF7);
					       if(sign)
						   op(0xE9);  /* IMUL CX */
					       else op(0xE1); /* MUL CX */
					       break;
			  case tk_longvar:
			  case tk_dwordvar:
			  case tk_intvar:
			  case tk_wordvar:  outseg(segm,post,2);
					    op(0xF7);
					    if(sign)
						op(0x28+rm);
					    else op(0x20+rm);
					    outaddress(rm,(unsigned int)number);
					    break;
			  case tk_reg32:
			  case tk_reg:	op(0xF7);
					if(sign)
					    op(0xE8+(unsigned int)number);
					else op(0xE0+(unsigned int)number);
					break;
			  case tk_seg:
			  case tk_charvar:
			  case tk_beg:
			  case tk_bytevar:  getintoreg(CX);
					    op(0xF7);
					    if(sign)
						op(0xE9);  /* IMUL CX */
					    else op(0xE1); /* MUL CX */
					    next=0;
					    break;
			  default:  valueexpected();  break;
			  }
		      break;
	case tk_xorminus:  vop += 0x08;
			   vop += 0x08;
	case tk_andminus:  vop += 0x18;
	case tk_orminus:   vop += 0x08;
			   nexttok();
			   if(tok == tk_number)
			       {number = -number;
			       op(0x05+vop);
			       outword((unsigned int)number);}
			   else{getintoreg(CX);
			       op(0xF7);
			       op(0xD9);      /* NEG CX */
			       op(0x01+vop);  /* opt AX,CX */
			       op(0xC8);
			       next=0;}
			   break;
	case tk_ll:  nexttok();
		     if(tok==tk_number)
			 {if( (unsigned int)number == 1 )
			     outword(0xE0D1);	/* SHL AX,1 */
			 else if( (unsigned int)number != 0 )
			     {if( chip < 2 )
				 {getintobeg(CL);
				 outword(0xE0D3);  /* SHL AX,CL */
				 next=0;}
			     else{outword(0xE0C1); /* SHL AX,imm8 */
				 op((unsigned int)number);
				 if( cpu < 2 )
				     cpu = 2;
				 }
			     }
			 }
		     else{getintobeg(CL);
			 outword(0xE0D3);  /* SHL AX,CL */
			 next=0;}
		     break;
	case tk_llminus:  tok = tk_minus;   // do optimization 286+ here later
			  getintobeg(CL);
			  outword(0xE0D3);  /* SHL AX,CL */
			  next=0;
			  break;
	case tk_rr:  if( sign )
			 vop = 0x10;
		     nexttok();
		     if(tok==tk_number)
			 {if( (unsigned int)number == 1 )
			     {op(0xD1);	op(0xE8+vop);  /* SR AX,1 */
			     }
			 else if( (unsigned int)number != 0 )
			     {if( chip < 2 )
				 {getintobeg(CL);
				 op(0xD3);  op(0xE8+vop);  /* SR AX,CL */
				 next=0;}
			     else{op(0xC1);  op(0xE8+vop);	/* SR AX,imm8 */
				 op((unsigned int)number);
				 if( cpu < 2 )
				     cpu = 2;
				 }
			     }
			 }
		     else{getintobeg(CL);
			 op(0xD3);  op(0xE8+vop);  /* SR AX,CL */
			 next=0;}
		     break;
	case tk_rrminus:  tok = tk_minus;  // do optimization 286+ here later
			  getintobeg(CL);
			  op(0xD3);
			  if(sign)
			      op(0xF8);	/* SAR AX,CL */
			  else op(0xE8);	/* SHR AX,CL */
			  next=0;
			  break;
	default:  operatorexpected();  break;
	}
    if(negflag)
	{outword(0xD8F7);  /* NEG AX */
	negflag = 0;
	}
    if(next)
	nexttok();
    }
if(tok==tk_eof)
    unexpectedeof();
}




getintoal (gtok,gnumber,grm,gsegm,gpost)	/* AH may also be changed */
int gtok,grm,gsegm,gpost;
unsigned int gnumber;
{switch(gtok)
    {case tk_number:  op(0xB0);	  /* MOV AL,# */
		      op(gnumber);
		      break;
    case tk_rmnumber:  op(0x8D);  /* LEA AX,[rm] */
		       op(grm);
		       if(gpost != 0)
			   setwordpost();
		       outaddress(grm,gnumber);
		       break;
    case tk_postnumber:	 op(0xB8);	/* MOV AX,# */
			 setwordpost();
			 outword(gnumber);
			 break;
    case tk_bytevar:
    case tk_charvar:
    case tk_longvar:
    case tk_dwordvar:
    case tk_intvar:
    case tk_wordvar:  if( grm == rm_d16 )
			  {outseg(gsegm,gpost,1);
			  op(0xA0);
			  outword(gnumber);}
		      else{outseg(gsegm,gpost,2);
			  op(0x8A);
			  op(grm);
			  outaddress(grm,gnumber);}
		      break;
    case tk_beg:  if( gnumber != AL )
		      {op(0x88);
		      op(0xC0 +gnumber*8);}
		  break;
    case tk_reg:  if( gnumber != AX )
		      {op(0x89);
		      op(0xC0 +gnumber*8);}
		  break;
    case tk_seg:  op(0x8C);  op(0xC0 +gnumber*8);  break;
    default:  valueexpected();	break;
    }
}


doalmath (sign)
char sign;
{int vop,negflag=0,next;

if(tok==tk_minus)
    {nexttok();
    if(tok==tk_number)
	number = -number;
    else negflag = 1;
    }
switch(tok)
    {case tk_number:  op(0xB0);
		      if(sign)
			  op(doconstlongmath());
		      else op(doconstdwordmath());
		      break;
    case tk_ID:
    case tk_id:
    case tk_proc:
    case tk_regproc:
    case tk_undefregproc:
    case tk_undefproc:	if(sign)
			    procdo(tk_char);
			else procdo(tk_byte);
			nexttok();
			break;
    case tk_at:  nexttok();
		 if(sign)
		     macros(tk_char);
		 else macros(tk_byte);
		 nexttok();
		 break;
    default:  getintoal(tok,(unsigned int)number,rm,segm,post);
	      nexttok();
	      break;
    }
if(negflag)
    {op(0xF6);	op(0xD8);	/* NEG AL */
    negflag = 0;}

while( tok!=tk_semicolon && tok!=tk_closebracket && tok!=tk_camma
		   && tok!=tk_eof && iscomparetok(tok)==0 )
    {vop=0;
    switch(tok)
	{case tk_xor:  vop += 0x08;
	case tk_minus: vop += 0x08;
	case tk_and:   vop += 0x18;
	case tk_or:    vop += 0x08;
	case tk_plus:  nexttok();
		       switch(tok)
			   {case tk_number:  op(0x04+vop);
					     op((unsigned int)number); /* OPT AL,num */
					     break;
			   case tk_rmnumber:  op(0x8D);	/* LEA CX,[rm] */
					      op(0x8+rm);
					      if(post != 0)
						  setwordpost();
					      outaddress(rm,(unsigned int)number);
					      op(vop); /* OPT AL,CL */
					      op(0xC8);
					      break;
			   case tk_postnumber:	op(0x05+vop); /* OPT AX,# */
						setwordpost();
						outword((unsigned int)number);
						break;
			   case tk_longvar:
			   case tk_dwordvar:
			   case tk_intvar:
			   case tk_charvar:
			   case tk_wordvar:
			   case tk_bytevar:  outseg(segm,post,2);
					     op(0x02+vop);  op(rm);
					     outaddress(rm,(unsigned int)number);
					     break;
			   case tk_beg:	 op(vop);
					 op(0xC0+(unsigned int)number*8);
					 break;
			   case tk_reg:	 if((unsigned int)number > BX)
					     op(0x89);	/* MOV CX,reg */
					 else op(0x88);  /* MOV CL,regL */
					 op(0xC1+(unsigned int)number*8); /* MOV instr */
					 op(vop);  op(0xC8);	 /* OPT AL,CL */
					 break;
			   default:  valueexpected();	break;
			   }
		       break;
	case tk_modminus:  negflag = 1;
	case tk_mod:	   negflag = 1 - negflag;
			   vop=1;
	case tk_divminus:  negflag = 1 - negflag;
	case tk_div:	   nexttok();
			   if( tok == tk_number )
			       {if(negflag)
				   {number = -number;
				   negflag = 0;}
			       number &= 255;
			       if(vop)
				   {switch((unsigned int)number)
				       {case 0:
					  preerror("Hey Larry!  What ya trying to MODULATE BY 0 fore?");
					  break;
				       case 1:
				       case 2:
				       case 4:
				       case 8:
				       case 16:
				       case 32:
				       case 64:
				       case 128:
					   op(0x24);  /* AND AL,number-1 */
					   op((unsigned int)number-1);
					   break;
				       default:
					   if(sign)
					       cbw();
					   else xorAHAH();
					   op(0xB1); op((unsigned int)number); /* MOV CL,# */
					   op(0xF6);
					   if(sign)
					       op(0xF9);	/* IDIV CL */
					   else op(0xF1); /* DIV CL */
					   op(0x88);  op(0xE0);	/* MOV AL,AH */
					   break;
				       }
				   vop = 0;
				   }
			       else{switch((unsigned int)number)
				       {case 0:
					   preerror("Hey Buddy!  What ya trying to DIVIDE BY 0 fur?");
					   break;
				       case 1:	break;
				       case 2:	if(sign)
						    {op(0xD0);
						    op(0xF8);} /* SAR AL,1 */
						else{op(0xD0);
						    op(0xE8);} /* SHR AL,1 */
						break;
				       case 128:  vop++;
				       case 64:   vop++;
				       case 32:	  vop++;
				       case 16:	  vop++;
				       case 8:	  vop++;
	   /* AL/4 = AL >> 2 */        case 4:	  vop+=2;
						  if( chip < 2 )
						      {op(0xB1);  op(vop); /* MOV CL,num */
						      if(sign)
							  {op(0xD2);
							  op(0xF8);} /* SAR AL,CL */
						      else{op(0xD2);
							  op(0xE8);} /* SHR AL,CL */
						      }
						  else{if(sign)
							  {op(0xC0);
							  op(0xF8);
							  op(vop);} /* SAR AL,num */
						      else{op(0xC0);
							  op(0xE8);
							  op(vop);} /* SHR AL,num */
						      if( cpu < 2 )
							  cpu = 2;
						      }
						  break;
				       default:
					   if(sign)
					       cbw();
									  else xorAHAH();
									  op(0xB1);  /* MOV CL,# */
									  op((unsigned int)number);
									  op(0xF6);
									  if(sign)
										  op(0xF9);  /* IDIV CL */
									  else op(0xF1); /* DIV CL */
									  break;
								  }
							  }
						  }
					  else{if(sign)
						   cbw();
					      else xorAHAH();
						   switch(tok)
							  {case tk_rmnumber:
								  op(0x8D);  /* LEA CX,[rm] */
								  op(0x8+rm);
								  if(post != 0)
									  setwordpost();
								  outaddress(rm,(unsigned int)number);
								  op(0xF6);
								  if(sign)
									  op(0xF9);  /* IDIV CL */
								  else op(0xF1); /* DIV CL */
								  break;
							  case tk_postnumber:
								  op(0xB9);  /* MOV CX,# */
								  setwordpost();
								  outword((unsigned int)number);
								  op(0xF6);
								  if(sign)
									  op(0xF9);  /* IDIV CL */
								  else op(0xF1); /* DIV CL */
								  break;
							  case tk_longvar:
							  case tk_dwordvar:
							  case tk_intvar:
							  case tk_charvar:
							  case tk_wordvar:
							  case tk_bytevar:  outseg(segm,post,2);
												op(0xF6);
												if(sign)
													op(0x38+rm);
												else op(0x30+rm);
												outaddress(rm,(unsigned int)number);
												break;
							  case tk_beg:  op(0xF6);
											if(sign)
												op(0xF8+(unsigned int)number);
											else op(0xF0+(unsigned int)number);
											break;
							  case tk_reg:	if((unsigned int)number > BX)
												op(0x89);  /* MOV CX,reg */
											else op(0x88);  /* MOV CL,regL */
											op(0xC1+(unsigned int)number*8); /* MOV instr */
											op(0xF6);
											if(sign)
												op(0xF9);  /* IDIV CL */
											else op(0xF1); /* DIV CL */
											break;
							  default:  valueexpected();  break;
							  }
						  if(vop)
							  {op(0x88);  op(0xE0);}  /* MOV AL,AH */
						  }
					  break;
	case tk_multminus:  negflag = 1;
	case tk_mult: nexttok();
		      switch(tok)
			  {case tk_number:  if( negflag )
						{number = -number;
						negflag = 0;}
					    number &= 255;
					    switch((unsigned int)number)
						{case 0: /* AL * 0 = MOV AL,0 */
							op(0xB0);  op(0x00);
							break;
						case 1:  /* AL * 1 = AL */
							break;
						case 2:  /* AL * 2 = ADD AL,AL */
							op(0x00);  op(0xC0);
							break;
						case 128:  vop++;
						case 64:  vop++;
						case 32:  vop++;
						case 16:  vop++;
						case 8:  vop++;
			/* AL*4 = AL << 2 */	case 4:  vop+=2;
							 if( chip < 2 )
							     {op(0xB1);	op(vop); /* MOV CL,num */
							     op(0xD2);	op(0xE0);} /* SHL AX,CL */
							 else{op(0xC0); op(0xE0);
							     op(vop);  /* SHL AX,num */
							     if( cpu < 2 )
								 cpu = 2;
							     }
							 break;
						default:  op(0xB1);  /* MOV CL,# */
							  op((unsigned int)number);
							  op(0xF6);
							  if(sign)
							      op(0xE9);	/* IMUL CL */
							  else op(0xE1);  /* MUL CL */
							  break;
						}
					    break;
			  case tk_rmnumber:  op(0x8D);	/* LEA CX,[rm] */
					 op(0x8+rm);
					 if(post != 0)
						 setwordpost();
					 outaddress(rm,(unsigned int)number);
					 op(0xF6);
					 if(sign)
					 op(0xE9);   /* IMUL CL */
					 else op(0xE1);  /* MUL CL */
					 break;
			  case tk_postnumber:  op(0xB9);  /* MOV CX,# */
											   setwordpost();
											   outword((unsigned int)number);
											   op(0xF6);
											   if(sign)
												   op(0xE9);   /* IMUL CL */
											   else op(0xE1);  /* MUL CL */
											   break;
						  case tk_longvar:
						  case tk_dwordvar:
						  case tk_intvar:
						  case tk_charvar:
						  case tk_wordvar:
						  case tk_bytevar:  outseg(segm,post,2);
											op(0xF6);
											if(sign)
												op(0x28+rm);
											else op(0x20+rm);
											outaddress(rm,(unsigned int)number);
											break;
						  case tk_beg:  op(0xF6);
										if(sign)
											op(0xE8+(unsigned int)number);
										else op(0xE0+(unsigned int)number);
										break;
						  case tk_reg:	if((unsigned int)number > BX)
											op(0x89);  /* MOV CX,reg */
										else op(0x88);  /* MOV CL,regL */
										op(0xC1+(unsigned int)number*8); /* MOV instr */
										op(0xF6);
										if(sign)
											op(0xE9);   /* IMUL CL */
										else op(0xE1);  /* MUL CL */
										break;
						  default:  valueexpected();  break;
						  }
					  break;
	case tk_xorminus:  vop += 0x08;
			   vop += 0x08;
	case tk_andminus:  vop += 0x18;
	case tk_orminus:   vop += 0x08;
			   nexttok();
			   if(tok == tk_number)
			       {number = -number;
			       op(0x04+vop);
			       op((unsigned int)number);}
			   else{getintobeg(CL);
			       outword(0xD9F6);  /* NEG CL */
			       op(0x00+vop);
			       op(0xC8);	/* opt AL,CL */
			       next=0;}
			   break;
	case tk_ll:  nexttok();
		     if(tok==tk_number)
			 {if( (unsigned int)number == 1 )
			     {outword(0xE0D0);	    /* SHL AL,1 */
			     }
			 else if( (unsigned int)number != 0 )
			     {if( chip < 2 )
				 {getintobeg(CL);
				 outword(0xE0D2);   /* SHL AL,CL */
				 next=0;}
			     else{outword(0xE0C0);  /* SHL AL,imm8 */
				 op((unsigned int)number);
				 if( cpu < 2 )
				     cpu = 2;
				 }
			     }
			 }
		     else{getintobeg(CL);
			 outword(0xE0D2);   /* SHL AL,CL */
			 next=0;}
		     break;
	case tk_llminus:  tok = tk_minus;   // need 286+ opt some time
			  getintobeg(CL);
			  outword(0xE0D2);  /* SHL AL,CL */
			  next=0;
			  break;
	case tk_rr:  if(sign)
			 vop = 0x10;
		     nexttok();
		     if(tok==tk_number)
			 {if( (unsigned int)number == 1 )
			     {op(0xD0); op(0xE8+vop);  /* SR AL,1 */
			     }
			 else if( (unsigned int)number != 0 )
			     {if( chip < 2 )
				 {getintobeg(CL);
				 op(0xD2);  op(0xE8+vop);  /* SR AL,CL */
				 next=0;}
			     else{op(0xC0);  op(0xE8+vop);	/* SR AL,imm8 */
				 op((unsigned int)number);
				 if( cpu < 2 )
				     cpu = 2;
				 }
			     }
			 }
		     else{getintobeg(CL);
			 op(0xD2);
			 if(sign)
			     op(0xF8);	/* SAR AX,CL */
			 else op(0xE8);  /* SHR AX,CL */
			 next=0;
			 }
		     break;
	case tk_rrminus:  tok = tk_minus;   // need 286+ opt some time
			  getintobeg(CL);
			  op(0xD2);
			  if(sign)
			      op(0xF8);	/* SAR AX,CL */
			  else op(0xE8);  /* SHR AX,CL */
			  next=0;
			  break;
	default:  operatorexpected();  break;
	}
    if( negflag )
	{outword(0xD8F6);   /* NEG AL */
	negflag = 0;}
    if(next)
	nexttok();
    }
if(tok==tk_eof)
    unexpectedeof();
}



/* =============== doreg32(), doreg(), dobeg(), doseg() ===============*/

doreg32 (reg32)
int reg32;
{unsigned char next=1;
int vop=0;
nexttok();
switch(tok)
    {case tk_assign:  nexttok();
		      if(reg32==EAX)
			  doeaxmath(0);
		      else{getintoreg32(reg32);
			  doreg32math(reg32);}
		      next = 0;
		      break;
    case tk_plusplus:  op(0x66);  op(0x40+reg32);  break;
    case tk_minusminus:	op(0x66);  op(0x48+reg32);  break;
    case tk_openbracket:  doparams();
			  op(0xFF66);  /* CALL reg32 with stack params */
			  op(0xD0+reg32);
			  break;
    case tk_swap:  nexttok();
		   switch(tok)
		       {case tk_longvar:
		       case tk_dwordvar:  outseg(segm,post,3);
					  outword(0x8766);
					  op(reg32*8+rm);
					  outaddress(rm,(unsigned int)number);
					  break;
		       case tk_reg32:  op(0x66);
				       if(reg32 != (unsigned int)number)
					   {if(reg32 == EAX)
					       op(0x90 +(unsigned int)number);
					 else if((unsigned int)number == EAX)
					     op(0x90 +reg32);
					 else{op(0x87);
					     op(0xC0 + (unsigned int)number + reg32*8);}
					 }
				     break;
		       default:	 swaperror();  break;
		       }
		   break;
    case tk_xorequals:	 vop += 0x08;
    case tk_minusequals: vop += 0x08;
    case tk_andequals:	 vop += 0x18;
    case tk_orequals:	 vop += 0x08;
    case tk_plusequals:	 nexttok();
			 switch(tok)
			     {case tk_number:  op(0x66);
					       if( reg32 == EAX )
						   op(0x05+vop);
					       else{op(0x81);
						   op(0xC0+vop +reg32);}
					       outdword(doconstlongmath());
					       next = 0;
					       break;
			     case tk_longvar:
			     case tk_dwordvar:	outseg(segm,post,3);
						op(0x66);
						op(0x03+vop);
						op(reg32*8+rm);
						outaddress(rm,(unsigned int)number);
						break;
			     case tk_reg32:  op(0x66);	op(0x01+vop);
					     op(0xC0 +reg32 +(unsigned int)number*8);
					     break;
			     case tk_ID:
			     case tk_id:
			     case tk_proc:
			     case tk_regproc:
			     case tk_undefregproc:
			     case tk_undefproc:	 if(reg32 == EAX)
						     procdo(tk_dword);
						 else regcallerror();
						 break;
			     case tk_reg:
			     case tk_wordvar:
			     case tk_intvar:
			     case tk_seg:
			     case tk_bytevar:
			     case tk_charvar:
			     case tk_beg:  if(reg32 == EAX)
					       {getintoreg32(ECX);
					       op(0x66);  /* OPT reg32,ECX */
					       op(0x01+vop);
					       op(0xC8+reg32);
					       next=0;}
					   else regbyteerror();
					   break;
			     case tk_minus:  if(tok2 == tk_number)
						 {op(0x66);
						 if(reg32==EAX)
						     op(0x05+vop);
						 else{op(0x81);
						     op(0xC0+vop+reg32);}
						 outdword(doconstlongmath());
						 next = 0;
						 }
					     else if(reg32 == EAX)
						 {getintoreg32(ECX);
						 op(0x66);  /* OPT reg,ECX */
						 op(0x01+vop);
						 op(0xC8+reg32);
						 next=0;}
					     else negregerror();
					     break;
			     default:  valueexpected();	break;
			     }
			 break;
    case tk_rrequals:  vop += 0x08;
    case tk_llequals:  nexttok();
		       if( tok2notstopper() )
			   {if(reg32==EAX)
			       {getintobeg(CL);
			       nexttok();
			       dobegmath(CL);
			       outword(0xD366);   /* SHL EAX,CL */
			       op(0xE0+vop);
			       next = 0;
			       }
			   else regshifterror();
			   }
		       else if(tok==tk_number)
			   {if( (unsigned int)number == 1 )
			       {outword(0xD166);  /* SHL reg,1 */
			       op(0xE0+reg32+vop);}
			   else if( (unsigned int)number != 0 )
			       {outword(0xC166);  /* SHL reg32,imm8 */
			       op(0xE0+reg32+vop);
			       op((unsigned int)number);
			       }
			   }
		       else{getintobeg(CL);
			   outword(0xD366);  /* SHL reg32,CL */
			   op(0xE0+reg32+vop);}
		       break;
    default:  operatorexpected();  break;
    }
if( cpu < 3 )
    cpu = 3;
if(next)
    nextseminext();
else seminext();
}


doreg (reg)
int reg;
{unsigned char next=1;
int vop=0;
nexttok();
switch(tok)
    {case tk_assign:  nexttok();
		      if(reg==AX)
			  doaxmath(0);
		      else{getintoreg(reg);
			  doregmath(reg);}
		      next = 0;
		      break;
    case tk_plusplus:  op(0x40+reg);  break;
    case tk_minusminus:	op(0x48+reg);  break;
    case tk_openbracket:  doparams();
			  op(0xFF);
			  op(0xD0 + reg);   /* CALL reg with stack params */
			  break;
    case tk_swap:  nexttok();
		   switch(tok)
		       {case tk_intvar:
		       case tk_wordvar:	outseg(segm,post,2);
					op(0x87);
					op(reg*8+rm);
					outaddress(rm,(unsigned int)number);
					break;
		       case tk_reg:  if(reg != (unsigned int)number)
					 {if(reg == AX)
					     op(0x90 +(unsigned int)number);
					 else if((unsigned int)number == AX)
					     op(0x90 +reg);
					 else{op(0x87);
					     op(0xC0 + (unsigned int)number + reg*8);}
					 }
				     break;
		       default:	 swaperror();  break;
		       }
		   break;
    case tk_xorequals:	 vop += 0x08;
    case tk_minusequals: vop += 0x08;
    case tk_andequals:	vop += 0x18;
    case tk_orequals:	vop += 0x08;
    case tk_plusequals:	 nexttok();
			 switch(tok)
			     {case tk_number:  if(reg==AX)
						   op(0x05+vop);
					       else{op(0x81);
						   op(0xC0+vop +reg);}
					       outword((unsigned int)doconstlongmath());
					       next = 0;
					       break;
			     case tk_longvar:
			     case tk_dwordvar:
			     case tk_intvar:
			     case tk_wordvar:  outseg(segm,post,2);
					       op(0x03+vop);
					       op(reg*8+rm);
					       outaddress(rm,(unsigned int)number);
					       break;
			     case tk_reg32:
			     case tk_reg:  op(0x01+vop);
					   op(0xC0 +reg +(unsigned int)number*8);
					   break;
			     case tk_ID:
			     case tk_id:
			     case tk_proc:
			     case tk_regproc:
			     case tk_undefregproc:
			     case tk_undefproc:	 if(reg == AX)
						     procdo(tk_word);
						 else regcallerror();
						 break;
			     case tk_bytevar:
			     case tk_charvar:
			     case tk_beg:  if(reg == AX)
					       {getintoreg(CX);
					       op(0x01+vop);
					       op(0xC8 +reg);
					       next=0;}	 /* OPT reg,CX */
					   else regbyteerror();
					   break;
			     case tk_minus:  if(tok2 == tk_number)
						 {if(reg==AX)
						     op(0x05+vop);
						 else{op(0x81);
						     op(0xC0+vop +reg);}
						 outword((unsigned int)doconstlongmath());
						 next = 0;
						 }
					     else if(reg == AX)
						 {getintoreg(CX);
						 op(0x01+vop);
						 op(0xC8 +reg);
						 next=0;}  /* OPT reg,CX */
					     else negregerror();
					     break;
			     default:  valueexpected();	break;
			     }
			 break;
    case tk_rrequals:  vop += 0x08;
    case tk_llequals:  nexttok();
		       if( tok2notstopper() )
			   {if(reg==AX)
			       {getintobeg(CL);
			       dobegmath(CL);
			       op(0xD3); op(0xE0+vop);	/* SHL AX,CL */
			       next = 0;
			       }
			   else regshifterror();
			   }
		       else if(tok==tk_number)
			   {if( (unsigned int)number == 1 )
			       {op(0xD1);  op(0xE0+reg+vop);}  /* SHL reg,1 */
			   else if( (unsigned int)number != 0 )
			       {if( chip < 2 )
				   {getintobeg(CL);
				   op(0xD3);  op(0xE0+reg+vop);  /* SHL reg,CL */
				   next = 0;}
			       else{op(0xC1);  op(0xE0+reg+vop);  /* SHL reg,imm8 */
				   op((unsigned int)number);
				   if( cpu < 2 )
				       cpu = 2;
				   }
			       }
			   }
		       else{getintobeg(CL);
			   op(0xD3); op(0xE0+vop+reg);	/* SHL reg,CL */
			   next = 0;}
		       break;
    default:  operatorexpected();  break;
    }
if(next)
    nextseminext();
else seminext();
}


dobeg (beg)
int beg;
{unsigned char next=1;
int vop=0;
nexttok();
switch(tok)
    {case tk_assign:  nexttok();
		      if( beg == AL )
			  {doalmath(0);
			  next = 0;}
		      else{getintobeg(beg);
			  dobegmath(beg);
			  next = 0;
			  }
		      break;
    case tk_plusplus:  op(0xFE);
		       op(0xC0+beg);
		       break;
    case tk_minusminus:	 op(0xFE);
			 op(0xC8+beg);
			 break;
    case tk_swap:  nexttok();
		   switch(tok)
		       {case tk_charvar:
		       case tk_bytevar:	 outseg(segm,post,2);
					 op(0x86);
					 op(beg*8+rm);
					 outaddress(rm,(unsigned int)number);
					 break;
		       case tk_beg:	if(beg!=(unsigned int)number)
					 {op(0x86);
					 op(0xC0 + (unsigned int)number + beg*8);}
					break;
		       default:	 swaperror();
				 break;
		       }
		   break;
    case tk_xorequals:	vop += 0x08;
    case tk_minusequals: vop += 0x08;
    case tk_andequals:	vop += 0x18;
    case tk_orequals:	vop += 0x08;
    case tk_plusequals:	nexttok();
			switch(tok)
			    {case tk_number:  if(beg==AL)
						  op(0x04+vop);
					      else{op(0x80);
						  op(0xC0+vop+beg);}
					      op((int)doconstlongmath());
					      next = 0;
					      break;
			    case tk_longvar:
			    case tk_dwordvar:
			    case tk_wordvar:
			    case tk_charvar:
			    case tk_intvar:
			    case tk_bytevar:  outseg(segm,post,2);
					      op(0x02+vop);
					      op(beg*8 +rm);
					      outaddress(rm,(unsigned int)number);
					      break;
			    case tk_beg:  op(0x00+vop);
					  op(0xC0 +beg +(unsigned int)number*8);
					  break;
			    case tk_proc:
			    case tk_regproc:
			    case tk_undefproc:
			    case tk_undefregproc:
			    case tk_ID:
			    case tk_id:	 if(beg == AL)
					     procdo(tk_byte);
					 else begcallerror();
					 break;
			    case tk_reg:  if( (unsigned int)number < BX )
					      {op(0x00+vop);
					      op(0xC0 +beg +(unsigned int)number*8);}
					  else begworderror();
					  break;
			    case tk_seg:  begworderror();  break;
			    default:  valueexpected();	break;
			    }
			break;
   case tk_rrequals:  vop += 0x08;
   case tk_llequals:  nexttok();	   // 286+ opt has still do be done
		      if(tok==tk_number && (unsigned int)number==0)
			  { /* no operation */ }
		      else if(tok==tk_number && (unsigned int)number==1)
			  {op(0xD0);  op(0xE0+beg+vop);}  /* SHL beg,1 */
		      else if(tok==tk_beg && (unsigned int)number==CL)
			  {op(0xD2);	op(0xE0+beg+vop);}  /* SHL beg,CL */
		      else{if(beg==AL)
			      {getintobeg(CL);
			      dobegmath(CL);
			      op(0xD2); op(0xE0+vop+beg);   /* SHL beg,CL */
			      next = 0;
			      }
			  else regshifterror();
			  }
		      break;
    default:  operatorexpected();  break;
    }
if(next)
    nextseminext();
else seminext();
}


doseg (seg)
int seg;
{unsigned char next =1;
int vop = 0;
if( seg == FS || seg == GS )
    if( cpu < 3 )
	cpu = 3;
if( seg == HS || seg == IS )
    if( cpu < 5 )
	cpu = 5;
nexttok();
if(tok==tk_assign)
    {nexttok();
    if( tok2isopperand()==0 && (tok==tk_reg || tok==tk_intvar ||
	    tok==tk_wordvar || tok==tk_longvar || tok==tk_dwordvar) )
	{if( tok == tk_reg )
	    {op(0x8E);
	    op(0xC0 +seg*8 +(unsigned int)number);  /* MOV seg,reg */
	    }
	else{outseg(segm,post,2);
	    op(0x8E);
	    op(seg*8 +rm);
	    outaddress(rm,(unsigned int)number);   /* MOV seg,[wordvar] */
	    }
	}
    else{doaxmath(0);
	op(0x8E);   /* MOV SEG,AX */
	op(0xC0 +seg*8);
	next=0;
	}
    }
else if(tok == tk_swap)
    {nexttok();
    switch(tok)
	{case tk_intvar:
	case tk_wordvar:  op(0x8C);
			  op(0xC0 +(unsigned int)number*8);	/* MOV AX,SEG */
			  outseg(segm,post,2);
			  op(0x87);
			  op(vop);
			  outaddress(rm,(unsigned int)number);	/* XCHG AX,[word] */
			  op(0x8E);
			  op(0xC0 +seg*8);  /* MOV seg,AX */
			  break;
	default:  preerror("Only int and word variables valid for segment register ><.");
		  break;
	}
    }
else segoperror();
if(next)
    nextseminext();
else seminext();
}


/* =============== doreg32math(), doregmath(), dobegmath() ===============*/


doreg32math (reg32)  /* math done is on all regs except EAX */
int reg32;	     /* all other registers preserved */
{int vop;
while(tok!=tk_semicolon &&tok!=tk_camma &&tok!=tk_closebracket &&tok!=tk_eof)
    {vop=0;
    switch(tok)
	{case tk_xor:  vop += 0x08;
	case tk_minus: vop += 0x08;
	case tk_and:   vop += 0x18;
	case tk_or:    vop += 0x08;
	case tk_plus:  nexttok();
		       switch(tok)
			   {case tk_number:  outword(0x8166);
					     op(0xC0+vop+reg32);
					     outdword(number);
					     break;
			   case tk_postnumber:	outword(0x8166);
						op(0xC0+vop+reg32);
						setdwordpost();
						outdword(number);
						break;
			   case tk_reg32:  op(0x66);  op(0x01+vop);
					   op(0xC0+reg32 +(unsigned int)number*8);
					   break;
			   case tk_longvar:
			   case tk_dwordvar:  outseg(segm,post,3);
					      op(0x66);	op(0x03+vop);
					      op(reg32*8+rm);
					      outaddress(rm,(unsigned int)number);
					      break;
			   case tk_bytevar:
			   case tk_charvar:
			   case tk_beg:  regbyteerror();  break;
			   case tk_wordvar:
			   case tk_intvar:
			   case tk_reg:  reg32regerror();  break;
			   case tk_ID:
			   case tk_id:
			   case tk_proc:
			   case tk_regproc:
			   case tk_undefregproc:
			   case tk_undefproc:  regcallerror();	break;
			   case tk_seg:
			   case tk_rmnumber:  regmatherror();  break;
			   default:  valueexpected();  break;
			   }
		       break;
	case tk_xorminus:  vop += 0x08;
			   vop += 0x08;
	case tk_andminus:  vop += 0x18;
	case tk_orminus:   vop += 0x08;
			   nexttok();
			   if(tok == tk_number)
			       {number = -number;
			       outword(0x8166);
			       op(0xC0+vop +reg32);
			       outdword(number);}
			   else negregerror();
			   break;
	case tk_multminus:
	case tk_divminus:
	case tk_modminus:
	case tk_mult:
	case tk_div:
	case tk_mod:
	case tk_rrminus:
	case tk_llminus:
	case tk_ll:
	case tk_rr:  regmathoperror();	break;
	default:  operatorexpected();  break;
	}
    nexttok();
    }
if( cpu < 3 )
    cpu = 3;
}


doregmath (reg)  /* math done is on all regs except AX */
int reg;	 /* all other registers preserved */
{int vop;
while(tok!=tk_semicolon &&tok!=tk_camma &&tok!=tk_closebracket &&tok!=tk_eof)
    {vop=0;
    switch(tok)
	{case tk_xor:  vop += 0x08;
	case tk_minus: vop += 0x08;
	case tk_and:   vop += 0x18;
	case tk_or:    vop += 0x08;
	case tk_plus:  nexttok();
		       switch(tok)
			   {case tk_number:  op(0x81);
					     op(0xC0+vop +reg);
					     outword((unsigned int)number);
					     break;
			   case tk_postnumber:	op(0x81);
						op(0xC0+vop +reg);
						setwordpost();
						outword((unsigned int)number);
						break;
			   case tk_reg:	 op(0x01+vop);
					 op(0xC0 +reg +(unsigned int)number*8);
					 break;
			   case tk_longvar:
			   case tk_dwordvar:
			   case tk_intvar:
			   case tk_wordvar:  outseg(segm,post,2);
					     op(0x03+vop);
					     op(reg*8+rm);
					     outaddress(rm,(unsigned int)number);
					     break;
			   case tk_charvar:
			   case tk_beg:
			   case tk_bytevar:  regbyteerror();  break;
			   case tk_ID:
			   case tk_id:
			   case tk_proc:
			   case tk_regproc:
			   case tk_undefregproc:
			   case tk_undefproc:  regcallerror();	break;
			   case tk_seg:
			   case tk_rmnumber:  regmatherror();  break;
			   default:  valueexpected();  break;
			   }
		       break;
	case tk_xorminus:  vop += 0x08;
			   vop += 0x08;
	case tk_andminus:  vop += 0x18;
	case tk_orminus:   vop += 0x08;
			   nexttok();
			   if(tok == tk_number)
			       {number = -number;
			       op(0x81);
			       op(0xC0+vop +reg);
			       outword((unsigned int)number);}
			   else negregerror();
			   break;
	case tk_multminus:
	case tk_divminus:
	case tk_modminus:
	case tk_mult:
	case tk_div:
	case tk_mod:
	case tk_rrminus:
	case tk_llminus:
	case tk_ll:
	case tk_rr:  regmathoperror();	break;
	default:  operatorexpected();  break;
	}
    nexttok();
    }
}


dobegmath (beg)  /* math done is on all begs except AL */
int beg;        /* all other registers preserved */
{int vop,next;
while(tok!=tk_semicolon &&tok!=tk_camma &&tok!=tk_closebracket &&tok!=tk_eof)
	{next=1;
	vop=0;
	switch(tok)
		{case tk_xor:  vop += 0x08;
		case tk_minus: vop += 0x08;
		case tk_and:   vop += 0x18;
		case tk_or:    vop += 0x08;
		case tk_plus:    nexttok();
						 switch(tok)
							 {case tk_number:  op(0x80);
											   op(0xC0+vop +beg);
											   op((unsigned int)number);
											   break;
							 case tk_beg:  op(0x00+vop);
										   op(0xC0 +beg +(unsigned int)number*8);
										   break;
							 case tk_longvar:
							 case tk_dwordvar:
							 case tk_bytevar:
							 case tk_charvar:
							 case tk_intvar:
							 case tk_wordvar:  outseg(segm,post,2);
											   op(0x02+vop);
											   op(beg*8 +rm);
											   outaddress(rm,(unsigned int)number);
											   break;
							 case tk_postnumber:
							 case tk_reg:
							 case tk_rmnumber:  begworderror();  break;
							 case tk_ID:
							 case tk_id:
							 case tk_proc:
							 case tk_regproc:
							 case tk_undefregproc:
							 case tk_undefproc:  begcallerror();  break;
							 default:  valueexpected();  break;
							 }
						  break;
		case tk_xorminus:  vop += 0x08;
						   vop += 0x08;
		case tk_andminus:  vop += 0x18;
		case tk_orminus:   vop += 0x08;
						   nexttok();
						   if(tok == tk_number)
							   {number = -number;
							   op(0x80);
							   op(0xC0+vop +beg);
							   op((unsigned int)number);}
						   else negregerror();
						   break;
		case tk_multminus:
		case tk_divminus:
		case tk_modminus:
		case tk_mult:
		case tk_div:
		case tk_mod:
		case tk_rrminus:
		case tk_llminus:
		case tk_ll:
		case tk_rr:  begmathoperror();  break;
		default:  operatorexpected();  break;
		}
	nexttok();
	}
}


/* ============= getintoreg32(), getintoreg(), getintobeg() ============ */


getintoreg32 (reg32)	/* get into 32 bit reg (except EAX) with enum */
int reg32;
{int negflag=0,next=1,getfrom16bit=0;
long holdnumber;

if(tok==tk_minus)
    {nexttok();
    if(tok==tk_number)
	number = -number;
    else negflag = 1;
    }
switch(tok)
    {case tk_number:  holdnumber = doconstlongmath();
		      if( holdnumber == 0 )
			  {outword(0x3166);  /* XOR reg32,reg32 */
			  op(0xC0+reg32*9);}
		      else{op(0x66); op(0xB8+reg32);  /* MOV reg32,# */
			  outdword(holdnumber);}
		      next = 0;
		      break;
    case tk_rmnumber:  op(0x8D);    /* LEA reg,[rm] */
		       op(reg32*8+rm);
		       if( post != 0 )
			   setwordpost();
		       outaddress(rm,(unsigned int)number);
		       op(0xC0+reg32*8+reg32);	/* MOVZE reg32,reg */
		       break;
    case tk_postnumber:  op(0x66);
			 op(0xB8+reg32);
			 setdwordpost();
			 outdword(number);
			 break;
    case tk_longvar:
    case tk_dwordvar:  outseg(segm,post,3);
		       outword(0x8B66);
		       op(reg32*8+rm);
		       outaddress(rm,(unsigned int)number);
		       break;
    case tk_intvar:
    case tk_wordvar:  /* MOVZE reg32,[wordvar] */
		      outseg(segm,post,4);
		      outword(0x0F66);
		      op(0xB7);  op(reg32*8+rm);
		      outaddress(rm,(unsigned int)number);
		      break;
    case tk_bytevar:
    case tk_charvar:  /* MOVZE reg32,[bytevar] */
		      outseg(segm,post,4);
		      outword(0x0F66);
		      op(0xB6);  op(reg32*8+rm);
		      outaddress(rm,(unsigned int)number);
		      break;
    case tk_reg32:  if(reg32 != (unsigned int)number)
			{outword(0x8966);
			op(0xC0+reg32+(unsigned int)number*8);}
		    break;
    case tk_reg:  /* MOVZE reg32,reg */
		  outword(0x0F66);  op(0xB7);
		  op(0xC0+reg32*8+(unsigned int)number);
		  break;
    case tk_beg:  /* MOVZE reg32,beg */
		  outword(0x0F66);  op(0xB6);
		  op(0xC0+reg32*8+(unsigned int)number);
		  break;
    case tk_seg:  outword(0x8C66);  /* MOV reg32,seg */
		  op(0xC0+reg32 +(unsigned int)number*8);
		  break;
    case tk_ID:
    case tk_id:
    case tk_proc:
    case tk_regproc:
    case tk_undefproc:
    case tk_undefregproc:  regcallerror();
			   break;
    default:  valueexpected();	break;
    }
if(negflag)
    {outword(0xF766);  /* NEG reg32 */
    op(0xD8+reg32);
    }
if( cpu < 3 )
    cpu = 3;
if(next)
    nexttok();
}


getintoreg (reg)	/* get into word reg (except AX) with enum */
int reg;
{int negflag=0,next=1;
unsigned int holdnumber;

if(tok==tk_minus)
    {nexttok();
    if(tok==tk_number)
	number = -number;
    else negflag = 1;
    }
switch(tok)
    {case tk_number:  holdnumber = doconstlongmath();
		      if( holdnumber == 0 )
			  {op(0x31); op(0xC0+reg*9);}  /* XOR reg,reg */
		      else{op(0xB8+reg);	/* MOV reg,# */
			  outword(holdnumber);}
		      next = 0;
		      break;
    case tk_rmnumber:  op(0x8D);	    /* LEA reg,[rm] */
		       op(reg*8+rm);
		       if( post != 0 )
			   setwordpost();
		       outaddress(rm,(unsigned int)number);
		       break;
    case tk_postnumber:	 op(0xB8+reg);
			 setwordpost();
			 outword((unsigned int)number);
			 break;
    case tk_longvar:
    case tk_dwordvar:
    case tk_intvar:
    case tk_wordvar:  outseg(segm,post,2);
		      op(0x8B);
		      op(reg*8+rm);
		      outaddress(rm,(unsigned int)number);
		      break;
    case tk_reg32:
    case tk_reg:  if(reg != (unsigned int)number)
		      {op(0x89);
		      op(0xC0+reg +(unsigned int)number*8);}
		  break;
    case tk_seg:  op(0x8C);
		  op(0xC0+reg +(unsigned int)number*8);
		  break;
    case tk_bytevar:
    case tk_charvar:  if(reg > BX)
			  regbyteerror();
		      /* notyet done >> add MOVZE stuff if chip > 3 */
		      else{if( reg==AX && rm == rm_d16 )
			      {outseg(segm,post,1);
			      op(0xA0);	/* MOV AL,[byte] */
			      outword((unsigned int)number);}
			  else{outseg(segm,post,2);
			      op(0x8A);
			      op(reg*8 +rm); /* MOV regL,[byte] */
			      outaddress(rm,(unsigned int)number);}
			  op(0x30);  op(0xC0+(reg+4)*9);} /* XOR regH,regH */
		      break;
    case tk_beg:  if( reg > BX)
		      regbyteerror();
		  else{op(0x88); op(0xC0+reg+(unsigned int)number*8); /* MOV regL,beg */
		      op(0x30);	op(0xC0+(reg+4)*9);}	/* XOR regH,regH */
		  break;
    case tk_ID:
    case tk_id:
    case tk_proc:
    case tk_regproc:
    case tk_undefproc:
    case tk_undefregproc:	regcallerror();	break;
    default:  valueexpected();	break;
    }
if(negflag)
    {op(0xF7);	/* NEG reg */
    op(0xD8+reg);
    }
if(next)
    nexttok();
}


getintobeg (beg)	/* get into beg (CL,DL,BL not others) with enum */
int beg;
{int negflag=0;

if(tok==tk_minus)
    {nexttok();
    if(tok==tk_number)
	number = -number;
    else negflag = 1;
    }
switch(tok)
    {case tk_number:  op(0xB0+beg);
		      op((int)doconstlongmath());
		      break;
	case tk_rmnumber:  op(0x8D);        /* LEA reg,[rm] */
					   op(beg*8+rm);
					   if( post != 0 )
						   setwordpost();
					   outaddress(rm,(unsigned int)number);
					   break;
	case tk_postnumber:  op(0xB8+beg);
						 setwordpost();
						 outword((unsigned int)number);
						 nexttok();
						 break;
	case tk_longvar:
	case tk_dwordvar:
	case tk_charvar:
	case tk_bytevar:
	case tk_intvar:
	case tk_wordvar:  outseg(segm,post,2);
					  op(0x8A);
					  op(beg*8+rm);
					  outaddress(rm,(unsigned int)number);
					  nexttok();
					  break;
	case tk_beg:  if(beg!=(unsigned int)number)
					  {op(0x88);
					  op(0xC0 +beg +(unsigned int)number*8);}
				  nexttok();
				  break;
	case tk_reg:  if(beg < BX)
					  {op(0x89);
					  op(0xC0+beg +(unsigned int)number*8);}
				  else begworderror();
				  nexttok();
				  break;
	case tk_seg:  op(0x8C);
				  op(0xC0+beg +(unsigned int)number*8);
				  nexttok();
				  break;
	case tk_ID:
	case tk_id:
	case tk_proc:
	case tk_regproc:
	case tk_undefproc:
	case tk_undefregproc:  regcallerror();  nexttok(); return(-1);  break;
	default:  valueexpected();  nexttok();  return(-1);  break;
	}
if(negflag)
	{op(0xF6);
	op(0xD8+beg);   /* NEG beg */
	}
return(0);
}


/* end of TOKB.C */
