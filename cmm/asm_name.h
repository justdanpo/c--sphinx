char* Mnemonics[] =
{
	// FIRST OF ALL THE COMMANDS WITH A P-FLAG. THIS"LL MAKE THINGS EASIER FOR
	// COMPARISON IN THE PARSE ENGINE
	//P1=0-7
	"ADD", "OR", "ADC", "SBB/BC", "AND", "SUB", "XOR", "CMP",
	//P2=4-7
	"NOT", "NEG", "MUL", "-", "DIV", "IDIV",
	//			IMUL ENTFERNT
	//P3=0-5/7
	"ROL", "ROR", "RCL", "RCR", "SHL/AL", "SHR", "-", "SAR",
	//P4=4-7
	"BT", "BTS", "BTR", "BTC",
	// USUAL COMMANDS
	"INC", "DEC", "TEST", "IMUL", "SHLD", "SHRD",
	"DAA", "DAS", "AAA", "AAS", "AAM", "AAD",
	"MOVZX", "MOVSX", "CBW", "CWDE", "CWD", "CDQ",
	"BSWAP", "XLAT/LATB", "BSF", "BSR",
	"CMPXCHG", "CMPXCHG8B", "XADD",
	"NOP", "WAIT", "LOCK", "HLT/ALT", "INT",
	"INTO", "IRET", "IRETD",
	"POPF", "POPFD", "PUSHF", "PUSHFD", "SAHF", "LAHF",
	"CMC", "CLC", "STC", "CLI", "STI", "CLD", "STD",
	"PUSH", "PUSHA", "PUSHAD", "POP", "POPA", "POPAD",
	"XCHG", "MOV", "LEA",
	"LFS", "LGS", "LSS",
	"LES", "LDS",
	"ADRSIZE",
	"IN", "OUT", "INSB", "INSW", "INSD", "OUTSB", "OUTSW", "OUTSD",
	"MOVSB", "MOVSW", "MOVSD", "CMPSB", "CMPSW", "CMPSD",
	"STOSB", "STOSW", "STOSD", "LODSB", "LODSW", "LODSD",
	"SCASB", "SCASW", "SCASD", "REPNE/EPNZ", "REP/EPE/EPZ",
	"JCXZ", "JECXZ", "LOOP/OOPW", "LOOPD", "LOOPZ/OOPE", "LOOPNZ/OOPNE",
	"JO", "JNO", "JC/B/NAE", "JNC/AE/NB",
	"JE/Z", "JNE/NZ", "JBE/NA", "JA/NBE",
	"JS", "JNS", "JP/PE", "JNP/PO", "JL/NGE", "JGE/NL",
	"JLE/NG", "JG/NLE",
	"SETO", "SETNO", "SETC/ETB/ETNAE", "SETNC/ETAE/ETNB",
	"SETE/ETZ", "SETNE/ETNZ", "SETBE/ETNA", "SETA/ETNBE",
	"SETS", "SETNS", "SETP/ETPE", "SETNP/ETPO", "SETL/ETNGE", "SETGE/ETNL",
	"SETLE/ETNG", "SETG/ETNLE",
	/*"JMPS","JMPN","JMPF",*/"JMP",
	"CALL",/*"CALLF",*/"RET", "RETF",
	"ENTER", "LEAVE", "BOUND", "ARPL",
	"SLDT", "STR", "LLDT", "LTR", "VERR", "VERW", "LAR", "LSL",
	"SGDT", "SIDT", "LGDT", "LIDT", "SMSW", "LMSW", "CLTS",
	"INVD", "WBINVD", //"INVLPD",
	//INTEL PENTIUM COMMANDS
	"WRMSR", "CPUID", "RDMSR", "RDTSC", "RSM",
	//INTEL PENTIUM PRO INSTRUCTIONS
	"RDPMC", "UD2", /*"EMMX","SETALC",*/
	//MMX INSTRUCTIONS

	"PUNPCKLBW", "PUNPCKLWD", "PUNPCKLDQ", //UNPACK LOW ORDER 60 -
	"PACKSSWB",
	"PCMPGTB", "PCMPGTW", "PCMPGTD",
	"PACKUSWB",			 //PACK MMX REG WITH UNSIGNED SATURATION
	"PUNPCKHBW", "PUNPCKHWD", "PUNPCKHDQ",		 //UNPACK HIGH ORDER
	"PACKSSDW",		 //PACK MMX REG WITH SIGNED SATURATION  - 6B

	"PSRLW", "PSRLD", "PSRLQ",
	"PSRAW", "PSRAD",
	"PSLLW", "PSLLD", "PSLLQ",

	"PCMPEQB", "PCMPEQW", "PCMPEQD", //74-76
	"PMULLW",	//d5

	"MOVD", "MOVQ",			 //MOVE MMX REG

	"PSUBUSB", "PSUBUSW", //d8-d9

	"EMMS",

	"PAND",	//db
	"PADDUSB", "PADDUSW",		 //" WITH UNSIGNED SATURATION
	"PANDN",	//df
	"PMULHW",	//e5
	"PSUBSB", "PSUBSW",	//e8-e9
	"POR",	//eb
	"PADDSB", "PADDSW",		 //" WITH SIGNED SATURATION
	"PXOR",	//ef
	"PMADDWD",	//f5
	"PSUBB", "PSUBW", "PSUBD", //SUBTRACT MMX REG f8-fa
	"PADDB", "PADDW", "PADDD", //ADD MMX REG WITH WRAP-AROUND fc-fe


	"DB", "DW", "DD", "INVLPG", "LOADALL", "OPSIZE",

	"F2XM1", "FABS", "FADD", "FADDP", "FBLD", "FBSTP", "FCHS", "FCLEX", "FCOM",
	"FCOMP", "FNCLEX", "FCOMPP", "FCOS", "FDECSTP", "FDISI", "FDIV", "FDIVP",
	"FDIVR", "FDIVRP", "FFREE", "FIADD", "FICOM", "FICOMP", "FIDIV", "FIDIVR",
	"FILD", "FILDQ", "FIMUL", "FIST", "FISTP", "FISUB", "FISUBR", "FENI", "FINCSTP",
	"FINIT", "FNINIT", "FLD", "FLDCW", "FLDENV", "FLDLG2", "FLDLN2", "FLDL2E",
	"FLDL2T", "FLDPI", "FLDZ", "FLD1", "FMUL", "FMULP", "FNOP", "FPATAN", "FPREM",
	"FPREM1", "FPTAN", "FRNDINT", "FSETPM", "FRSTOR", "FSAVE", "FNSAVE", "FSCALE",
	"FSIN", "FSINCOS", "FSQRT", "FST", "FSTCW", "FNSTCW", "FSTP", "FSTSW", "FNSTSW",
	"FSTENV", "FNSTENV", "FSUB", "FSUBP", "FSUBR", "FSUBRP", "FTST", "FUCOM", "FUCOMP",
	"FUCOMPP", "FXCH", "FWAIT", "FXAM", "FXTRACT", "FYL2X", "FYL2XP1",
	"SYSENTER", "SYSEXIT", "FCMOVB", "FCMOVE", "FCMOVBE", "FCMOVU", "FCMOVNB",
	"FCMOVNE", "FCMOVNBE", "FCMOVNU", "FCOMI", "FCOMIP", "FUCOMI", "FUCOMIP",
	"FXRSTOR", "FXSAVE", "FNDISI", "FNENI", "FNSETPM",

	"CMOVO", "CMOVNO", "CMOVB/MOVNAE/MOVC", "CMOVAE/MOVNB/MOVNC", "CMOVE/MOVZ",
	"CMOVNE/MOVNZ", "CMOVBE/MOVNA", "CMOVA/MOVNBE", "CMOVS", "CMOVNS",
	"CMOVP/MOVPE", "CMOVNP/MOVPO", "CMOVL/MOVNGE", "CMOVGE/MOVNL",
	"CMOVLE/MOVNG", "CMOVG/MOVNLE",

	//MMX Pentium III extention
	"MASKMOVQ",  "MOVNTQ",    "PAVGB",     "PAVGW",      "PEXTRW",  "PINSRW",
	"PMAXUB",    "PMAXSW",    "PMINUB",    "PMINSW",     "PMOVMSKB", "PMULHUW",
	"PREFETCHT0", "PREFETCHT1", "PREFETCHT2", "PREFETCHNTA", "SFENCE",  "PSADBW",
	"PSHUFW",
	//XMM extentions Pentium III
	"ADDPS",    "ADDSS",   "ANDNPS",  "ANDPS",   "CMPPS",   "CMPSS",
	"COMISS",   "CVTPI2PS", "CVTPS2PI", "CVTSI2SS", "CVTSS2SI", "CVTTPS2PI",
	"CVTTSS2SI", "DIVPS",   "DIVSS",   "LDMXCSR", "MAXPS",   "MAXSS",
	"MINPS",    "MINSS",   "MOVAPS",  "MOVHLPS", "MOVHPS",  "MOVLHPS",
	"MOVLPS",   "MOVMSKPS", "MOVSS",   "MOVUPS",  "MULPS",   "MULSS",
	"MOVNTPS",  "ORPS",    "RCPPS",   "RCPSS",   "RSQRTPS", "RSQRTSS",
	"SHUFPS",   "SQRTPS",  "SQRTSS",  "STMXCSR", "SUBPS",   "SUBSS",
	"UCOMISS",  "UNPCKHPS", "UNPCKLPS", "XORPS",

	// Pentium IV
	"LFENCE",  "MFENCE",    "ADDPD",     "ADDSD",     "ANDPD",     "ANDNPD",
	"CMPPD",   "COMISD",    "CVTDQ2PD",  "CVTDQ2PS",  "CVTPD2DQ",  "CVTPD2PI",
	"CVTPD2PS", "CVTPI2PD",  "CVTPS2DQ",  "CVTPS2PD",  "CVTSD2SI",  "CVTSD2SS",
	"CVTSI2SD", "CVTSS2SD",  "CVTTPD2PI", "CVTTPD2DQ", "CVTTPS2DQ", "CVTTSD2SI",
	"DIVPD",   "DIVSD",     "MASKMOVDQU", "MAXPD",     "MAXSD",     "MINPD",
	"MINSD",   "MOVAPD",    "MOVDQA",    "MOVDQU",    "MOVDQ2Q",   "MOVHPD",
	"MOVLPD",  "MOVMSKPD",  "MOVNTDQ",   "MOVNTPD",   "MOVNTI",    "MOVQ2DQ",
	"MOVUPD",  "MULPD",     "MULSD",     "ORPD",      "PSHUFD",    "PSHUFHW",
	"PSHUFLW", "PSLLDQ",    "PSRLDQ",    "SHUFPD",    "SQRTPD",    "SQRTSD",
	"SUBPD",   "SUBSD",     "UCOMISD",   "UNPCKHPD",  "UNPCKLPD",  "XORPD",
	"PADDQ",   "PMULUDQ",   "PSUBQ",     "PUNPCKHQDQ", "PUNPCKLQDQ", "CLFLUSH",
	"MONITOR", "MWAIT",     "ADDSUBPD",  "ADDSUBPS",  "CMPEQSD",   "CMPLTSD",
	"CMPLESD", "CMPUNORDSD", "CMPNEQSD",  "CMPNLTSD",  "CMPNLESD",  "CMPORDSD",
	"CMPEQPD", "CMPLTPD",   "CMPLEPD",   "CMPUNORDPD", "CMPNEQPD",  "CMPNLTPD",
	"CMPNLEPD", "CMPORDPD",  "CMPEQPS",   "CMPLTPS",   "CMPLEPS",   "CMPUNORDPS",
	"CMPNEQPS", "CMPNLTPS",  "CMPNLEPS",  "CMPORDPS",  "CMPEQSS",   "CMPLTSS",
	"CMPLESS", "CMPUNORDSS", "CMPNEQSS",  "CMPNLTSS",  "CMPNLESS",  "CMPORDSS",
	"HADDPD",  "HADDPS",    "HSUBPD",    "HSUBPS",    "LDDQU",     "MOVDDUP",
	"MOVSHDUP", "MOVSLDUP",  "PAUSE",

	NULL
};

