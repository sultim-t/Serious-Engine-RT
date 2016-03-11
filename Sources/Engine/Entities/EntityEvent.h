/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_ENTITYEVENT_H
#define SE_INCL_ENTITYEVENT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// a BOOL that is constructed with value of FALSE (used in some entity initializations)
class ENGINE_API CBoolDefaultFalse {
public:
  BOOL bdf_bValue;
  CBoolDefaultFalse(void) : bdf_bValue(FALSE) {};
};

/*
 * An base class for event message passed to AI functions.
 */
class ENGINE_API CEntityEvent {
public:
  SLONG ee_slEvent;   // event code
  // other data members are placed in derived class

  CEntityEvent(SLONG slEventCode) : ee_slEvent(slEventCode) {};
  virtual ~CEntityEvent(void) {}; // destructor must be virtual
  // used for copying events for later delivery
  virtual CEntityEvent *MakeCopy(void) {
    CEntityEvent *peeCopy = new CEntityEvent(*this);
    return peeCopy;
  };
};
// a reference to a void event for use as default parameter
ENGINE_API extern const CEntityEvent &_eeVoid;

#endif  /* include-once check. */

