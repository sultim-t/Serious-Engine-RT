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
#include "MInGame.h"


void CInGameMenu::Initialize_t(void)
{
  // intialize main menu
  gm_mgTitle.mg_strText = TRANS("GAME");
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

  gm_mgLabel1.mg_strText = "";
  gm_mgLabel1.mg_boxOnScreen = BoxMediumRow(-2.0);
  gm_mgLabel1.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgLabel1.mg_iCenterI = -1;
  gm_mgLabel1.mg_bEnabled = FALSE;
  gm_mgLabel1.mg_bLabel = TRUE;
  gm_lhGadgets.AddTail(gm_mgLabel1.mg_lnNode);

  gm_mgLabel2.mg_strText = "";
  gm_mgLabel2.mg_boxOnScreen = BoxMediumRow(-1.0);
  gm_mgLabel2.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgLabel2.mg_iCenterI = -1;
  gm_mgLabel2.mg_bEnabled = FALSE;
  gm_mgLabel2.mg_bLabel = TRUE;
  gm_lhGadgets.AddTail(gm_mgLabel2.mg_lnNode);

  gm_mgQuickLoad.mg_strText = TRANS("QUICK LOAD");
  gm_mgQuickLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuickLoad.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgQuickLoad.mg_strTip = TRANS("load a quick-saved game (F9)");
  gm_lhGadgets.AddTail(gm_mgQuickLoad.mg_lnNode);
  gm_mgQuickLoad.mg_pmgUp = &gm_mgQuit;
  gm_mgQuickLoad.mg_pmgDown = &gm_mgQuickSave;
  gm_mgQuickLoad.mg_pActivatedFunction = NULL;

  gm_mgQuickSave.mg_strText = TRANS("QUICK SAVE");
  gm_mgQuickSave.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuickSave.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgQuickSave.mg_strTip = TRANS("quick-save current game (F6)");
  gm_lhGadgets.AddTail(gm_mgQuickSave.mg_lnNode);
  gm_mgQuickSave.mg_pmgUp = &gm_mgQuickLoad;
  gm_mgQuickSave.mg_pmgDown = &gm_mgLoad;
  gm_mgQuickSave.mg_pActivatedFunction = NULL;

  gm_mgLoad.mg_strText = TRANS("LOAD");
  gm_mgLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgLoad.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgLoad.mg_strTip = TRANS("load a saved game");
  gm_lhGadgets.AddTail(gm_mgLoad.mg_lnNode);
  gm_mgLoad.mg_pmgUp = &gm_mgQuickSave;
  gm_mgLoad.mg_pmgDown = &gm_mgSave;
  gm_mgLoad.mg_pActivatedFunction = NULL;

  gm_mgSave.mg_strText = TRANS("SAVE");
  gm_mgSave.mg_bfsFontSize = BFS_LARGE;
  gm_mgSave.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgSave.mg_strTip = TRANS("save current game (each player has own slots!)");
  gm_lhGadgets.AddTail(gm_mgSave.mg_lnNode);
  gm_mgSave.mg_pmgUp = &gm_mgLoad;
  gm_mgSave.mg_pmgDown = &gm_mgDemoRec;
  gm_mgSave.mg_pActivatedFunction = NULL;

  gm_mgDemoRec.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgDemoRec.mg_bfsFontSize = BFS_LARGE;
  gm_mgDemoRec.mg_pmgUp = &gm_mgSave;
  gm_mgDemoRec.mg_pmgDown = &gm_mgHighScore;
  gm_mgDemoRec.mg_strText = "Text not set";
  gm_lhGadgets.AddTail(gm_mgDemoRec.mg_lnNode);
  gm_mgDemoRec.mg_pActivatedFunction = NULL;

  gm_mgHighScore.mg_strText = TRANS("HIGH SCORES");
  gm_mgHighScore.mg_bfsFontSize = BFS_LARGE;
  gm_mgHighScore.mg_boxOnScreen = BoxBigRow(5.0f);
  gm_mgHighScore.mg_strTip = TRANS("view list of top ten best scores");
  gm_lhGadgets.AddTail(gm_mgHighScore.mg_lnNode);
  gm_mgHighScore.mg_pmgUp = &gm_mgDemoRec;
  gm_mgHighScore.mg_pmgDown = &gm_mgOptions;
  gm_mgHighScore.mg_pActivatedFunction = NULL;

  gm_mgOptions.mg_strText = TRANS("OPTIONS");
  gm_mgOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgOptions.mg_boxOnScreen = BoxBigRow(6.0f);
  gm_mgOptions.mg_strTip = TRANS("adjust video, audio and input options");
  gm_lhGadgets.AddTail(gm_mgOptions.mg_lnNode);
  gm_mgOptions.mg_pmgUp = &gm_mgHighScore;
  gm_mgOptions.mg_pmgDown = &gm_mgStop;
  gm_mgOptions.mg_pActivatedFunction = NULL;

  gm_mgStop.mg_strText = TRANS("STOP GAME");
  gm_mgStop.mg_bfsFontSize = BFS_LARGE;
  gm_mgStop.mg_boxOnScreen = BoxBigRow(7.0f);
  gm_mgStop.mg_strTip = TRANS("stop currently running game");
  gm_lhGadgets.AddTail(gm_mgStop.mg_lnNode);
  gm_mgStop.mg_pmgUp = &gm_mgOptions;
  gm_mgStop.mg_pmgDown = &gm_mgQuit;
  gm_mgStop.mg_pActivatedFunction = NULL;

  gm_mgQuit.mg_strText = TRANS("QUIT");
  gm_mgQuit.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuit.mg_boxOnScreen = BoxBigRow(8.0f);
  gm_mgQuit.mg_strTip = TRANS("exit game immediately");
  gm_lhGadgets.AddTail(gm_mgQuit.mg_lnNode);
  gm_mgQuit.mg_pmgUp = &gm_mgStop;
  gm_mgQuit.mg_pmgDown = &gm_mgQuickLoad;
  gm_mgQuit.mg_pActivatedFunction = NULL;
}

