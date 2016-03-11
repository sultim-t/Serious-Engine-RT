/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STATISTICS_H
#define SE_INCL_STATISTICS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// reset all values
ENGINE_API void STAT_Reset(void);
// make a new report
ENGINE_API void STAT_Report(CTString &strReport);


#endif  /* include-once check. */

