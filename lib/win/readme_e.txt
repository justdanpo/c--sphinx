// ------------------------------------------------------------------- //
//                        Libraries Win32 API                          //
//                       for Sphinx C-- 0.229+                         //
// ------------------------------------------------------------------- //

From what ALL BEGAN

... By undertaking for programming on C-- under Windows, I have come across
absence of libraries satisfying to my requests. Here and it was necessary
before of writing of the programs to undertake development of these
libraries.
    This package was written because of of analogs, taken from Borland Delphi
and C++ Builder. From source codes were eliminated little use in practice of
a structure and constant. Quite probably, in the consequent versions of
library will be extended and are supplemented.
    Despite of a significant diminution of volume of an initial code on a
comparison with analogs C++, the libraries are still too great for the
version of the compiler C-under DOS. For example, on compilation WINUSER.H--
any more there is no memory. However, it can easily be bypassed, by taking
advantage the version of the compiler C--32 under a cantilever mode Windows.
I do not see in it of a large problem: time you have undertaken to program
under Windows - use the compiler under Windows.

    The libraries Win32 API, realized for C++ Builder and C-- 0.229, are
connected as follows: constants at first are declared, and then one file is
connected:
    #define _WINBASE_ TRUE
    #define _WINGDI_ TRUE
    #include "windows.h--"
I like a method of connection of several files more:
    #include "winbase.h--"
    #include "wingdi.h--"
    #include "winuser.h--"
The former method is still supported and will be supported. The WINDOWS.H--
file exists just for assuring compatibility with this method.

// ------------------------------------------------------------------- //

THAT IS ALREADY MADE

The given realization includes the following files:
ADVAPI   H-- Work with the register
CCANIM   H-- Service functions for animation
CCHEADER H-- Service functions for separators
CCLIST   H-- Service functions for the lists
CCPRSHT  H-- Service functions for multipage boards
CCTAB    H-- Service functions for bookmarks
CCTREE   H-- Service functions for trees
COMMCTRL H-- Lists, trees, прогрессы, bookmark, status bar...
COMMDLG  H-- Dialogues of discovery of the file, preservation, printing,
             search.
LZEXPAND H-- Unpacking of files, wrapped up COMPRESS.EXE
SHELLAPI H-- Work with трэйем, drag and drop, documents...
STRING32 H-- Classical functions of work with lines under Win32
WINBASE  H-- Work with memory, files, DLL, console, lines...
WINDEF   H-- Main declarations of types, structures and макросов
WINDOWS  H-- Is for compatibility at a level of source codes
WINERROR H-- Codes of errors Windows
WINGDI   H-- Work with contexts of systems(devices), graphics and fonts
WINUSER  H-- Event, messages, resources(safe lifes) and other interface
README32 TXT This file
STRING32 TXT the Description of functions STRING32.H--

    Now is a little bit more detailed about all these files. At once I shall
tell, that at me is not present slightest desire to describe ALL or even some
functions Win32 API. For the sake of study of the mechanism of programming
under Windows I can only advise to read a pair of the good books, and as
reference "on-line" of a manual - well-known MS SDK Help.

    WINDEF.H--
    Contains the descriptions of main data types, constants and structures.
Some macros also are circumscribed. You hardly need to connect this file to
the program obviously, as it will be connected for want of use WINDOWS.H-- or
WINBASE.H--.

    WINDOWS.H--
    Is intended for compatibility with a "classical" method of connection of
libraries. Using this file, you should up to its connection declare
appropriate constants.

    WINBASE.H--
    Is for import of functions from KERNEL32.DLL, also contains the
descriptions of constants and structures necessary for work with these
functions. In this and in all remaining libraries the preference is given up
to functions and structures for work with ANSI-lines, the UNICODE-functions
are not realized by virtue of their extremely rare application.
    The functions of a Windows kernel are used for a memory allocation and
other system resources, management of processes and streams, work with files,
and ASCIIZ-lines, loading DLL, cantilever input-output... Perhaps, any
program under Windows will use functions of this library.

    WINGDI.H--
    Imports functions from GDI32.DLL, destination for work with such graphic
objects, as contexts of devices (DC), fonts (Font), paintbrushes (Brush),
pens (Pen), figures (Bitmap, Metafile), and also graphic objects. The
functions of the interface of graphic devices (GDI) will be useful first of
all to the one who attends to creation of own interface elements of the
window, and also founders graphic demo.

    WINUSER.H--
    Will realize functions USER32.DLL, which are used for work with windows,
