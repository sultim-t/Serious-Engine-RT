/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_TRANSLATIONPAIR_H
#define SE_INCL_TRANSLATIONPAIR_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/CTString.h>

class CTranslationPair {
public:
  BOOL m_bUsed;             // for internal use while building the table
  CTString tp_strSrc;       // original string
  CTString tp_strDst;       // translated string

  CTranslationPair(void) : m_bUsed(FALSE) {};

  inline void Clear(void) {
    tp_strSrc.Clear();
    tp_strDst.Clear();
  };

  // getname function for addinf to nametable
  inline const CTString &GetName(void) const { return tp_strSrc; };
};



#endif  /* include-once check. */

