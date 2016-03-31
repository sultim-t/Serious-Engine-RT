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
#include "VarList.h"
#include "MVar.h"

extern BOOL _bVarChanged;


void CVarMenu::Initialize_t(void)
{
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = "";
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

  for (INDEX iLabel = 0; iLabel<VARS_ON_SCREEN; iLabel++)
  {
    INDEX iPrev = (VARS_ON_SCREEN + iLabel - 1) % VARS_ON_SCREEN;
    INDEX iNext = (iLabel + 1) % VARS_ON_SCREEN;
    // initialize label gadgets
    gm_mgVar[iLabel].mg_pmgUp = &gm_mgVar[iPrev];
    gm_mgVar[iLabel].mg_pmgDown = &gm_mgVar[iNext];
    gm_mgVar[iLabel].mg_pmgLeft = &gm_mgApply;
    gm_mgVar[iLabel].mg_boxOnScreen = BoxMediumRow(iLabel);
    gm_mgVar[iLabel].mg_pActivatedFunction = NULL; // never called!
    gm_lhGadgets.AddTail(gm_mgVar[iLabel].mg_lnNode);
  }

  gm_mgApply.mg_boxOnScreen = BoxMediumRow(16.5f);
  gm_mgApply.mg_bfsFontSize = BFS_LARGE;
  gm_mgApply.mg_iCenterI = 1;
  gm_mgApply.mg_pmgLeft =
    gm_mgApply.mg_pmgRight =
    gm_mgApply.mg_pmgUp =
    gm_mgApply.mg_pmgDown = &gm_mgVar[0];
  gm_mgApply.mg_strText = TRANS("APPLY");
  gm_mgApply.mg_strTip = TRANS("apply changes");
  gm_lhGadgets.AddTail(gm_mgApply.mg_lnNode);
  gm_mgApply.mg_pActivatedFunction = NULL;

  gm_lhGadgets.AddTail(gm_mgArrowUp.mg_lnNode);
  gm_lhGadgets.AddTail(gm_mgArrowDn.mg_lnNode);
  gm_mgArrowUp.mg_adDirection = AD_UP;
  gm_mgArrowDn.mg_adDirection = AD_DOWN;
  gm_mgArrowUp.mg_boxOnScreen = BoxArrow(AD_UP);
  gm_mgArrowDn.mg_boxOnScreen = BoxArrow(AD_DOWN);
  gm_mgArrowUp.mg_pmgRight = gm_mgArrowUp.mg_pmgDown =
    &gm_mgVar[0];
  gm_mgArrowDn.mg_pmgRight = gm_mgArrowDn.mg_pmgUp =
    &gm_mgVar[VARS_ON_SCREEN - 1];

  gm_ctListVisible = VARS_ON_SCREEN;
  gm_pmgArrowUp = &gm_mgArrowUp;
  gm_pmgArrowDn = &gm_mgArrowDn;
  gm_pmgListTop = &gm_mgVar[0];
  gm_pmgListBottom = &gm_mgVar[VARS_ON_SCREEN - 1];
}

void CVarMenu::FillListItems(void)
{
  // disable all items first
  for (INDEX i = 0; i<VARS_ON_SCREEN; i++) {
    gm_mgVar[i].mg_bEnabled = FALSE;
    gm_mgVar[i].mg_pvsVar = NULL;
    gm_mgVar[i].mg_iInList = -2;
  }
  BOOL bHasFirst = FALSE;
  BOOL bHasLast = FALSE;
  INDEX ctLabels = _lhVarSettings.Count();
  INDEX iLabel = 0;

  FOREACHINLIST(CVarSetting, vs_lnNode, _lhVarSettings, itvs) {
    CVarSetting &vs = *itvs;
    INDEX iInMenu = iLabel - gm_iListOffset;
    if ((iLabel >= gm_iListOffset) &&
      (iLabel<(gm_iListOffset + VARS_ON_SCREEN)))
    {
      bHasFirst |= (iLabel == 0);
      bHasLast |= (iLabel == ctLabels - 1);
      gm_mgVar[iInMenu].mg_pvsVar = &vs;
      gm_mgVar[iInMenu].mg_strTip = vs.vs_strTip;
      gm_mgVar[iInMenu].mg_bEnabled = gm_mgVar[iInMenu].IsEnabled();
      gm_mgVar[iInMenu].mg_iInList = iLabel;
    }
    iLabel++;
  }
  // enable/disable up/down arrows
  gm_mgArrowUp.mg_bEnabled = !bHasFirst && ctLabels>0;
  gm_mgArrowDn.mg_bEnabled = !bHasLast  && ctLabels>0;
}

void CVarMenu::StartMenu(void)
{
  LoadVarSettings(gm_fnmMenuCFG);
  // set default parameters for the list
  gm_iListOffset = 0;
  gm_ctListTotal = _lhVarSettings.Count();
  gm_iListWantedItem = 0;
  CGameMenu::StartMenu();
}

void CVarMenu::EndMenu(void)
{
  // disable all items first
  for (INDEX i = 0; i<VARS_ON_SCREEN; i++) {
    gm_mgVar[i].mg_bEnabled = FALSE;
    gm_mgVar[i].mg_pvsVar = NULL;
    gm_mgVar[i].mg_iInList = -2;
  }
  FlushVarSettings(FALSE);
  _bVarChanged = FALSE;
}

void CVarMenu::Think(void)
{
  gm_mgApply.mg_bEnabled = _bVarChanged;
  extern void FixupBackButton(CGameMenu *pgm);
  FixupBackButton(this);
}