void CInGameMenu::StartMenu(void)
{
  gm_mgQuickLoad.mg_bEnabled = _pNetwork->IsServer();
  gm_mgQuickSave.mg_bEnabled = _pNetwork->IsServer();
  gm_mgLoad.mg_bEnabled = _pNetwork->IsServer();
  gm_mgSave.mg_bEnabled = _pNetwork->IsServer();
  gm_mgDemoRec.mg_bEnabled = TRUE;//_pNetwork->IsServer();
  extern void SetDemoStartStopRecText();
  SetDemoStartStopRecText();


  if (_gmRunningGameMode == GM_SINGLE_PLAYER) {
    CPlayerCharacter &pc = _pGame->gm_apcPlayers[_pGame->gm_iSinglePlayer];
    gm_mgLabel1.mg_strText.PrintF(TRANS("Player: %s"), pc.GetNameForPrinting());
    gm_mgLabel2.mg_strText = "";

  } else {
    if (_pNetwork->IsServer()) {

      CTString strHost, strAddress;
      CTString strHostName;
      _pNetwork->GetHostName(strHost, strAddress);
      if (strHost == "") {
        strHostName = TRANS("<not started yet>");
      }
      else {
        strHostName = strHost + " (" + strAddress + ")";
      }

      gm_mgLabel1.mg_strText = TRANS("Address: ") + strHostName;
      gm_mgLabel2.mg_strText = "";

    } else {

      CTString strConfig;
      strConfig = TRANS("<not adjusted>");
      extern CTString sam_strNetworkSettings;
      if (sam_strNetworkSettings != "") {
        LoadStringVar(CTFileName(sam_strNetworkSettings).NoExt() + ".des", strConfig);
        strConfig.OnlyFirstLine();
      }

      gm_mgLabel1.mg_strText = TRANS("Connected to: ") + _pGame->gam_strJoinAddress;
      gm_mgLabel2.mg_strText = TRANS("Connection: ") + strConfig;
    }
  }

  CGameMenu::StartMenu();
}