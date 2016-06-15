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

/* This file contains starter fuctions for all menus. */

#include "StdH.h"
#include <Engine/Build.h>

#include "MenuManager.h"
#include "MenuStarters.h"
#include "MenuStartersAF.h"
#include "MenuStuff.h"
#include "LevelInfo.h"

extern void(*_pAfterLevelChosen)(void);
extern BOOL _bPlayerMenuFromSinglePlayer;

extern CTString _strLastPlayerAppearance;
extern CTString sam_strNetworkSettings;

extern CTFileName _fnmModSelected;
extern CTString _strModURLSelected;
extern CTString _strModServerSelected;


void StartVideoOptionsMenu(void)
{
  ChangeToMenu(&_pGUIM->gmVideoOptionsMenu);
}

void StartAudioOptionsMenu(void)
{
  ChangeToMenu(&_pGUIM->gmAudioOptionsMenu);
}

void StartSinglePlayerMenu(void)
{
  ChangeToMenu(&_pGUIM->gmSinglePlayerMenu);
}

void StartNetworkMenu(void)
{
  ChangeToMenu(&_pGUIM->gmNetworkMenu);
}

void StartNetworkJoinMenu(void)
{
  ChangeToMenu(&_pGUIM->gmNetworkJoinMenu);
}

void StartNetworkStartMenu(void)
{
  ChangeToMenu(&_pGUIM->gmNetworkStartMenu);
}

void StartNetworkOpenMenu(void)
{
  ChangeToMenu(&_pGUIM->gmNetworkOpenMenu);
}

void StartSplitScreenMenu(void)
{
  ChangeToMenu(&_pGUIM->gmSplitScreenMenu);
}

void StartSplitStartMenu(void)
{
  ChangeToMenu(&_pGUIM->gmSplitStartMenu);
}

void StartSinglePlayerNewMenuCustom(void)
{
  _pGUIM->gmSinglePlayerNewMenu.gm_pgmParentMenu = &_pGUIM->gmLevelsMenu;
  ChangeToMenu(&_pGUIM->gmSinglePlayerNewMenu);
}

static void SetQuickLoadNotes(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  if (_pShell->GetINDEX("gam_iQuickSaveSlots") <= 8) {
    gmCurrent.gm_mgNotes.mg_strText = TRANS(
      "In-game QuickSave shortcuts:\n"
      "F6 - save a new QuickSave\n"
      "F9 - load the last QuickSave\n");
  } else {
    gmCurrent.gm_mgNotes.mg_strText = "";
  }
}

extern CTString sam_strFirstLevel;

void StartSinglePlayerNewMenu(void)
{
  CSinglePlayerNewMenu &gmCurrent = _pGUIM->gmSinglePlayerNewMenu;

  _pGame->gam_strCustomLevel = sam_strFirstLevel;

  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
  ChangeToMenu(&gmCurrent);
}

// game options var settings
void StartVarGameOptions(void)
{
  CVarMenu &gmCurrent = _pGUIM->gmVarMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("GAME OPTIONS");
  gmCurrent.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\GameOptions.cfg");
  ChangeToMenu(&gmCurrent);
}

void StartSinglePlayerGameOptions(void)
{
  CVarMenu &gmCurrent = _pGUIM->gmVarMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("GAME OPTIONS");
  gmCurrent.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\SPOptions.cfg");
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
  ChangeToMenu(&gmCurrent);
}

void StartGameOptionsFromNetwork(void)
{
  StartVarGameOptions();
  _pGUIM->gmVarMenu.gm_pgmParentMenu = &_pGUIM->gmNetworkStartMenu;
}

void StartGameOptionsFromSplitScreen(void)
{
  StartVarGameOptions();
  _pGUIM->gmVarMenu.gm_pgmParentMenu = &_pGUIM->gmSplitStartMenu;
}

// rendering options var settings
void StartRenderingOptionsMenu(void)
{
  CVarMenu &gmCurrent = _pGUIM->gmVarMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("RENDERING OPTIONS");
  gmCurrent.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\RenderingOptions.cfg");
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmVideoOptionsMenu;
  ChangeToMenu(&gmCurrent);
}

void StartCustomizeKeyboardMenu(void)
{
  ChangeToMenu(&_pGUIM->gmCustomizeKeyboardMenu);
}

void StartCustomizeAxisMenu(void)
{
  ChangeToMenu(&_pGUIM->gmCustomizeAxisMenu);
}

