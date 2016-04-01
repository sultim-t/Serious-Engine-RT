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
#include "MNetwork.h"


void CNetworkMenu::Initialize_t(void)
{
  // intialize network menu
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("NETWORK");
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

  gm_mgJoin.mg_bfsFontSize = BFS_LARGE;
  gm_mgJoin.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgJoin.mg_pmgUp = &gm_mgLoad;
  gm_mgJoin.mg_pmgDown = &gm_mgStart;
  gm_mgJoin.mg_strText = TRANS("JOIN GAME");
  gm_mgJoin.mg_strTip = TRANS("join a network game");
  gm_lhGadgets.AddTail(gm_mgJoin.mg_lnNode);
  gm_mgJoin.mg_pActivatedFunction = NULL;

  gm_mgStart.mg_bfsFontSize = BFS_LARGE;
  gm_mgStart.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgStart.mg_pmgUp = &gm_mgJoin;
  gm_mgStart.mg_pmgDown = &gm_mgQuickLoad;
  gm_mgStart.mg_strText = TRANS("START SERVER");
  gm_mgStart.mg_strTip = TRANS("start a network game server");
  gm_lhGadgets.AddTail(gm_mgStart.mg_lnNode);
  gm_mgStart.mg_pActivatedFunction = NULL;

  gm_mgQuickLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuickLoad.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgQuickLoad.mg_pmgUp = &gm_mgStart;
  gm_mgQuickLoad.mg_pmgDown = &gm_mgLoad;
  gm_mgQuickLoad.mg_strText = TRANS("QUICK LOAD");
  gm_mgQuickLoad.mg_strTip = TRANS("load a quick-saved game (F9)");
  gm_lhGadgets.AddTail(gm_mgQuickLoad.mg_lnNode);
  gm_mgQuickLoad.mg_pActivatedFunction = NULL;

  gm_mgLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgLoad.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgLoad.mg_pmgUp = &gm_mgQuickLoad;
  gm_mgLoad.mg_pmgDown = &gm_mgJoin;
  gm_mgLoad.mg_strText = TRANS("LOAD");
  gm_mgLoad.mg_strTip = TRANS("start server and load a network game (server only)");
  gm_lhGadgets.AddTail(gm_mgLoad.mg_lnNode);
  gm_mgLoad.mg_pActivatedFunction = NULL;
}

void CNetworkMenu::StartMenu(void)
{
  CGameMenu::StartMenu();
}