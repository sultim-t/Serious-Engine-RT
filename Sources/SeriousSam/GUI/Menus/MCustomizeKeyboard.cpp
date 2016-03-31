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
#include "MCustomizeKeyboard.h"


void CCustomizeKeyboardMenu::FillListItems(void)
{
  // disable all items first
  for (INDEX i = 0; i<KEYS_ON_SCREEN; i++) {
    gm_mgKey[i].mg_bEnabled = FALSE;
    gm_mgKey[i].mg_iInList = -2;
  }

  BOOL bHasFirst = FALSE;
  BOOL bHasLast = FALSE;
  // set diks to key buttons
  INDEX iLabel = 0;
  INDEX ctLabels = _pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions.Count();
  FOREACHINLIST(CButtonAction, ba_lnNode, _pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions, itAct)
  {
    INDEX iInMenu = iLabel - gm_iListOffset;
    if ((iLabel >= gm_iListOffset) &&
      (iLabel<(gm_iListOffset + gm_ctListVisible)))
    {
      bHasFirst |= (iLabel == 0);
      bHasLast |= (iLabel == ctLabels - 1);
      gm_mgKey[iInMenu].mg_strLabel = TranslateConst(itAct->ba_strName, 0);
      gm_mgKey[iInMenu].mg_iControlNumber = iLabel;
      gm_mgKey[iInMenu].SetBindingNames(FALSE);
      gm_mgKey[iInMenu].mg_strTip = TRANS("Enter - change binding, Backspace - unbind");
      gm_mgKey[iInMenu].mg_bEnabled = TRUE;
      gm_mgKey[iInMenu].mg_iInList = iLabel;
    }
    iLabel++;
  }

  // enable/disable up/down arrows
  gm_mgArrowUp.mg_bEnabled = !bHasFirst && ctLabels>0;
  gm_mgArrowDn.mg_bEnabled = !bHasLast  && ctLabels>0;
}

void CCustomizeKeyboardMenu::Initialize_t(void)
{
  // intialize Audio options menu
  gm_mgTitle.mg_strText = TRANS("CUSTOMIZE BUTTONS");
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

#define KL_START 3.0f
#define KL_STEEP -1.45f
  for (INDEX iLabel = 0; iLabel<KEYS_ON_SCREEN; iLabel++)
  {
    INDEX iPrev = (gm_ctListVisible + iLabel - 1) % KEYS_ON_SCREEN;
    INDEX iNext = (iLabel + 1) % KEYS_ON_SCREEN;
    // initialize label entities
    gm_mgKey[iLabel].mg_boxOnScreen = BoxKeyRow(iLabel);
    // initialize label gadgets
    gm_mgKey[iLabel].mg_pmgUp = &gm_mgKey[iPrev];
    gm_mgKey[iLabel].mg_pmgDown = &gm_mgKey[iNext];
    gm_mgKey[iLabel].mg_bVisible = TRUE;
    gm_lhGadgets.AddTail(gm_mgKey[iLabel].mg_lnNode);
  }
  // arrows just exist
  gm_lhGadgets.AddTail(gm_mgArrowDn.mg_lnNode);
  gm_lhGadgets.AddTail(gm_mgArrowUp.mg_lnNode);
  gm_mgArrowDn.mg_adDirection = AD_DOWN;
  gm_mgArrowUp.mg_adDirection = AD_UP;
  gm_mgArrowDn.mg_boxOnScreen = BoxArrow(AD_DOWN);
  gm_mgArrowUp.mg_boxOnScreen = BoxArrow(AD_UP);
  gm_mgArrowDn.mg_pmgRight = gm_mgArrowDn.mg_pmgUp =
    &gm_mgKey[KEYS_ON_SCREEN - 1];
  gm_mgArrowUp.mg_pmgRight = gm_mgArrowUp.mg_pmgDown =
    &gm_mgKey[0];

  gm_ctListVisible = KEYS_ON_SCREEN;
  gm_pmgArrowUp = &gm_mgArrowUp;
  gm_pmgArrowDn = &gm_mgArrowDn;
  gm_pmgListTop = &gm_mgKey[0];
  gm_pmgListBottom = &gm_mgKey[KEYS_ON_SCREEN - 1];
}

void CCustomizeKeyboardMenu::StartMenu(void)
{
  ControlsMenuOn();
  gm_iListOffset = 0;
  gm_ctListTotal = _pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions.Count();
  gm_iListWantedItem = 0;
  CGameMenu::StartMenu();
}

void CCustomizeKeyboardMenu::EndMenu(void)
{
  ControlsMenuOff();
  CGameMenu::EndMenu();
}