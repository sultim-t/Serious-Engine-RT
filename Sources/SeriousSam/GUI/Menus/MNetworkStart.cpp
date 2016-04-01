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
#include "LevelInfo.h"
#include "MenuStuff.h"
#include "MNetworkStart.h"

extern void UpdateNetworkLevel(INDEX iDummy);


void CNetworkStartMenu::Initialize_t(void)
{
  // title
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("START SERVER");
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

  // session name edit box
  gm_mgSessionName.mg_strText = _pGame->gam_strSessionName;
  gm_mgSessionName.mg_strLabel = TRANS("Session name:");
  gm_mgSessionName.mg_ctMaxStringLen = 25;
  gm_mgSessionName.mg_pstrToChange = &_pGame->gam_strSessionName;
  gm_mgSessionName.mg_boxOnScreen = BoxMediumRow(1);
  gm_mgSessionName.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgSessionName.mg_iCenterI = -1;
  gm_mgSessionName.mg_pmgUp = &gm_mgStart;
  gm_mgSessionName.mg_pmgDown = &gm_mgGameType;
  gm_mgSessionName.mg_strTip = TRANS("name the session to start");
  gm_lhGadgets.AddTail(gm_mgSessionName.mg_lnNode);

  // game type trigger
  TRIGGER_MG(gm_mgGameType, 2,
    gm_mgSessionName, gm_mgDifficulty, TRANS("Game type:"), astrGameTypeRadioTexts);
  gm_mgGameType.mg_ctTexts = ctGameTypeRadioTexts;
  gm_mgGameType.mg_strTip = TRANS("choose type of multiplayer game");
  gm_mgGameType.mg_pOnTriggerChange = &UpdateNetworkLevel;

  // difficulty trigger
  TRIGGER_MG(gm_mgDifficulty, 3,
    gm_mgGameType, gm_mgLevel, TRANS("Difficulty:"), astrDifficultyRadioTexts);
  gm_mgDifficulty.mg_strTip = TRANS("choose difficulty level");

  // level name
  gm_mgLevel.mg_strText = "";
  gm_mgLevel.mg_strLabel = TRANS("Level:");
  gm_mgLevel.mg_boxOnScreen = BoxMediumRow(4);
  gm_mgLevel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgLevel.mg_iCenterI = -1;
  gm_mgLevel.mg_pmgUp = &gm_mgDifficulty;
  gm_mgLevel.mg_pmgDown = &gm_mgMaxPlayers;
  gm_mgLevel.mg_strTip = TRANS("choose the level to start");
  gm_mgLevel.mg_pActivatedFunction = NULL;
  gm_lhGadgets.AddTail(gm_mgLevel.mg_lnNode);

  // max players trigger
  TRIGGER_MG(gm_mgMaxPlayers, 5,
    gm_mgLevel, gm_mgWaitAllPlayers, TRANS("Max players:"), astrMaxPlayersRadioTexts);
  gm_mgMaxPlayers.mg_strTip = TRANS("choose maximum allowed number of players");

  // wait all players trigger
  TRIGGER_MG(gm_mgWaitAllPlayers, 6,
    gm_mgMaxPlayers, gm_mgVisible, TRANS("Wait for all players:"), astrNoYes);
  gm_mgWaitAllPlayers.mg_strTip = TRANS("if on, game won't start until all players have joined");

  // server visible trigger
  TRIGGER_MG(gm_mgVisible, 7,
    gm_mgMaxPlayers, gm_mgGameOptions, TRANS("Server visible:"), astrNoYes);
  gm_mgVisible.mg_strTip = TRANS("invisible servers are not listed, cleints have to join manually");

  // options button
  gm_mgGameOptions.mg_strText = TRANS("Game options");
  gm_mgGameOptions.mg_boxOnScreen = BoxMediumRow(8);
  gm_mgGameOptions.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgGameOptions.mg_iCenterI = 0;
  gm_mgGameOptions.mg_pmgUp = &gm_mgVisible;
  gm_mgGameOptions.mg_pmgDown = &gm_mgStart;
  gm_mgGameOptions.mg_strTip = TRANS("adjust game rules");
  gm_mgGameOptions.mg_pActivatedFunction = NULL;
  gm_lhGadgets.AddTail(gm_mgGameOptions.mg_lnNode);

  // start button
  gm_mgStart.mg_bfsFontSize = BFS_LARGE;
  gm_mgStart.mg_boxOnScreen = BoxBigRow(7);
  gm_mgStart.mg_pmgUp = &gm_mgGameOptions;
  gm_mgStart.mg_pmgDown = &gm_mgSessionName;
  gm_mgStart.mg_strText = TRANS("START");
  gm_lhGadgets.AddTail(gm_mgStart.mg_lnNode);
  gm_mgStart.mg_pActivatedFunction = NULL;
}

void CNetworkStartMenu::StartMenu(void)
{
  extern INDEX sam_bMentalActivated;
  gm_mgDifficulty.mg_ctTexts = sam_bMentalActivated ? 6 : 5;

  gm_mgGameType.mg_iSelected = Clamp(_pShell->GetINDEX("gam_iStartMode"), 0L, ctGameTypeRadioTexts - 1L);
  gm_mgGameType.ApplyCurrentSelection();
  gm_mgDifficulty.mg_iSelected = _pShell->GetINDEX("gam_iStartDifficulty") + 1;
  gm_mgDifficulty.ApplyCurrentSelection();

  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);

  INDEX ctMaxPlayers = _pShell->GetINDEX("gam_ctMaxPlayers");
  if (ctMaxPlayers<2 || ctMaxPlayers>16) {
    ctMaxPlayers = 2;
    _pShell->SetINDEX("gam_ctMaxPlayers", ctMaxPlayers);
  }

  gm_mgMaxPlayers.mg_iSelected = ctMaxPlayers - 2;
  gm_mgMaxPlayers.ApplyCurrentSelection();

  gm_mgWaitAllPlayers.mg_iSelected = Clamp(_pShell->GetINDEX("gam_bWaitAllPlayers"), 0L, 1L);
  gm_mgWaitAllPlayers.ApplyCurrentSelection();

  gm_mgVisible.mg_iSelected = _pShell->GetINDEX("ser_bEnumeration");
  gm_mgVisible.ApplyCurrentSelection();

  UpdateNetworkLevel(0);

  CGameMenu::StartMenu();
}

void CNetworkStartMenu::EndMenu(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", gm_mgDifficulty.mg_iSelected - 1);
  _pShell->SetINDEX("gam_iStartMode", gm_mgGameType.mg_iSelected);
  _pShell->SetINDEX("gam_bWaitAllPlayers", gm_mgWaitAllPlayers.mg_iSelected);
  _pShell->SetINDEX("gam_ctMaxPlayers", gm_mgMaxPlayers.mg_iSelected + 2);
  _pShell->SetINDEX("ser_bEnumeration", gm_mgVisible.mg_iSelected);

  CGameMenu::EndMenu();
}
