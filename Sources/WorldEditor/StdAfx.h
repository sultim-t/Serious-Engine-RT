/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxadv.h>
#include <afxole.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <htmlhelp.h>

#define ENGINE_INTERNAL 1
#include <EngineGUI/EngineGUI.h>
#include <Engine/GameShell.h>
#include <Engine/Base/ChangeableRT.h>
#include <Engine/Base/UpdateableRT.h>
#include <Engine/Terrain/Terrain.h>
#include <Engine/Terrain/TerrainMisc.h>

#include "WorldEditor.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")