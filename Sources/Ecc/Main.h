/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */


/* rcg10042001 */
#ifdef PLATFORM_WIN32
#define alloca _alloca
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

extern int yylex(void);
extern void yyerror(char *s);
extern int yyparse(void);

extern FILE *_fInput;
extern int _iLinesCt;
extern int _bTrackLineInformation;   // this is set if #line should be inserted in tokens
extern char *_strInputFileName;

extern FILE *_fImplementation;
extern FILE *_fDeclaration;
extern FILE *_fTables;
extern FILE *_fExports;
extern char *_strFileNameBase;
extern char *_strFileNameBaseIdentifier;

struct SType {
  char *strString;
  int bCrossesStates;
  int iLine;

  SType(void) {
    strString = strdup("");
    bCrossesStates = 0;
    iLine = -1;
  };
  SType(const char *str) {
    strString = strdup(str);
    bCrossesStates = 0;
    iLine = -1;
  };
  SType(const SType &other) {
    strString = strdup(other.strString);
    bCrossesStates = other.bCrossesStates;
    iLine = other.iLine;
  };
  const SType &operator=(const SType &other) {
    strString = strdup(other.strString);
    bCrossesStates = other.bCrossesStates;
    iLine = other.iLine;
    return *this;
  };
  const SType &operator=(char *str) {
    strString = strdup(str);
    bCrossesStates = 0;
    iLine = -1;
    return *this;
  };
  SType operator+(const SType &other);
};

#define YYSTYPE SType
