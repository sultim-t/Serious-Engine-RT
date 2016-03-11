/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_SELECTION_H
#define SE_INCL_SELECTION_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Templates/DynamicContainer.h>

/*
 * A selection of some objects that support selecting.
 */
template <class cType, unsigned long ulFlag>
class CSelection : public CDynamicContainer<cType> {
public:
  /* Deselect all objects. */
  void Clear(void);
  /* Destructor. */
  ~CSelection(void) { Clear(); }

  /* Select one object. */
  void Select(cType &tToSelect);
  /* Deselect one object. */
  void Deselect(cType &tToDeselect);
  /* Test if one object is selected. */
  BOOL IsSelected(cType &tToSelect);
  /* Get first in selection. NULL if empty selection */
  cType *GetFirstInSelection(void);
};

// macro for implementing selecting features in a class
#define IMPLEMENT_SELECTING(m_ulFlags)                  \
  inline void Select(unsigned long ulFlag) {            \
    m_ulFlags |= ulFlag;                                \
  }                                                     \
  inline void Deselect(unsigned long ulFlag) {          \
    m_ulFlags &= ~ulFlag;                               \
  }                                                     \
  inline BOOL IsSelected(unsigned long ulFlag) const {  \
    return m_ulFlags & ulFlag;                          \
  }



#endif  /* include-once check. */

