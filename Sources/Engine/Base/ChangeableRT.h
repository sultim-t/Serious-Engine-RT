/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_CHANGEABLERT_H
#define SE_INCL_CHANGEABLERT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

/*
 * Object that can change in time.
 */
class ENGINE_API CChangeableRT {
private:
  TIME ch_LastChangeTime;   // last time this object has been changed
public:
  /* Constructor. */
  CChangeableRT(void);
  /* Mark that something has changed in this object. */
  void MarkChanged(void);
  /* Test if some updateable object is up to date with this changeable. */
  BOOL IsUpToDate(const CUpdateableRT &ud) const;
};



#endif  /* include-once check. */

