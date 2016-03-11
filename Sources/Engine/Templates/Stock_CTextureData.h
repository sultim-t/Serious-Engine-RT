/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CTEXTUREDATA_H
#define SE_INCL_STOCK_CTEXTUREDATA_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Graphics/Texture.h>

#define TYPE CTextureData
#define CStock_TYPE CStock_CTextureData
#define CNameTable_TYPE CNameTable_CTextureData
#define CNameTableSlot_TYPE CNameTableSlot_CTextureData

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CTextureData *_pTextureStock;


#endif  /* include-once check. */

