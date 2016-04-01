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
#include "MServers.h"

CTString _strServerFilter[7];
CMGButton mgServerColumn[7];
CMGEdit mgServerFilter[7];

void CServersMenu::Initialize_t(void)
{
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("CHOOSE SERVER");
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

  gm_mgList.mg_boxOnScreen = FLOATaabbox2D(FLOAT2D(0, 0), FLOAT2D(1, 1));
  gm_mgList.mg_pmgLeft = &gm_mgList;  // make sure it can get focus
  gm_mgList.mg_bEnabled = TRUE;
  gm_lhGadgets.AddTail(gm_mgList.mg_lnNode);

  ASSERT(ARRAYCOUNT(mgServerColumn) == ARRAYCOUNT(mgServerFilter));
  for (INDEX i = 0; i<ARRAYCOUNT(mgServerFilter); i++) {
    mgServerColumn[i].mg_strText = "";
    mgServerColumn[i].mg_boxOnScreen = BoxPlayerEdit(5.0);
    mgServerColumn[i].mg_bfsFontSize = BFS_SMALL;
    mgServerColumn[i].mg_iCenterI = -1;
    mgServerColumn[i].mg_pmgUp = &gm_mgList;
    mgServerColumn[i].mg_pmgDown = &mgServerFilter[i];
    gm_lhGadgets.AddTail(mgServerColumn[i].mg_lnNode);

    mgServerFilter[i].mg_ctMaxStringLen = 25;
    mgServerFilter[i].mg_boxOnScreen = BoxPlayerEdit(5.0);
    mgServerFilter[i].mg_bfsFontSize = BFS_SMALL;
    mgServerFilter[i].mg_iCenterI = -1;
    mgServerFilter[i].mg_pmgUp = &mgServerColumn[i];
    mgServerFilter[i].mg_pmgDown = &gm_mgList;
    gm_lhGadgets.AddTail(mgServerFilter[i].mg_lnNode);
    mgServerFilter[i].mg_pstrToChange = &_strServerFilter[i];
    mgServerFilter[i].SetText(*mgServerFilter[i].mg_pstrToChange);
  }

  gm_mgRefresh.mg_strText = TRANS("REFRESH");
  gm_mgRefresh.mg_boxOnScreen = BoxLeftColumn(15.0);
  gm_mgRefresh.mg_bfsFontSize = BFS_SMALL;
  gm_mgRefresh.mg_iCenterI = -1;
  gm_mgRefresh.mg_pmgDown = &gm_mgList;
  gm_mgRefresh.mg_pActivatedFunction = NULL;
  gm_lhGadgets.AddTail(gm_mgRefresh.mg_lnNode);

  CTString astrColumns[7];
  mgServerColumn[0].mg_strText = TRANS("Server");
  mgServerColumn[1].mg_strText = TRANS("Map");
  mgServerColumn[2].mg_strText = TRANS("Ping");
  mgServerColumn[3].mg_strText = TRANS("Players");
  mgServerColumn[4].mg_strText = TRANS("Game");
  mgServerColumn[5].mg_strText = TRANS("Mod");
  mgServerColumn[6].mg_strText = TRANS("Ver");
  mgServerColumn[0].mg_pActivatedFunction = NULL;
  mgServerColumn[1].mg_pActivatedFunction = NULL;
  mgServerColumn[2].mg_pActivatedFunction = NULL;
  mgServerColumn[3].mg_pActivatedFunction = NULL;
  mgServerColumn[4].mg_pActivatedFunction = NULL;
  mgServerColumn[5].mg_pActivatedFunction = NULL;
  mgServerColumn[6].mg_pActivatedFunction = NULL;
  mgServerColumn[0].mg_strTip = TRANS("sort by server");
  mgServerColumn[1].mg_strTip = TRANS("sort by map");
  mgServerColumn[2].mg_strTip = TRANS("sort by ping");
  mgServerColumn[3].mg_strTip = TRANS("sort by players");
  mgServerColumn[4].mg_strTip = TRANS("sort by game");
  mgServerColumn[5].mg_strTip = TRANS("sort by mod");
  mgServerColumn[6].mg_strTip = TRANS("sort by version");
  mgServerFilter[0].mg_strTip = TRANS("filter by server");
  mgServerFilter[1].mg_strTip = TRANS("filter by map");
  mgServerFilter[2].mg_strTip = TRANS("filter by ping (ie. <200)");
  mgServerFilter[3].mg_strTip = TRANS("filter by players (ie. >=2)");
  mgServerFilter[4].mg_strTip = TRANS("filter by game (ie. coop)");
  mgServerFilter[5].mg_strTip = TRANS("filter by mod");
  mgServerFilter[6].mg_strTip = TRANS("filter by version");
}

void CServersMenu::StartMenu(void)
{
  extern void RefreshServerList(void);
  RefreshServerList();

  CGameMenu::StartMenu();
}

void CServersMenu::Think(void)
{
  if (!_pNetwork->ga_bEnumerationChange) {
    return;
  }
  _pNetwork->ga_bEnumerationChange = FALSE;
}
