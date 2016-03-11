/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_NAMETABLE_CTRANSLATIONPAIR_H
#define SE_INCL_NAMETABLE_CTRANSLATIONPAIR_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#define TYPE CTranslationPair
#define CNameTable_TYPE CNameTable_CTranslationPair
#define CNameTableSlot_TYPE CNameTableSlot_CTranslationPair
#include <Engine/Templates/NameTable.h>
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE



#endif  /* include-once check. */

