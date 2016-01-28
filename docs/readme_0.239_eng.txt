Attention!
~~~~~~~~~~
    Corrected error in compiler in comparisons for procedure returning
flag. Earlier the opposite code of comparison was generated. For example:
void CARRYFLAG TEST();	//declaration procedure
...
{
  IF ( TEST() ) AX = 1 ;  //check carry flag returned procedure TEST
}
   Presently, compiler generates such code:
     call    103h
     jnc     10Ch
     mov     ax,1
   Earlier, there was so:
     call    103h
     jc      10Ch
     mov     ax,1


Type-out warning as errors.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    By means of options of command line /we=#nn or directive #pragma option
we=#nn possible to indicate the compiler, what warning to type-out as errors.
For this moment possible thereby to change the status by 15 types of warning.
I.e. #nn can take numeric values from 1 to 15.  Output these 15 types of
warning possible to disable the option /nw=#nn.  Here is list of warning,
which possible or disable, or type-out as errors:

1 -  Optimize numerical expressions
2 -  'RegName' has been used by compiler
3 -  Short operator 'if | else | ...' may be used
4 -  String "..." repeated
5 -  Expansion variable
6 -  Signed value returned
7 -  'fun.name' defined above, therefore skipped
8 -  Variable | structure | ... 'name' possible not used
9 -  Non-initialized variable 'name' may have been used
10 - Return flag was destroyed
11 - Code may not be executable
12 - Don't use local/parametric values in inline functions
13 - Sources size exceed destination size
14 - Pragma pack pop with no matching pack push
15 - Missing parameter in function 'name'


Register for calculation an index.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    At calculation of subscripted expressions a compiler by default can use
the register: (E)SI, (E)DI, (E)BX, (E)DX.  These register is specified in
order a priority a choice.  More often the first register is used.  If
calculation an index comes short register, that compiler uses following.

    List of these registers possible to change the directive #pragma
indexregs.  As parameter to this directive to is necessary indicate the list
of registers prepared by space or comma.  It is necessary to indicate
before four registers. Example:

#pragma indexregs ECX EDX

    Action of this directive if it is specified outside of functions, spreads
on all following functions.  If directive is specified inwardly functions,
that its action spreads on this function only.


Location local variable in stack.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Now local variable are situated in stack in that order, in what they were
declared - most first variable inheres in top chosen for local variable
fragment of stack, following are situated below it.  Earlier the last inhered
in top declared local variable.

    If in functions for local variable stands out less 128 bytes, that such
change the distribution variable at all is not reflected on size of generated
code.  But if size local variable exceeds 128 bytes then for reception of
more compact code, most often used variable to is necessary declare first,
but greater array, structure and seldom used variable better to declare the
lasts.


Replace variable on constant.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Use a constant instead of variable raises the velocity of functioning the
program and in some cases shortens the size program.

    Compiler traces initialization variable constants and, when use such
variable, compiler replaces address to him on this constant (if this variable
was not changed).

    This method of optimization is enabled by default.  For disable its
necessary use the key of command line /orv-.


Directives #if #elif.
~~~~~~~~~~~~~~~~~~~~~~

    Directive #if in C-- similar directive #ifdef.  After directive #if
possible to use the operation defined or !defined.

    Directive #elif can be situated only between directives #if/#ifdef and
#endif.  Number of directives #elif can be any.

    After these directives follow or identifier, or expression of comparison.
Identifier and expressions of comparison can unite in complex expression by
operators || and &&.  But the priority of brackets is not supported in C--
not.  Checking the identifiers and expressions of comparison is produced
consecutively, since they recorded.


Directive #pragma pack.
~~~~~~~~~~~~~~~~~~~~~~~~

    Directive #pragma pack defines aligning in tags a structure.  By default
