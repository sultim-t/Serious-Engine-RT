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
#include "MNetworkJoin.h"


void CNetworkJoinMenu::Initialize_t(void)
{
  // title
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("JOIN GAME");
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

  gm_mgLAN.mg_bfsFontSize = BFS_LARGE;
  gm_mgLAN.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgLAN.mg_pmgUp = &gm_mgOpen;
  gm_mgLAN.mg_pmgDown = &gm_mgNET;
  gm_mgLAN.mg_strText = TRANS("SEARCH LAN");
  gm_mgLAN.mg_strTip = TRANS("search local network for servers");
  gm_lhGadgets.AddTail(gm_mgLAN.mg_lnNode);
  gm_mgLAN.mg_pActivatedFunction = NULL;

  gm_mgNET.mg_bfsFontSize = BFS_LARGE;
  gm_mgNET.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgNET.mg_pmgUp = &gm_mgLAN;
  gm_mgNET.mg_pmgDown = &gm_mgOpen;
  gm_mgNET.mg_strText = TRANS("SEARCH INTERNET");
  gm_mgNET.mg_strTip = TRANS("search internet for servers");
  gm_lhGadgets.AddTail(gm_mgNET.mg_lnNode);
  gm_mgNET.mg_pActivatedFunction = NULL;

  gm_mgOpen.mg_bfsFontSize = BFS_LARGE;
  gm_mgOpen.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgOpen.mg_pmgUp = &gm_mgNET;
  gm_mgOpen.mg_pmgDown = &gm_mgLAN;
  gm_mgOpen.mg_strText = TRANS("SPECIFY SERVER");
  gm_mgOpen.mg_strTip = TRANS("type in server address to connect to");
  gm_lhGadgets.AddTail(gm_mgOpen.mg_lnNode);
  gm_mgOpen.mg_pActivatedFunction = NULL;
}