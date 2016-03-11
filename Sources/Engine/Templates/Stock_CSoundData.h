/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CSOUNDDATA_H
#define SE_INCL_STOCK_CSOUNDDATA_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Sound/SoundData.h>

#define TYPE CSoundData
#define CStock_TYPE CStock_CSoundData
#define CNameTable_TYPE CNameTable_CSoundData
#define CNameTableSlot_TYPE CNameTableSlot_CSoundData

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CSoundData *_pSoundStock;


#endif  /* include-once check. */

