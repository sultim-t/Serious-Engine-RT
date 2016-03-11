typedef union {
  int i;
  float f;
  const char *str;
  CModelInstance *pmi;
  float f6[6];
} YYSTYPE;
#define	c_float	258
#define	c_int	259
#define	c_string	260
#define	c_modelinstance	261
#define	k_PARENTBONE	262
#define	k_SE_SMC	263
#define	k_SE_END	264
#define	k_NAME	265
#define	k_TFNM	266
#define	k_MESH	267
#define	k_SKELETON	268
#define	k_ANIMSET	269
#define	K_ANIMATION	270
#define	k_TEXTURES	271
#define	k_OFFSET	272
#define	k_COLISION	273
#define	k_ALLFRAMESBBOX	274
#define	k_ANIMSPEED	275
#define	k_COLOR	276


extern YYSTYPE syylval;
