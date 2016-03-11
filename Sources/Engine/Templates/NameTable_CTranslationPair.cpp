/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "stdh.h"

#include <Engine/Base/TranslationPair.h>

#define NAMETABLE_CASESENSITIVE 1
#define TYPE CTranslationPair
#define CNameTable_TYPE CNameTable_CTranslationPair
#define CNameTableSlot_TYPE CNameTableSlot_CTranslationPair

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/NameTable.cpp>

#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