in compiler this aligning is 1.  Change value of aligning by by default
possible option of command line /as=n, n can take values 1, 2, 4, 8...

    Directive #pragma pack supports following syntax:

  #pragma pack(n)
    Set new value of aligning.

  #pragma pack(push)
    Remembers current value of aligning in stack.

  #pragma pack(push,n)
    Remembers current value of aligning in stack and set new.

  #pragma pack(push,id)
    Remembers current value of aligning in stack and assigns him name id.

  #pragma pack(push,id,n)
    Remembers current value of aligning in stack, assigns him name id and
set new value of aligning n.

  #pragma pack(pop)
    Extracts from stack value of aligning and does its current.

  #pragma pack(pop,n)
    Extracts from stack value of aligning and set new value of aligning n.

  #pragma pack(pop,id)
    Extracts from stack rememberred earlier values of aligning to that, time
while his name will not be equal id.  Will then is installed value of
aligning corresponding to identifier id.  If in stack such identifier does
not turn out to be, that stack will is cleared and will be set value of
aligning by default.

  #pragma pack(pop,id,n)
    Extracts from stack rememberred earlier values of aligning to that, time
while his name will not be equal id.  If in stack such identifier does not
turn out to be, that stack will is cleared.  Will then is set new value of
aligning n.


Transfer a structure as parameters of function.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Some are API-functions receive a parameter contents a structure.  Show on
example as this possible makes:

//declare tag structure
struct TAG
{
  int var_a;
  int var_b;
};

//declare function receiving structure as parameter
void fun1 ( TAG str);

//define structure
TAG struct1;

//define function
void fun1 ( TAG str)
{
  EAX = str.var_a + str.var_b;
...
}

//do call to functions with transfer a structure as parameter
main()
{
  fun1(struct1);
...
}


Termwise addressing in array.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    When referencing to array data, in C-- if as index is used the numeric
constant or register, address is calculated disregarding type an array that
is to say it is produced byte-by-byte addressing.  If as index is used
variable or complex expression, that this value multiplies an array on
dimensionality that is to say termwise addressing is produced.  Such duality
brings to mess sometimes.

    Applicable key of command line /ba-, or having added in file c--.ini line
ba-, or through directive #pragma option ba- possible get only termwise
addressing.  Exceptions constitute the expressions of type DSDWORD[...],
DSINT[...]...  In these expressions an address will always be calculated
without multiplying on dimensionality an array.


'static' functions and variable in structure.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Support 'static' functions in structures/classes exists else with past
version a compiler.  Support 'static' variable is added in this version.

    'static' functions and variable this usual functions and variable
visibility (accessibility) which is limited by methods (functions) of class
(structure) in which they are declared.

    Memory for 'static' variable stands out immediately at announcement teg
structure (class).  'static' variable always remains in single copy,
regardless of numbers of createded objects and available all objects given
class.  'static' variable possible to initialize immediately at announcement
of class. Example:

struct info
{
  info (int num); // declare constructor of class info, receiving one
                  // parameter.
  int number;
  static int svar = 5; // announcement and initialization static variable svar
};

    Access to 'static' variable and functions from functions one and the same
the class is realized as to usual variable and functions. Example:

info::info (int num)
{
  IF ( this ) {
    svar = num ;
  }
  return this;
}

    For access to 'static' variable and functions from functions not being
methods of this class to is necessary set the name teg class, in which is
described this 'static' function or variable.  Example:

void main()
{
  EAX = info.svar;
}


New material types variables.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Type variable 'double' is actively used and in assembler and in C so in
detail about it shall not speak.

    Functions, having type a return 'double', leave the result in top of
stack FPU.  All are other types of return from functions on this uses the
register (AL, AX, EAX, EDX:EAX).  This allows disposes of, often not
necessary, loading material number in register.

    Other new type in compiler are a register FPU.

    Whole exists 8 registers FPU.  Marked they so: st(0), st(1), st(2)...
st(7).  Register are organized in stack, which top is a register st(0).  Over
he is produced loading and unloading material number.  This register possible
to mark as st.  In more detail about work and characteristics of registers
FPU, You may read in special literature.

    Stacked organization of registers FPU complicates use of these registers
