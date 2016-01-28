{**************************************************}
{ Programmers work bench for SPHINX C-- using      }
{ Turbo Pascal 6.0 and the Turbo Vision Editor     }
{ I fixed the stupid borland bug in the editor     }
{ SPHINX Programming (C) 1994 All Rights Reserved. }
{ Last modified 24 July 1994                       }
{**************************************************}

{
24 July 94:  - Added text mode line length recalling.
11 Aug 94:  - Added *.?-- file search critera.
16 Aug 94:  - Added C-- inline ASM help menu option.
}


PROGRAM CminusminusWORKBENCH;

{$M 16384,8192,655360}
{$X+,S-}


USES
     Crt, Dos, Objects, Drivers, Memory, Views, Menus, Dialogs, StdDlg,
     MsgBox, App, Buffers, wbeditor, AsciiTab, wbcalc;


CONST
       HeapSize = 32 * (1024 div 16);

       cmOpen            = 100;
       cmChangeDir       = 101;
       cmDosShell        = 102;
       cmShowClip        = 106;
       cmCharTable       = 107;
       cmMainHelp        = 108;
       cmStackProcHelp   = 109;
       cmREGProcHelp     = 110;
       cmAllProcHelp     = 111;
       cmSetRunParams    = 112;
       cmEditHelp        = 113;
       cmchangemode      = 114;
       cmAbout           = 115;
       cmViewOutput      = 116;
       cmCompilerOptions = 117;
       cmSetRunFile      = 118;
       cmSetCompileFile  = 119;
       cmBlank           = 120;
       cmFullDOSshell    = 121;
       cmCalculator      = 122;
       cmColor1          = 123;
       cmColor2          = 124;
       cmColor3          = 125;
       cmResetSettings   = 126;
       cmNew             = 127;
       cmASMHelp         = 128;


VAR   count : word;
      driverseg,driveroff : word;   { address of driver data }
      wbpath : STRING[80];

CONST sphinx : string[21] = chr(130)+'$$$$WTLMR\$G11$$$$$'+chr(130);
      ConfigFileName = 'WB.CFG';
      ConfigHeader : STRING[40]='SPHINX C-- Work Bench Config File.'+CHR(26);
      ALIGNWORDS   =  1;     { compiler option bit values }
      CTRLCIGNORE  =  2;
      DSequalsSS   =  4;
      MAPFILE      =  8;
      PARSECOMLINE = 16;
      RESIZEMEM    = 32;

TYPE
    CompilerData = RECORD
                     CheckOptions: Word;
                     CPUSelect: Word;
                     OptimizeSpeed: Word;
                     JumpType: Word;
                     OutputType: Word;
                     ExtraOptions: string[128];
                   END;
    PCompilerDialog = ^TCompilerDialog;
    TCompilerDialog = object(TDialog) END;

VAR  CompilerOptData: CompilerData;


TYPE
      StrRecType = RECORD
                     Str: string[128];
                   END;
      PRunDialog = ^TRunDialog;
      TRunDialog = object(TDialog) END;

VAR  RunOptData,RunFileData,CompileFileData: StrRecType;

CONST cfg_good = 0;
      cfg_bad = 1;
      cfg_missing = 2;

TYPE
      PEditorApp = ^TEditorApp;
      TEditorApp = object(TApplication)
                     constructor Init;
                       destructor Done; virtual;
                       procedure HandleEvent(var Event: TEvent); virtual;
                       procedure InitMenuBar; virtual;
                       Function GetPalette : PPalette; virtual;
                       procedure InitStatusLine; virtual;
                       procedure OutOfMemory; virtual;
                   END;
      PALTYPE = ARRAY [0..63] OF BYTE;
      VIDEOMODETYPE = (vm_text25,vm_text28,vm_text43,vm_text50);
      CONFIGTYPE =
          RECORD
            CFGHeader : STRING[40];
            CFGThePalette : PALTYPE;
            CFGRunOptData,CFGRunFileData,CFGCompileFileData : StrRecType;
            CFGCompilerOptData : CompilerData;
            CFGTransfilename : STRING[128];
            CFGFindStr : STRING[80];
            CFGReplaceStr : STRING[80];
            CFGEditorFlags : Word;
            CFGVideoMode : VIDEOMODETYPE;
           END;

VAR
      ThePalette : PALTYPE;
      VideoMode : VIDEOMODETYPE;
      Config : CONFIGTYPE;

