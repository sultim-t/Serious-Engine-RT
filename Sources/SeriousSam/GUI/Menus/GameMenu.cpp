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
#include "GameMenu.h"


CGameMenu::CGameMenu(void)
{
  gm_pgmParentMenu = NULL;
  gm_pmgSelectedByDefault = NULL;
  gm_pmgArrowUp = NULL;
  gm_pmgArrowDn = NULL;
  gm_pmgListTop = NULL;
  gm_pmgListBottom = NULL;
  gm_iListOffset = 0;
  gm_ctListVisible = 0;
  gm_ctListTotal = 0;
  gm_bPopup = FALSE;
}

void CGameMenu::Initialize_t(void)
{
}

void CGameMenu::Destroy(void)
{
}

void CGameMenu::FillListItems(void)
{
  ASSERT(FALSE);  // must be implemented to scroll up/down
}

void CGameMenu::KillAllFocuses(void)
{
  // for each menu gadget in menu
  FOREACHINLIST(CMenuGadget, mg_lnNode, gm_lhGadgets, itmg) {
    itmg->mg_bFocused = FALSE;
  }
}

void CGameMenu::Think(void)
{
}

// +-1 -> hit top/bottom when pressing up/down on keyboard
// +-2 -> pressed pageup/pagedown on keyboard
// +-3 -> pressed arrow up/down  button in menu
// +-4 -> scrolling with mouse wheel
void CGameMenu::ScrollList(INDEX iDir)
{
  // if not valid for scrolling
  if (gm_ctListTotal <= 0
    || gm_pmgArrowUp == NULL || gm_pmgArrowDn == NULL
    || gm_pmgListTop == NULL || gm_pmgListBottom == NULL) {
    // do nothing
    return;
  }

  INDEX iOldTopKey = gm_iListOffset;
  // change offset
  switch (iDir) {
  case -1:
    gm_iListOffset -= 1;
    break;
  case -4:
    gm_iListOffset -= 3;
    break;
  case -2:
  case -3:
    gm_iListOffset -= gm_ctListVisible;
    break;
  case +1:
    gm_iListOffset += 1;
    break;
  case +4:
    gm_iListOffset += 3;
    break;
  case +2:
  case +3:
    gm_iListOffset += gm_ctListVisible;
    break;
  default:
    ASSERT(FALSE);
    return;
  }
  if (gm_ctListTotal <= gm_ctListVisible) {
    gm_iListOffset = 0;
  }
  else {
    gm_iListOffset = Clamp(gm_iListOffset, INDEX(0), INDEX(gm_ctListTotal - gm_ctListVisible));
  }

  // set new names
  FillListItems();

  // if scroling with wheel
  if (iDir == +4 || iDir == -4) {
    // no focus changing
    return;
  }

  // delete all focuses
  FOREACHINLIST(CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    itmg->OnKillFocus();
  }

  // set new focus
  const INDEX iFirst = 0;
  const INDEX iLast = gm_ctListVisible - 1;
  switch (iDir) {
  case +1:
    gm_pmgListBottom->OnSetFocus();
    break;
  case +2:
    if (gm_iListOffset != iOldTopKey) {
      gm_pmgListTop->OnSetFocus();
    }
    else {
      gm_pmgListBottom->OnSetFocus();
    }
    break;
  case +3:
    gm_pmgArrowDn->OnSetFocus();
    break;
  case -1:
    gm_pmgListTop->OnSetFocus();
    break;
  case -2:
    gm_pmgListTop->OnSetFocus();
    break;
  case -3:
    gm_pmgArrowUp->OnSetFocus();
    break;
  }
}

BOOL CGameMenu::OnChar(MSG msg)
{
  // find curently active gadget
  CMenuGadget *pmgActive = NULL;
  // for each menu gadget in menu
  FOREACHINLIST(CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if focused
    if (itmg->mg_bFocused) {
      // remember as active
      pmgActive = &itmg.Current();
    }
  }

  // if none focused
  if (pmgActive == NULL) {
    // do nothing
    return FALSE;
  }

  // if active gadget handles it
  if (pmgActive->OnChar(msg)) {
    // key is handled
    return TRUE;
  }

  // key is not handled
  return FALSE;
}

