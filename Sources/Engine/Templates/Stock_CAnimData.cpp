/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Templates/Stock_CAnimData.h>

#define TYPE CAnimData
#define CStock_TYPE CStock_CAnimData
#define CNameTable_TYPE CNameTable_CAnimData
#define CNameTableSlot_TYPE CNameTableSlot_CAnimData

#include <Engine/Templates/NameTable.cpp>
#include <Engine/Templates/Stock.cpp>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

CStock_CAnimData *_pAnimStock = NULL;
