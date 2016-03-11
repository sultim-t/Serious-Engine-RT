/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Templates/Stock_CModelData.h>

#define TYPE CModelData
#define CStock_TYPE CStock_CModelData
#define CNameTable_TYPE CNameTable_CModelData
#define CNameTableSlot_TYPE CNameTableSlot_CModelData

#include <Engine/Templates/NameTable.cpp>
#include <Engine/Templates/Stock.cpp>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

CStock_CModelData *_pModelStock = NULL;
