/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CANIMDATA_H
#define SE_INCL_STOCK_CANIMDATA_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Anim.h>

#define TYPE CAnimData
#define CStock_TYPE CStock_CAnimData
#define CNameTable_TYPE CNameTable_CAnimData
#define CNameTableSlot_TYPE CNameTableSlot_CAnimData

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CAnimData *_pAnimStock;


#endif  /* include-once check. */

