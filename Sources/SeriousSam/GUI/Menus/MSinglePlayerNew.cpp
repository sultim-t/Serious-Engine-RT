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
#include "MSinglePlayerNew.h"


void CSinglePlayerNewMenu::Initialize_t(void)
{
  // intialize single player new menu
  gm_mgTitle.mg_strText = TRANS("NEW GAME");
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

  gm_mgTourist.mg_strText = TRANS("TOURIST");
  gm_mgTourist.mg_bfsFontSize = BFS_LARGE;
  gm_mgTourist.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgTourist.mg_strTip = TRANS("for non-FPS players");
  gm_lhGadgets.AddTail(gm_mgTourist.mg_lnNode);
  gm_mgTourist.mg_pmgUp = &gm_mgSerious;
  gm_mgTourist.mg_pmgDown = &gm_mgEasy;
  gm_mgTourist.mg_pActivatedFunction = NULL;

  gm_mgEasy.mg_strText = TRANS("EASY");
  gm_mgEasy.mg_bfsFontSize = BFS_LARGE;
  gm_mgEasy.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgEasy.mg_strTip = TRANS("for unexperienced FPS players");
  gm_lhGadgets.AddTail(gm_mgEasy.mg_lnNode);
  gm_mgEasy.mg_pmgUp = &gm_mgTourist;
  gm_mgEasy.mg_pmgDown = &gm_mgMedium;
  gm_mgEasy.mg_pActivatedFunction = NULL;

  gm_mgMedium.mg_strText = TRANS("NORMAL");
  gm_mgMedium.mg_bfsFontSize = BFS_LARGE;
  gm_mgMedium.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgMedium.mg_strTip = TRANS("for experienced FPS players");
  gm_lhGadgets.AddTail(gm_mgMedium.mg_lnNode);
  gm_mgMedium.mg_pmgUp = &gm_mgEasy;
  gm_mgMedium.mg_pmgDown = &gm_mgHard;
  gm_mgMedium.mg_pActivatedFunction = NULL;

  gm_mgHard.mg_strText = TRANS("HARD");
  gm_mgHard.mg_bfsFontSize = BFS_LARGE;
  gm_mgHard.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgHard.mg_strTip = TRANS("for experienced Serious Sam players");
  gm_lhGadgets.AddTail(gm_mgHard.mg_lnNode);
  gm_mgHard.mg_pmgUp = &gm_mgMedium;
  gm_mgHard.mg_pmgDown = &gm_mgSerious;
  gm_mgHard.mg_pActivatedFunction = NULL;

  gm_mgSerious.mg_strText = TRANS("SERIOUS");
  gm_mgSerious.mg_bfsFontSize = BFS_LARGE;
  gm_mgSerious.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgSerious.mg_strTip = TRANS("are you serious?");
  gm_lhGadgets.AddTail(gm_mgSerious.mg_lnNode);
  gm_mgSerious.mg_pmgUp = &gm_mgHard;
  gm_mgSerious.mg_pmgDown = &gm_mgTourist;
  gm_mgSerious.mg_pActivatedFunction = NULL;

  gm_mgMental.mg_strText = TRANS("MENTAL");
  gm_mgMental.mg_bfsFontSize = BFS_LARGE;
  gm_mgMental.mg_boxOnScreen = BoxBigRow(5.0f);
  gm_mgMental.mg_strTip = TRANS("you are not serious!");
  gm_lhGadgets.AddTail(gm_mgMental.mg_lnNode);
  gm_mgMental.mg_pmgUp = &gm_mgSerious;
  gm_mgMental.mg_pmgDown = &gm_mgTourist;
  gm_mgMental.mg_pActivatedFunction = NULL;
  gm_mgMental.mg_bMental = TRUE;


}
void CSinglePlayerNewMenu::StartMenu(void)
{
  CGameMenu::StartMenu();
  extern INDEX sam_bMentalActivated;
  if (sam_bMentalActivated) {
    gm_mgMental.Appear();
    gm_mgSerious.mg_pmgDown = &gm_mgMental;
    gm_mgTourist.mg_pmgUp = &gm_mgMental;
  } else {
    gm_mgMental.Disappear();
    gm_mgSerious.mg_pmgDown = &gm_mgTourist;
    gm_mgTourist.mg_pmgUp = &gm_mgSerious;
  }
}
