(*
    NAME: PAS_TEST.PAS
    DESCRIPTION:  This program tests the use of C-- OBJ files with Borland
                  Pascal.  Besure to compile OBJTEST.C-- to an OBJ file
                  before attempting to build this program.

                  This program demonstrates interfacing of both procedures
                  and functions.
*)

{$L OBJTEST.OBJ}

  (* The following procedure and function are defined in OBJTEST.C-- *)

	Procedure Display_Word(wordvalue : WORD); Far; External;
	Function Double_It(wordvalue : WORD): WORD; Far; External;

BEGIN

  Display_Word(12345);
  Display_Word( Double_It(12345) );

END.

(* end of PAS_TEST.PAS *)