buttons, menu, text, icons, fields of input and other main elements of dialog
boxes. Also contains the descriptions of functions, structures and constants
for work with events and messages, there are functions for transformation and
formatting of ASCIIZ-lines. The programs using the graphic user interface
(GUI), necessarily should use this library.  Three above named DLL - KERNEL,
GDI, USER - make basis as well as most operational system Windows, and any
program under it.

    WINERROR.H--
    Contains the descriptions of constants - codes of errors Windows. The
library can be useful at the stage of a program debugging.

    COMMCTRL.H--
    Contains imported from COMCTL32.DLL of function, and also description of
structures and constants for the messages.  The animation (Animation),
various lists is used for creation of such interface elements, as ( Drag
List, Image List, List View, Tree View), bookmark (Tab Control), multipage
dialogues (Property Sheets), status bar and progress (Status Bar, Progress
Bar), (Toolbar) toolbar, movable indicators (Trackbar), pop-pup of the helps
(Tooltip), titles and separators (Header Control). With the purposes of a
diminution of volume of a target code the auxiliary service functions
engaging in the sum about 4K of an object code, are born in files CC*.H--. If
you do not want to use them, you can send the appropriate messages by
API-function SendMessage.

    COMMDLG.H--
    Will realize the interface with COMDLG32.DLL, is used for work with
standard dialog boxes: discovery of the file, preservation of the file,
search and replacement of the text, choice of colour, choice of the font,
preparation for listing.

    ADVAPI.H--
    Is for import of functions from ADVAPI32.DLL, which are applied to work
with the system register both modes of safety and access (ACL, ACE, SID). If
somebody knows, what favour can be extracted from structures of a type ACL
and SID - write.

    SHELLAPI.H--
    Imports functions from SHELL32.DLL - extension an envelope. You can use
this library for work with protocol Drag and Drop, tram of a board of
problems, extraction of icons from the programs and libraries, discovery of
the documents and appendices, working with them, and much other.

    LZEXPAND.H--
    Will realize functions LZ32.DLL - library of unpacking of datas. Allows
to unpack datas packed by the program COMPRESS. You can use this library, by
creating the file with a graphics and - or by music for demo and then by
packing it by the program COMPRESS.

    STRING32.H--
    Contains realization classical for C of functions of processing of
ASCIIZ-lines and simply of lines under Win32. Certainly, you can quite do
without by use appropriate WinAPI of functions, but if for you the velocity
has critical significance - these functions for you. For the detail
description of functions look the file STRING32.TXT.

CCANIM.H--   _CCANIM _
CCHEADER.H-- _CCHEADER_
CCLIST.H--   _CCLIST_
CCPRSHT.H--  _CCPRSHT_
CCTAB.H--    _CCTAB_
CCTREE.H--   _CCTREE_
    Contains the description of service functions for the following interface
elements accordingly: animation (Animation Control), separators (Header
Control), lists (List View), multipage dialogue boards (Property Sheet),
bookmark (Tab Control) and trees (Tree View). By virtue of impossibility of
call of API-functions of dynamic procedures all service functions should be
made static, and for a diminution of volume of an object code to bear them in
"own" libraries.

    COMPRESS.EXE
    The program of compression of datas from Microsoft. Uses algorithm LZ.
The serious restriction - in one archive can be only one file (catalogue is
not present). Can be applied to compression of the file with datas for
consequent unpacking by functions of library LZEXPAND.H--.

// ------------------------------------------------------------------- //

WHAT STILL IT WOULD BE DESIRABLE TO MAKE

    By unique restriction of development of libraries of a possibility of the
C-- compiler. Many things under Windows (like indirect calls or work with
structures under the reference) are rather complicated for organizing without
a normal model of the pointers. The development of libraries will be
conducted in parallel to development of the compiler.

    In the consequent versions of libraries the support is planned:
1) WinSocket and WinInet
2) DirectX (in particular, DirectDraw and DirectInput)
3) Interfaces of objects an envelope (Shell Library Object)

// ------------------------------------------------------------------- //

HOW TO CONTACT to the AUTHOR

You can write to me on E-Mail or in FIDO:

E-Mail: hd_keeper@mail.ru
FIDO: 2:5024/22.13@fidonet

    Send me your offers, note, and especially messages on detected errors. I
should know, in what direction to work further. You can freely modify
libraries, if the introduced modifications can, under your judgement, to
reduce in their improving. Only do not overlook then to inform me on the most
successful updatings. I do not promise, that I shall answer all letters, but
necessarily I shall read and I shall take part to an information.

Have a nice coding!
Anton Kopeev AKA Keeper





