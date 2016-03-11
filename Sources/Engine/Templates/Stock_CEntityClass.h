/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CENTITYCLASS_H
#define SE_INCL_STOCK_CENTITYCLASS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Entities/EntityClass.h>

#define TYPE CEntityClass
#define CStock_TYPE CStock_CEntityClass
#define CNameTable_TYPE CNameTable_CEntityClass
#define CNameTableSlot_TYPE CNameTableSlot_CEntityClass

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CEntityClass *_pEntityClassStock;


#endif  /* include-once check. */

