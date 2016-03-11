/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CSKELETON_H
#define SE_INCL_STOCK_CSKELETON_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Ska/Skeleton.h>

#define TYPE CSkeleton
#define CStock_TYPE CStock_CSkeleton
#define CNameTable_TYPE CNameTable_CSkeleton
#define CNameTableSlot_TYPE CNameTableSlot_CSkeleton

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CSkeleton *_pSkeletonStock;


#endif  /* include-once check. */

