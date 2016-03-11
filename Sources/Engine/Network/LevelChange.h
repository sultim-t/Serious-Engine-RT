/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_LEVELCHANGE_H
#define SE_INCL_LEVELCHANGE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

enum LevelChangePhase {
  // initial state
  LCP_NOCHANGE = 0,
  // ->initiation->
  LCP_INITIATED,
  // ->pre-change signalled->
  LCP_SIGNALLED,
  // ->change done->
  LCP_CHANGED,
  // ->post-change signalled->
  //LCP_NOCHANGE
};

extern LevelChangePhase _lphCurrent;

class CRememberedLevel {
public:
  CListNode rl_lnInSessionState;      // for linking in list of all remembered levels
  CTString rl_strFileName;            // file name of the level
  CTMemoryStream rl_strmSessionState; // saved session state
};


#endif  /* include-once check. */

