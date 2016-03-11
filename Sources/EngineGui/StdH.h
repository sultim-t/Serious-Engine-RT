/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <commdlg.h>

#define ENGINE_INTERNAL 1
#define ENGINEGUI_EXPORTS 1
#include <EngineGUI/EngineGUI.h>

#include "Resource.h"
#include "DlgSelectMode.h"
#include "WndDisplayTexture.h"
#include "DlgChooseTextureType.h"
#include "DlgCreateNormalTexture.h"
#include "DlgCreateAnimatedTexture.h"
#include "DlgCreateEffectTexture.h"

// this is needed for resource setting
#ifndef NDEBUG
  #define ENGINEGUI_DLL_NAME "EngineGUID.dll"
#else
  #define ENGINEGUI_DLL_NAME "EngineGUI.dll"
#endif

