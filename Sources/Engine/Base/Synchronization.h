/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_SYNCHRONIZATION_H
#define SE_INCL_SYNCHRONIZATION_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// intra-process mutex (only used by thread of same process)
// NOTE: mutex has no interface - it is locked using CTSingleLock
class CTCriticalSection {
public:
  void *cs_pvObject;  // object is internal to implementation
  INDEX cs_iIndex;    // index of mutex used to prevent deadlock with assertions
  // use numbers from 1 and above for deadlock control, or -1 for no deadlock control
  ENGINE_API CTCriticalSection(void);
  ENGINE_API ~CTCriticalSection(void);
  INDEX Lock(void);
  INDEX TryToLock(void);
  INDEX Unlock(void);
};

// lock object for locking a mutex with automatic unlocking
class CTSingleLock {
public:
  CTCriticalSection &sl_cs;   // the mutex this object refers to
  BOOL sl_bLocked;            // set while locked
  INDEX sl_iLastLockedIndex;    // index of mutex that was locked before this lock
  ENGINE_API CTSingleLock(CTCriticalSection *pcs, BOOL bLock);
  ENGINE_API ~CTSingleLock(void);
  ENGINE_API void Lock(void);
  ENGINE_API BOOL TryToLock(void);
  ENGINE_API BOOL IsLocked(void);
  ENGINE_API void Unlock(void);
};


#endif  /* include-once check. */