CONST
      MyColors : PALTYPE =
         (71,                      { ?? what the hell is 71 ??? }
          Blue*16   +Black,        { BackGround Fill Char Color }
         {-------------------------- View Menus, Status Lines:  }
          Brown*16  +LightGray,    { Normal Text                }
          Brown*16  +Black    ,    { Disabled Text              }
          Brown*16  +Yellow   ,    { ShortCut key               }
          Green*16  +White    ,    { Normal Selection           }
          Green*16  +DarkGray ,    { Disabled Selection         }
          Green*16  +Yellow   ,    { Selection Shortcut key     }
         {-------------------------- Used By "Blue" Windows: (?)}
          Black*16  +LightGray,    { Inactive Frame             }
          Black*16  +LightGreen,   { Active Frame               }
          Black*16  +LightGreen,   { Frame Icons                }
          Black*16  +LightGreen,   { ScrollBar Page             }
          Cyan*16   +LightGreen,   { ScrollBar Reserved ????    }
          Black*16  +Green,        { Normal Text                }
          Red*16    +Black,        { ScrollBar Selected Text    }
          0,                       { Reserved                   }
         {-------------------------- Used By "Cyan" Windows: (?)}
          red*16     +Yellow,      { Inactive Frame             }
          red*16     +White,       { Active Frame               }
          red*16     +Yellow,      { Frame Icons                }
          Magenta*16 +Black,       { ScrollBar Page             }
          Brown*16   +Brown,       { ScrollBar Reserved ????    }
          Brown*16   +Yellow,      { Normal Text      }
          Green*16   +White,       { ScrollBar Selected Text    }
          0,                       { Reserved                   }
         {-------------------------- Used By "GRay" Windows: (?)}
          lightGray*16 +Yellow,    { Inactive Frame             }
          lightGray*16 +White,     { Active Frame               }
          lightGray*16 +Yellow,    { Frame Icons                }
          Magenta*16   +Black,     { ScrollBar Page             }
          lightGray*16 +Brown,     { ScrollBar Reserved ????    }
          lightGray*16 +Magenta,   { ScrollBar Normal Text      }
          Green*16     +White,     { ScrollBar Selected Text    }
          0,                       { Reserved                   }
         {-------------------------- Dialog Box Objects:        }
          Black*16    +DarkGray,   { Inactive Frame             }
          red*16      +LightGray,  { Active Frame               }
          red*16      +LightGreen, { Frame Icons                }
          Cyan*16     +Blue,       { ScrollBar Page             }
          Cyan*16     +LightGreen, { ScrollBar Controls         }
          red*16      +LightGray,  { Static Text                }
          red*16      +LightGray,  { Label: Normal              }
          red*16      +Yellow,     {        HighLIght           }
          red*16      +Yellow,     {        ShortCut            }
          LightGray*16+Black,      { Button: Normal             }
          LIghtGray*16+Yellow,     {         Default            }
          LightGray*16+White,      {         Selected           }
          LIghtGray*16+DarkGray,   {         Disabled           }
          LIghtGray*16+Red,        {         ShortCut           }
          red*16      +Black,      {         Shadow             }
          Magenta*16  +Black,      { Cluster: Normal            }
          Magenta*16  +White,      {          Selected          }
          Magenta*16  +Yellow,     {          ShortCut          }
          blue*16     +LightGray,  { InputLine: Normal          }
          blue*16     +Yellow,     {            Selected        }
          blue*16     +White,      {            Arrows          }
          Magenta*16  +White,      { History Arrow              }
          Magenta*16  +Yellow,     { History Sides              }
          Green*16    +White,      { HistoryWindow ScrollBar Pg }
          Brown*16    +Yellow,     { " ScrollBar Controls       }
          Magenta*16  +Black,      { ListViewer:Normal          }
          lightgray*16+Yellow,     {            Focused         }
          Brown*16    +Yellow,     {            Selected        }
          Green*16    +White,      {            Divider         }
          Brown*16    +Yellow,     { Information Pane           }
          0,                       { Reserved                   }
          0      );                { Reserved                   }

      MyColors2 : PALTYPE =        { this is the LCD colour set }
         (71,                      { ?? what the hell is 71 ??? }
          LightGray*16+Black,      { BackGround Fill Char Color }
         {-------------------------- View Menus, Status Lines:  }
          LightGray*16 +Black,     { Normal Text                }
          Black*16     +LightGray, { Disabled Text              }
          Black*16     +LightGray, { ShortCut key               }
          Black*16     +LightGray, { Normal Selection           }
          Black*16     +LightGray, { Disabled Selection         }
          Black*16     +LightGray, { Selection Shortcut key     }
         {-------------------------- Used By "Blue" Windows: (?)}
          Black*16  +LightGray,    { Inactive Frame             }
          Black*16  +White,        { Active Frame               }
          Black*16  +White,        { Frame Icons                }
          Black*16  +White,        { ScrollBar Page             }
          LightGray*16+Black,      { ScrollBar Reserved ????    }
          Black*16  +White,        { Normal Text                }
          LightGray*16 +Black,     { ScrollBar Selected Text    }
          0,                       { Reserved                   }
         {-------------------------- Used By "Cyan" Windows: (?)}
          Black*16  +LightGray,    { Inactive Frame             }
          Black*16  +White,        { Active Frame               }
          Black*16  +LightGray,    { Frame Icons                }
          Black*16  +LightGray,    { ScrollBar Page             }
          LightGray*16+Black,      { ScrollBar Reserved ????    }
          Black*16  +LightGray,    { Normal Text                }
          LightGray*16 +Black,     { ScrollBar Selected Text    }
          0,                       { Reserved                   }
         {-------------------------- Used By "GRay" Windows: (?)}
          Black*16  +LightGray,    { Inactive Frame             }
          Black*16  +White,        { Active Frame               }
          Black*16  +White,        { Frame Icons                }
          Black*16  +White,        { ScrollBar Page             }
          LightGray*16+Black,      { ScrollBar Reserved ????    }
          Black*16  +White,        { Normal Text                }
          LightGray*16 +Black,     { ScrollBar Selected Text    }
          0,                       { Reserved                   }
         {-------------------------- Dialog Box Objects:        }
          Black*16  +LightGray,    { Inactive Frame             }
          Black*16  +White,        { Active Frame               }
          Black*16  +LightGray,    { Frame Icons                }
          Black*16  +LightGray,    { ScrollBar Page             }
          LightGray*16 +Black,     { ScrollBar Reserved ????    }
          Black*16  +LightGray,    { Normal Text                }
          Black*16  +LightGray,    { Label: Normal              }
          Black*16  +White,        {        HighLIght           }
          Black*16  +White,        {        ShortCut            }
          LightGray*16+Black,      { Button: Normal             }
          Black*16  +White,        {         Default            }
          Black*16  +White,        {         Selected           }
          LightGray*16+Black,      {         Disabled           }
          Black*16   +White,       {         ShortCut           }
          Black*16    +White,      {         Shadow             }
          LightGray*16+Black,      { Cluster: Normal            }
          White*16    +Black,      {          Selected          }
          Black*16    +LightGray,  {          ShortCut          }
          Black*16    +White,      { InputLine: Normal          }
          Black*16    +White,      {            Selected        }
          Black*16    +White,      {            Arrows          }
          Black*16    +White,      { History Arrow              }
          Black*16    +LightGray,  { History Sides              }
          Black*16    +White,      { HistoryWindow ScrollBar Pg }
          Black*16    +White,      { " ScrollBar Controls       }
          LightGray*16+Black,      { ListViewer:Normal          }
          Black*16    +White,      {            Focused         }
          White*16    +Black,      {            Selected        }
          Black*16    +White,      {            Divider         }
          Black*16    +White,      { Information Pane           }
          0,                       { Reserved                   }
          0      );                { Reserved                   }

      MyColors3 : PALTYPE =      { This is the boring conventional colours }
         (71,                      { ?? what the hell is 71 ??? }
          Blue*16     +Black,      { BackGround Fill Char Color }
         {-------------------------- View Menus, Status Lines:  }
          LightGray*16 +Black,     { Normal Text                }
          LightGray*16 +DarkGray,  { Disabled Text              }
          LightGray*16 +White   , { ShortCut key               }
          Green*16  +White    ,    { Normal Selection           }
          Green*16  +DarkGray ,    { Disabled Selection         }
          Green*16  +Yellow   ,    { Selection Shortcut key     }
         {-------------------------- Used By "Blue" Windows: (?)}
          Blue*16  +LightGray,    { Inactive Frame             }
          Blue*16  +White,        { Active Frame               }
          Blue*16  +LightGray,    { Frame Icons                }
          Blue*16  +LightGray,    { ScrollBar Page             }
          Cyan*16  +Black,        { ScrollBar Reserved ????    }
          Blue*16  +LightGray,    { Normal Text                }
          LightGray*16 +Black,    { ScrollBar Selected Text    }
          0,                      { Reserved                   }
         {-------------------------- Used By "Cyan" Windows: (?)}
          red*16     +Yellow,      { Inactive Frame             }
          red*16     +White,       { Active Frame               }
          red*16     +Yellow,      { Frame Icons                }
          Magenta*16 +Black,       { ScrollBar Page             }
          Brown*16   +Brown,       { ScrollBar Reserved ????    }
          Brown*16   +Yellow,      { Normal Text      }
          Green*16   +White,       { ScrollBar Selected Text    }
          0,                       { Reserved                   }
         {-------------------------- Used By "GRay" Windows: (?)}
          lightGray*16 +Yellow,    { Inactive Frame             }
          lightGray*16 +White,     { Active Frame               }
          lightGray*16 +Yellow,    { Frame Icons                }
          Magenta*16   +Black,     { ScrollBar Page             }
          lightGray*16 +Brown,     { ScrollBar Reserved ????    }
          lightGray*16 +Magenta,   { ScrollBar Normal Text      }
          Green*16     +White,     { ScrollBar Selected Text    }
          0,                       { Reserved                   }
         {-------------------------- Dialog Box Objects:        }
          Black*16    +DarkGray,   { Inactive Frame             }
          red*16      +LightGray,  { Active Frame               }
          red*16      +LightGreen, { Frame Icons                }
          Cyan*16     +Blue,       { ScrollBar Page             }
          Cyan*16     +LightGreen, { ScrollBar Controls         }
          red*16      +LightGray,  { Static Text                }
          red*16      +LightGray,  { Label: Normal              }
          red*16      +Yellow,     {        HighLIght           }
          red*16      +Yellow,     {        ShortCut            }
          LightGray*16+Black,      { Button: Normal             }
          LIghtGray*16+Yellow,     {         Default            }
          LightGray*16+White,      {         Selected           }
          LIghtGray*16+DarkGray,   {         Disabled           }
          LIghtGray*16+Red,        {         ShortCut           }
          red*16      +Black,      {         Shadow             }
          Magenta*16  +Black,      { Cluster: Normal            }
          Magenta*16  +White,      {          Selected          }
          Magenta*16  +Yellow,     {          ShortCut          }
          blue*16     +LightGray,  { InputLine: Normal          }
          blue*16     +Yellow,     {            Selected        }
          blue*16     +White,      {            Arrows          }
          Magenta*16  +White,      { History Arrow              }
          Magenta*16  +Yellow,     { History Sides              }
          Green*16    +White,      { HistoryWindow ScrollBar Pg }
          Brown*16    +Yellow,     { " ScrollBar Controls       }
          Magenta*16  +Black,      { ListViewer:Normal          }
          lightgray*16+Yellow,     {            Focused         }
          Brown*16    +Yellow,     {            Selected        }
          Green*16    +White,      {            Divider         }
          Brown*16    +Yellow,     { Information Pane           }
          0,                       { Reserved                   }
          0      );                { Reserved                   }



VAR
      EditorApp: TEditorApp;
      ClipWindow: PEditWindow;


PROCEDURE ResetSettings;
BEGIN
  WITH CompilerOptData DO
    BEGIN
      CheckOptions := ALIGNWORDS OR RESIZEMEM;
      CPUSelect := 0;
      OptimizeSpeed := 1;
      JumpType := 2;
      OutputType := 0;
      ExtraOptions := '';
    END;
  RunOptData.Str := '';
  RunFileData.Str := '';
  CompileFileData.Str := '';
  FindStr := '';
  ReplaceStr := '';
  EditorFlags := efPromptOnReplace + efReplaceAll;
END;


FUNCTION LoadConfig : BYTE;
VAR disk : FILE OF CONFIGTYPE;
    i : WORD;
BEGIN
  LoadConfig := cfg_good;
  Assign(disk,wbpath+ConfigFileName);
  Reset(disk);
  IF IOResult <> 0 THEN
    BEGIN
      LoadConfig := cfg_missing;
      EXIT;
    END;
  Read(disk,Config);
  IF IOResult <> 0 THEN
    BEGIN
      Close(disk);
      LoadConfig := cfg_bad;
      EXIT;
    END;
  Close(disk);
  WITH Config DO
    BEGIN
      FOR i := 1 TO Length(ConfigHeader) DO
          IF CFGHeader[i-1] <> ConfigHeader[i] THEN
            BEGIN
              LoadConfig := cfg_bad;
              EXIT;
            END;

      ThePalette := CFGThePalette;
      IF thepalette[2] = Brown*16+LightGray THEN
          thepalette[1] := mycolors[1];
      RunOptData := CFGRunOptData;
      RunFileData := CFGRunFileData;
      CompileFileData := CFGCompileFileData;
      CompilerOptData := CFGCompilerOptData;
      Transfilename := CFGTransFileName;
      FindStr := CFGFindStr;
      ReplaceStr := CFGReplaceStr;
      EditorFlags := CFGEditorFlags;
      VideoMode := CFGVideoMode;
    END;
END;


PROCEDURE SaveConfig;
VAR disk : FILE OF CONFIGTYPE;
    i : WORD;
BEGIN
  WITH Config DO
    BEGIN
      FOR i := 1 TO Length(ConfigHeader) DO
          CFGHeader[i-1] := ConfigHeader[i];
      CFGThePalette := ThePalette;
      CFGRunOptData := RunOptData;
      CFGRunFileData := RunFileData;
      CFGCompileFileData := CompileFileData;
      CFGCompilerOptData := CompilerOptData;
      CFGTransfilename := TransFileName;
      CFGFindStr := FindStr;
      CFGReplaceStr := ReplaceStr;
      CFGEditorFlags := EditorFlags;
      CFGVideoMode := VideoMode;
    END;
  Assign(disk,wbpath+ConfigFileName);
  Rewrite(disk);
  IF IOResult <> 0 THEN
    BEGIN
      MessageBox('Error Creating Configuration File.',nil,mferror+mfOkButton);
      EXIT;
    END;
  Write(disk,Config);
  IF IOResult <> 0 THEN
      MessageBox('Error Writing Configuration File.',nil,mferror+mfOkButton);
  Close(disk);
END;


PROCEDURE setsphinxstate( state : byte );
BEGIN
  ASM
      MOV AH,'S'
      MOV AL,'P'
      MOV BH,'H'
      MOV BL,'I'
      MOV CH,'N'
      MOV CL,'X'
      MOV DH,1
      MOV DL,state  { spawn program and return to work bench }
      PUSH DI
      PUSH SI
      INT 18h
      POP SI
      POP DI
    END;
END;


FUNCTION getrunfilename : string;
VAR
    dir : DirStr;
    name : NameStr;
    extension : ExtStr;
    runextension : string[5];
BEGIN
  IF CompilerOptData.OutputType = 1 THEN
      runextension := '.EXE'
  ELSE IF CompilerOptData.OutputType = 2 THEN
      runextension := '.OBJ'
  ELSE runextension := '.COM';

  IF RunFileData.Str = '' THEN
    BEGIN
      IF CompileFileData.Str = '' THEN
        BEGIN
          fsplit(transfilename,dir,name,extension);
          getrunfilename := dir+name+runextension;
        END
      ELSE BEGIN
          fsplit(CompileFileData.Str,dir,name,extension);
          getrunfilename := dir+name+runextension;
        END;
    END
  ELSE BEGIN
      fsplit(RunFileData.Str,dir,name,extension);
      IF (extension = '') OR (extension = '.C--') THEN
          getrunfilename := dir+name+runextension
      ELSE getrunfilename := RunFileData.Str;
    END;
END;


FUNCTION getcompilefilename : string;
VAR
    dir : DirStr;
    name : NameStr;
    extension : ExtStr;
BEGIN
  IF CompileFileData.Str = '' THEN
      fsplit(transfilename,dir,name,extension)
  ELSE fsplit(CompileFileData.Str,dir,name,extension);

  IF (extension = '.C--') OR (extension = '') THEN
      getcompilefilename := dir+name
  ELSE getcompilefilename := '';
END;


procedure About;
var
  D: PDialog;
  Control: PView;
  R: TRect;
BEGIN
  R.Assign(0, 0, 35, 14);
  D := New(PDialog, Init(R, ''));
  with D^ do
  BEGIN
    Options := Options OR ofCentered;

    R.Grow(-1, -1);
    Dec(R.B.Y, 3);
    Insert(New(PStaticText, Init(R,
      #13 +
      ^C'S P H I N X   C--'#13 +
      ^C'Programmer''s Work Bench'#13 +
      ^C'Version 0.126'#13 +
      #13 +
      ^C'Copyright (C) 1994 Peter Cellik'#13 +
      ^C'SPHINX Programming 1994'
      )));

    R.Assign(8, 9, 27, 13);
    Insert(New(PButton, Init(R, '~O~K', cmOk, bfDefault)));
  END;
  DeskTop^.ExecView(D);
  Dispose(D, Done);
END;


procedure setrunparams;
var
  ViewPointer: PView;
  Dialog: PRunDialog;
  R: TRect;
BEGIN
  R.Assign(5, 4, 75, 12);
  Dialog := New(PRunDialog, Init(R, 'Run Time Parameters'));
  with Dialog^ do
    BEGIN
      R.Assign(3, 3, 67, 4);
      ViewPointer := New(PInputLine, Init(R, 128));
      R.Assign(2, 2, 44, 3);
      Insert(New(PLabel, Init(R, 'Run Time Parameters:', ViewPointer)));
      Insert(ViewPointer);
      R.Assign(15, 5, 25, 7);
      Insert(New(PButton, Init(R, '~O~k', cmOK, bfDefault)));
      R.Assign(28, 5, 38, 7);
      Insert(New(PButton, Init(R, 'Cancel', cmCancel, bfNormal)));
    END;
  Dialog^.SetData(RunOptData);
  IF DeskTop^.ExecView(Dialog) <> cmCancel THEN
      Dialog^.GetData(RunOptData);
  Dispose(Dialog, Done);
END;


procedure setcompilefile;
var
  ViewPointer: PView;
  Dialog: PRunDialog;
  R: TRect;
BEGIN
  R.Assign(5, 4, 75, 12);
  Dialog := New(PRunDialog, Init(R, 'Set Primary Compile File'));
  with Dialog^ do
    BEGIN
      R.Assign(3, 3, 67, 4);
      ViewPointer := New(PInputLine, Init(R, 128));
      R.Assign(2, 2, 44, 3);
      Insert(New(PLabel, Init(R, 'Primary Compile Filename:', ViewPointer)));
      Insert(ViewPointer);
      R.Assign(15, 5, 25, 7);
      Insert(New(PButton, Init(R, '~O~k', cmOK, bfDefault)));
      R.Assign(28, 5, 38, 7);
      Insert(New(PButton, Init(R, 'Cancel', cmCancel, bfNormal)));
    END;
  Dialog^.SetData(CompileFileData);
  IF DeskTop^.ExecView(Dialog) <> cmCancel THEN
      Dialog^.GetData(CompileFileData);
  Dispose(Dialog, Done);
END;


procedure setrunfile;
var
  ViewPointer: PView;
  Dialog: PRunDialog;
  R: TRect;
BEGIN
  R.Assign(5, 4, 75, 12);
  Dialog := New(PRunDialog, Init(R, 'Set Primary Run File'));
  with Dialog^ do
    BEGIN
      R.Assign(3, 3, 67, 4);
      ViewPointer := New(PInputLine, Init(R, 128));
      R.Assign(2, 2, 44, 3);
      Insert(New(PLabel, Init(R, 'Primary Run Filename:', ViewPointer)));
      Insert(ViewPointer);
      R.Assign(15, 5, 25, 7);
      Insert(New(PButton, Init(R, '~O~k', cmOK, bfDefault)));
      R.Assign(28, 5, 38, 7);
      Insert(New(PButton, Init(R, 'Cancel', cmCancel, bfNormal)));
    END;
  Dialog^.SetData(RunFileData);
  IF DeskTop^.ExecView(Dialog) <> cmCancel THEN
      Dialog^.GetData(RunFileData);
  Dispose(Dialog, Done);
END;


procedure CompilerOptions;
var
  ViewPointer: PView;
  Dialog: PCompilerDialog;
  R: TRect;
BEGIN
  R.Assign(12, 1, 68, 21);
  Dialog := New(PCompilerDialog, Init(R, 'Compiler Options'));
  with Dialog^ do
  BEGIN
    R.Assign(3, 3, 22, 9);
    ViewPointer := New(PCheckBoxes, Init(R,
      NewSItem('~A~lign Words',
      NewSItem('Ctrl-C ~I~gnore',
      NewSItem('~D~S=SS',
      NewSItem('~M~AP File',
      NewSItem('~P~arse Com.Line',
      NewSItem('~R~esize Memory',
      nil)))))))
    );
    Insert(ViewPointer);
    R.Assign(3, 2, 22, 3);
    Insert(New(PLabel, Init(R, 'Switches:', ViewPointer)));

    R.Assign(24, 3, 35, 9);
    ViewPointer := New(PRadioButtons, Init(R,
      NewSItem(' 8~0~86',
      NewSItem('80~1~86',
      NewSItem('80~2~86',
      NewSItem('80~3~86',
      NewSItem('80~4~86',
      NewSItem('80~5~86',
      nil))))))
    ));
    Insert(ViewPointer);
    R.Assign(24, 2, 35, 3);
    Insert(New(PLabel, Init(R, 'CPU Chip:', ViewPointer)));

    R.Assign(3, 11, 18, 13);
    ViewPointer := New(PRadioButtons, Init(R,
      NewSItem('~C~ode Size',
      NewSItem('~S~peed',
      nil))
    ));
    Insert(ViewPointer);
    R.Assign(3, 10, 18, 11);
    Insert(New(PLabel, Init(R, 'Optimize For:', ViewPointer)));

    R.Assign(37, 3, 53, 6);
    ViewPointer := New(PRadioButtons, Init(R,
      NewSItem('~N~o Jump',
      NewSItem('S~h~ort Jump',
      NewSItem('Near ~J~ump',
      nil)))
    ));
    Insert(ViewPointer);
    R.Assign(37, 2, 53, 3);
    Insert(New(PLabel, Init(R, 'main() Jump:', ViewPointer)));

    R.Assign(37, 8, 53, 11);
    ViewPointer := New(PRadioButtons, Init(R,
      NewSItem('COM ~F~ile',
      NewSItem('~E~XE File',
      NewSItem('~O~BJ File',
      nil)))
    ));
    Insert(ViewPointer);
    R.Assign(37, 7, 53, 8);
    Insert(New(PLabel, Init(R, 'Output Type:', ViewPointer)));

    R.Assign(3, 15, 53, 16);
    ViewPointer := New(PInputLine, Init(R, 128));
    Insert(ViewPointer);
    R.Assign(2, 14, 24, 15);
    Insert(New(PLabel, Init(R, 'Additional Options:', ViewPointer)));

    R.Assign(20, 17, 33, 19);
    Insert(New(PButton, Init(R, '~O~k', cmOK, bfDefault)));
    R.Assign(40, 17, 53, 19);
    Insert(New(PButton, Init(R, 'Cancel', cmCancel, bfNormal)));
  END;
  Dialog^.SetData(CompilerOptData);
  IF DeskTop^.ExecView(Dialog) <> cmCancel THEN
      Dialog^.GetData(CompilerOptData);
  Dispose(Dialog, Done);
END;


function ExecDialog(P: PDialog; Data: Pointer): Word;
var
  Result: Word;
BEGIN
  Result := cmCancel;
  P := PDialog(Application^.ValidView(P));
  if P <> nil then
  BEGIN
    if Data <> nil then P^.SetData(Data^);
    Result := DeskTop^.ExecView(P);
    if (Result <> cmCancel) and (Data <> nil) then P^.GetData(Data^);
    Dispose(P, Done);
  END;
  ExecDialog := Result;
END;


function CreateFindDialog: PDialog;
var
  D: PDialog;
  Control: PView;
  R: TRect;
BEGIN
  R.Assign(0, 0, 38, 12);
  D := New(PDialog, Init(R, 'Find'));
  with D^ do
  BEGIN
    Options := Options or ofCentered;

    R.Assign(3, 3, 32, 4);
    Control := New(PInputLine, Init(R, 80));
    Insert(Control);
    R.Assign(2, 2, 15, 3);
    Insert(New(PLabel, Init(R, '~T~ext to find', Control)));
    R.Assign(32, 3, 35, 4);
    Insert(New(PHistory, Init(R, PInputLine(Control), 10)));

    R.Assign(3, 5, 35, 7);
    Insert(New(PCheckBoxes, Init(R,
             NewSItem('~C~ase sensitive',
             NewSItem('~W~hole words only', nil)))));

    R.Assign(14, 9, 24, 11);
    Insert(New(PButton, Init(R, 'O~K~', cmOk, bfDefault)));
    Inc(R.A.X, 12); Inc(R.B.X, 12);
    Insert(New(PButton, Init(R, 'Cancel', cmCancel, bfNormal)));

    SelectNext(False);
  END;
  CreateFindDialog := D;
END;


function CreateReplaceDialog: PDialog;
var
  D: PDialog;
  Control: PView;
  R: TRect;
BEGIN
  R.Assign(0, 0, 40, 16);
  D := New(PDialog, Init(R, 'Replace'));
  with D^ do
  BEGIN
    Options := Options or ofCentered;

    R.Assign(3, 3, 34, 4);
    Control := New(PInputLine, Init(R, 80));
    Insert(Control);
    R.Assign(2, 2, 15, 3);
    Insert(New(PLabel, Init(R, '~T~ext to find', Control)));
    R.Assign(34, 3, 37, 4);
    Insert(New(PHistory, Init(R, PInputLine(Control), 10)));

    R.Assign(3, 6, 34, 7);
    Control := New(PInputLine, Init(R, 80));
    Insert(Control);
    R.Assign(2, 5, 12, 6);
    Insert(New(PLabel, Init(R, '~N~ew text', Control)));
    R.Assign(34, 6, 37, 7);
    Insert(New(PHistory, Init(R, PInputLine(Control), 11)));

    R.Assign(3, 8, 37, 12);
    Insert(New(PCheckBoxes, Init(R,
      NewSItem('~C~ase sensitive',
      NewSItem('~W~hole words only',
      NewSItem('~P~rompt on replace',
      NewSItem('~R~eplace all', nil)))))));

    R.Assign(17, 13, 27, 15);
    Insert(New(PButton, Init(R, 'O~K~', cmOk, bfDefault)));
    R.Assign(28, 13, 38, 15);
    Insert(New(PButton, Init(R, 'Cancel', cmCancel, bfNormal)));

    SelectNext(False);
  END;
  CreateReplaceDialog := D;
END;


function DoEditDialog(Dialog: Integer; Info: Pointer): Word; far;
var
  R: TRect;
  T: TPoint;
BEGIN
  case Dialog of
    edOutOfMemory:
      DoEditDialog := MessageBox('Not enough memory for this operation or file larger than 64K.',
        nil, mfError + mfOkButton);
    edReadError:
      DoEditDialog := MessageBox('Error reading file %s',
        @Info, mfError + mfOkButton);
    edWriteError:
      DoEditDialog := MessageBox('Error writing file %s',
        @Info, mfError + mfOkButton);
    edCreateError:
      DoEditDialog := MessageBox('Error creating file %s',
        @Info, mfError + mfOkButton);
    edSaveModify:
      DoEditDialog := MessageBox('%s has been modified. Save?',
        @Info, mfWarning + mfYesNoCancel);
    edSaveUntitled:
      DoEditDialog := MessageBox('Save untitled file?',
        nil, mfWarning + mfYesNoCancel);
    edSaveAs:
      DoEditDialog := ExecDialog(New(PFileDialog, Init('*.C--',
        'Save file as', '~N~ame', fdOkButton, 101)), Info);
    edFind:
      DoEditDialog := ExecDialog(CreateFindDialog, Info);
    edSearchFailed:
      DoEditDialog := MessageBox('Search string not found.',
        nil, mfError + mfOkButton);
    edReplace:
      DoEditDialog := ExecDialog(CreateReplaceDialog, Info);
    edReplacePrompt:
      BEGIN
        { Avoid placing the dialog on the same line as the cursor }
        R.Assign(0, 1, 40, 8);
        R.Move((Desktop^.Size.X - R.B.X) div 2, 0);
        Desktop^.MakeGlobal(R.B, T);
        Inc(T.Y);
        if TPoint(Info).Y <= T.Y then
          R.Move(0, Desktop^.Size.Y - R.B.Y - 2);
        DoEditDialog := MessageBoxRect(R, 'Replace this occurence?',
          nil, mfYesNoCancel + mfInformation);
      END;
  END;
END;


function OpenEditor(FileName: FNameStr; Visible: Boolean): PEditWindow;
var
  P: PView;
  R: TRect;
BEGIN
  DeskTop^.GetExtent(R);
  P := Application^.ValidView(New(PEditWindow,
    Init(R, FileName, wnNoNumber)));
  if not Visible then P^.Hide;
  DeskTop^.Insert(P);
  OpenEditor := PEditWindow(P);
END;


constructor TEditorApp.Init;
var
  H: Word;
  R: TRect;
BEGIN
  H := PtrRec(HeapEnd).Seg - PtrRec(HeapPtr).Seg;
  IF H > HeapSize THEN
      BufHeapSize := H - HeapSize
  ELSE BufHeapSize := 0;
  InitBuffers;
  TApplication.Init;
  DisableCommands([cmSave, cmSaveAs, cmCut, cmCopy, cmPaste, cmClear,
    cmUndo, cmFind, cmReplace, cmSearchAgain, cmCompile, cmCompileandRun,
    cmRun]);
  EditorDialog := DoEditDialog;
  ClipWindow := OpenEditor('', False);
  if ClipWindow <> nil then
    BEGIN
      Clipboard := ClipWindow^.Editor;
      Clipboard^.CanUndo := False;
    END;
END;

destructor TEditorApp.Done;
BEGIN
  TApplication.Done;
  DoneBuffers;
END;


procedure TEditorApp.HandleEvent(var Event: TEvent);
VAR  NewMode : word;


procedure FileOpen;
var
  FileName: FNameStr;
BEGIN
  FileName := '*.?--';
  IF ExecDialog(New(PFileDialog, Init('*.?--', 'Open file',
    '~N~ame', fdOpenButton, 100)), @FileName) <> cmCancel THEN
    BEGIN
      OpenEditor(FileName, True);
    END;
END;


procedure ChangeDir;
BEGIN
  ExecDialog( New(PChDirDialog,Init(cdNormal,0)), nil);
END;


procedure DosShell;
BEGIN
  DoneSysError;
  DoneEvents;
  DoneVideo;
  DoneMemory;
  SetMemTop( Ptr(BufHeapPtr,0) );
  PrintStr('Type EXIT to return to SPHINX C-- Work Bench...');
  SwapVectors;
  Exec(GetEnv('COMSPEC'),'');
  SwapVectors;
  SetMemTop( Ptr(BufHeapEnd,0) );
  InitMemory;
  InitVideo;
  InitEvents;
  InitSysError;
  Redraw;
END;


procedure blankscreen;
BEGIN
  DoneSysError;
  DoneEvents;
  DoneVideo;
  DoneMemory;
  SwapVectors;
  ASM
      MOV AH,0
      INT 16h
    END;
  SwapVectors;
  InitMemory;
  InitVideo;
  InitEvents;
  InitSysError;
  Redraw;
END;


procedure ViewOutput;
BEGIN
  DoneSysError;
  DoneEvents;
  DoneVideo;
  DoneMemory;
  SwapVectors;
  ASM
      MOV AH,'S'
      MOV AL,'P'
      MOV BH,'H'
      MOV BL,'I'
      MOV CH,'N'
      MOV CL,'X'
      MOV DH,6
      PUSH DI
      PUSH SI
      INT 18h
      POP SI
      POP DI
    END;
  SwapVectors;
  InitMemory;
  InitVideo;
  InitEvents;
  InitSysError;
  Redraw;
END;


PROCEDURE NewPalette ( VAR Pal : PALTYPE );
BEGIN
  DoneSysError;
  DoneEvents;
  DoneVideo;
  DoneMemory;
  ThePalette := Pal;
  InitMemory;
  InitVideo;
  InitEvents;
  InitSysError;
  Redraw;
END;


FUNCTION setdriverstuff : BOOLEAN;
VAR  stropt : STRING;
     i : WORD;
BEGIN
  WITH compileroptdata DO
    BEGIN
      CASE CPUselect OF
          0: stropt := ' /0';
          1: stropt := ' /1';
          2: stropt := ' /2';
          3: stropt := ' /3';
          4: stropt := ' /4';
          5: stropt := ' /5'
          ELSE BEGIN
                 write(chr(7));
                 stropt := ' ';
               END;
        END;
      IF (CheckOptions AND ALIGNWORDS) = ALIGNWORDS THEN
          stropt := stropt + ' /+A'
      ELSE stropt := stropt + ' /-A';
      IF (CheckOptions AND CTRLCIGNORE) = CTRLCIGNORE THEN
          stropt := stropt + ' /+C'
      ELSE stropt := stropt + ' /-C';
      IF (CheckOptions AND DSequalsSS) = DSequalsSS THEN
          stropt := stropt + ' /+D'
      ELSE stropt := stropt + ' /-D';
      IF (CheckOptions AND MAPFILE) = MAPFILE THEN
          stropt := stropt + ' /+M'
      ELSE stropt := stropt + ' /-M';
      IF (CheckOptions AND PARSECOMLINE) = PARSECOMLINE THEN
          stropt := stropt + ' /+P'
      ELSE stropt := stropt + ' /-P';
      IF (CheckOptions AND RESIZEMEM) = RESIZEMEM THEN
          stropt := stropt + ' /+R'
      ELSE stropt := stropt + ' /-R';

      IF OptimizeSpeed = 1 THEN
          stropt := stropt + ' /OS'    { optimize for speed }
      ELSE stropt := stropt + ' /OC';  { optimize for code size }

      stropt := stropt + ' /J'+CHR(JumpType+ORD('0'));  { /J0 /J1 /J2 }

      IF OutputType = 1 THEN
          stropt := stropt + ' /EXE'
      ELSE IF OutputType = 2 THEN
          stropt := stropt + ' /OBJ';

      IF ExtraOptions <> '' THEN
          stropt := stropt + ' ' + ExtraOptions;
    END;

  stropt := stropt + ' ' + getcompilefilename;
  IF length(stropt) >= $7F THEN
    BEGIN
      MessageBox('Command Line Options For Compiler Too Long.',nil,mferror+mfOkButton);
      setdriverstuff := FALSE;
      EXIT;
    END;

  mem[driverseg:$80] := length(stropt);
  FOR i := 1 to length(stropt) DO
      mem[driverseg:$80+i] := ord(stropt[i]);
  mem[driverseg:$81+length(stropt)] := 13;
  setdriverstuff := TRUE;

  stropt := getrunfilename;
  IF length(stropt) >= $7F THEN
    BEGIN
      MessageBox('Run File Name Too Long.',nil,mferror+mfOkButton);
      setdriverstuff := FALSE;
      EXIT;
    END;
  FOR i := 1 to length(stropt) DO
      mem[driverseg:driveroff+i-1] := ord(stropt[i]);
  mem[driverseg:driveroff+length(stropt)] := 0;

  stropt := RunOptData.Str;
  IF (length(stropt) > 0) AND (stropt[1] <> ' ') THEN
      stropt := ' '+stropt;
  IF length(stropt) >= $7F THEN
    BEGIN
      MessageBox('Run File Command Line Too Long.',nil,mferror+mfOkButton);
      setdriverstuff := FALSE;
      EXIT;
    END;
  mem[driverseg:driveroff+$80] := length(stropt);
  FOR i := 1 to length(stropt) DO
      mem[driverseg:driveroff+$80+i] := ord(stropt[i]);
  mem[driverseg:driveroff+$81+length(stropt)] := 13;

  SaveConfig;
END;


FUNCTION SpawnApp : boolean;
VAR str : string;
BEGIN
  str := getrunfilename;
  IF str = '' THEN 
    BEGIN
      MessageBox('No Run File Specified.',nil,mferror+mfOkButton);
      SpawnApp := FALSE;
      EXIT;
    END;

  IF (str[length(str)] = 'j') OR (str[length(str)] = 'J') THEN
    BEGIN
      MessageBox('Can''t Run an OBJ File.',nil,mferror+mfOkButton);
      SpawnApp := FALSE;
      EXIT;
    END;

  IF setdriverstuff = FALSE THEN
    BEGIN
      SpawnApp := FALSE;
      EXIT;
    END;

  setsphinxstate(3);   { spawn program and return to work bench }
  spawnapp := TRUE;
END;


FUNCTION SpawnCompiler( runflag : BOOLEAN ) : BOOLEAN;
BEGIN
  IF getcompilefilename = '' THEN
    BEGIN
      MessageBox('Invalid File Name To Compile.  File Name Must Be *.C--.',nil,mferror+mfOkButton);
      spawncompiler := FALSE;
      EXIT;
    END;

  IF setdriverstuff = FALSE THEN
    BEGIN
      SpawnCompiler := FALSE;
      EXIT;
    END;

  IF runflag THEN
    BEGIN
      IF SpawnApp = FALSE THEN
        BEGIN
          SpawnCompiler := FALSE;
          EXIT;
        END;
      setsphinxstate(5)    { compile then run }
    END
  ELSE setsphinxstate(4);  { just compile }

  spawncompiler := TRUE;
END;


FUNCTION fullDOSshell : BOOLEAN;
var  stropt : string;
     i : word;
BEGIN
  IF setdriverstuff = FALSE THEN
    BEGIN
      fullDOSshell := FALSE;
      EXIT;
    END;

  stropt := GetEnv('COMSPEC');
  IF stropt = '' THEN
    BEGIN
      MessageBox('Environment variable COMSPEC is unknown.',nil,mferror+mfOkButton);
      fullDOSshell := FALSE;
      EXIT;
    END;
  FOR i := 1 to length(stropt) DO
      mem[driverseg:driveroff+$100+i-1] := ord(stropt[i]);
  mem[driverseg:driveroff+$100+length(stropt)] := 0;
  setsphinxstate(1);    { spawn DOS shell }

  fullDOSshell := TRUE;
END;


procedure ShowClip;
BEGIN
  ClipWindow^.Select;
  ClipWindow^.Show;
END;

procedure AsciiTab;
var
  P: PAsciiChart;
BEGIN
  P := New(PAsciiChart, Init);
  Desktop^.Insert(ValidView(P));
END;

procedure Calculator;
var
  P: PCalculator;
BEGIN
  P := New(PCalculator, Init);
  if ValidView(P) <> nil then
    Desktop^.Insert(P);
END;

procedure Tile;
var
  R: TRect;
BEGIN
  Desktop^.GetExtent(R);
  Desktop^.Tile(R);
END;


procedure Cascade;
var
  R: TRect;
BEGIN
  Desktop^.GetExtent(R);
  Desktop^.Cascade(R);
END;


PROCEDURE OpenHelp( helpfilename : string );
VAR f : file;
BEGIN
  Assign(f,wbpath+helpfilename);  { check if output file made }
  Reset(f);
  IF IOResult = 0 THEN
    BEGIN
      close(f);
      openeditor(wbpath+helpfilename,TRUE);
    END
  ELSE MessageBox('Unable To Open Help File '+wbpath+helpfilename,nil,mferror+mfOkButton);
END;


BEGIN
  IF (Event.What = evCommand) AND (Event.Command = cmQuit) THEN
    BEGIN
      setsphinxstate(0);
      SaveConfig;
    END;
  TApplication.HandleEvent(Event);
  case Event.What of
    evCommand:
      case Event.Command of
        cmOpen: FileOpen;
        cmChangeDir: ChangeDir;
        cmDosShell: DosShell;
        cmFullDosShell:  BEGIN
                           IF FullDosShell THEN
                             BEGIN
                               event.command := cmQuit;
                               TApplication.HandleEvent(Event);
                               exit;
                             END;
                         END;
        cmCompile: BEGIN
                     IF SpawnCompiler( FALSE ) THEN
                       BEGIN
                         event.command := cmQuit;
                         TApplication.HandleEvent(Event);
                         exit;
                       END;
                   END;
        cmCompileAndRun: BEGIN
                           IF SpawnCompiler( TRUE ) THEN
                             BEGIN
                               event.command := cmQuit;
                               TApplication.HandleEvent(Event);
                               exit;
                             END;
                         END;
        cmRun:  BEGIN
                  IF Spawnapp THEN
                    BEGIN
                      event.command := cmQuit;
                      TApplication.HandleEvent(Event);
                      exit;
                    END;
                END;
        cmShowClip: ShowClip;
        cmCharTable: AscIItab;
        cmMainHelp: openhelp('C--INFO.DOC');
        cmStackProcHelp: openhelp('STAKPROC.DOC');
        cmREGProcHelp: openhelp('REGPROCS.DOC');
        cmAllProcHelp: openhelp('AllPROCS.DOC');
        cmEditHelp: openhelp('WBHELP.DOC');
        cmASMHelp: openhelp('C--ASM.DOC');
        cmNew: openeditor('NAMELESS.C--',TRUE);
        cmSetRunParams: setrunparams;
        cmAbout: About;
        cmCompilerOptions: CompilerOptions;
        cmViewOutput:  ViewOutput;
        cmSetRunFile:  SetRunFile;
        cmSetCompileFile:  SetCompileFile;
        cmBlank:  BlankScreen;
        cmColor1:  NewPalette(MyColors);
        cmColor2:  NewPalette(MyColors2);
        cmColor3:  NewPalette(MyColors3);
        cmChangeMode: BEGIN
                        NewMode := ScreenMode xor smFont8x8;
                        IF NewMode and smFont8x8 <> 0 THEN
                          BEGIN
                            ShadowSize.X := 1;
                            videomode := vm_text50;
                          END
                        ELSE BEGIN
                            ShadowSize.X := 2;
                            videomode := vm_text25;
                          END;
                        SetScreenMode(NewMode);
                      END;
        cmTile: Tile;
        cmCascade: Cascade;
        cmCalculator: Calculator;
        cmResetSettings: ResetSettings;
      else
        Exit;
      END;
  else
    Exit;
  END;
  ClearEvent(Event);
END;

Function TEditorApp.GetPalette : PPalette;
 BEGIN
   GetPalette := @ThePalette;
 END;

procedure TEditorApp.InitMenuBar;
var
 R: TRect;
BEGIN
  GetExtent(R);
  R.B.Y := R.A.Y + 1;
  MenuBar := New(PMenuBar, Init(R, NewMenu(
    NewSubMenu('~F~ile', hcNoContext, NewMenu(
      NewItem('~N~ew', 'Shift-F3', kbSHIFTF3, cmNew, hcNoContext,
      NewItem('~O~pen...', 'F3', kbF3, cmOpen, hcNoContext,
      NewItem('~S~ave', 'F2', kbF2, cmSave, hcNoContext,
      NewItem('Save ~a~s...', 'Shift-F2', kbSHIFTF2, cmSaveAs, hcNoContext,
      NewLine(
      NewItem('~C~hange dir...', 'Alt-P', kbAltP, cmChangeDir, hcNoContext,
      NewItem('~D~OS shell', 'Alt-D', kbAltD, cmDosShell, hcNoContext,
      NewItem('~F~ull DOS shell', 'Alt-Q', kbAltQ, cmFullDosShell, hcNoContext,
      NewLine(
      NewItem('~E~xit', 'Alt-X', kbAltX, cmQuit, hcNoContext,
      nil))))))))))),
    NewSubMenu('~E~dit', hcNoContext, NewMenu(
      NewItem('~U~ndo', 'Alt-U', kbAltU, cmUndo, hcNoContext,
      NewLine(
      NewItem('Cu~t~', 'Shift-Del', kbShiftDel, cmCut, hcNoContext,
      NewItem('~C~opy', 'Ctrl-Ins', kbCtrlIns, cmCopy, hcNoContext,
      NewItem('~P~aste', 'Shift-Ins', kbShiftIns, cmPaste, hcNoContext,
      NewLine(
      NewItem('C~l~ear', 'Ctrl-Del', kbCtrlDel, cmClear, hcNoContext,
      nil)))))))),
    NewSubMenu('~S~earch', hcNoContext, NewMenu(
      NewItem('~F~ind...', 'Shift-F9', kbShiftF9, cmFind, hcNoContext,
      NewItem('~R~eplace...', 'Ctrl-F9', kbCtrlF9, cmReplace, hcNoContext,
      NewItem('~S~earch again', 'F9', kbF9, cmSearchAgain, hcNoContext,
      nil)))),
    NewSubMenu('~W~indows', hcNoContext, NewMenu(
      NewItem('~S~ize/move','Alt-F7',  kbAltF7,   cmResize,  hcNoContext,
      NewItem('~Z~oom',     'F7',      kbF7,      cmZoom,    hcNoContext,
      NewItem('~T~ile',     'Shift-F7', kbShiftF7, cmTile,   hcNoContext,
      NewItem('Casca~d~e',  'Ctrl-F7', kbCtrlF7,  cmCascade, hcNoContext,
      NewItem('~N~ext',     'F6',      kbF6,      cmNext,    hcNoContext,
      NewItem('~P~revious', 'Shift-F6', kbShiftF6, cmPrev,   hcNoContext,
      NewItem('~C~lose',    'Alt-F4',  kbAltF4,   cmClose,   hcNoContext,
      NewLine(
      NewItem('C~l~ipboard', 'Alt-L', kbAltL, cmShowClip,  hcNoContext,
      NewItem('~A~SCII Table', 'Alt-A', kbAltA, cmCharTable, hcNoContext,
      NewItem('Calc~u~lator', 'Alt-K', kbAltK, cmCalculator, hcNoContext,
      nil)))))))))))),
    NewSubMenu('~R~un', hcNoContext, NewMenu(
      NewItem('~C~ompile & Run', 'F5', kbF5, cmCompileAndRun, hcNoContext,
      NewItem('~J~ust Execute', 'Shift-F4', kbShiftF4, cmRun, hcNoContext,
      NewLine(
      NewItem('~S~et Run File', 'Ctrl-F5', kbCtrlF5, cmSetRunFile, hcNoContext,
      NewItem('Run ~P~arameters','Alt-F5',kbAltF5,cmsetrunparams,hcNoContext,
      nil)))))),
    NewSubMenu('~C~ompiler', hcNoContext, NewMenu(
      NewItem('~C~ompile', 'Shift-F5', kbShiftF5, cmCompile, hcNoContext,
      NewLine(
      NewItem('~S~et Compile File', 'Shift-F8', kbShiftF8, cmSetCompileFile, hcNoContext,
      NewLine(
      NewItem('~V~iew Output Screen', 'F4', kbF4, cmViewOutput, hcNoContext,
      nil)))))),
    NewSubMenu('~O~ptions', hcNoContext, NewMenu(
      NewItem('~V~ideo Mode','Alt-V', kbAltV,cmChangeMode,hcNoContext,
      NewItem('Colour Scheme ~1~','Alt-1', kbAlt1,cmColor1,hcNoContext,
      NewItem('Colour Scheme ~2~','Alt-2', kbAlt2,cmColor2,hcNoContext,
      NewItem('Colour Scheme ~3~','Alt-3', kbAlt3,cmColor3,hcNoContext,
      NewLine(
      NewItem('~O~ptions of Compiler', 'F8', kbF8, cmCompilerOptions, hcNoContext,
      NewLine(
      NewItem('~R~eset all settings','Ctrl-F1', kbCtrlF1,cmResetSettings,hcNoContext,
      nil))))))))),
    NewSubMenu('~H~ELP', hcNoContext, NewMenu(
      NewItem('~W~ork Bench Help', 'F1', kbF1, cmEditHelp, hcNoContext,
      NewItem('~M~ain General Help', 'Shift-F1', kbShiftF1, cmMainHelp, hcNoContext,
      NewItem('~A~ll Procedure List', 'F10', kbF10, cmallProcHelp, hcNoContext,
      NewItem('~S~tack Procedure List', 'Shift-F10', kbShiftF10, cmStackProcHelp, hcNoContext,
      NewItem('~R~EG Procedure List', 'Ctrl-F10', kbCtrlF10, cmREGProcHelp, hcNoContext,
      NewItem('~I~nline ASM Help', 'Alt-F10', kbAltF10, cmASMHelp, hcNoContext,
      nil))))))),
    NewSubMenu(sphinx, hcNoContext, NewMenu(
      NewItem('~A~bout C-- Work Bench', 'Alt-F1', kbAltF1, cmAbout, hcNoContext,
     nil)),
    nil))))))))))));
END;


procedure TEditorApp.InitStatusLine;
var
  R: TRect;
BEGIN
  GetExtent(R);
  R.A.Y := R.B.Y - 1;
  New(StatusLine, Init(R,
    NewStatusDef(0, $FFFF,
      NewStatusKey('Save', kbF2, cmSave,
      NewStatusKey('Open', kbF3, cmOpen,
      NewStatusKey('Close', kbAltF4, cmClose,
      NewStatusKey('Output', kbF4, cmViewOutput,
      NewStatusKey('Compile&Run', kbF5, cmCompileAndRun,
      NewStatusKey('Compile', kbShiftF5, cmCompile,
      NewStatusKey('Next', kbF6, cmNext,
      NewStatusKey('Previous', kbShiftF6, cmPrev,
      NewStatusKey('Shell', kbAltD, cmDOSshell,
      NewStatusKey('EXIT', kbAltX, cmQuit,
      NewStatusKey('~'+#1+'~',kbAltB,cmBlank,
      nil))))))))))),
    nil)));
END;


procedure TEditorApp.OutOfMemory;
BEGIN
  MessageBox('Not Enough Memory For This Operation.',nil,mfError+mfOkButton);
END;


PROCEDURE opengivenfiles;
VAR
    dir : DirStr;
    name : NameStr;
    extension : ExtStr;
    count : word;
    holdstr : STRING[128];
    DirInfo : SearchRec;
BEGIN
  FOR count := 1 TO paramcount DO
    BEGIN
      holdstr := paramstr(count);
      IF holdstr[1] <> '/' THEN
        BEGIN
          fsplit(fexpand(holdstr),dir,name,extension);
          IF extension = '' THEN
              extension := '.C--';
          IF (pos('*',holdstr) > 0) OR
                    (pos('?',holdstr) > 0) THEN
            BEGIN
              FindFirst(dir+name+extension,ANYFILE-DIRECTORY-VOLUMEID-SYSFILE,
                                                       DirInfo);
              WHILE DosError = 0 DO
                BEGIN
                  openeditor(dir+DirInfo.Name,TRUE);
                  FindNext(DirInfo);
                END;
            END
          ELSE openeditor(dir+name+extension,TRUE);
        END;
    END;
END;


PROCEDURE docommandlineopt;
VAR
    count,i : word;
    holdstr : STRING[128];
BEGIN
  FOR count := 1 TO paramcount DO
    BEGIN
      holdstr := paramstr(count);
      IF holdstr[1] = '/' THEN
        BEGIN
          for i := 1 to Length(holdstr) do
              holdstr[i] := UpCase(holdstr[i]);
          WITH CompilerOptData DO
            BEGIN
          IF (holdstr = '/8086') OR (holdstr = '/8088')
                       OR (holdstr = '/0') THEN
              CPUSelect := 0
          ELSE IF (holdstr = '/80186') OR (holdstr = '/1') THEN
              CPUSelect := 1
          ELSE IF (holdstr = '/80286') OR (holdstr = '/2') THEN
              CPUSelect := 2
          ELSE IF (holdstr = '/80386') OR (holdstr = '/3') THEN
              CPUSelect := 3
          ELSE IF (holdstr = '/80486') OR (holdstr = '/4') THEN
              CPUSelect := 4
          ELSE IF (holdstr = '/80586') OR (holdstr = '/5') THEN
              CPUSelect := 5
          ELSE IF (holdstr = '/-ALIGN') OR (holdstr = '/-A') THEN
              CheckOptions := CheckOptions AND (255-AlignWords)
          ELSE IF (holdstr = '/+ALIGN') OR (holdstr = '/+A') THEN
              CheckOptions := CheckOptions OR AlignWords
          ELSE IF (holdstr = '/-ASSUMEDSSS') OR (holdstr = '/-D') THEN
              CheckOptions := CheckOptions AND (255-DSequalsSS)
          ELSE IF (holdstr = '/+ASSUMEDSSS') OR (holdstr = '/+D') THEN
              CheckOptions := CheckOptions OR DSequalsSS
          ELSE IF (holdstr = '/-CTRLC') OR (holdstr = '/-C') THEN
              CheckOptions := CheckOptions AND (255-CTRLCIGNORE)
          ELSE IF (holdstr = '/+CTRLC') OR (holdstr = '/+C') THEN
              CheckOptions := CheckOptions OR CTRLCIGNORE
          ELSE IF (holdstr = '/-MAP') OR (holdstr = '/-M') THEN
              CheckOptions := CheckOptions AND (255-MAPFILE)
          ELSE IF (holdstr = '/+MAP') OR (holdstr = '/+M') THEN
              CheckOptions := CheckOptions OR MAPFILE
          ELSE IF (holdstr = '/-PARSE') OR (holdstr = '/-P') THEN
              CheckOptions := CheckOptions AND (255-PARSECOMLINE)
          ELSE IF (holdstr = '/+PARSE') OR (holdstr = '/+P') THEN
              CheckOptions := CheckOptions OR PARSECOMLINE
          ELSE IF (holdstr = '/-RESIZE') OR (holdstr = '/-R') THEN
              CheckOptions := CheckOptions AND (255-RESIZEMEM)
          ELSE IF (holdstr = '/+RESIZE') OR (holdstr = '/+R') THEN
              CheckOptions := CheckOptions OR RESIZEMEM
          ELSE IF (holdstr = '/SIZE') OR (holdstr = '/OC') THEN
              OptimizeSpeed := 0
          ELSE IF (holdstr = '/SPEED') OR (holdstr = '/OS') THEN
              OptimizeSpeed := 1
          ELSE IF (holdstr = '/-MAIN') OR (holdstr = '/J0') THEN
              JumpType := 0
          ELSE IF (holdstr = '/SHORTMAIN') OR (holdstr = '/J1') THEN
              JumpType := 1
          ELSE IF (holdstr = '/+MAIN') OR (holdstr = '/J2') THEN
              JumpType := 2
          ELSE IF (holdstr = '/EXE') OR (holdstr = '/E') THEN
              OutputType := 1
          ELSE IF holdstr = '/OBJ' THEN
              OutputType := 2
          ELSE IF holdstr = '/25' THEN
              VideoMode := vm_text25
          ELSE IF holdstr = '/28' THEN
              VideoMode := vm_text28
          ELSE IF holdstr = '/43' THEN
              VideoMode := vm_text43
          ELSE IF holdstr = '/50' THEN
              VideoMode := vm_text50
          ELSE IF (holdstr = '/HELP') OR (holdstr = '/?') THEN
            BEGIN
              WriteLn;
              WriteLn('Some of The Command Line Options For C-- Work Bench:');
              WriteLn('/25           << force editor display to 25 lines (VGA or greater required.)');
              WriteLn('/28           << set editor display to 28 lines (VGA or greater required.)');
              WriteLn('/43           << set editor display to 43 lines (EGA required.)');
              WriteLn('/50           << set editor display to 50 lines (VGA or greater required.)');
              WriteLn('/8086         << restrict code to 8086 instructions (default)');
              WriteLn('/80286        << enable 80286 code optimizations');
              WriteLn('/80386        << enable 80386 code optimizations');
              WriteLn('/80486        << enable 80486 code optimizations');
              WriteLn('/-ALIGN       << disable even word address alignment');
              WriteLn('/+ASSUMEDSSS  << enable assumption of DS=SS optimization');
              WriteLn('/EXE          << produce EXE run file (almost available)');
              WriteLn('/+MAP         << display list of all identifiers');
              WriteLn('/-MAIN        << disable initial jump to main()');
              WriteLn('/+MAIN        << near jump to main() (default)');
              WriteLn('/OBJ          << produce OBJ output file');
              WriteLn('/-PARSE       << do not include parse command line code (default)');
              WriteLn('/+PARSE       << include parse command line code');
              WriteLn('/-RESIZE      << do not resize prog memory to 64K');
              WriteLn('/+RESIZE      << resize prog memory to 64K (default)');
              WriteLn('/SHORTMAIN    << short jump to main()');
              WriteLn('/SIZE         << optimize for code size');
              WriteLn('/SPEED        << optimize for speed (default)');
              HALT;
            END
          ELSE ExtraOptions := ExtraOptions + ' ' + holdstr;
            END;  { of WITH }
        END
      ELSE
        BEGIN
          TransFileName := '';  (* forget about the config edit file *)
        END;
    END;
END;




FUNCTION getdriverinfo : BOOLEAN;
VAR byte1,byte2,byte3,byte4,byte5,byte6 : byte;
BEGIN
  ASM
      MOV AH,'S'
      MOV AL,'P'
      MOV BH,'H'
      MOV BL,'I'
      MOV CH,'N'
      MOV CL,'X'
      MOV DH,3   { get driver buffer address }
      PUSH DI
      PUSH SI
      INT 18h
      MOV byte1,AL
      MOV byte2,AH
      MOV byte3,BL
      MOV byte4,BH
      MOV byte5,CL
      MOV byte6,CH
      MOV driverseg,ES
      MOV driveroff,DI
      POP SI
      POP DI
    END;
  IF byte1 = ord('s') THEN
      IF byte2 = ord('p') THEN
          IF byte3 = ord('h') THEN
              IF byte4 = ord('i') THEN
                  IF byte5 = ord('n') THEN
                      IF byte6 = ord('x') THEN
                        BEGIN
                          getdriverinfo := TRUE;
                          exit;
                        END;
  getdriverinfo := FALSE;
END;


VAR
    year,month,day,dayofweek : word;
    cfgloadflag : BYTE;

BEGIN
  thepalette := mycolors;
  videomode := vm_text25;
  transfilename := '';
  wbpath := GETENV('C--');
  IF wbpath = '' THEN
      GetDir(0,wbpath);
  IF wbpath[length(wbpath)-1] <> '\' THEN
      wbpath := wbpath + '\';
  ResetSettings;
  IF getdriverinfo = FALSE THEN
    BEGIN
      WriteLn('Driver Program WB.COM not active!');
      WriteLn('Run WB.COM not WB.EXE.');
      HALT;
    END;
  getdate(year,month,day,dayofweek);
  mycolors[1] := (((month*dayofweek) MOD 8) * 16) + (day MOD 16);
  if thepalette[2] = Brown*16+LightGray then
      thepalette[1] := mycolors[1];

  cfgloadflag := LoadConfig;
  docommandlineopt;

  FOR count := 1 TO 21 DO
      sphinx[count] := CHR( ORD(sphinx[count]) - 4 );

  EditorApp.Init;
  RegisterCalc;
  IF cfgloadflag = cfg_missing THEN
      MessageBox('Configuration File Is Missing.  Assuming default configuration.',nil,mferror+mfOkButton)
  ELSE IF cfgloadflag = cfg_bad THEN
      MessageBox('Error Reading Configuration File.  Assuming default configuration.',nil,mferror+mfOkButton);

  IF (videomode = vm_text43) OR (videomode = vm_text50) THEN
    BEGIN
      ShadowSize.X := 1;
      EditorApp.SetScreenMode(ScreenMode OR smFont8x8);
    END;

  IF (TransFileName = '') AND (ParamCount = 0) THEN
      About
  ELSE
    BEGIN
      IF Transfilename <> '' THEN
          OpenEditor(Transfilename,TRUE);
      opengivenfiles;
    END;
  EditorApp.Run;
  EditorApp.Done;
END.


{ END OF WB.PAS }