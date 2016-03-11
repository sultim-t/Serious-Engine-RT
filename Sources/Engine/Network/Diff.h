/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_DIFF_H
#define SE_INCL_DIFF_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// make a difference file from two saved games
void DIFF_Diff_t(CTStream *pstrmOld, CTStream *pstrmNew, CTStream *pstrmDiff); // throw char *
// make a new saved game from difference file and old saved game
void DIFF_Undiff_t(CTStream *pstrmOld, CTStream *pstrmDiff, CTStream *pstrmNew); // throw char *


#endif  /* include-once check. */

