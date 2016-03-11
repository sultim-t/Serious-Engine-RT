/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_STOCK_CSHADER_H
#define SE_INCL_STOCK_CSHADER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Graphics/Shader.h>

#define TYPE CShader
#define CStock_TYPE CStock_CShader
#define CNameTable_TYPE CNameTable_CShader
#define CNameTableSlot_TYPE CNameTableSlot_CShader

#include <Engine/Templates/NameTable.h>
#include <Engine/Templates/Stock.h>

#undef CStock_TYPE
#undef CNameTableSlot_TYPE
#undef CNameTable_TYPE
#undef TYPE

ENGINE_API extern CStock_CShader *_pShaderStock;


#endif  /* include-once check. */

