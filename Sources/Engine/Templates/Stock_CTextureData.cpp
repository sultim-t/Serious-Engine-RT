/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Templates/Stock_CTextureData.h>

#define TYPE CTextureData
#define CStock_TYPE CStock_CTextureData
#define CNameTable_TYPE CNameTable_CTextureData
#define CNameTableSlot_TYPE CNameTableSlot_CTextureData

#include <Engine/Templates/NameTable.cpp>
#include <Engine/Templates/Stock.cpp>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

CStock_CTextureData *_pTextureStock = NULL;
