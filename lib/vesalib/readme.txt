VesaLibrary 1.0 Copyright Mikael Nilsson (C) 1999.  All rights reserved.
------------------------------------------------------------------------

Welcome to VesaLibrary 1.0 by PurpleProphet

This is a library of graphic procedures in C-- for VESA compatible graphic
cards with minimum 1MB memory.

It supports modes up to 1024x768 with 256 colors but uses only
one set of procedures for all resolutions.

VesaLibrary 1.0 is GREENWARE, a concept by Peter Cellik (author of C--)
and you are free to use it so long as you make an effort everyday to
help out the environment.  A few ideas:
	- use only recycled computer paper
	- be sure to recycle the computer paper after you use it
	- use public transport
	- sell that 80 cylinder car of yours and buy a small 4 cylinder, or
	  better yet, buy a motorbike
	- support Green Peace
	- REDUCE-REUSE-recycle
	- stop smoking
	- ride a bike to work or school
	- don't buy products that are harmful to the environment
	- stop using weed killers on your lawn
	- support Friends of the Earth
	- recycle your cans
	- don't buy products that have lots of extra packaging
	- use a fax modem instead of a paper fax machine
	- reuse your plastic bags
	- (you get the idea)


DISCLAIMER:
~~~~~~~~~~~
I accept no responsibility for any damage or loss of time, hardware, sanity, 
software or data caused by this product or programs made with it.


Bug Reports:
  Of course is this code not bugfree so if you discover a bug, please
  report it to me (email address below)

Happy Programming!

Mikael Nilsson
Uppsala, SWEDEN
Email: purpleprophet@telia.com


VesaLibrary 1.0 mini-FAQ
------------------------

Q: How do i clear the screen?.
A: Use barVESA(0,0,screenwidth-1,screenheigth-1,0);

Q: How do i use my own palette in VESAmode?
A: Use the ordinary C-- VGA palette procedures.

Q: Why include all procedures in the compiled file instead of using
   dynamic procedures?
A: Because nearly all procedures uses other procedures and C-- does not
   allow nested dynamic procedures. Its a pain! Any solutions?

Q: How can it be only one set of procedures for all graphic modes?
A: Because the screen scanline length is always set to 1024 pixels
   regardless current resolution.

Q: Why must the graphic card have 1MB internal memory?
A: It takes 768KB to set the screen to 1024x768 resolution but if you only
   use 640x480 it should work on 512KB card(i have not tested that yet!) 

Q: Has the VesaLibrary 1.0 procedures clipping against the screen(1024x768)?
A: No, only trigonfillVESA ,pixelclipVESA and xorpixelclipVESA has clipping.
   But, for an example, take circleVESA() and replace all putpixelVESA with
   pixelclipVESA() (use the same values) and you have a circle with clipping.

Q: Why is your english so bad?
A: I am from Sweden ;-) 

FILES INCLUDED:
---------------
README.TXT -You are reading it!
ALLPROC.TXT -Information about VESAlibrary routines
DEMO.C-- -A demo with some routines
DEMO.H-- -The H-- file to DEMO.C--
XORDEMO.C-- -A demo with xor routines
XORDEMO.H-- -The H-- file to XORDEMO.C--
TRIDEMO.C-- -A demo of filled trigons
TRIDEMO.H-- -The H-- file to TRIDEMO.C--
VESAINFO.C-- -A program for checking your VESA cards info
PROPHET.PCX -An bitmap used in DEMO.C--
BRAIN.PCX -An bitmap used in DEMO.C--
FONTS\ -Directory with font files
INCLUDE\ -Directory with VESAlibrary include files