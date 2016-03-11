/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Templates/Stock_CSoundData.h>

#define TYPE CSoundData
#define CStock_TYPE CStock_CSoundData
#define CNameTable_TYPE CNameTable_CSoundData
#define CNameTableSlot_TYPE CNameTableSlot_CSoundData

#include <Engine/Templates/NameTable.cpp>
#include <Engine/Templates/Stock.cpp>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

CStock_CSoundData *_pSoundStock = NULL;
