/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_REGISTRY_H
#define SE_INCL_REGISTRY_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

ENGINE_API BOOL REG_GetString(const CTString &strKey, CTString &strString);
ENGINE_API BOOL REG_SetString(const CTString &strKey, const CTString &strString);
ENGINE_API BOOL REG_GetLong(const CTString &strKey, ULONG &ulLong);


#endif  /* include-once check. */

