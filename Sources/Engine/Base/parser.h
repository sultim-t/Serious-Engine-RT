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

