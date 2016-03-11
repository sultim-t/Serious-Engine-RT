/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Templates/Stock_CMesh.h>

#define TYPE CMesh
#define CStock_TYPE CStock_CMesh
#define CNameTable_TYPE CNameTable_CMesh
#define CNameTableSlot_TYPE CNameTableSlot_CMesh

#include <Engine/Templates/NameTable.cpp>
#include <Engine/Templates/Stock.cpp>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

CStock_CMesh *_pMeshStock = NULL;