// return TRUE if handled
BOOL CGameMenu::OnKeyDown(int iVKey)
{
  // find curently active gadget
  CMenuGadget *pmgActive = NULL;
  // for each menu gadget in menu
  FOREACHINLIST(CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if focused
    if (itmg->mg_bFocused) {
      // remember as active
      pmgActive = &itmg.Current();
    }
  }

  // if none focused
  if (pmgActive == NULL) {
    // do nothing
    return FALSE;
  }

  // if active gadget handles it
  if (pmgActive->OnKeyDown(iVKey)) {
    // key is handled
    return TRUE;
  }

  // process normal in menu movement
  switch (iVKey) {
  case VK_PRIOR:
    ScrollList(-2);
    return TRUE;
  case VK_NEXT:
    ScrollList(+2);
    return TRUE;

  case 11:
    ScrollList(-4);
    return TRUE;
  case 10:
    ScrollList(+4);
    return TRUE;

  case VK_UP:
    // if this is top button in list
    if (pmgActive == gm_pmgListTop) {
      // scroll list up
      ScrollList(-1);
      // key is handled
      return TRUE;
    }
    // if we can go up
    if (pmgActive->mg_pmgUp != NULL && pmgActive->mg_pmgUp->mg_bVisible) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      pmgActive = pmgActive->mg_pmgUp;
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_DOWN:
    // if this is bottom button in list
    if (pmgActive == gm_pmgListBottom) {
      // scroll list down
      ScrollList(+1);
      // key is handled
      return TRUE;
    }
    // if we can go down
    if (pmgActive->mg_pmgDown != NULL && pmgActive->mg_pmgDown->mg_bVisible) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      pmgActive = pmgActive->mg_pmgDown;
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_LEFT:
    // if we can go left
    if (pmgActive->mg_pmgLeft != NULL) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      if (!pmgActive->mg_pmgLeft->mg_bVisible && gm_pmgSelectedByDefault != NULL) {
        pmgActive = gm_pmgSelectedByDefault;
      }
      else {
        pmgActive = pmgActive->mg_pmgLeft;
      }
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_RIGHT:
    // if we can go right
    if (pmgActive->mg_pmgRight != NULL) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      if (!pmgActive->mg_pmgRight->mg_bVisible && gm_pmgSelectedByDefault != NULL) {
        pmgActive = gm_pmgSelectedByDefault;
      }
      else {
        pmgActive = pmgActive->mg_pmgRight;
      }
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  }

  // key is not handled
  return FALSE;
}

void CGameMenu::StartMenu(void)
{
  // for each menu gadget in menu
  FOREACHINLIST(CMenuGadget, mg_lnNode, gm_lhGadgets, itmg)
  {
    itmg->mg_bFocused = FALSE;
    // call appear
    itmg->Appear();
  }

  // if there is a list
  if (gm_pmgListTop != NULL) {
    // scroll it so that the wanted tem is centered
    gm_iListOffset = gm_iListWantedItem - gm_ctListVisible / 2;
    // clamp the scrolling
    gm_iListOffset = Clamp(gm_iListOffset, 0L, Max(0L, gm_ctListTotal - gm_ctListVisible));

    // fill the list
    FillListItems();

    // for each menu gadget in menu
    FOREACHINLIST(CMenuGadget, mg_lnNode, gm_lhGadgets, itmg) {
      // if in list, but disabled
      if (itmg->mg_iInList == -2) {
        // hide it
        itmg->mg_bVisible = FALSE;
      // if in list
      } else if (itmg->mg_iInList >= 0) {
        // show it
        itmg->mg_bVisible = TRUE;
      }
      // if wanted
      if (itmg->mg_iInList == gm_iListWantedItem) {
        // focus it
        itmg->OnSetFocus();
        gm_pmgSelectedByDefault = itmg;
      }
    }
  }
}

void CGameMenu::EndMenu(void)
{
  // for each menu gadget in menu
  FOREACHINLIST(CMenuGadget, mg_lnNode, gm_lhGadgets, itmg)
  {
    // call disappear
    itmg->Disappear();
  }
}