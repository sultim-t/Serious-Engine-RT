/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CMODELDATA_H
#define SE_INCL_STOCK_CMODELDATA_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Models/ModelData.h>
#include <Engine/Models/Model_internal.h>

#define TYPE CModelData
#define CStock_TYPE CStock_CModelData
#define CNameTable_TYPE CNameTable_CModelData
#define CNameTableSlot_TYPE CNameTableSlot_CModelData

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CModelData *_pModelStock;


#endif  /* include-once check. */

