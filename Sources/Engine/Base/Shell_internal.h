/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_SHELL_INTERNAL_H
#define SE_INCL_SHELL_INTERNAL_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/CTString.h>

// basic data type types
enum ShellTypeType {
  STT_ILLEGAL  ,
  STT_POINTER  ,
  STT_FUNCTION ,
  STT_ARRAY    ,
  STT_VOID     ,
  STT_INDEX    ,
  STT_FLOAT    ,
  STT_STRING   ,
};


// data type structure
struct ShellType {
  enum ShellTypeType st_sttType; 

  INDEX st_ctArraySize;             // number of members if an array

  INDEX st_istBaseType;    // type pointed to by a pointer, or member of array
     // or result of a function
  
  INDEX st_istFirstArgument;     // first argument in function
  INDEX st_istLastArgument;      // last argument in function
  INDEX st_istNextInArguments;   // next argument in function
  INDEX st_istPrevInArguments;   // previous argument in function
};

// symbol flags
#define SSF_CONSTANT    (1L<<0)   // the symbol cannot be changed
#define SSF_USER        (1L<<1)   // the symbol is visible to user (with ListSymbols() )
#define SSF_PERSISTENT  (1L<<2)   // the symbol is saved when exiting
#define SSF_EXTERNAL    (1L<<3)   // the symbol was declared externally (in a script)

// A symbol defined for using in shell.
class CShellSymbol {
public:
// implementation:
  INDEX ss_istType;   // type of the symbol
  CTString ss_strName;    // symbol name
  void *ss_pvValue;       // symbol value
  BOOL (*ss_pPreFunc)(void *);
  void (*ss_pPostFunc)(void *);
  ULONG ss_ulFlags;       // various flags
  // Clear function.
  void Clear(void);
  // check if declared
  BOOL IsDeclared(void);
// interface:
  // get string for 'tab' completion in console 
  ENGINE_API CTString GetCompletionString(void) const;
};


#endif  /* include-once check. */

