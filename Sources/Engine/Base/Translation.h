/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_TRANSLATION_H
#define SE_INCL_TRANSLATION_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// init translation routines with given translation table
ENGINE_API void InitTranslation(void);
// add given translation table
ENGINE_API void AddTranslationTable_t(const CTFileName &fnmTable); // throw char *
// add several tables from a directory using wildcards
ENGINE_API void AddTranslationTablesDir_t(const CTFileName &fnmDir, const CTFileName &fnmPattern); // throw char *
// finish translation table building
ENGINE_API void FinishTranslationTable(void);

// read given translation table from file - for internal use
ENGINE_API void ReadTranslationTable_t(
  CDynamicArray<class CTranslationPair> &atpPairs, const CTFileName &fnmTable); // throw char *

// translate a string
ENGINE_API char * Translate(char *str, INDEX iOffset=0);
ENGINE_API const char * TranslateConst(const char *str, INDEX iOffset=0);

// macro for inserting a string for translation into executables
#define TRANS(str) Translate("ETRS" str, 4)
// macro for translating a variable string (usually can be CTString)
#define TRANSV(str) TranslateConst(str, 0)


#endif  /* include-once check. */

