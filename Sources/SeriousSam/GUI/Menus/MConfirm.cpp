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
#include "MConfirm.h"

void CConfirmMenu::Initialize_t(void)
{
  gm_bPopup = TRUE;

  gm_mgConfirmLabel.mg_strText = "";
  gm_lhGadgets.AddTail(gm_mgConfirmLabel.mg_lnNode);
  gm_mgConfirmLabel.mg_boxOnScreen = BoxPopupLabel();
  gm_mgConfirmLabel.mg_iCenterI = 0;
  gm_mgConfirmLabel.mg_bfsFontSize = BFS_LARGE;

  gm_mgConfirmYes.mg_strText = TRANS("YES");
  gm_lhGadgets.AddTail(gm_mgConfirmYes.mg_lnNode);
  gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesLarge();
  gm_mgConfirmYes.mg_pActivatedFunction = NULL;
  gm_mgConfirmYes.mg_pmgLeft =
    gm_mgConfirmYes.mg_pmgRight = &gm_mgConfirmNo;
  gm_mgConfirmYes.mg_iCenterI = 1;
  gm_mgConfirmYes.mg_bfsFontSize = BFS_LARGE;

  gm_mgConfirmNo.mg_strText = TRANS("NO");
  gm_lhGadgets.AddTail(gm_mgConfirmNo.mg_lnNode);
  gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoLarge();
  gm_mgConfirmNo.mg_pActivatedFunction = NULL;
  gm_mgConfirmNo.mg_pmgLeft =
    gm_mgConfirmNo.mg_pmgRight = &gm_mgConfirmYes;
  gm_mgConfirmNo.mg_iCenterI = -1;
  gm_mgConfirmNo.mg_bfsFontSize = BFS_LARGE;

  _pConfimedYes = NULL;
  _pConfimedNo = NULL;
}

void CConfirmMenu::BeLarge(void)
{
  gm_mgConfirmLabel.mg_bfsFontSize = BFS_LARGE;
  gm_mgConfirmYes.mg_bfsFontSize = BFS_LARGE;
  gm_mgConfirmNo.mg_bfsFontSize = BFS_LARGE;

  gm_mgConfirmLabel.mg_iCenterI = 0;
  gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesLarge();
  gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoLarge();
}

void CConfirmMenu::BeSmall(void)
{
  gm_mgConfirmLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgConfirmYes.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgConfirmNo.mg_bfsFontSize = BFS_MEDIUM;

  gm_mgConfirmLabel.mg_iCenterI = -1;
  gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesSmall();
  gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoSmall();
}

// return TRUE if handled
BOOL CConfirmMenu::OnKeyDown(int iVKey)
{
  if ((iVKey == VK_ESCAPE || iVKey == VK_RBUTTON) && gm_mgConfirmNo.mg_pActivatedFunction != NULL) {
    gm_mgConfirmNo.OnActivate();
    return TRUE;
  }
  return CGameMenu::OnKeyDown(iVKey);
}