and requires the attentive relations to him.  It is Necessary constantly to
keep a check on all modification a top of stack, which can change the number
a register or damage Your variable.

    Register FPU possible to use in calculations of expressions.  Example:

double d,d2;

  st(3) = st(2) + 1 * d;
  d2 = PI / st(3);

    Register FPU possible to use in announcements of functions as parameters
in combination with other types variables. Example:

void cdecl proc (int, st(3), st(4) );
int fastcall func (ECX, st(3), st(4) );

    Register st(0) possible use as return from function.  Herewith will be
generated such code, either as when use a return double.  Example:

st ctdcall func (dword, st(2));


64-bit variable.
~~~~~~~~~~~~~~~~~

    Use 64-bit variable on 32-bit a platform will bring trivial growing of
size of code of Your programs and reduction of their speed.  So use 64-bit
variable to is necessary only then, when this really necessary.

    In C-- are supported 64-bit unsign types variable - 'qword'.  For
compatibility with standard C is supported the indication of this type as
'long long', to numeric constants possible to add the suffix 'I64'.  To
64-bit to areas a memory possible to address with use the reserveded
identifiers:  DSQWORD, ESQWORD, SSQWORD, CSQWORD, FSQWORD, GSQWORD.  As a
matter of convenience work with 64-bit variable possible to merge 32-bit a
register in pairs. Example:

  EBX:ECX = DSQWORD[EAX] + 1;

    In this example a register EBX and ECX are used as united 64-bit
register.  The senior part 64-bit a value inheres in register EBX, but in
register ECX - low-order.

    Functions, having type a return 'qword', use for return 64-bit pair of
registers EDX:EAX.

    With 64-bit variable possible to execute such operations, either as from
variable other types.  For operations of multiplying and division a compiler
uses the functions residing in file 'math64.h--'.  If You use the operations
of multiplying or division on 64-bit variables, that You necessary will
include this file of Your project.


Operations of comparison with use the flags.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    In operations of comparison possible to use only that combinations of
flags, which allow to get one assembler instruction conditional.  All are an
available combinations of flags were provided in file 'flags.c--'.


Clearing the stack from parameters cdecl-functions.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Clearing the stack from parameters after calling the functions of type
cdecl, was earlier produced right after call to this functions.  Now this is
doed, as far as possible, more late.  This allows to accumulate the parameter
several cdecl-functions in stack and, afterwards clean the stack for one
acceptance.

    Return to old way a peelings of stack by means possible of options
command line /js-.

    Stack is cleared the assembler instruction 'add esp,sizeparam' and