void StartOptionsMenu(void)
{
  _pGUIM->gmOptionsMenu.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&_pGUIM->gmOptionsMenu);
}

void StartCurrentLoadMenu()
{
  if (_gmRunningGameMode == GM_NETWORK) {
    void StartNetworkLoadMenu(void);
    StartNetworkLoadMenu();
  } else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
    void StartSplitScreenLoadMenu(void);
    StartSplitScreenLoadMenu();
  } else {
    void StartSinglePlayerLoadMenu(void);
    StartSinglePlayerLoadMenu();
  }
}

void StartCurrentSaveMenu()
{
  if (_gmRunningGameMode == GM_NETWORK) {
    void StartNetworkSaveMenu(void);
    StartNetworkSaveMenu();
  } else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
    void StartSplitScreenSaveMenu(void);
    StartSplitScreenSaveMenu();
  } else {
    void StartSinglePlayerSaveMenu(void);
    StartSinglePlayerSaveMenu();
  }
}

void StartCurrentQuickLoadMenu()
{
  if (_gmRunningGameMode == GM_NETWORK) {
    void StartNetworkQuickLoadMenu(void);
    StartNetworkQuickLoadMenu();
  } else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
    void StartSplitScreenQuickLoadMenu(void);
    StartSplitScreenQuickLoadMenu();
  } else {
    void StartSinglePlayerQuickLoadMenu(void);
    StartSinglePlayerQuickLoadMenu();
  }
}

void StartChangePlayerMenuFromOptions(void)
{
  _bPlayerMenuFromSinglePlayer = FALSE;
  _pGUIM->gmPlayerProfile.gm_piCurrentPlayer = &_pGame->gm_iSinglePlayer;
  _pGUIM->gmPlayerProfile.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
  ChangeToMenu(&_pGUIM->gmPlayerProfile);
}

void StartChangePlayerMenuFromSinglePlayer(void)
{
  _iLocalPlayer = -1;
  _bPlayerMenuFromSinglePlayer = TRUE;
  _pGUIM->gmPlayerProfile.gm_piCurrentPlayer = &_pGame->gm_iSinglePlayer;
  _pGUIM->gmPlayerProfile.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
  ChangeToMenu(&_pGUIM->gmPlayerProfile);
}

void StartControlsMenuFromPlayer(void)
{
  _pGUIM->gmControls.gm_pgmParentMenu = &_pGUIM->gmPlayerProfile;
  ChangeToMenu(&_pGUIM->gmControls);
}

void StartControlsMenuFromOptions(void)
{
  _pGUIM->gmControls.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
  ChangeToMenu(&_pGUIM->gmControls);
}

void StartHighScoreMenu(void)
{
  _pGUIM->gmHighScoreMenu.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&_pGUIM->gmHighScoreMenu);
}

