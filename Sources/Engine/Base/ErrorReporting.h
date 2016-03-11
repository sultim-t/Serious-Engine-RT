/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_ERRORREPORTING_H
#define SE_INCL_ERRORREPORTING_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

/* Throw an exception of formatted string. */
ENGINE_API extern void ThrowF_t(char *strFormat, ...); // throws char *
/* Report error and terminate program. */
ENGINE_API extern void FatalError(const char *strFormat, ...);
/* Report warning without terminating program (stops program until user responds). */
ENGINE_API extern void WarningMessage(const char *strFormat, ...);
/* Report information message to user (stops program until user responds). */
ENGINE_API extern void InfoMessage(const char *strFormat, ...);
/* Ask user for yes/no answer(stops program until user responds). */
ENGINE_API extern BOOL YesNoMessage(const char *strFormat, ...);
/* Get the description string for windows error code. */
ENGINE_API extern const CTString GetWindowsError(DWORD dwWindowsErrorCode);


#endif  /* include-once check. */