herewith flag spoil.  Postponed clearing the stack is produced before
operators of comparison, but if this operator checks the flag, that program
will work not correct.  That this did not occur to is necessary do clearing
the stack before place, where are formed the flag.  For this enough before
revenge of shaping the flags to put the local label, which also causes
clearing the stack. Example:

  printf("Password: ";
  scanf("%s",#pword);
  ESI = #right;
  EDI = #pword;
  ECX = sizeof(right)-1;
LOCAL:        // before this label a compiler will unload stack
  $rep $cmpsb // These instructions install flag
              // Which can be throwed unless will be a labels
  IF (ZEROFLAG) printf("OK\n";
  ELSE printf("Incorrect Password!!!\n";


COFF object files.
~~~~~~~~~~~~~~~~~~~

    COFF format of object files is supported only for programs under Windows.
That compiler generated the object file in format COFF necessary to usual set
of options of command line, which required for making the object file
Windows, add the option /COFF.

    Object files a format COFF possible to attach of its project by directive
#include. Example:

#include "filename.obj"

    This file must have the extension *.obj.  Attach the object file possible
only to project for Windows.  For functions object file, which You want to
use, advisable do the announcement.


Directive #define.
~~~~~~~~~~~~~~~~~~~

    By means of directives #define in C-- possible was define: single lexeme,
numeric expression (which is always calculated in numeric constant) or string
constant.  Now possibility of this directive extended.

    Expression determined through #define, possible redefy:

#define WORD word
#define VERSION 1
 . . .
#define WORD dword
#define VERSION 2

    Possible define the empty identifier:

#define WORD
#define VERSION

    For identifier definied through #define possible to assign the argument:

#define SQR( v )         v * v
#define AREA( r , a )    2 * SQR ( r ) * a

 main ( )
{
  AX = AREA ( BX , CX ) ;
}

    Compiler will generate such code:

test.c-- 6: AX = AREA ( BX , CX ) ;
0100 B80200                   mov     ax,2
0103 F7E3                     mul     bx
0105 F7E3                     mul     bx
0107 F7E1                     mul     cx
0109 C3                       ret

    For #define with arguments there is obligatory conditions in its syntax:
    1 - openning bracket with list of arguments '(' must go immediately after
identifier without spaces.
    2 - rigth part #define begins after symbol closing bracket ')' and ends
the symbol a turning to new line (the code 13).

    For long expressions, occupying several strings, possible use the symbol
'\' as symbol of continuation of string.

    If You necessary to define the identifier, consisting of several lexemes,
but without parameters, that necessary without fall after name an identifier
to put the pair of parenthesis:

int i,j;
#define ADDVAR( )    i + j

    If You will write this #define without brackets, as this is doed in
languages C, that compiler will perceive only one lexeme, but remained lexeme
will cause the report on error.  Use such identifier possible both with
brackets, and without them:

  AX = ADDVAR ( ) ;
  BX = ADDVAR + 1 ;

    Changes to directive #define have required to contribute some changes to
directives #undef and #ifdef/ifndef.  Also, there were is removed from list
of reserveded words:  FALSE, TRUE, __SECOND__, __MINUTE__, __HOUR__, __DAY__,
__MONTH__, __YEAR__, __WEEKDAY__, __VER1__, __VER2__", __COMPILER__,
__DATESTR__, __DATE__, __TIME__.  All these constant now are automatically
declared the compiler through #define.  You may their change or delete.


Directive #undef.
~~~~~~~~~~~~~~~~~~

    Directive #undef possible to delete only identifier declared through
#define.


Directives #ifdef/ifndef.
~~~~~~~~~~~~~~~~~~~~~~~~~~

    If value checked identifier is a zero, that result of using hereto
identifier of directives #ifdef/ifndef will accordingly TRUE/FALSE.  Earlier,
there was FALSE/TRUE.


Directive #inline.
~~~~~~~~~~~~~~~~~~~

    Directive #inline received two values - TRUE and FALSE, which defined the
behaviour of dynamic functions, declared as inline.  Under TRUE these
functions were inserted in code, but under FALSE was doed call to functions.
If directive #inline was not used, that these functions at optimization on
velocity were inserted in code, but at optimization on size was doed call.

    Now the third condition appeared beside directives #inline - AUTO.  This
mode is by default.  Herewith, functions behave, is since waked directive
#inline was not used earlier i.e. at optimization on velocity of functions
will be inserted, but at optimization on size will be doed their call.


Unicode strings.
~~~~~~~~~~~~~~~~~

    That string was saved in program in format unicode, before this string
must stand the prefix symbol 'L'. Example:

char string = L"This unicode string";

    This are limited support unicode strings by compiler.  Manipulates with
such strings (copying, comparison, output on screen...) must be doed the
library functions, which You else there is write.


New possibilities 'sizeof'.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Four new operands appeared for sizeof: __CODESIZE, __DATASIZE,
__POSTSIZE, __STACKSIZE, which accordingly return the sizes of code,
initialized data, uninitialized data and stack of compilable program.  For
these four constant available operation adding with each other and with
numeric constants.  Result of these operations will is saved in program in
the manner of one numeric value. Example:

  sizeCOMfile = sizeof(__CODESIZE) + sizeof(__DATASIZE);

    Operand 'sizeof' now possible not to bracket:

int sizeCOMfile = sizeof __CODESIZE + sizeof __DATASIZE;

    Attention: constant '__sizepostarea' more compiler is not supported. If
You its used in their own programs, that its necessary to replace on
expression 'sizeof (__POSTSIZE)'.


New syntax for local labels.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Earlier, local labels differred from global that that they were writed
only by greater letters.  Now local labels possible to write any letters.
But to label that given label is local, before its name to is necessary
indicate the prefix - a symbol '@'.  This prefix must be only before name the
most labels.  Use the name such local label necessary without this prefix.
Example:

void proc()
{
  . . .
  goto local_label;
  . . .
@ local_label:
  . . .
}


Keyword 'uses'.
~~~~~~~~~~~~~~~~

    Keyword 'uses' is used at announcements and determinations of functions
and will send the compiler a list of registers, which will be used this
function.  Depending on that, is used 'uses' in announcement of functions or
under its determination, it carries the function different sense.

    At announcement of functions a keyword 'uses' will send the compiler a
list of registers, which this function will destroy.  This list together with
'uses' must be inclosed in parentheses, and be situated between list of
parameters to functions and completing ';'. Here is example:

void proc (int param) (uses EAX,ECX);

    Register possible to separate or comma, or space.  If Your function
destroys all register then instead of list of these registers possible to use
other keyword - 'uses allregs'.

    By default if for function does not exist any announcements or is not
specified list of destroyed registers, is taken that function destroys all
register.  Exception form the functions Windows API.  For them is accepted
that they destroy all register except EBX, EDI, ESI.

    At determination of functions a keyword 'uses' will send the list of
registers, which will are saved in stack, but at termination of functions
will are restored.  This list must be situated right after name of function
with list of parameters.  Contain this list in parentheses unnecessary.  Name
of registers are separated spaces or coma To save all register, possible use
the keyword 'allregs'.  For conservation and recovering of all registers a
compiler will use the assembler instructions PUSHA(D)/POPA(D).  Example:

void proc (int param)
uses EBX ECX
int localvar;
{
int localvar2;
  EBX=param;
  . . .
}

    If You terminate the function by assembler instruction 'ret', that these
register will not are restored from stack and Your program will work wrong.


Use of local variable other functions.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Use of local variable other functions possible in inline-functions only
(inline-functions this that, for which is not formed its stacked frame and is
not generated the code of termination of function).

    That referencing to local variable other functions was correct,
inline-functions must have the announcement local variable and parameters of
function, exactly such, either as in functions local variable which will are
used. Example:

void proc (int param)
uses EBX ECX
int localvar;
{
int localvar2;
  EBX=param;
  proc2();
  . . .
}

void proc2 (int param)
inline
int localvar;
{
int localvar2;
  localvar=param;
  $ret
}

    For such inline-functions it is impossible use the keyword 'uses', at
announcement local change to initialize them.  Terminated such functions must
assembler instruction 'ret'.


Directive #print.
~~~~~~~~~~~~~~~~~~

    Directive #print if for she inheres the word 'error', after output on
screen of information message terminates functioning a compiler with code of
error 10.  Directive #print has the following format:

  #print [error] string or number


Directives #ifdef #ifndef.
~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Directives #ifdef and #ifndef support operations of comparison of two
constants. Example:

  #define VERSION 1
  . . .
  #ifdef VERSION < 2
  . . .
  #else
  . . .
  #endif

    The left part of expression of comparison can be compound constant
expression. The right part can be only a constant.


New types of resources.
~~~~~~~~~~~~~~~~~~~~~~~~

    In compiler exists the possibility to use new, different from standard,
types of resources. For this in file of resources You necessary to add the
line following format:

  id_name new_type "filename"

  id_name    - an identifier a resource, can take numerical or text value.
  new_type   - numerical value of new type of resources. Standard types of
	       resources, supported by compiler, have values from 1 to 22.
  "filename" - a filename, in which inheres information, definied this
               resource.

    Example of use, a resource definied by user, possible peer into file
control.zip.


Obj-file for programs under Windows.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Using combinations option /obj and /w32 you get object file, which
afterwards, using link, possible join with other object files or libraries in
program for Windows.

    Link by default produce the section of code with attributes only read.
Compiler C-- generates code and initialized variables in one block and so for
it necessary access for writing. So You necessary will endeavour to get the
section of code with attributes a read-write. If You for link use tlink32.exe
then for this You necessary will create file with extension *.def, which must
without fall contain strings:

SECTIONS
    .text READ WRITE

    Name this def-file afterwards is necessary will reference in command line
tlink32:

tlink32 *.obj [*.lib],,,,*.def


New possibilities when work with classes.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Allocate memory under class by now possible operator 'new', which
received address of block a memory beside systems, will send its constructor
of this class. If for given class does not exist constructor, that address of
chosenned block memory returns from operator 'new'.

    Constructor this special procedure for given class, which can initialize
class or conduct other starting-up actions for further functioning the class.
Constructor possible to send required for its work a parameter. For
constructor exists several limits:

 - a constructor has the name such, either as name of class.
 - for constructor it is impossible indicate the type a return and modifier.
 - since operator 'new' in C-- does not check, was is chosen memory operating
   system then in constructor advisable to do this check.
 - be terminated the constructor without fall must return variable 'this'.

    Here is example syntax of class with use a constructor:

struct info
{
  info (int num); // declare constructor of class info, receiving one
                  // parameter.
  int number;
};

info::info (int num)
{
  IF ( this ) {
    number = num ;
  }
  return this;
}

 Operator 'new' has following syntax:

  var = new info ( 5 );

    Constructor is called automatically by operator 'new'. If You allocate
memory for class in stack or reserve it in program, that use the operator
'new' there is no need to. In this case You may call the constructor as usual
procedure from class:

void proc()
{
info s1; //allocate memory for class in stack
  s1.info ( 5 ); //call constructor
 ...
}

    If your class inherits other classes with its constructors, that
initialize these constructor possible either as in C++, having indicated list
inheritted constructors via camma at determination a constructor. In change
from C++ this list without fall must terminate semicolon ';'.

info::info (int num)
: info2 (int num);
{
  IF ( this ) {
    number = num ;
  }
  return this;
}

    But possible initialize inheritted constructor by call them from basic
constructor.

    To free memory chosen by operator 'new' is incorporated operator
'delete', which first calls destructor, but then frees block a memory. If for
given class no destructor, that operator 'delete' simply frees the memory.

    Destructor this special procedure for given class, which can conduct
starting-up work before deleting the class. For destructor exists several
limits:

 - a destructor has a name such, either as name of class, but in begin name
    added the symbol '~'.
 - for destructor it is impossible indicate the type a return and modifier.
 - a destructor must not have the parameters.
 - a destructor is called only automatically, its it is impossible call as usual
    procedure of class.

struct info
{
  info (int num); // declare constructor of class info, receiving one
                  // parameter.
  ~info();	//declare destructor
  int number;
};

info::info (int num)
{
  IF ( this ) {
    number = num ;
    WRITESTR("Create new class\n");
  }
  return this;
}

info::~info ()
{
  WRITESTR("Release class\n");
}

    If memory for class is chosen in stack then at termination a procedure
will is automatically caused the destructor. In all rest events a call
destructor can be realized only by operator 'delete'. Example use an operator
'delete':

void proc ()
{
info s1; //allocate memory for class in stack
word w1; //variable for address of class in public memory
  s1.info ( 5 ); //call constructor
  w1 = new info (4); //allocate memory and call constructor
  delete s1; //call destructor for class in stack
  s1.info ( 3 ); //once again actuate class in stack
  delete w1 info; //call destructor and free memory of class in
                  //distributed memory
}

    Operator 'delete' for class created by means of operator 'new', in C--
must have a second obligatory parameter - a name of class to is used this
operator.

    If you class inherits other classes, that calling the destructors for
these classes will are automatically generated the compiler. If class has not
destructor, but inheritted classes has a destructor, that compiler will
automatically generate the procedure a destructor.

    For functioning the operators 'new' and 'delete' You necessary will
comprise of its project header file class.h--, which together with example
use of these operators inheres in this archive.


Optimization of the repeated loading the registers.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Who looked listings generated by compiler of code, possible, have turned
attention that sometimes in register is produced calculation of expression,
which several operations earlier, was already calculated in this or other
register. Experienced programmer will die to reorganize its source file to
such situations was as small as possible, but on this leaves many time. Now
compiler will keep a check on that was recorded in register and if it find
that in register want to write that already was recorded in this or other
register, that compiler, or will skip such operation, or will vastly simplify
it.

    This mechanism of optimization not yet marketed on all amounts and so
can be a situations, when compiler will skip some repetition. Hereinafter
this method of optimization will be improved. If You will find that
compiler in some situations did not optimize any code, that reports on this me.
Such events (the skipping of possibility to optimize) not dangerous. Dangerous situation
when compiler does not notice that contents a register already changed and will
do the optimization. About such events without fall report me.

    If You do not want to test new method of optimization or You have no on
this time, that option of command line /oir- possible disconnect this
optimization.


32-bit binary files.
~~~~~~~~~~~~~~~~~~~~~

    Having indicated in command line an option /b32, You get 32-bit code
without any headlines. By default, generated code is waked is adjusted on
zero start address.  If You Need other start address, that its possible
indicate by means of directives #imagebase startaddress or by means of
options command line /wib=startaddress. It is impossible indicate start
address before instructions of type of generated file.  Otherwise compiler
will generate code with zero address.

    When compiling 32-bit of binary file, header file startup.h-- compiler is
not used.


Call procedure on address in register or variable.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Earlier C-- allowed to do call a procedure to, residing in register.
Herewith parameter this procedure were send in Windows programs in accordance
with type a call 'stdcall' but in rest programs in accordance with type a
call 'pascal'. Now, except these, used by default, types, possible assign any
other type an transfer of parameters:

  (E)AX fastcall (par1,par2);
  (E)AX cdecl (par1,par2);
  (E)AX stdcall (par1,par2);
  (E)AX =паскаль (par1,par2);

    When use the type a call a procedure 'fastcall', wake attentive to
transmitted procedure a parameter did not destroy any address caused
procedure. Herewith type a call a procedure a parameter are transmitted as
follows: 1 - AX, 2 - BX, 3 - CX...

    Such image possible to do call a procedure to residing in variable.  For
these call a procedure possible also to use the modifier of ways an transfer
of parameters:

unsigned int var;  //variable, in which inheres address a procedure
. . .
  var cdecl (par1,par2);  // call a procedure, which address inheres in
                          // variable 'var' and transfer to her parameters in
                          // correspondence to with type 'cdecl'


Suffix an string constant.
~~~~~~~~~~~~~~~~~~~~~~~~~~~

    By the end of string a constant, by default, is added the symbol with
code a zero, which is a sign of the end of text string - terminator.
Compiler can generate the string constant without terminator and with symbol
'$' as terminator. For this at the end an string constant to is necessary
indicate necessary You suffix. The following suffix supports C--:

   suffix |terminator
  ------------------
    'z'   |  0
    '$'   |  $
    'n'   | no

    Suffix to write immediately after symbol double quotation marks without
spaces. Example:

    AX = "test"n; // this string not will have terminator
    BX = "test 2"$; // but by this string will be an terminator a symbol '$'

    Console version a compiler supports else two suffixes, at presence which
compiler realizes the re-coding an string constant.

   suffix |re-coding
  -----------------------
    'o'   |from OEM to Windows
    'w'   |from Windows to OEM


Truncated table an import.
~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Table an import usually consists of four tables in turn. Two tables
LookUp Table and Import Address Table absolutely identical.

    Option of command line /WSI You may force compiler to generate only one
of these two alike tables (is generated only Import Address Table). Hereunder
beside You are getted more compact table an import, that will bring, in some
cases, to creation more compact output file.


Generation of files for Menuet OS.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  Executed files for operational system Menuet have heading of the following
format:

char   os_name[8]
dword  os_version
dword  start_addr
dword  size_img
dword  alloc_mem
dword  esp_reg
dword  I_Param
dword  I_Icon

    os_name Always contains a line 'MENUET01'.

    os_version - compiler C-- in this field always writes down 1.

    start_addr Contains the address of a point of an entrance in the program.

    size_img Contains the size of a file.

    aloc_mem Defines the size of memory allocated by operational system to
    the program. Compiler C-- fills in this field value calculated under the
    formula:

      aloc_mem = align on 16 (size_img + size_post + size_stack)

    esp_reg Defines top of a stack. Compiler C-- establishes this value in
    the end to the allocated program of memory.

    I_Param Contains the address of the buffer in the size of 256 bytes in
    which parameters transmitted to the program enter the name at start. This
    buffer is created if you in the program have specified directive #argc
    TRUE or #parsecommandline TRUE, differently field I_Param is established
    in 0.

    I_Icon By the compiler it is not supported yet and it is always
    established in 0.

    For creation of programs under MeOS in a command line of compiler C-- it
is necessary to add a key '/meos'. At compilation with this key the
identifier __MEOS__ which can be used for creation of libraries is defined.
The compiler supports internal functions for this operational system:
ATEXIT(), EXIT(), PARAMCOUNT(), PARAMSTR().

    In a file startup.h-- added blocks of analysis of a command line,
mechanism ATEXIT and a call of end of the program in MeOS. If at compilation
the key of a command line '/j0 ' or the directive '#jumptomain NONE' the file
startup.h-- is not used by the compiler.


Changes to file startup.h--.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    In file startup.h--, except supports MeOS, transcribeded blocks an
parsing of command line. For 16-bit of modes of compiling is added the
possibility, perforce comprise of program an parsing of command line with
provision for long names of files. For activation this parsing to You at the
beginning initially compilable file to define the identifier _USELONGNAME_ or
in command line to indicate the key '/D=_USELONGNAME_'.

    Except this in file startup.h-- are standed from compiler a procedure
'PARAMCOUNT()' and 'PARAMSTR()'.


Third algorithm of realization an operator 'switch/SWITCH'.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Earlier operator 'switch/SWITCH' could be marketed two ways: method of
consequent comparison of importances 'case' and tabular method.

    Tabular method is most quick, but its use is limited that that tables can
have very big size.

    New method is intermediate between the first two: two tables - a table of
the addresses an entry in body an operator 'switch/SWITCH' and table of
importances 'case'.  Generated the procedure of comparison of input
importance with importances in the second table.  If there is coincidence,
that is doed transition to from the second tables.  New method is most slow,
but at big count importances 'case' (more than 15) it becomes most compact.



New possibilities of operators of comparison.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Earlier if in operator of comparison to indicate numeric importance, for
instance:

    WHILE (1) { code }

    that compiler generated the code of comparison this numeric importance
with zero.  Now if numeric importance fine from zero is immediately generated
the block of code for operator of comparison.  But if numeric importance is a
zero, that block of code following for operator of comparison, will is
skipped.  In operator of comparison possible now in general nothing not to
indicate:

   WHILE () { code }

    In this case result of operation of comparison is considered TRUE and is
generated immediately block of code.

    All these innovation possible to use only in single operations of
comparison (their it is impossible use in logical associations of
conditions).

    If in operations of comparison is doed the comparison a register (E)CX
with zero, that compiler can use the assembler instruction 'jcxz' that allows
to get the more compact code.



Change a segment of variable.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    For global variable is by default used the segment DS, but for local
segment SS.  Now for variable possible to indicate, comparatively what
segment register must is produced addressing.  For change a register a
segment to is necessary before variable to indicate the new register together
with colon, for instance:

   ES:var=1;

