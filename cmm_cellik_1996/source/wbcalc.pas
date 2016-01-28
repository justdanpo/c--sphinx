{******************************************************}
{   Turbo Vision Calc modified by SPHINX programming   }
{   Used for SPHINX C-- work bench                     }
{******************************************************}

UNIT WBCalc;

{$F+,S-,D-}

INTERFACE

USES Drivers, Objects, Views, Dialogs;

TYPE

  TCalcState = (csFirst, csValid, csError);

  PCalcDisplay = ^TCalcDisplay;
  TCalcDisplay = object(TView)
    Status: TCalcState;
    Number: string[15];
    Sign: Char;
    Operator: Char;
    Operand: Real;
    CONSTRUCTOR Init(var Bounds: TRect);
    CONSTRUCTOR Load(var S: TStream);
    PROCEDURE CalcKey(Key: Char);
    PROCEDURE Clear;
    PROCEDURE Draw; virtual;
    function GetPalette: PPalette; virtual;
    PROCEDURE HandleEvent(var Event: TEvent); virtual;
    PROCEDURE Store(var S: TStream);
  END;

  PCalculator = ^TCalculator;
  TCalculator = object(TDialog)
    CONSTRUCTOR Init;
  END;

CONST
  RCalcDisplay: TStreamRec = (
     ObjType: 10040;
     VmtLink: Ofs(TypeOf(TCalcDisplay)^);
     Load:    @TCalcDisplay.Load;
     Store:   @TCalcDisplay.Store
  );
  RCalculator: TStreamRec = (
     ObjType: 10041;
     VmtLink: Ofs(TypeOf(TCalculator)^);
     Load:    @TCalculator.Load;
     Store:   @TCalculator.Store
  );

PROCEDURE RegisterCalc;


IMPLEMENTATION

CONST
  cmCalcButton = 100;

CONSTRUCTOR TCalcDisplay.Init(var Bounds: TRect);
BEGIN
  TView.Init(Bounds);
  Options := Options or ofSelectable;
  EventMask := evKeyDown + evBroadcast;
  Clear;
END;

CONSTRUCTOR TCalcDisplay.Load(var S: TStream);
BEGIN
  TView.Load(S);
  S.Read(Status, SizeOf(Status) + SizeOf(Number) + SizeOf(Sign) +
    SizeOf(Operator) + SizeOf(Operand));
END;


PROCEDURE TCalcDisplay.CalcKey(Key: Char);
var
  R: Real;

PROCEDURE Error;
BEGIN
  Status := csError;
  Number := 'Error';
  Sign := ' ';
END;

PROCEDURE SetDisplay(R: Real);
var
  S: string[63];
BEGIN
  Str(R: 0: 10, S);
  if S[1] <> '-' then Sign := ' ' else
  BEGIN
    Delete(S, 1, 1);
    Sign := '-';
  END;
  IF Length(S) > 15 + 1 + 10 THEN
      Error
  ELSE BEGIN
      while S[Length(S)] = '0' do Dec(S[0]);
      if S[Length(S)] = '.' then Dec(S[0]);
      Number := S;
    END;
END;

PROCEDURE GetDisplay(var R: Real);
VAR
  E: Integer;
BEGIN
  Val(Sign + Number, R, E);
END;

PROCEDURE CheckFirst;
BEGIN
  IF Status = csFirst THEN
    BEGIN
      Status := csValid;
      Number := '0';
      Sign := ' ';
    END;
END;

