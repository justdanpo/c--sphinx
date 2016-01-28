/*
    NAME: C_TEST.C
    DESCRIPTION:  This program tests the use of C-- OBJ files with Microsoft
                  C.  Besure to compile OBJTEST.C-- to an OBJ file before
                  attempting to build this program.
                  
                  This program demonstrates interfacing of both procedures
                  and functions.
                  
                  When building this program you will have to specify that
                  you wish to link it with OBJTEST.OBJ.  This is usually
                  done through a make file.  The method of creating and
                  editing a make file varies from compiler to compiler, so
                  its up to you to find out how to do if for your specific
                  C compiler.  You may also have to tell your linker to
                  ignore case sensitivity.
*/

// the following procedure and function are defined in OBJTEST.C--

void extern far pascal display_word(unsigned int wordvalue);
unsigned int extern far pascal double_it(unsigned int wordvalue);


void main ()
{
display_word(12345);
display_word( double_it(12345) );
}


/* end of C_TEST.C */