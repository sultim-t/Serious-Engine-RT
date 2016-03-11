/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Base/FileName.h>

#define NAMETABLE_CASESENSITIVE 0
#define TYPE CTFileName
#define CNameTable_TYPE CNameTable_CTFileName
#define CNameTableSlot_TYPE CNameTableSlot_CTFileName

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/NameTable.cpp>

#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

