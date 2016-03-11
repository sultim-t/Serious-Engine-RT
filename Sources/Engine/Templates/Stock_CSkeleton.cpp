/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Templates/Stock_CSkeleton.h>

#define TYPE CSkeleton
#define CStock_TYPE CStock_CSkeleton
#define CNameTable_TYPE CNameTable_CSkeleton
#define CNameTableSlot_TYPE CNameTableSlot_CSkeleton

#include <Engine/Templates/NameTable.cpp>
#include <Engine/Templates/Stock.cpp>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

CStock_CSkeleton *_pSkeletonStock = NULL;
