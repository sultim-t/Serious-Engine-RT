/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// needed for parser and scanner
extern int yylex(void);
extern void yyerror(const char *strFormat, ...);
extern int yyparse(void);
extern void yyrestart(FILE *f);
extern FILE *yyin;

#define YY_NEVER_INTERACTIVE 1
#define MAX_INCLUDE_DEPTH 32

// temporary values for parsing
extern INDEX _yy_iLine;
extern CMesh *_yy_pMesh;
extern CSkeleton *_yy_pSkeleton;
extern CAnimSet *_yy_pAnimSet;
extern INDEX _yy_iIndex;
extern INDEX _yy_jIndex;
extern INDEX _yy_iWIndex; // index for weightmaps
extern INDEX _yy_iMIndex; // index for mophmaps
