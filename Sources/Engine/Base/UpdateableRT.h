/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_UPDATEABLERT_H
#define SE_INCL_UPDATEABLERT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

/*
 * Object that can be updated to reflect changes in some changeable object(s).
 */
class ENGINE_API CUpdateableRT {
private:
  TIME ud_LastUpdateTime;   // last time this object has been updated
public:
  /* Constructor. */
  CUpdateableRT(void);
  /* Get time when last updated. */
  TIME LastUpdateTime(void) const ;
  /* Mark that the object has been updated. */
  void MarkUpdated(void);
  /* Mark that the object has become invalid in spite of its time stamp. */
  void Invalidate(void);
};



#endif  /* include-once check. */

