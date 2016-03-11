/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CANIMSET_H
#define SE_INCL_STOCK_CANIMSET_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Ska/AnimSet.h>

#define TYPE CAnimSet
#define CStock_TYPE CStock_CAnimSet
#define CNameTable_TYPE CNameTable_CAnimSet
#define CNameTableSlot_TYPE CNameTableSlot_CAnimSet

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CAnimSet *_pAnimSetStock;


#endif  /* include-once check. */

