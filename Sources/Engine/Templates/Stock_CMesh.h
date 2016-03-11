/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CMESH_H
#define SE_INCL_STOCK_CMESH_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Ska/Mesh.h>

#define TYPE CMesh
#define CStock_TYPE CStock_CMesh
#define CNameTable_TYPE CNameTable_CMesh
#define CNameTableSlot_TYPE CNameTableSlot_CMesh

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CMesh *_pMeshStock;


#endif  /* include-once check. */

