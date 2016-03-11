/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_PROGRESSHOOK_H
#define SE_INCL_PROGRESSHOOK_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// structure describing current state during loading, passed to loading hook
class CProgressHookInfo {
public:
  CTString phi_strDescription;    // world/savegame/session that is loading/connecting, etc.
  FLOAT phi_fCompleted;           // completed ratio [0..1]
};

// set hook for loading/connecting
extern ENGINE_API void SetProgressHook(void (*pHook_t)(CProgressHookInfo *pgli));
// call loading/connecting hook
extern ENGINE_API void SetProgressDescription(const CTString &strDescription);
extern ENGINE_API void CallProgressHook_t(FLOAT fCompleted);


#endif  /* include-once check. */

