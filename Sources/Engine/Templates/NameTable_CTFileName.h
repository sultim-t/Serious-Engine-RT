/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_NAMETABLE_CTFILENAME_H
#define SE_INCL_NAMETABLE_CTFILENAME_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#define TYPE CTFileName
#define CNameTable_TYPE CNameTable_CTFileName
#define CNameTableSlot_TYPE CNameTableSlot_CTFileName
#include <Engine/Templates/NameTable.h>
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE



#endif  /* include-once check. */

