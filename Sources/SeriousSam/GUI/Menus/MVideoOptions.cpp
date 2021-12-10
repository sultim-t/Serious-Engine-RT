/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include "MenuStuff.h"
#include "MVideoOptions.h"

extern void InitVideoOptionsButtons();
extern void UpdateVideoOptionsButtons(INDEX iSelected);


void CVideoOptionsMenu::Initialize_t(void)
{
  // intialize video options menu
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("VIDEO");
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

#ifndef SE1_RAYTRACING
  TRIGGER_MG(gm_mgDisplayAPITrigger, 0,
    gm_mgApply, gm_mgDisplayAdaptersTrigger, TRANS("GRAPHICS API"), astrDisplayAPIRadioTexts);
  gm_mgDisplayAPITrigger.mg_strTip = TRANS("choose graphics API to be used");
  TRIGGER_MG(gm_mgDisplayAdaptersTrigger, 1,
    gm_mgDisplayAPITrigger, gm_mgDisplayPrefsTrigger, TRANS("DISPLAY ADAPTER"), astrNoYes);
  gm_mgDisplayAdaptersTrigger.mg_strTip = TRANS("choose display adapter to be used");
  TRIGGER_MG(gm_mgDisplayPrefsTrigger, 2,
    gm_mgDisplayAdaptersTrigger, gm_mgResolutionsTrigger, TRANS("PREFERENCES"), astrDisplayPrefsRadioTexts);
  gm_mgDisplayPrefsTrigger.mg_strTip = TRANS("balance between speed and rendering quality, depending on your system");
  TRIGGER_MG(gm_mgResolutionsTrigger, 3,
    gm_mgDisplayPrefsTrigger, gm_mgFullScreenTrigger, TRANS("RESOLUTION"), astrNoYes);
  gm_mgResolutionsTrigger.mg_strTip = TRANS("select video mode resolution");
  TRIGGER_MG(gm_mgFullScreenTrigger, 4,
    gm_mgResolutionsTrigger, gm_mgBitsPerPixelTrigger, TRANS("FULL SCREEN"), astrNoYes);
  gm_mgFullScreenTrigger.mg_strTip = TRANS("make game run in a window or in full screen");
  TRIGGER_MG(gm_mgBitsPerPixelTrigger, 5,
    gm_mgFullScreenTrigger, gm_mgVideoRendering, TRANS("BITS PER PIXEL"), astrBitsPerPixelRadioTexts);
  gm_mgBitsPerPixelTrigger.mg_strTip = TRANS("select number of colors used for display");
#else
  TRIGGER_MG(gm_mgDisplayAPITrigger, 0,
    gm_mgApply, gm_mgDisplayAdaptersTrigger, TRANS("GRAPHICS API"), astrDisplayAPIRadioTexts);
  gm_mgDisplayAPITrigger.mg_strTip = TRANS("choose graphics API to be used");
  TRIGGER_MG(gm_mgDisplayAdaptersTrigger, 1,
    gm_mgDisplayAPITrigger, gm_mgResolutionsTrigger, TRANS("DISPLAY ADAPTER"), astrNoYes);
  gm_mgDisplayAdaptersTrigger.mg_strTip = TRANS("choose display adapter to be used");
  TRIGGER_MG(gm_mgResolutionsTrigger, 2,
    gm_mgDisplayAdaptersTrigger, gm_mgNvDlssTrigger, TRANS("WINDOW SIZE"), astrNoYes);
  gm_mgResolutionsTrigger.mg_strTip = TRANS("select size of the presentation window");

  // new
  TRIGGER_MG(gm_mgNvDlssTrigger, 3.5,
    gm_mgResolutionsTrigger, gm_mgAmdFsrTrigger, TRANS("NVIDIA DLSS"), astrNvDlssTexts);
  gm_mgNvDlssTrigger.mg_strTip = TRANS("set Nvidia DLSS quality mode");
  TRIGGER_MG(gm_mgAmdFsrTrigger, 4.5,
    gm_mgNvDlssTrigger, gm_mgRenderScaleTrigger, TRANS("AMD FSR"), astrAmdFsrTexts);
  gm_mgAmdFsrTrigger.mg_strTip = TRANS("set AMD FSR 1.0 quality mode");
  TRIGGER_MG(gm_mgRenderScaleTrigger, 5.5,
    gm_mgAmdFsrTrigger, gm_mgCPUPerformanceTrigger, TRANS("CUSTOM RENDER SCALE"), astrRenderScalePercentsTexts);
  gm_mgRenderScaleTrigger.mg_strTip = TRANS("set render resolution scale, if FSR and DLSS are disabled");  
  TRIGGER_MG(gm_mgCPUPerformanceTrigger, 7,
    gm_mgRenderScaleTrigger, gm_mgVideoRendering, TRANS("CPU PRESET"), astrCPUPerformancePresetTexts);
  gm_mgCPUPerformanceTrigger.mg_strTip = TRANS("set CPU performance preset");

  // ignored
  const int iOutOfScreenRow = 128;
  
  TRIGGER_MG(gm_mgFullScreenTrigger, iOutOfScreenRow,
    gm_mgResolutionsTrigger, gm_mgVideoRendering, TRANS("FULL SCREEN"), astrNoYes);
  gm_mgFullScreenTrigger.mg_strTip = TRANS("make game run in a window or in full screen");

  TRIGGER_MG(gm_mgDisplayPrefsTrigger, iOutOfScreenRow,
    gm_mgDisplayAdaptersTrigger, gm_mgResolutionsTrigger, TRANS("PREFERENCES"), astrDisplayPrefsRadioTexts);
  gm_mgDisplayPrefsTrigger.mg_strTip = TRANS("balance between speed and rendering quality, depending on your system");
  TRIGGER_MG(gm_mgBitsPerPixelTrigger, iOutOfScreenRow,
    gm_mgFullScreenTrigger, gm_mgVideoRendering, TRANS("BITS PER PIXEL"), astrBitsPerPixelRadioTexts);
  gm_mgBitsPerPixelTrigger.mg_strTip = TRANS("select number of colors used for display");
#endif

  gm_mgDisplayPrefsTrigger.mg_pOnTriggerChange = NULL;
  gm_mgDisplayAPITrigger.mg_pOnTriggerChange = NULL;
  gm_mgDisplayAdaptersTrigger.mg_pOnTriggerChange = NULL;
  gm_mgFullScreenTrigger.mg_pOnTriggerChange = NULL;
  gm_mgResolutionsTrigger.mg_pOnTriggerChange = NULL;
#ifdef SE1_RAYTRACING
  gm_mgRenderScaleTrigger.mg_pOnTriggerChange = NULL;
  gm_mgCPUPerformanceTrigger.mg_pOnTriggerChange = NULL;
  gm_mgNvDlssTrigger.mg_pOnTriggerChange = NULL;
  gm_mgAmdFsrTrigger.mg_pOnTriggerChange = NULL;
#endif // SE1_RAYTRACING
  gm_mgBitsPerPixelTrigger.mg_pOnTriggerChange = NULL;

  gm_mgVideoRendering.mg_bfsFontSize = BFS_MEDIUM;
#ifndef SE1_RAYTRACING
  gm_mgVideoRendering.mg_boxOnScreen = BoxMediumRow(7.0f);
  gm_mgVideoRendering.mg_pmgUp = &gm_mgBitsPerPixelTrigger;
#else
  gm_mgVideoRendering.mg_boxOnScreen = BoxMediumRow(9.0f);
  gm_mgVideoRendering.mg_pmgUp = &gm_mgCPUPerformanceTrigger;
#endif // !SE1_RAYTRACING
  gm_mgVideoRendering.mg_pmgDown = &gm_mgApply;
  gm_mgVideoRendering.mg_strText = TRANS("RENDERING OPTIONS");
  gm_mgVideoRendering.mg_strTip = TRANS("manually adjust rendering settings");
  gm_lhGadgets.AddTail(gm_mgVideoRendering.mg_lnNode);
  gm_mgVideoRendering.mg_pActivatedFunction = NULL;

  gm_mgApply.mg_bfsFontSize = BFS_LARGE;
  gm_mgApply.mg_boxOnScreen = BoxBigRow(7.0f);
  gm_mgApply.mg_pmgUp = &gm_mgVideoRendering;
  gm_mgApply.mg_pmgDown = &gm_mgDisplayAPITrigger;
  gm_mgApply.mg_strText = TRANS("APPLY");
  gm_mgApply.mg_strTip = TRANS("apply selected options");
  gm_lhGadgets.AddTail(gm_mgApply.mg_lnNode);
  gm_mgApply.mg_pActivatedFunction = NULL;
}

void CVideoOptionsMenu::StartMenu(void)
{
  InitVideoOptionsButtons();

  CGameMenu::StartMenu();

  UpdateVideoOptionsButtons(-1);
}