/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_IFEEL_H
#define SE_INCL_IFEEL_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

ENGINE_API BOOL IFeel_InitDevice(HINSTANCE &hInstance, HWND &hWnd);
ENGINE_API void IFeel_DeleteDevice();
ENGINE_API CTString IFeel_GetProductName();
ENGINE_API CTString IFeel_GetProjectFileName();
ENGINE_API BOOL IFeel_LoadFile(CTFileName fnFile);
ENGINE_API void IFeel_UnLoadFile();
ENGINE_API void IFeel_PlayEffect(char *pstrEffectName);
ENGINE_API void IFeel_StopEffect(char *pstrEffectName);
ENGINE_API void IFeel_ChangeGain(FLOAT fGain);

#endif  /* include-once check. */
