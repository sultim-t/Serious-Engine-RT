/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_CONSOLE_H
#define SE_INCL_CONSOLE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// Print formated text to the main console.
ENGINE_API extern void CPrintF(const char *strFormat, ...);
// Add a string of text to console
ENGINE_API void CPutString(const char *strString);

// Get number of lines newer than given time
ENGINE_API INDEX CON_NumberOfLinesAfter(TIME tmLast);
// Get one of last lines
ENGINE_API CTString CON_GetLastLine(INDEX iLine);
// Discard timing info for last lines
ENGINE_API void CON_DiscardLastLineTimes(void);
// Get current console buffer.
ENGINE_API const char *CON_GetBuffer(void);
ENGINE_API INDEX CON_GetBufferSize(void);


#endif  /* include-once check. */