void StartSplitScreenGame(void)
{
  //  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) mgSplitScreenCfg.mg_iSelected;
  _pGame->gm_StartSplitScreenCfg = _pGame->gm_MenuSplitScreenCfg;

  _pGame->gm_aiStartLocalPlayers[0] = _pGame->gm_aiMenuLocalPlayers[0];
  _pGame->gm_aiStartLocalPlayers[1] = _pGame->gm_aiMenuLocalPlayers[1];
  _pGame->gm_aiStartLocalPlayers[2] = _pGame->gm_aiMenuLocalPlayers[2];
  _pGame->gm_aiStartLocalPlayers[3] = _pGame->gm_aiMenuLocalPlayers[3];

  CTFileName fnWorld = _pGame->gam_strCustomLevel;

  _pGame->gm_strNetworkProvider = "Local";
  CUniversalSessionProperties sp;
  _pGame->SetMultiPlayerSession(sp);
  if (_pGame->NewGame(fnWorld.FileName(), fnWorld, sp))
  {
    StopMenus();
    _gmRunningGameMode = GM_SPLIT_SCREEN;
  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

void StartNetworkGame(void)
{
  //  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) mgSplitScreenCfg.mg_iSelected;
  _pGame->gm_StartSplitScreenCfg = _pGame->gm_MenuSplitScreenCfg;

  _pGame->gm_aiStartLocalPlayers[0] = _pGame->gm_aiMenuLocalPlayers[0];
  _pGame->gm_aiStartLocalPlayers[1] = _pGame->gm_aiMenuLocalPlayers[1];
  _pGame->gm_aiStartLocalPlayers[2] = _pGame->gm_aiMenuLocalPlayers[2];
  _pGame->gm_aiStartLocalPlayers[3] = _pGame->gm_aiMenuLocalPlayers[3];

  CTFileName fnWorld = _pGame->gam_strCustomLevel;

  _pGame->gm_strNetworkProvider = "TCP/IP Server";
  CUniversalSessionProperties sp;
  _pGame->SetMultiPlayerSession(sp);
  if (_pGame->NewGame(_pGame->gam_strSessionName, fnWorld, sp))
  {
    StopMenus();
    _gmRunningGameMode = GM_NETWORK;
    // if starting a dedicated server
    if (_pGame->gm_MenuSplitScreenCfg == CGame::SSC_DEDICATED) {
      // pull down the console
      extern INDEX sam_bToggleConsole;
      sam_bToggleConsole = TRUE;
    }
  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

void JoinNetworkGame(void)
{
  //  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) mgSplitScreenCfg.mg_iSelected;
  _pGame->gm_StartSplitScreenCfg = _pGame->gm_MenuSplitScreenCfg;

  _pGame->gm_aiStartLocalPlayers[0] = _pGame->gm_aiMenuLocalPlayers[0];
  _pGame->gm_aiStartLocalPlayers[1] = _pGame->gm_aiMenuLocalPlayers[1];
  _pGame->gm_aiStartLocalPlayers[2] = _pGame->gm_aiMenuLocalPlayers[2];
  _pGame->gm_aiStartLocalPlayers[3] = _pGame->gm_aiMenuLocalPlayers[3];

  _pGame->gm_strNetworkProvider = "TCP/IP Client";
  if (_pGame->JoinGame(CNetworkSession(_pGame->gam_strJoinAddress)))
  {
    StopMenus();
    _gmRunningGameMode = GM_NETWORK;
  } else {
    if (_pNetwork->ga_strRequiredMod != "") {
      extern CTFileName _fnmModToLoad;
      extern CTString _strModServerJoin;
      char strModName[256] = { 0 };
      char strModURL[256] = { 0 };
      _pNetwork->ga_strRequiredMod.ScanF("%250[^\\]\\%s", &strModName, &strModURL);
      _fnmModSelected = CTString(strModName);
      _strModURLSelected = strModURL;
      if (_strModURLSelected = "") {
        _strModURLSelected = "http://www.croteam.com/mods/Old";
      }
      _strModServerSelected.PrintF("%s:%s", _pGame->gam_strJoinAddress, _pShell->GetValue("net_iPort"));
      extern void ModConnectConfirm(void);
      ModConnectConfirm();
    }
    _gmRunningGameMode = GM_NONE;
  }
}

// -------- Servers Menu Functions
void StartSelectServerLAN(void)
{
  CServersMenu &gmCurrent = _pGUIM->gmServersMenu;

  gmCurrent.m_bInternet = FALSE;
  ChangeToMenu(&gmCurrent);
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmNetworkJoinMenu;
}

void StartSelectServerNET(void)
{
  CServersMenu &gmCurrent = _pGUIM->gmServersMenu;

  gmCurrent.m_bInternet = TRUE;
  ChangeToMenu(&gmCurrent);
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmNetworkJoinMenu;
}

// -------- Levels Menu Functions
void StartSelectLevelFromSingle(void)
{
  CLevelsMenu &gmCurrent = _pGUIM->gmLevelsMenu;

  FilterLevels(GetSpawnFlagsForGameType(-1));
  _pAfterLevelChosen = StartSinglePlayerNewMenuCustom;
  ChangeToMenu(&gmCurrent);
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
}

void StartSelectLevelFromSplit(void)
{
  CLevelsMenu &gmCurrent = _pGUIM->gmLevelsMenu;

  FilterLevels(GetSpawnFlagsForGameType(_pGUIM->gmSplitStartMenu.gm_mgGameType.mg_iSelected));
  void StartSplitStartMenu(void);
  _pAfterLevelChosen = StartSplitStartMenu;
  ChangeToMenu(&gmCurrent);
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmSplitStartMenu;
}

void StartSelectLevelFromNetwork(void)
{
  CLevelsMenu &gmCurrent = _pGUIM->gmLevelsMenu;

  FilterLevels(GetSpawnFlagsForGameType(_pGUIM->gmNetworkStartMenu.gm_mgGameType.mg_iSelected));
  void StartNetworkStartMenu(void);
  _pAfterLevelChosen = StartNetworkStartMenu;
  ChangeToMenu(&gmCurrent);
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmNetworkStartMenu;
}

// -------- Players Selection Menu Functions
void StartSelectPlayersMenuFromSplit(void)
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_bAllowDedicated = FALSE;
  gmCurrent.gm_bAllowObserving = FALSE;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmSplitStartMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSelectPlayersMenuFromNetwork(void)
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_bAllowDedicated = TRUE;
  gmCurrent.gm_bAllowObserving = TRUE;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartNetworkGame;
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmNetworkStartMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSelectPlayersMenuFromNetworkLoad(void)
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_bAllowDedicated = FALSE;
  gmCurrent.gm_bAllowObserving = TRUE;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartNetworkLoadGame;
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSelectPlayersMenuFromSplitScreenLoad(void)
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_bAllowDedicated = FALSE;
  gmCurrent.gm_bAllowObserving = FALSE;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGameLoad;
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSelectPlayersMenuFromOpen(void)
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_bAllowDedicated = FALSE;
  gmCurrent.gm_bAllowObserving = TRUE;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmNetworkOpenMenu;
  ChangeToMenu(&gmCurrent);

  /*if (sam_strNetworkSettings=="")*/ {
    void StartNetworkSettingsMenu(void);
    StartNetworkSettingsMenu();
    _pGUIM->gmLoadSaveMenu.gm_bNoEscape = TRUE;
    _pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmNetworkOpenMenu;
    _pGUIM->gmLoadSaveMenu.gm_pgmNextMenu = &gmCurrent;
  }
}

void StartSelectPlayersMenuFromServers(void)
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_bAllowDedicated = FALSE;
  gmCurrent.gm_bAllowObserving = TRUE;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmServersMenu;
  ChangeToMenu(&gmCurrent);

  /*if (sam_strNetworkSettings=="")*/ {
    void StartNetworkSettingsMenu(void);
    StartNetworkSettingsMenu();
    _pGUIM->gmLoadSaveMenu.gm_bNoEscape = TRUE;
    _pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmServersMenu;
    _pGUIM->gmLoadSaveMenu.gm_pgmNextMenu = &gmCurrent;
  }
}

// -------- Save/Load Menu Calling Functions
void StartPlayerModelLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("CHOOSE MODEL");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Models\\Player\\");
  gmCurrent.gm_fnmSelected = _strLastPlayerAppearance;
  gmCurrent.gm_fnmExt = CTString(".amc");
  gmCurrent.gm_pAfterFileChosen = &LSLoadPlayerModel;
  gmCurrent.gm_mgNotes.mg_strText = "";

  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmPlayerProfile;
  ChangeToMenu(&gmCurrent);
}

void StartControlsLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("LOAD CONTROLS");
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Controls\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".ctl");
  gmCurrent.gm_pAfterFileChosen = &LSLoadControls;
  gmCurrent.gm_mgNotes.mg_strText = "";

  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmControls;
  ChangeToMenu(&gmCurrent);
}

void StartCustomLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("ADVANCED OPTIONS");
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_NAMEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Scripts\\CustomOptions\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".cfg");
  gmCurrent.gm_pAfterFileChosen = &LSLoadCustom;
  gmCurrent.gm_mgNotes.mg_strText = "";

  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
  ChangeToMenu(&gmCurrent);
}

void StartAddonsLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("EXECUTE ADDON");
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_NAMEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Scripts\\Addons\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".ini");
  gmCurrent.gm_pAfterFileChosen = &LSLoadAddon;
  gmCurrent.gm_mgNotes.mg_strText = "";

  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
  ChangeToMenu(&gmCurrent);
}

void StartModsLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("CHOOSE MOD");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_NAMEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Mods\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".des");
  gmCurrent.gm_pAfterFileChosen = &LSLoadMod;

  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmMainMenu;
  ChangeToMenu(&gmCurrent);
}

void StartNetworkSettingsMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("CONNECTION SETTINGS");
  gmCurrent.gm_bAllowThumbnails = FALSE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = FALSE;
  gmCurrent.gm_fnmDirectory = CTString("Scripts\\NetSettings\\");
  gmCurrent.gm_fnmSelected = sam_strNetworkSettings;
  gmCurrent.gm_fnmExt = CTString(".ini");
  gmCurrent.gm_pAfterFileChosen = &LSLoadNetSettings;

  if (sam_strNetworkSettings == "") {
    gmCurrent.gm_mgNotes.mg_strText = TRANS(
      "Before joining a network game,\n"
      "you have to adjust your connection parameters.\n"
      "Choose one option from the list.\n"
      "If you have problems with connection, you can adjust\n"
      "these parameters again from the Options menu.\n"
      );
  } else {
    gmCurrent.gm_mgNotes.mg_strText = "";
  }

  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
  ChangeToMenu(&gmCurrent);
}


void StartSinglePlayerQuickLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SINGLE_PLAYER;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\Quick\\", _pGame->gm_iSinglePlayer);
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadSinglePlayer;
  SetQuickLoadNotes();

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSinglePlayerLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SINGLE_PLAYER;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("LOAD");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", _pGame->gm_iSinglePlayer);
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadSinglePlayer;
  gmCurrent.gm_mgNotes.mg_strText = "";

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSinglePlayerSaveMenu(void)
{
  if (_gmRunningGameMode != GM_SINGLE_PLAYER) return;

  // if no live players
  if (_pGame->GetPlayersCount()>0 && _pGame->GetLivePlayersCount() <= 0) {
    // do nothing
    return;
  }

  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SINGLE_PLAYER;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("SAVE");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = TRUE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", _pGame->gm_iSinglePlayer);
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmBaseName = CTString("SaveGame");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSSaveAnyGame;
  gmCurrent.gm_mgNotes.mg_strText = "";
  gmCurrent.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartDemoLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_DEMO;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("PLAY DEMO");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("Demos\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".dem");
  gmCurrent.gm_pAfterFileChosen = &LSLoadDemo;
  gmCurrent.gm_mgNotes.mg_strText = "";

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartDemoSaveMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  if (_gmRunningGameMode == GM_NONE) return;
  _gmMenuGameMode = GM_DEMO;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("RECORD DEMO");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEUP;
  gmCurrent.gm_bSave = TRUE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("Demos\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmBaseName = CTString("Demo");
  gmCurrent.gm_fnmExt = CTString(".dem");
  gmCurrent.gm_pAfterFileChosen = &LSSaveDemo;
  gmCurrent.gm_mgNotes.mg_strText = "";
  gmCurrent.gm_strSaveDes = _pGame->GetDefaultGameDescription(FALSE);

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartNetworkQuickLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_NETWORK;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\Network\\Quick\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadNetwork;
  SetQuickLoadNotes();

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartNetworkLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_NETWORK;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("LOAD");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\Network\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadNetwork;
  gmCurrent.gm_mgNotes.mg_strText = "";

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartNetworkSaveMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  if (_gmRunningGameMode != GM_NETWORK) return;
  _gmMenuGameMode = GM_NETWORK;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("SAVE");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = TRUE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\Network\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmBaseName = CTString("SaveGame");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSSaveAnyGame;
  gmCurrent.gm_mgNotes.mg_strText = "";
  gmCurrent.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSplitScreenQuickLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SPLIT_SCREEN;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\Quick\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadSplitScreen;
  SetQuickLoadNotes();

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSplitScreenLoadMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  _gmMenuGameMode = GM_SPLIT_SCREEN;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("LOAD");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = FALSE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSLoadSplitScreen;
  gmCurrent.gm_mgNotes.mg_strText = "";

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

void StartSplitScreenSaveMenu(void)
{
  CLoadSaveMenu &gmCurrent = _pGUIM->gmLoadSaveMenu;

  if (_gmRunningGameMode != GM_SPLIT_SCREEN) return;
  _gmMenuGameMode = GM_SPLIT_SCREEN;

  gmCurrent.gm_mgTitle.mg_strText = TRANS("SAVE");
  gmCurrent.gm_bAllowThumbnails = TRUE;
  gmCurrent.gm_iSortType = LSSORT_FILEDN;
  gmCurrent.gm_bSave = TRUE;
  gmCurrent.gm_bManage = TRUE;
  gmCurrent.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
  gmCurrent.gm_fnmSelected = CTString("");
  gmCurrent.gm_fnmBaseName = CTString("SaveGame");
  gmCurrent.gm_fnmExt = CTString(".sav");
  gmCurrent.gm_pAfterFileChosen = &LSSaveAnyGame;
  gmCurrent.gm_mgNotes.mg_strText = "";
  gmCurrent.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu(&gmCurrent);
}

// -------- Disabled Menu Calling Function
void DisabledFunction(void)
{
  CDisabledMenu &gmCurrent = _pGUIM->gmDisabledFunction;

  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  gmCurrent.gm_mgButton.mg_strText = TRANS("The feature is not available in this version!");
  gmCurrent.gm_mgTitle.mg_strText = TRANS("DISABLED");
  ChangeToMenu(&gmCurrent);
}