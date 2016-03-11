/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_SHELL_H
#define SE_INCL_SHELL_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Synchronization.h>

#include <Engine/Templates/DynamicArray.h>
#include <Engine/Base/Shell_internal.h>

#define NEXTARGUMENT(type) ( *((type*&)pArgs)++ )

// Object that takes care of shell functions, variables, macros etc.
class ENGINE_API CShell {
public:
// implementation:
  CTCriticalSection sh_csShell; // critical section for access to shell data
  CDynamicArray<CShellSymbol> sh_assSymbols;  // all defined symbols

  // Get a shell symbol by its name.
  CShellSymbol *GetSymbol(const CTString &strName, BOOL bDeclaredOnly);
  // Report error in shell script processing.
  void ErrorF(const char *strFormat, ...);

// interface:

  // Constructor.
  CShell(void);
  ~CShell(void);

  // Initialize the shell.
  void Initialize(void);

  // Declare a symbol in the shell.
  void DeclareSymbol(const CTString &strDeclaration, void *pvValue);
  // Execute command(s).
  void Execute(const CTString &strCommands);
  // Save shell commands to restore persistent symbols to a script file
  void StorePersistentSymbols(const CTFileName &fnScript);

  // get/set symbols
  FLOAT GetFLOAT(const CTString &strName);
  void SetFLOAT(const CTString &strName, FLOAT fValue);
  INDEX GetINDEX(const CTString &strName);
  void SetINDEX(const CTString &strName, INDEX iValue);
  CTString GetString(const CTString &strName);
  void SetString(const CTString &strName, const CTString &strValue);

  CTString GetValue(const CTString &strName);
  void SetValue(const CTString &strName, const CTString &strValue);
};

// pointer to global shell object
ENGINE_API extern CShell *_pShell;


#endif  /* include-once check. */