BEGIN
  Key := UpCase(Key);
  if (Status = csError) and (Key <> 'C') then Key := ' ';
  case Key of
    'C':  Clear;
    '0'..'9':  BEGIN
                 CheckFirst;
                 if Number = '0' then
                     Number := '';
                 Number := Number + Key;
               END;
    '.':  BEGIN
            CheckFirst;
            IF Pos('.', Number) = 0 THEN
                Number := Number + '.';
          END;
    #8, #27:  BEGIN
                CheckFirst;
                IF Length(Number) = 1 THEN
                    Number := '0'
                ELSE Dec(Number[0]);
              END;
    '_', #241:  BEGIN
                  IF Sign = ' ' THEN
                      Sign := '-'
                  ELSE Sign := ' ';
                END;
    '+', '-', '*', '/', '=', '%', #13:
      BEGIN
        if (Status = csValid) then
          BEGIN
            Status := csFirst;
            GetDisplay(R);
            IF Key = '%' THEN
              BEGIN
                CASE Operator of
                  '+', '-': R := Operand * R / 100;
                  '*', '/': R := R / 100;
                 END;
              END;
            CASE Operator of
              '+': SetDisplay(Operand + R);
              '-': SetDisplay(Operand - R);
              '*': SetDisplay(Operand * R);
              '/': BEGIN
                     IF R = 0 THEN
                         Error
                     ELSE SetDisplay(Operand / R);
                   END;
             END;
          END;
        IF key <> '=' THEN
          BEGIN
            Operator := Key;
            GetDisplay(Operand);
          END;
      END;
  END;
  DrawView;
END;


PROCEDURE TCalcDisplay.Clear;
BEGIN
  Status := csFirst;
  Number := '0';
  Sign := ' ';
  Operator := '=';
END;

PROCEDURE TCalcDisplay.Draw;
var
  Color: Byte;
  I: Integer;
  B: TDrawBuffer;
BEGIN
  Color := GetColor(1);
  I := Size.X - Length(Number) - 2;
  MoveChar(B, ' ', Color, Size.X);
  MoveChar(B[I], Sign, Color, 1);
  MoveStr(B[I + 1], Number, Color);
  WriteBuf(0, 0, Size.X, 1, B);
END;

function TCalcDisplay.GetPalette: PPalette;
const
  P: string[1] = #19;
BEGIN
  GetPalette := @P;
END;

PROCEDURE TCalcDisplay.HandleEvent(var Event: TEvent);
BEGIN
  TView.HandleEvent(Event);
  case Event.What of
    evKeyDown:
      BEGIN
        CalcKey(Event.CharCode);
        ClearEvent(Event);
      END;
    evBroadcast:
      if Event.Command = cmCalcButton then
      BEGIN
        CalcKey(PButton(Event.InfoPtr)^.Title^[1]);
        ClearEvent(Event);
      END;
  END;
END;

PROCEDURE TCalcDisplay.Store(var S: TStream);
BEGIN
  TView.Store(S);
  S.Write(Status, SizeOf(Status) + SizeOf(Number) + SizeOf(Sign) +
    SizeOf(Operator) + SizeOf(Operand));
END;

{ TCalculator }

CONSTRUCTOR TCalculator.Init;
const
  KeyChar: array[0..19] of Char = 'C'#27'%'#241'789/456*123-0.=+';
var
  I: Integer;
  P: PView;
  R: TRect;
BEGIN
  R.Assign(5, 3, 29, 18);
  TDialog.Init(R, 'Calculator');
  Options := Options or ofFirstClick;
  for I := 0 to 19 do
  BEGIN
    R.A.X := (I mod 4) * 5 + 2;
    R.A.Y := (I div 4) * 2 + 4;
    R.B.X := R.A.X + 5;
    R.B.Y := R.A.Y + 2;
    P := New(PButton, Init(R, KeyChar[I], cmCalcButton,
      bfNormal + bfBroadcast));
    P^.Options := P^.Options and not ofSelectable;
    Insert(P);
  END;
  R.Assign(3, 2, 21, 3);
  Insert(New(PCalcDisplay, Init(R)));
END;

PROCEDURE RegisterCalc;
BEGIN
  RegisterType(RCalcDisplay);
  RegisterType(RCalculator);
END;

END.

{ end of WBCALC.PAS }