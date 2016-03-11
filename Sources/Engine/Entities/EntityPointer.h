/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_ENTITYPOINTER_H
#define SE_INCL_ENTITYPOINTER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

/*
 * Smart pointer to entity objects, does the book-keeping for reference counting.
 */
class CEntityPointer {
public:
  CEntity *ep_pen;  // the pointer itself
public:
  // all standard smart pointer functions are defined as inlines in Entity.h
  // (due to strange order of inclusion needed for events and enums)
  inline CEntityPointer(void);
  inline ~CEntityPointer(void);
  inline CEntityPointer(const CEntityPointer &penOther);
  inline CEntityPointer(CEntity *pen);
  inline const CEntityPointer &operator=(CEntity *pen);
  inline const CEntityPointer &operator=(const CEntityPointer &penOther);
  inline CEntity* operator->(void) const;
  inline operator CEntity*(void) const;
  inline CEntity& operator*(void) const;
};


#endif  /* include-once check. */

