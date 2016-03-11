/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#define APPLICATION_NAME "SeriousSam"
#include "CDCheck.h"

extern HINSTANCE _hInstance;
extern BOOL _bRunning, _bQuitScreen;

extern INDEX sam_bFullScreenActive;
extern INDEX sam_iScreenSizeI;
extern INDEX sam_iScreenSizeJ;
extern INDEX sam_iDisplayDepth;
extern INDEX sam_iDisplayAdapter;
extern INDEX sam_iGfxAPI;
extern INDEX sam_iVideoSetup;  // 0==speed, 1==normal, 2==quality, 3==custom
extern BOOL  sam_bAutoAdjustAudio;

extern void StartNewMode( enum GfxAPIType eAPI, INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ,
                          enum DisplayDepth eColorDepth, BOOL bFullScreenMode);
struct KeyData {
  char *kd_strASCII;
};

extern CGame *_pGame;