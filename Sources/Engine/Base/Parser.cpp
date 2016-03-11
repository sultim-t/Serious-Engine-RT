
/*  A Bison parser, made from engine/base/parser.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	c_float	258
#define	c_int	259
#define	c_string	260
#define	c_char	261
#define	identifier	262
#define	k_INDEX	263
#define	k_FLOAT	264
#define	k_CTString	265
#define	k_void	266
#define	k_const	267
#define	k_user	268
#define	k_persistent	269
#define	k_extern	270
#define	k_pre	271
#define	k_post	272
#define	k_help	273
#define	k_if	274
#define	k_else	275
#define	k_else_if	276
#define	SHL	277
#define	SHR	278
#define	EQ	279
#define	NEQ	280
#define	GEQ	281
#define	LEQ	282
#define	LOGAND	283
#define	LOGOR	284
#define	block_beg	285
#define	block_end	286
#define	TYPECAST	287
#define	SIGN	288

#line 1 "engine/base/parser.y"

#include "StdH.h"

#include <Engine/Base/Console.h>
#include <Engine/Base/Shell.h>
#include "ParsingSymbols.h"

#include <Engine/Templates/DynamicStackArray.cpp>
#include <Engine/Templates/AllocationArray.cpp>

#line 13 "engine/base/parser.y"

#define YYERROR_VERBOSE 1
// if error occurs in parsing
void yyerror(char *str)
{
  // just report the string
  _pShell->ErrorF("%s", str);
};

static BOOL _bExecNextElse = FALSE;
FLOAT fDummy = -666.0f;

static INDEX _iStack = 0;
static UBYTE _ubStack[1024];

INDEX PushExpression(value &v)
{
  if (v.sttType==STT_FLOAT) {
    FLOAT f = v.fFloat;
    memcpy(_ubStack+_iStack, &f, sizeof(f));
    _iStack+=sizeof(f);
    return sizeof(f);
  } else if (v.sttType==STT_INDEX) {
    INDEX i = v.iIndex;
    memcpy(_ubStack+_iStack, &i, sizeof(i));
    _iStack+=sizeof(i);
    return sizeof(i);
  } else if (v.sttType==STT_STRING) {
    CTString &str = _shell_astrTempStrings.Push();
    str = v.strString;
    CTString *pstr = &str;
    memcpy(_ubStack+_iStack, &pstr, sizeof(pstr));
    _iStack+=sizeof(pstr);
    return sizeof(pstr);
  } else {
    return 0;
  }
}

BOOL MatchTypes(value &v1, value &v2)
{
  if (v1.sttType==STT_FLOAT && v2.sttType==STT_FLOAT) {
    return TRUE;
  } else if (v1.sttType==STT_STRING && v2.sttType==STT_STRING) {
    return TRUE;
  } else if (v1.sttType==STT_INDEX && v2.sttType==STT_INDEX) {
    return TRUE;
  } else {
    v1.sttType = STT_ILLEGAL;
    v2.sttType = STT_ILLEGAL;
    _pShell->ErrorF("Type mismatch");
    return FALSE;
  }
}

void Declaration(
  ULONG ulQualifiers, INDEX istType, CShellSymbol &ssNew,
  INDEX (*pPreFunc)(INDEX), void (*pPostFunc)(INDEX))
{
  // if external
  if (ulQualifiers&SSF_EXTERNAL) {
    // get it a new value
    if (_shell_ast[istType].st_sttType==STT_INDEX
      ||_shell_ast[istType].st_sttType==STT_FLOAT) {
      _pvNextToDeclare = &_shell_afExtFloats.Push();
    } else if (_shell_ast[istType].st_sttType==STT_STRING) {
      _pvNextToDeclare = &_shell_astrExtStrings.Push();
    } else {
      NOTHING;
    }
  }

  // if not parsing an external declaration
  if (_pvNextToDeclare==NULL) {
    // error
    _pShell->ErrorF("Only external declarations are supported");
    return;
  }

  // if the symbol is declared already
  if (ssNew.IsDeclared()) {
    // if the declaration is not identical
    if (!ShellTypeIsSame(ssNew.ss_istType, istType) || 
      ((ssNew.ss_ulFlags&SSF_CONSTANT)!=(ulQualifiers&SSF_CONSTANT))) {
      // error
      _pShell->ErrorF("Symbol '%s' is already declared diferrently", ssNew.ss_strName);
      return;
    }

    // copy its value
    if (_shell_ast[ssNew.ss_istType].st_sttType==STT_INDEX) {
      *(INDEX*)_pvNextToDeclare = *(INDEX*)ssNew.ss_pvValue;
    } else if (_shell_ast[ssNew.ss_istType].st_sttType==STT_FLOAT) {
      *(FLOAT*)_pvNextToDeclare = *(FLOAT*)ssNew.ss_pvValue;
    } else if (_shell_ast[ssNew.ss_istType].st_sttType==STT_STRING) {
      *(CTString*)_pvNextToDeclare = *(CTString*)ssNew.ss_pvValue;
    } else if (_shell_ast[ssNew.ss_istType].st_sttType==STT_ARRAY) {
      NOTHING;  // array values are not retained
    } else if (_shell_ast[ssNew.ss_istType].st_sttType==STT_FUNCTION) {
      NOTHING;  // function values are not retained
    } else {
      // error
      _pShell->ErrorF("'%s': old value couldn't be retained", ssNew.ss_strName);
      return;
    }
  }

  // set the type to given type
  if (!ssNew.IsDeclared()) {
    ssNew.ss_istType = ShellTypeMakeDuplicate(istType);
  }
  // set the value for the external symbol if not already set
  if (ssNew.ss_pvValue==NULL || !(ulQualifiers&SSF_EXTERNAL)) {
    ssNew.ss_pvValue = _pvNextToDeclare;
  }
  // remember qualifiers (if already predeclared - keep old flags)
  ssNew.ss_ulFlags |= ulQualifiers;
  // remember pre and post functions
  if (ssNew.ss_pPreFunc==NULL) {
    ssNew.ss_pPreFunc = (BOOL (*)(void *))pPreFunc;
  }
  if (ssNew.ss_pPostFunc==NULL) {
    ssNew.ss_pPostFunc = (void (*)(void *))pPostFunc;
  }
}

void DoComparison(value &vRes, value &v0, value &v1, int token)
{
  MatchTypes(v0, v1);

  vRes.sttType = STT_INDEX;
  if (v0.sttType == STT_FLOAT) {
    switch (token) {
    case '<': vRes.iIndex = v0.fFloat <v1.fFloat; break;
    case '>': vRes.iIndex = v0.fFloat >v1.fFloat; break;
    case '=': vRes.iIndex = v0.fFloat==v1.fFloat; break;
    case '!': vRes.iIndex = v0.fFloat!=v1.fFloat; break;
    case '}': vRes.iIndex = v0.fFloat>=v1.fFloat; break;
    case '{': vRes.iIndex = v0.fFloat<=v1.fFloat; break;
    default: ASSERT(FALSE);
      vRes.sttType = STT_INDEX;
      vRes.iIndex = 0;
    }
  } else if (v0.sttType == STT_INDEX) {
    switch (token) {
    case '<': vRes.iIndex = v0.iIndex <v1.iIndex; break;
    case '>': vRes.iIndex = v0.iIndex >v1.iIndex; break;
    case '=': vRes.iIndex = v0.iIndex==v1.iIndex; break;
    case '!': vRes.iIndex = v0.iIndex!=v1.iIndex; break;
    case '}': vRes.iIndex = v0.iIndex>=v1.iIndex; break;
    case '{': vRes.iIndex = v0.iIndex<=v1.iIndex; break;
    default: ASSERT(FALSE);
      vRes.sttType = STT_INDEX;
      vRes.iIndex = 0;
    }
  } else if (v0.sttType == STT_STRING) {
    switch (token) {
    case '<': vRes.iIndex = stricmp(v0.strString, v1.strString)  < 0; break;
    case '>': vRes.iIndex = stricmp(v0.strString, v1.strString)  > 0; break;
    case '=': vRes.iIndex = stricmp(v0.strString, v1.strString) == 0; break;
    case '!': vRes.iIndex = stricmp(v0.strString, v1.strString) != 0; break;
    case '}': vRes.iIndex = stricmp(v0.strString, v1.strString) >= 0; break;
    case '{': vRes.iIndex = stricmp(v0.strString, v1.strString) <= 0; break;
    default: ASSERT(FALSE);
      vRes.sttType = STT_INDEX;
      vRes.iIndex = 0;
    }
  } else {
    vRes.sttType = STT_INDEX;
    vRes.iIndex = 0;
  }
}

#line 192 "engine/base/parser.y"
typedef union {
  value val;                  // for constants and expressions
  arguments arg;               // for function input arguments
  ULONG ulFlags;              // for declaration qualifiers
  INDEX istType;              // for types
  CShellSymbol *pssSymbol;    // for symbols
  struct LValue lvLValue;
  INDEX (*pPreFunc)(INDEX);  // pre-set function for a variable
  void (*pPostFunc)(INDEX); // post-set function for a variable
} YYSTYPE;
#line 203 "engine/base/parser.y"

  extern int yylex(YYSTYPE *lvalp);

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		156
#define	YYFLAG		-32768
#define	YYNTBASE	53

#define YYTRANSLATE(x) ((unsigned)(x) <= 288 ? yytranslate[x] : 75)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    45,     2,     2,     2,    42,    33,     2,    49,
    50,    40,    39,    47,    38,     2,    41,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    46,    48,    36,
    32,    37,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    51,     2,    52,    34,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    35,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    43,    44
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     8,    12,    13,    16,    17,    20,    23,
    26,    29,    30,    32,    34,    36,    38,    40,    41,    45,
    46,    50,    51,    53,    55,    59,    67,    76,    87,    89,
    91,    94,    99,   106,   109,   114,   119,   120,   121,   130,
   131,   132,   133,   142,   143,   147,   149,   154,   155,   157,
   159,   163,   165,   167,   169,   171,   175,   179,   183,   187,
   191,   195,   199,   203,   207,   211,   215,   219,   223,   227,
   231,   235,   239,   243,   246,   249,   252,   257,   262,   267,
   272
};

static const short yyrhs[] = {    63,
     0,    55,     0,    30,    55,    31,     0,    30,    55,    31,
     0,     0,    64,    55,     0,     0,    56,    12,     0,    56,
    13,     0,    56,    14,     0,    56,    15,     0,     0,     5,
     0,     9,     0,     8,     0,    10,     0,    11,     0,     0,
    16,    46,     7,     0,     0,    17,    46,     7,     0,     0,
    62,     0,    58,     0,    62,    47,    58,     0,    56,    58,
     7,    59,    60,    57,    48,     0,    56,    58,     7,    49,
    61,    50,    57,    48,     0,    56,    58,     7,    51,    74,
    52,    59,    60,    57,    48,     0,    48,     0,    54,     0,
    74,    48,     0,    71,    32,    74,    48,     0,    56,    58,
     7,    32,    74,    48,     0,    18,     7,     0,    18,     7,
    49,    50,     0,    18,     7,    51,    52,     0,     0,     0,
    19,    49,    74,    50,    65,    54,    66,    67,     0,     0,
     0,     0,    21,    49,    74,    50,    68,    54,    69,    67,
     0,     0,    20,    70,    54,     0,     7,     0,     7,    51,
    74,    52,     0,     0,    73,     0,    74,     0,    73,    47,
    74,     0,     3,     0,     4,     0,     5,     0,    71,     0,
    74,    22,    74,     0,    74,    23,    74,     0,    74,    33,
    74,     0,    74,    35,    74,     0,    74,    34,    74,     0,
    74,    28,    74,     0,    74,    29,    74,     0,    74,    39,
    74,     0,    74,    38,    74,     0,    74,    40,    74,     0,
    74,    41,    74,     0,    74,    42,    74,     0,    74,    36,
    74,     0,    74,    37,    74,     0,    74,    24,    74,     0,
    74,    25,    74,     0,    74,    26,    74,     0,    74,    27,
    74,     0,    38,    74,     0,    39,    74,     0,    45,    74,
     0,    49,     9,    50,    74,     0,    49,     8,    50,    74,
     0,    49,    10,    50,    74,     0,     7,    49,    72,    50,
     0,    49,    74,    50,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   270,   271,   275,   276,   280,   281,   285,   288,   291,   294,
   297,   302,   305,   312,   315,   318,   321,   326,   329,   342,
   345,   358,   362,   368,   376,   387,   391,   400,   410,   413,
   416,   430,   468,   502,   506,   510,   514,   524,   528,   531,
   533,   549,   554,   554,   556,   562,   587,   627,   632,   638,
   643,   650,   654,   658,   662,   680,   693,   707,   721,   735,
   751,   765,   780,   799,   815,   831,   853,   869,   872,   875,
   878,   881,   884,   890,   904,   916,   929,   943,   957,   974,
  1046
};

static const char * const yytname[] = {   "$","error","$undefined.","c_float",
"c_int","c_string","c_char","identifier","k_INDEX","k_FLOAT","k_CTString","k_void",
"k_const","k_user","k_persistent","k_extern","k_pre","k_post","k_help","k_if",
"k_else","k_else_if","SHL","SHR","EQ","NEQ","GEQ","LEQ","LOGAND","LOGOR","block_beg",
"block_end","'='","'&'","'^'","'|'","'<'","'>'","'-'","'+'","'*'","'/'","'%'",
"TYPECAST","SIGN","'!'","':'","','","';'","'('","')'","'['","']'","program",
"block","statements","declaration_qualifiers","opt_string","type_specifier",
"pre_func_opt","post_func_opt","parameter_list_opt","parameter_list","declaration",
"statement","@1","@2","opt_else","@3","@4","@5","lvalue","argument_expression_list_opt",
"argument_expression_list","expression",""
};
#endif

static const short yyr1[] = {     0,
    53,    53,    54,    54,    55,    55,    56,    56,    56,    56,
    56,    57,    57,    58,    58,    58,    58,    59,    59,    60,
    60,    61,    61,    62,    62,    63,    63,    63,    64,    64,
    64,    64,    64,    64,    64,    64,    65,    66,    64,    67,
    68,    69,    67,    70,    67,    71,    71,    72,    72,    73,
    73,    74,    74,    74,    74,    74,    74,    74,    74,    74,
    74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
    74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
    74
};

static const short yyr2[] = {     0,
     1,     1,     3,     3,     0,     2,     0,     2,     2,     2,
     2,     0,     1,     1,     1,     1,     1,     0,     3,     0,
     3,     0,     1,     1,     3,     7,     8,    10,     1,     1,
     2,     4,     6,     2,     4,     4,     0,     0,     8,     0,
     0,     0,     8,     0,     3,     1,     4,     0,     1,     1,
     3,     1,     1,     1,     1,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     2,     2,     2,     4,     4,     4,     4,
     3
};

static const short yydefact[] = {     7,
    52,    53,    54,    46,     0,     0,     7,     0,     0,     0,
    29,     0,    30,     2,     0,     1,     7,    55,     0,    48,
     0,    34,     0,     0,     0,    55,    74,    75,    76,     0,
     0,     0,     0,    15,    14,    16,    17,     8,     9,    10,
    11,     0,     6,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    31,     0,    49,    50,     0,     0,     0,     0,
     3,     0,     0,     0,     0,    81,    18,     0,    56,    57,
    70,    71,    72,    73,    61,    62,    58,    60,    59,    68,
    69,    64,    63,    65,    66,    67,    80,     0,    47,    35,
    36,    37,     0,    78,    77,    79,     0,     0,    22,     0,
    20,    32,    51,     0,     0,     0,    24,     0,    23,     0,
     0,    12,    38,    19,    33,    12,     0,    18,     0,    13,
     0,    40,     0,    25,    20,    21,    26,    44,     0,    39,
    27,    12,     0,     0,     0,    45,     0,    28,    41,     0,
    42,    40,    43,     0,     0,     0
};

static const short yydefgoto[] = {   154,
    13,    14,    25,   131,    42,   111,   122,   118,   119,    16,
    17,   114,   132,   140,   150,   152,   143,    26,    64,    65,
    19
};

static const short yypact[] = {   110,
-32768,-32768,-32768,   -44,     4,   -19,   134,    81,    81,    81,
-32768,    19,-32768,-32768,   434,-32768,    73,     0,   291,    81,
    81,   -41,    81,     3,   434,-32768,-32768,-32768,-32768,   -15,
    10,    11,   204,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    52,-32768,    81,    81,    81,    81,    81,    81,    81,
    81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
    81,    81,-32768,    24,    28,   372,   162,    31,    37,   233,
-32768,    75,    81,    81,    81,-32768,    47,   318,    93,   104,
   128,   128,   128,   128,   393,   393,   414,   414,   414,   128,
   128,    30,    30,-32768,-32768,-32768,-32768,    81,-32768,-32768,
-32768,-32768,    51,-32768,-32768,-32768,    48,    81,     9,    81,
    76,-32768,   372,    65,    90,   345,-32768,    49,    54,   183,
    60,   102,-32768,-32768,-32768,   102,     9,    92,   116,-32768,
    61,    35,    77,-32768,    76,-32768,-32768,-32768,    78,-32768,
-32768,   102,    65,    81,    99,-32768,   262,-32768,-32768,    65,
-32768,    35,-32768,   124,   154,-32768
};

static const short yypgoto[] = {-32768,
   -81,    -1,   156,  -117,   -22,    29,    25,-32768,-32768,-32768,
-32768,-32768,-32768,    22,-32768,-32768,-32768,    14,-32768,-32768,
    -8
};


#define	YYLAST		456


static const short yytable[] = {    27,
    28,    29,    72,    33,    20,    24,    21,    68,   133,    69,
    22,    66,    67,    18,    70,    43,    34,    35,    36,    37,
    18,     1,     2,     3,   145,     4,    30,    31,    32,    23,
    18,    44,   123,    71,    73,    78,    79,    80,    81,    82,
    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
    93,    94,    95,    96,   138,   139,     8,     9,    77,    74,
    75,   146,   107,    10,   104,   105,   106,    12,   151,    60,
    61,    62,    -5,    97,    98,     1,     2,     3,   108,     4,
   100,   103,   108,     1,     2,     3,   117,     4,   101,   113,
     5,     6,   121,   115,     7,   109,   124,   110,   126,   116,
   127,   120,     7,    -5,   134,   129,   130,   107,   137,    -5,
     8,     9,     1,     2,     3,    46,     4,    10,     8,     9,
    11,    12,   136,   155,   141,    10,   144,     5,     6,    12,
    58,    59,    60,    61,    62,   147,     1,     2,     3,     7,
     4,    58,    59,    60,    61,    62,   148,     8,     9,    45,
    46,     5,     6,   156,    10,    15,   135,    11,    12,   142,
     0,     0,     0,     7,    -5,    58,    59,    60,    61,    62,
     0,     8,     9,   153,     0,     0,     0,     0,    10,     0,
     0,    11,    12,    45,    46,    47,    48,    49,    50,    51,
    52,     0,     0,     0,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    45,    46,    47,    48,    49,    50,
    51,    52,     0,    99,     0,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    45,    46,    47,    48,    49,
    50,    51,    52,     0,   128,     0,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,     0,     0,     0,     0,
     0,     0,     0,    76,    45,    46,    47,    48,    49,    50,
    51,    52,     0,     0,     0,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,     0,     0,     0,     0,     0,
     0,     0,   102,    45,    46,    47,    48,    49,    50,    51,
    52,     0,     0,     0,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,     0,     0,     0,     0,     0,     0,
     0,   149,    45,    46,    47,    48,    49,    50,    51,    52,
     0,     0,     0,    53,    54,    55,    56,    57,    58,    59,
    60,    61,    62,     0,     0,     0,     0,     0,    63,    45,
    46,    47,    48,    49,    50,    51,    52,     0,     0,     0,
    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
     0,     0,     0,     0,     0,   112,    45,    46,    47,    48,
    49,    50,    51,    52,     0,     0,     0,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,     0,     0,     0,
     0,     0,   125,    45,    46,    47,    48,    49,    50,    51,
    52,     0,     0,     0,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    45,    46,    47,    48,    49,    50,
     0,     0,     0,     0,     0,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    45,    46,    47,    48,    49,
    50,    34,    35,    36,    37,    38,    39,    40,    41,    56,
    57,    58,    59,    60,    61,    62
};

static const short yycheck[] = {     8,
     9,    10,    25,    12,    49,     7,    51,    49,   126,    51,
     7,    20,    21,     0,    23,    17,     8,     9,    10,    11,
     7,     3,     4,     5,   142,     7,     8,     9,    10,    49,
    17,    32,   114,    31,    50,    44,    45,    46,    47,    48,
    49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    62,    20,    21,    38,    39,     7,    50,
    50,   143,    16,    45,    73,    74,    75,    49,   150,    40,
    41,    42,     0,    50,    47,     3,     4,     5,    32,     7,
    50,     7,    32,     3,     4,     5,   109,     7,    52,    98,
    18,    19,    17,    46,    30,    49,     7,    51,    50,   108,
    47,   110,    30,    31,   127,    46,     5,    16,    48,     0,
    38,    39,     3,     4,     5,    23,     7,    45,    38,    39,
    48,    49,     7,     0,    48,    45,    49,    18,    19,    49,
    38,    39,    40,    41,    42,   144,     3,     4,     5,    30,
     7,    38,    39,    40,    41,    42,    48,    38,    39,    22,
    23,    18,    19,     0,    45,     0,   128,    48,    49,   135,
    -1,    -1,    -1,    30,    31,    38,    39,    40,    41,    42,
    -1,    38,    39,   152,    -1,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,    22,    23,    24,    25,    26,    27,    28,
    29,    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    22,    23,    24,    25,    26,    27,
    28,    29,    -1,    52,    -1,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    22,    23,    24,    25,    26,
    27,    28,    29,    -1,    52,    -1,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    50,    22,    23,    24,    25,    26,    27,
    28,    29,    -1,    -1,    -1,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    50,    22,    23,    24,    25,    26,    27,    28,
    29,    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    50,    22,    23,    24,    25,    26,    27,    28,    29,
    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    -1,    -1,    -1,    -1,    -1,    48,    22,
    23,    24,    25,    26,    27,    28,    29,    -1,    -1,    -1,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    -1,    -1,    -1,    -1,    -1,    48,    22,    23,    24,    25,
    26,    27,    28,    29,    -1,    -1,    -1,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    -1,    -1,    -1,
    -1,    -1,    48,    22,    23,    24,    25,    26,    27,    28,
    29,    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,
    39,    40,    41,    42,    22,    23,    24,    25,    26,    27,
    -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    22,    23,    24,    25,    26,
    27,     8,     9,    10,    11,    12,    13,    14,    15,    36,
    37,    38,    39,    40,    41,    42
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#undef YYERROR_VERBOSE
#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 7:
#line 285 "engine/base/parser.y"
{
  yyval.ulFlags = 0;
;
    break;}
case 8:
#line 288 "engine/base/parser.y"
{
  yyval.ulFlags = yyvsp[-1].ulFlags | SSF_CONSTANT;
;
    break;}
case 9:
#line 291 "engine/base/parser.y"
{
  yyval.ulFlags = yyvsp[-1].ulFlags | SSF_USER;
;
    break;}
case 10:
#line 294 "engine/base/parser.y"
{
  yyval.ulFlags = yyvsp[-1].ulFlags | SSF_PERSISTENT;
;
    break;}
case 11:
#line 297 "engine/base/parser.y"
{
  yyval.ulFlags = yyvsp[-1].ulFlags | SSF_EXTERNAL;
;
    break;}
case 12:
#line 302 "engine/base/parser.y"
{
  yyval.val.strString = "";
;
    break;}
case 13:
#line 305 "engine/base/parser.y"
{
  // !!!! remove this option
  //_pShell->ErrorF("Warning: symbol comments are not supported");
  yyval.val.strString = yyvsp[0].val.strString;
;
    break;}
case 14:
#line 312 "engine/base/parser.y"
{
  yyval.istType = ShellTypeNewFloat();
;
    break;}
case 15:
#line 315 "engine/base/parser.y"
{
  yyval.istType = ShellTypeNewIndex();
;
    break;}
case 16:
#line 318 "engine/base/parser.y"
{
  yyval.istType = ShellTypeNewString();
;
    break;}
case 17:
#line 321 "engine/base/parser.y"
{
  yyval.istType = ShellTypeNewVoid();
;
    break;}
case 18:
#line 326 "engine/base/parser.y"
{
  yyval.pPreFunc = NULL;
;
    break;}
case 19:
#line 329 "engine/base/parser.y"
{
  if (_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_sttType!=STT_FUNCTION
    ||_shell_ast[_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_istBaseType].st_sttType!=STT_INDEX
    ||_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_istFirstArgument!=_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_istLastArgument
    ||_shell_ast[_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_istFirstArgument].st_sttType!=STT_INDEX) {
    _pShell->ErrorF("'%s' must return 'INDEX' and take 'INDEX' as input", yyvsp[0].pssSymbol->ss_strName);
  } else {
    void *pv = yyvsp[0].pssSymbol->ss_pvValue;
    yyval.pPreFunc = (INDEX(*)(INDEX))yyvsp[0].pssSymbol->ss_pvValue;
  }
;
    break;}
case 20:
#line 342 "engine/base/parser.y"
{
  yyval.pPostFunc = NULL;
;
    break;}
case 21:
#line 345 "engine/base/parser.y"
{
  if (_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_sttType!=STT_FUNCTION
    ||_shell_ast[_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_istBaseType].st_sttType!=STT_VOID
    ||_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_istFirstArgument!=_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_istLastArgument
    ||_shell_ast[_shell_ast[yyvsp[0].pssSymbol->ss_istType].st_istFirstArgument].st_sttType!=STT_INDEX) {
    _pShell->ErrorF("'%s' must return 'void' and take 'INDEX' as input", yyvsp[0].pssSymbol->ss_strName);
  } else {
    yyval.pPostFunc = (void(*)(INDEX))yyvsp[0].pssSymbol->ss_pvValue;
  }
;
    break;}
case 22:
#line 358 "engine/base/parser.y"
{
  yyval.istType = ShellTypeNewFunction(0);
  ShellTypeAddFunctionArgument(yyval.istType, ShellTypeNewVoid());
;
    break;}
case 23:
#line 362 "engine/base/parser.y"
{
  yyval.istType = yyvsp[0].istType;
;
    break;}
case 24:
#line 368 "engine/base/parser.y"
{
  yyval.istType = ShellTypeNewFunction(0);
  ShellTypeAddFunctionArgument(yyval.istType, yyvsp[0].istType);
;
    break;}
case 25:
#line 376 "engine/base/parser.y"
{
  yyval.istType = yyvsp[-2].istType;
  ShellTypeAddFunctionArgument(yyval.istType, yyvsp[0].istType);
;
    break;}
case 26:
#line 387 "engine/base/parser.y"
{
  Declaration(yyvsp[-6].ulFlags, yyvsp[-5].istType, *yyvsp[-4].pssSymbol, yyvsp[-3].pPreFunc, yyvsp[-2].pPostFunc);
  ShellTypeDelete(yyvsp[-5].istType);
;
    break;}
case 27:
#line 391 "engine/base/parser.y"
{
  // take function from the parameter list and set its return type
  _shell_ast[yyvsp[-3].istType].st_istBaseType = yyvsp[-6].istType;
  yyvsp[-6].istType = yyvsp[-3].istType;
  // declare it
  Declaration(yyvsp[-7].ulFlags, yyvsp[-6].istType, *yyvsp[-5].pssSymbol, NULL, NULL);
  // free the type (declaration will make a copy)
  ShellTypeDelete(yyvsp[-6].istType);
;
    break;}
case 28:
#line 400 "engine/base/parser.y"
{
  if (yyvsp[-5].val.sttType!=STT_INDEX) {
    _pShell->ErrorF("Array size is not integral");
  }
  yyvsp[-8].istType = ShellTypeNewArray(yyvsp[-8].istType, yyvsp[-5].val.iIndex);
  Declaration(yyvsp[-9].ulFlags, yyvsp[-8].istType, *yyvsp[-7].pssSymbol, NULL, NULL);
  ShellTypeDelete(yyvsp[-8].istType);
;
    break;}
case 29:
#line 410 "engine/base/parser.y"
{
  // dummy
;
    break;}
case 30:
#line 413 "engine/base/parser.y"
{
  // dummy
;
    break;}
case 31:
#line 416 "engine/base/parser.y"
{
  // print its value
  if (yyvsp[-1].val.sttType == STT_VOID) {
    NOTHING;
  } else if (yyvsp[-1].val.sttType == STT_FLOAT) {
    CPrintF("%g\n", yyvsp[-1].val.fFloat);
  } else if (yyvsp[-1].val.sttType == STT_STRING) {
    CPrintF("\"%s\"\n", yyvsp[-1].val.strString);
  } else if (yyvsp[-1].val.sttType == STT_INDEX) {
    CPrintF("%d(0x%08X)\n", yyvsp[-1].val.iIndex, yyvsp[-1].val.iIndex);
  } else {
    _pShell->ErrorF("Expression cannot be printed");
  }
;
    break;}
case 32:
#line 430 "engine/base/parser.y"
{
  // if it is constant
  if (yyvsp[-3].lvLValue.lv_pssSymbol->ss_ulFlags&SSF_CONSTANT) {
    _pShell->ErrorF("Symbol '%s' is a constant", yyvsp[-3].lvLValue.lv_pssSymbol->ss_strName);
  // if it is not constant
  } else {
    // if it can be changed
    if (yyvsp[-3].lvLValue.lv_pssSymbol->ss_pPreFunc==NULL || yyvsp[-3].lvLValue.lv_pssSymbol->ss_pPreFunc(yyvsp[-3].lvLValue.lv_pvAddress)) {
      // if floats
      if (yyvsp[-3].lvLValue.lv_sttType == STT_FLOAT && yyvsp[-1].val.sttType==STT_FLOAT) {
        // assign value
        *(FLOAT*)yyvsp[-3].lvLValue.lv_pvAddress = yyvsp[-1].val.fFloat;
      // if indices
      } else if (yyvsp[-3].lvLValue.lv_sttType == STT_INDEX && yyvsp[-1].val.sttType==STT_INDEX) {
        // assign value
        *(INDEX*)yyvsp[-3].lvLValue.lv_pvAddress = yyvsp[-1].val.iIndex;

      // if strings
      } else if (yyvsp[-3].lvLValue.lv_sttType == STT_STRING && yyvsp[-1].val.sttType==STT_STRING) {
        // assign value
        *(CTString*)yyvsp[-3].lvLValue.lv_pvAddress = yyvsp[-1].val.strString;

      // if assigning index to float
      } else if (yyvsp[-3].lvLValue.lv_sttType == STT_FLOAT && yyvsp[-1].val.sttType==STT_INDEX) {
        *(FLOAT*)yyvsp[-3].lvLValue.lv_pvAddress = yyvsp[-1].val.iIndex;
      // otherwise
      } else {
        // error
        _pShell->ErrorF("Cannot assign: different types");
      }

      // call post-change function
      if (yyvsp[-3].lvLValue.lv_pssSymbol->ss_pPostFunc!=NULL) {
        yyvsp[-3].lvLValue.lv_pssSymbol->ss_pPostFunc(yyvsp[-3].lvLValue.lv_pvAddress);
      }
    }
  }
;
    break;}
case 33:
#line 468 "engine/base/parser.y"
{
  Declaration(yyvsp[-5].ulFlags, yyvsp[-4].istType, *yyvsp[-3].pssSymbol, NULL, NULL);
  ShellTypeDelete(yyvsp[-4].istType);

  CShellSymbol &ssSymbol = *yyvsp[-3].pssSymbol;
  // if it is constant
  if (ssSymbol.ss_ulFlags&SSF_CONSTANT) {
    // error
    _pShell->ErrorF("Symbol '%s' is a constant", ssSymbol.ss_strName);
  }

  // get symbol type
  ShellTypeType stt = _shell_ast[yyvsp[-4].istType].st_sttType;

  // if floats
  if (stt == STT_FLOAT && yyvsp[-1].val.sttType==STT_FLOAT) {
    // assign value
    *(FLOAT*)ssSymbol.ss_pvValue = yyvsp[-1].val.fFloat;
  // if indices
  } else if (stt == STT_INDEX && yyvsp[-1].val.sttType==STT_INDEX) {
    // assign value
    *(INDEX*)ssSymbol.ss_pvValue = yyvsp[-1].val.iIndex;
  // if strings
  } else if (stt == STT_STRING && yyvsp[-1].val.sttType==STT_STRING) {
    // assign value
    *(CTString*)ssSymbol.ss_pvValue = yyvsp[-1].val.strString;
  // !!!! remove this conversion
  } else if (stt == STT_FLOAT && yyvsp[-1].val.sttType==STT_INDEX) {
    _pShell->ErrorF("Warning: assigning INDEX to FLOAT!");  
    *(FLOAT*)ssSymbol.ss_pvValue = yyvsp[-1].val.iIndex;
  } else {
    _pShell->ErrorF("Symbol '%s' and its initializer have different types", ssSymbol.ss_strName);
  }
;
    break;}
case 34:
#line 502 "engine/base/parser.y"
{ 
extern void PrintShellSymbolHelp(const CTString &strSymbol);
  PrintShellSymbolHelp(yyvsp[0].pssSymbol->ss_strName);
;
    break;}
case 35:
#line 506 "engine/base/parser.y"
{ 
extern void PrintShellSymbolHelp(const CTString &strSymbol);
  PrintShellSymbolHelp(yyvsp[-2].pssSymbol->ss_strName);
;
    break;}
case 36:
#line 510 "engine/base/parser.y"
{ 
extern void PrintShellSymbolHelp(const CTString &strSymbol);
  PrintShellSymbolHelp(yyvsp[-2].pssSymbol->ss_strName);
;
    break;}
case 37:
#line 514 "engine/base/parser.y"
{ 
  _bExecNextBlock = FALSE;
  if (yyvsp[-1].val.sttType == STT_INDEX) {
    _bExecNextBlock = yyvsp[-1].val.iIndex!=0;
  } else if (yyvsp[-1].val.sttType == STT_FLOAT) {
    _bExecNextBlock = yyvsp[-1].val.fFloat!=0;
  } else {
    _pShell->ErrorF("If expression is not integral");
  }
  yyvsp[-3].ulFlags = _bExecNextBlock;
;
    break;}
case 38:
#line 524 "engine/base/parser.y"
{
  _bExecNextElse = !yyvsp[-5].ulFlags;
  _bExecNextBlock = TRUE;
;
    break;}
case 41:
#line 533 "engine/base/parser.y"
{
  if (_bExecNextElse) {  
    _bExecNextBlock = FALSE;
    if (yyvsp[-1].val.sttType == STT_INDEX) {
      _bExecNextBlock = yyvsp[-1].val.iIndex!=0;
    } else if (yyvsp[-1].val.sttType == STT_FLOAT) {
      _bExecNextBlock = yyvsp[-1].val.fFloat!=0;
    } else {
      _pShell->ErrorF("If expression is not integral");
    }
    yyvsp[-3].ulFlags = _bExecNextBlock;
  } else {
    _bExecNextBlock = FALSE;
    _bExecNextElse = FALSE;
    yyvsp[-3].ulFlags = TRUE;
  }
;
    break;}
case 42:
#line 549 "engine/base/parser.y"
{
  _bExecNextElse = !yyvsp[-5].ulFlags;
  _bExecNextBlock = TRUE;
;
    break;}
case 44:
#line 554 "engine/base/parser.y"
{
  _bExecNextBlock = _bExecNextElse;  
;
    break;}
case 45:
#line 556 "engine/base/parser.y"
{
  _bExecNextBlock = TRUE;
;
    break;}
case 46:
#line 562 "engine/base/parser.y"
{
  CShellSymbol &ssSymbol = *yyvsp[0].pssSymbol;
  const ShellType &stType = _shell_ast[ssSymbol.ss_istType];

  yyval.lvLValue.lv_pssSymbol = &ssSymbol;
  if (!ssSymbol.IsDeclared()) {
    // error
    _pShell->ErrorF("Identifier '%s' is not declared", yyvsp[0].pssSymbol->ss_strName);
    fDummy = -666;
    yyval.lvLValue.lv_sttType = STT_VOID;
    yyval.lvLValue.lv_pvAddress = &fDummy;
  // if the identifier is a float, int or string
  } else if (stType.st_sttType==STT_FLOAT || stType.st_sttType==STT_INDEX || stType.st_sttType==STT_STRING) {
    // get its value and type
    yyval.lvLValue.lv_sttType = stType.st_sttType;
    yyval.lvLValue.lv_pvAddress = ssSymbol.ss_pvValue;
  // if the identifier is something else
  } else {
    // error
    _pShell->ErrorF("'%s' doesn't have a value", yyvsp[0].pssSymbol->ss_strName);
    fDummy = -666.0f;
    yyval.lvLValue.lv_sttType = STT_VOID;
    yyval.lvLValue.lv_pvAddress = &fDummy;
  }
;
    break;}
case 47:
#line 587 "engine/base/parser.y"
{
  CShellSymbol &ssSymbol = *yyvsp[-3].pssSymbol;
  const ShellType &stType = _shell_ast[ssSymbol.ss_istType];
  yyval.lvLValue.lv_pssSymbol = &ssSymbol;

  int iIndex = 0;
  // if subscript is index
  if (yyvsp[-1].val.sttType==STT_INDEX) {
    // get the index
    iIndex = yyvsp[-1].val.iIndex;
  // if subscript is not index
  } else {
    // error
    _pShell->ErrorF("Array subscript is not integral");
  }
  // if the symbol is array 
  if (stType.st_sttType==STT_ARRAY) {
    const ShellType &stBase = _shell_ast[stType.st_istBaseType];
    // if it is float or int array
    if (stBase.st_sttType==STT_FLOAT || stBase.st_sttType==STT_INDEX) {
      // if the index is out of range
      if (iIndex<0 || iIndex>=stType.st_ctArraySize) {
        _pShell->ErrorF("Array member out of range");
        fDummy = -666.0f;
        yyval.lvLValue.lv_pvAddress = &fDummy;
      } else {
        // get its value and type
        yyval.lvLValue.lv_sttType = stBase.st_sttType;
        yyval.lvLValue.lv_pvAddress = (FLOAT*)ssSymbol.ss_pvValue+iIndex;
      }
    }
  } else {
    _pShell->ErrorF("'%s[]' doesn't have a value", yyvsp[-3].pssSymbol->ss_strName);
    fDummy = -666.0f;
    yyval.lvLValue.lv_pvAddress = &fDummy;
  }
;
    break;}
case 48:
#line 627 "engine/base/parser.y"
{
  yyval.arg.istType = ShellTypeNewFunction(ShellTypeNewVoid());
  ShellTypeAddFunctionArgument(yyval.arg.istType, ShellTypeNewVoid());
  yyval.arg.ctBytes = 0;
;
    break;}
case 49:
#line 632 "engine/base/parser.y"
{
  yyval.arg = yyvsp[0].arg;
;
    break;}
case 50:
#line 638 "engine/base/parser.y"
{
  yyval.arg.istType = ShellTypeNewFunction(ShellTypeNewVoid());
  ShellTypeAddFunctionArgument(yyval.arg.istType, ShellTypeNewByType(yyvsp[0].val.sttType));
  yyval.arg.ctBytes = PushExpression(yyvsp[0].val);
;
    break;}
case 51:
#line 643 "engine/base/parser.y"
{
  yyval.arg = yyvsp[-2].arg;
  ShellTypeAddFunctionArgument(yyval.arg.istType, ShellTypeNewByType(yyvsp[0].val.sttType));
  yyval.arg.ctBytes += PushExpression(yyvsp[0].val);
;
    break;}
case 52:
#line 650 "engine/base/parser.y"
{
  yyval.val.sttType = STT_FLOAT;  
  yyval.val.fFloat = yyvsp[0].val.fFloat;
;
    break;}
case 53:
#line 654 "engine/base/parser.y"
{
  yyval.val.sttType = STT_INDEX;  
  yyval.val.iIndex = yyvsp[0].val.iIndex;
;
    break;}
case 54:
#line 658 "engine/base/parser.y"
{
  yyval.val.sttType = STT_STRING;  
  yyval.val.strString = yyvsp[0].val.strString;
;
    break;}
case 55:
#line 662 "engine/base/parser.y"
{
  // get its value
  yyval.val.sttType = yyvsp[0].lvLValue.lv_sttType;
  if (yyvsp[0].lvLValue.lv_sttType==STT_VOID) {
    NOTHING;
  } else if (yyvsp[0].lvLValue.lv_sttType==STT_FLOAT) {
    yyval.val.fFloat = *(FLOAT*)yyvsp[0].lvLValue.lv_pvAddress;
  } else if (yyvsp[0].lvLValue.lv_sttType==STT_INDEX) {
    yyval.val.iIndex = *(INDEX*)yyvsp[0].lvLValue.lv_pvAddress;
  } else if (yyvsp[0].lvLValue.lv_sttType==STT_STRING) {
    yyval.val.strString = (const char*)*(CTString*)yyvsp[0].lvLValue.lv_pvAddress;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
    _pShell->ErrorF("'%s' is of wrong type", yyvsp[0].lvLValue.lv_pssSymbol->ss_strName);
  }
;
    break;}
case 56:
#line 680 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex<<yyvsp[0].val.iIndex;
  } else {
    _pShell->ErrorF("Wrong arguments for '<<'");
    yyval.val.sttType = STT_INDEX;
    yyval.val.iIndex = -666;
  }
;
    break;}
case 57:
#line 693 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex>>yyvsp[0].val.iIndex;
  } else {
    _pShell->ErrorF("Wrong arguments for '>>'");
    yyval.val.sttType = STT_INDEX;
    yyval.val.iIndex = -666;
  }
;
    break;}
case 58:
#line 707 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    _pShell->ErrorF("'&' is illegal for FLOAT values");
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex&yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 59:
#line 721 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    _pShell->ErrorF("'|' is illegal for FLOAT values");
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex|yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 60:
#line 735 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    _pShell->ErrorF("'^' is illegal for FLOAT values");
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex^yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 61:
#line 751 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    _pShell->ErrorF("'&&' is illegal for FLOAT values");
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex&&yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 62:
#line 765 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    _pShell->ErrorF("'||' is illegal for FLOAT values");
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex||yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 63:
#line 780 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    yyval.val.fFloat = yyvsp[-2].val.fFloat+yyvsp[0].val.fFloat;
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex+yyvsp[0].val.iIndex;
  } else if (yyvsp[-2].val.sttType == STT_STRING) {
    CTString &strNew = _shell_astrTempStrings.Push();
    strNew = CTString(yyvsp[-2].val.strString)+yyvsp[0].val.strString;
    yyval.val.strString = (const char*)strNew;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 64:
#line 799 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    yyval.val.fFloat = yyvsp[-2].val.fFloat-yyvsp[0].val.fFloat;
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex-yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 65:
#line 815 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    yyval.val.fFloat = yyvsp[-2].val.fFloat*yyvsp[0].val.fFloat;
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex*yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }

;
    break;}
case 66:
#line 831 "engine/base/parser.y"
{

  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    yyval.val.fFloat = yyvsp[-2].val.fFloat/yyvsp[0].val.fFloat;
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    if (yyvsp[0].val.iIndex==0) {
      _pShell->ErrorF("Division by zero!\n");
      yyval.val.iIndex = 0;
    } else {
      yyval.val.iIndex = yyvsp[-2].val.iIndex/yyvsp[0].val.iIndex;
    }
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }

;
    break;}
case 67:
#line 853 "engine/base/parser.y"
{
  MatchTypes(yyvsp[-2].val, yyvsp[0].val);

  yyval.val.sttType = yyvsp[-2].val.sttType;
  if (yyvsp[-2].val.sttType == STT_FLOAT) {
    _pShell->ErrorF("'%' is illegal for FLOAT values");
  } else if (yyvsp[-2].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[-2].val.iIndex%yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }

;
    break;}
case 68:
#line 869 "engine/base/parser.y"
{
  DoComparison(yyval.val, yyvsp[-2].val, yyvsp[0].val, '<');
;
    break;}
case 69:
#line 872 "engine/base/parser.y"
{
  DoComparison(yyval.val, yyvsp[-2].val, yyvsp[0].val, '>');
;
    break;}
case 70:
#line 875 "engine/base/parser.y"
{
  DoComparison(yyval.val, yyvsp[-2].val, yyvsp[0].val, '=');
;
    break;}
case 71:
#line 878 "engine/base/parser.y"
{
  DoComparison(yyval.val, yyvsp[-2].val, yyvsp[0].val, '!');
;
    break;}
case 72:
#line 881 "engine/base/parser.y"
{
  DoComparison(yyval.val, yyvsp[-2].val, yyvsp[0].val, '}');
;
    break;}
case 73:
#line 884 "engine/base/parser.y"
{
  DoComparison(yyval.val, yyvsp[-2].val, yyvsp[0].val, '{');
;
    break;}
case 74:
#line 890 "engine/base/parser.y"
{
  yyval.val.sttType = yyvsp[0].val.sttType;
  if (yyvsp[0].val.sttType == STT_FLOAT) {
    yyval.val.fFloat = -yyvsp[0].val.fFloat;
  } else if (yyvsp[0].val.sttType == STT_INDEX) {
    yyval.val.iIndex = -yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 75:
#line 904 "engine/base/parser.y"
{
  yyval.val.sttType = yyvsp[0].val.sttType;
  if (yyvsp[0].val.sttType == STT_FLOAT) {
    yyval.val.fFloat = yyvsp[0].val.fFloat;
  } else if (yyvsp[0].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 76:
#line 916 "engine/base/parser.y"
{
  yyval.val.sttType = yyvsp[0].val.sttType;
  if (yyvsp[0].val.sttType == STT_FLOAT) {
    _pShell->ErrorF("'!' is illegal for FLOAT values");
    yyval.val.fFloat = yyvsp[0].val.fFloat;
  } else if (yyvsp[0].val.sttType == STT_INDEX) {
    yyval.val.iIndex = !yyvsp[0].val.iIndex;
  } else {
    yyval.val.sttType = STT_FLOAT;
    yyval.val.fFloat = -666.0f;
  }
;
    break;}
case 77:
#line 929 "engine/base/parser.y"
{
  yyval.val.sttType = STT_FLOAT;
  if (yyvsp[0].val.sttType == STT_FLOAT) {
    yyval.val.fFloat = yyvsp[0].val.fFloat;
  } else if (yyvsp[0].val.sttType == STT_INDEX) {
    yyval.val.fFloat = FLOAT(yyvsp[0].val.iIndex);
  } else if (yyvsp[0].val.sttType == STT_STRING) {
    yyval.val.fFloat = atof(yyvsp[0].val.strString);
  } else {
    _pShell->ErrorF("Cannot convert to FLOAT");
    yyval.val.sttType = STT_VOID;
  }
;
    break;}
case 78:
#line 943 "engine/base/parser.y"
{
  yyval.val.sttType = STT_INDEX;
  if (yyvsp[0].val.sttType == STT_FLOAT) {
    yyval.val.iIndex = INDEX(yyvsp[0].val.fFloat);
  } else if (yyvsp[0].val.sttType == STT_INDEX) {
    yyval.val.iIndex = yyvsp[0].val.iIndex;
  } else if (yyvsp[0].val.sttType == STT_STRING) {
    yyval.val.iIndex = atol(yyvsp[0].val.strString);
  } else {
    _pShell->ErrorF("Cannot convert to INDEX");
    yyval.val.sttType = STT_VOID;
  }
;
    break;}
case 79:
#line 957 "engine/base/parser.y"
{
  CTString &strNew = _shell_astrTempStrings.Push();
  yyval.val.sttType = STT_STRING;
  if (yyvsp[0].val.sttType == STT_FLOAT) {
    strNew.PrintF("%g", yyvsp[0].val.fFloat);
  } else if (yyvsp[0].val.sttType == STT_INDEX) {
    strNew.PrintF("%d", yyvsp[0].val.iIndex);
  } else if (yyvsp[0].val.sttType == STT_STRING) {
    strNew = yyvsp[0].val.strString;
  } else {
    _pShell->ErrorF("Cannot convert to CTString");
    yyval.val.sttType = STT_VOID;
  }
  yyval.val.strString = (const char*)strNew;
;
    break;}
case 80:
#line 974 "engine/base/parser.y"
{
  // if the identifier is not declared
  if (!yyvsp[-3].pssSymbol->IsDeclared()) {
    // error
    _pShell->ErrorF("Identifier '%s' is not declared", yyvsp[-3].pssSymbol->ss_strName);
  // if the identifier is declared
  } else {
    // get its type
    ShellType &stFunc = _shell_ast[yyvsp[-3].pssSymbol->ss_istType];

    // if the identifier is a function
    if (stFunc.st_sttType==STT_FUNCTION) {
      // determine result type
      ShellType &stResult =  _shell_ast[stFunc.st_istBaseType];
      // match argument list result to that result
      _shell_ast[_shell_ast[yyvsp[-1].arg.istType].st_istBaseType].st_sttType = stResult.st_sttType;
      // if types are same
      if (ShellTypeIsSame(yyvsp[-1].arg.istType, yyvsp[-3].pssSymbol->ss_istType)) {

#define PUSHPARAMS \
  memcpy(_alloca(yyvsp[-1].arg.ctBytes), _ubStack+_iStack-yyvsp[-1].arg.ctBytes, yyvsp[-1].arg.ctBytes);

        // if void
        if (stResult.st_sttType==STT_VOID) {
          // just call the function
          yyval.val.sttType = STT_VOID;
          //PUSHPARAMS;
          ((void (*)(void*))yyvsp[-3].pssSymbol->ss_pvValue)(_ubStack+_iStack-yyvsp[-1].arg.ctBytes);
        // if index
        } else if (stResult.st_sttType==STT_INDEX) {
          // call the function and return result
          yyval.val.sttType = STT_INDEX;
          PUSHPARAMS;
          yyval.val.iIndex = ((INDEX (*)(void))yyvsp[-3].pssSymbol->ss_pvValue)();
        // if float
        } else if (stResult.st_sttType==STT_FLOAT) {
          // call the function and return result
          yyval.val.sttType = STT_FLOAT;
          PUSHPARAMS;
          yyval.val.fFloat = ((FLOAT (*)(void))yyvsp[-3].pssSymbol->ss_pvValue)();
        // if string
        } else if (stResult.st_sttType==STT_STRING) {
          // call the function and return result
          yyval.val.sttType = STT_STRING;
          CTString &strNew = _shell_astrTempStrings.Push();
          PUSHPARAMS;
          strNew = ((CTString (*)(void))yyvsp[-3].pssSymbol->ss_pvValue)();
          yyval.val.strString = (const char*)strNew;
        } else {
          ASSERT(FALSE);
          yyval.val.sttType = STT_FLOAT;
          yyval.val.fFloat = -666.0f;
        }
      // if types are different
      } else {
        // error
        yyval.val.sttType = STT_VOID;
        _pShell->ErrorF("Wrong parameters for '%s'", yyvsp[-3].pssSymbol->ss_strName);
      }
    // if the identifier is something else
    } else {
      // error
      yyval.val.sttType = STT_VOID;
      _pShell->ErrorF("Can't call '%s'", yyvsp[-3].pssSymbol->ss_strName);
    }
  }

  // pop arguments and free type info
  _iStack-=yyvsp[-1].arg.ctBytes;
  ShellTypeDelete(yyvsp[-1].arg.istType);
;
    break;}
case 81:
#line 1046 "engine/base/parser.y"
{
  yyval.val = yyvsp[-1].val;
;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 1051 "engine/base/parser.y"

