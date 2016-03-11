/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_ACTIONBUFFER_H
#define SE_INCL_ACTIONBUFFER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Lists.h>

// buffer of player actions, sorted by time of arrival
class CActionBuffer {
public:
  CListHead ab_lhActions;
public:
  CActionBuffer(void);
  ~CActionBuffer(void);
  void Clear(void);

  // add a new action to the buffer
  void AddAction(const CPlayerAction &pa);
  // remove oldest buffered action
  void RemoveOldest(void);
  // flush all actions up to given time tag
  void FlushUntilTime(__int64 llNewest);
  // get number of actions buffered
  INDEX GetCount(void);
  // get an action by its index (0=oldest)
  void GetActionByIndex(INDEX i, CPlayerAction &pa);
  // get last action older than given timetag
  CPlayerAction *GetLastOlderThan(__int64 llTime);
};


#endif  /* include-once check. */

