/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

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
	if (_pShell->GetINDEX("gam_iQuickSaveSlots") <= 8) {
		_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = TRANS(
			"In-game QuickSave shortcuts:\n"
			"F6 - save a new QuickSave\n"
			"F9 - load the last QuickSave\n");
	} else {
		_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	}
}

void StartSinglePlayerNewMenu(void)
{
	_pGUIM->gmSinglePlayerNewMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
	extern CTString sam_strFirstLevel;
	_pGame->gam_strCustomLevel = sam_strFirstLevel;
	ChangeToMenu(&_pGUIM->gmSinglePlayerNewMenu);
}

#define CMENU _pGUIM->gmLoadSaveMenu

void StartSinglePlayerQuickLoadMenu(void)
{
	_gmMenuGameMode = GM_SINGLE_PLAYER;

	CMENU.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\Quick\\", _pGame->gm_iSinglePlayer);
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSLoadSinglePlayer;
	SetQuickLoadNotes();

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartSinglePlayerLoadMenu(void)
{
	_gmMenuGameMode = GM_SINGLE_PLAYER;

	CMENU.gm_mgTitle.mg_strText = TRANS("LOAD");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", _pGame->gm_iSinglePlayer);
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSLoadSinglePlayer;
	CMENU.gm_mgNotes.mg_strText = "";

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartSinglePlayerSaveMenu(void)
{
	if (_gmRunningGameMode != GM_SINGLE_PLAYER) return;
	// if no live players
	if (_pGame->GetPlayersCount()>0 && _pGame->GetLivePlayersCount() <= 0) {
		// do nothing
		return;
	}
	_gmMenuGameMode = GM_SINGLE_PLAYER;
	CMENU.gm_mgTitle.mg_strText = TRANS("SAVE");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = TRUE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", _pGame->gm_iSinglePlayer);
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmBaseName = CTString("SaveGame");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSSaveAnyGame;
	CMENU.gm_mgNotes.mg_strText = "";
	CMENU.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartDemoLoadMenu(void)
{
	_gmMenuGameMode = GM_DEMO;

	CMENU.gm_mgTitle.mg_strText = TRANS("PLAY DEMO");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory = CTString("Demos\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".dem");
	CMENU.gm_pAfterFileChosen = &LSLoadDemo;
	CMENU.gm_mgNotes.mg_strText = "";

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartDemoSaveMenu(void)
{
	if (_gmRunningGameMode == GM_NONE) return;
	_gmMenuGameMode = GM_DEMO;

	CMENU.gm_mgTitle.mg_strText = TRANS("RECORD DEMO");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEUP;
	CMENU.gm_bSave = TRUE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory = CTString("Demos\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmBaseName = CTString("Demo");
	CMENU.gm_fnmExt = CTString(".dem");
	CMENU.gm_pAfterFileChosen = &LSSaveDemo;
	CMENU.gm_mgNotes.mg_strText = "";
	CMENU.gm_strSaveDes = _pGame->GetDefaultGameDescription(FALSE);

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartNetworkQuickLoadMenu(void)
{
	_gmMenuGameMode = GM_NETWORK;

	CMENU.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory = CTString("SaveGame\\Network\\Quick\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSLoadNetwork;
	SetQuickLoadNotes();

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartNetworkLoadMenu(void)
{
	_gmMenuGameMode = GM_NETWORK;

	CMENU.gm_mgTitle.mg_strText = TRANS("LOAD");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory = CTString("SaveGame\\Network\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSLoadNetwork;
	CMENU.gm_mgNotes.mg_strText = "";

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartNetworkSaveMenu(void)
{
	if (_gmRunningGameMode != GM_NETWORK) return;
	_gmMenuGameMode = GM_NETWORK;

	CMENU.gm_mgTitle.mg_strText = TRANS("SAVE");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = TRUE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory = CTString("SaveGame\\Network\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmBaseName = CTString("SaveGame");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSSaveAnyGame;
	CMENU.gm_mgNotes.mg_strText = "";
	CMENU.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartSplitScreenQuickLoadMenu(void)
{
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	CMENU.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\Quick\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSLoadSplitScreen;
	SetQuickLoadNotes();

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartSplitScreenLoadMenu(void)
{
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	CMENU.gm_mgTitle.mg_strText = TRANS("LOAD");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSLoadSplitScreen;
	CMENU.gm_mgNotes.mg_strText = "";

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

void StartSplitScreenSaveMenu(void)
{
	if (_gmRunningGameMode != GM_SPLIT_SCREEN) return;
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	CMENU.gm_mgTitle.mg_strText = TRANS("SAVE");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEDN;
	CMENU.gm_bSave = TRUE;
	CMENU.gm_bManage = TRUE;
	CMENU.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmBaseName = CTString("SaveGame");
	CMENU.gm_fnmExt = CTString(".sav");
	CMENU.gm_pAfterFileChosen = &LSSaveAnyGame;
	CMENU.gm_mgNotes.mg_strText = "";
	CMENU.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	CMENU.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&CMENU);
}

#undef CMENU

// game options var settings
void StartVarGameOptions(void)
{
	_pGUIM->gmVarMenu.gm_mgTitle.mg_strText = TRANS("GAME OPTIONS");
	_pGUIM->gmVarMenu.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\GameOptions.cfg");
	ChangeToMenu(&_pGUIM->gmVarMenu);
}

void StartSinglePlayerGameOptions(void)
{
	_pGUIM->gmVarMenu.gm_mgTitle.mg_strText = TRANS("GAME OPTIONS");
	_pGUIM->gmVarMenu.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\SPOptions.cfg");
	ChangeToMenu(&_pGUIM->gmVarMenu);
	_pGUIM->gmVarMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
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
	_pGUIM->gmVarMenu.gm_mgTitle.mg_strText = TRANS("RENDERING OPTIONS");
	_pGUIM->gmVarMenu.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\RenderingOptions.cfg");
	_pGUIM->gmVarMenu.gm_pgmParentMenu = &_pGUIM->gmVideoOptionsMenu;
	ChangeToMenu(&_pGUIM->gmVarMenu);
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

void StartSelectLevelFromSingle(void)
{
	FilterLevels(GetSpawnFlagsForGameType(-1));
	_pAfterLevelChosen = StartSinglePlayerNewMenuCustom;
	ChangeToMenu(&_pGUIM->gmLevelsMenu);
	_pGUIM->gmLevelsMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
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

void StartSelectPlayersMenuFromSplit(void)
{
	_pGUIM->gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	_pGUIM->gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
	_pGUIM->gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
	_pGUIM->gmSelectPlayersMenu.gm_pgmParentMenu = &_pGUIM->gmSplitStartMenu;
	ChangeToMenu(&_pGUIM->gmSelectPlayersMenu);
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

void StartSelectPlayersMenuFromNetwork(void)
{
	_pGUIM->gmSelectPlayersMenu.gm_bAllowDedicated = TRUE;
	_pGUIM->gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
	_pGUIM->gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartNetworkGame;
	_pGUIM->gmSelectPlayersMenu.gm_pgmParentMenu = &_pGUIM->gmNetworkStartMenu;
	ChangeToMenu(&_pGUIM->gmSelectPlayersMenu);
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

void StartSelectPlayersMenuFromOpen(void)
{
	_pGUIM->gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	_pGUIM->gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
	_pGUIM->gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
	_pGUIM->gmSelectPlayersMenu.gm_pgmParentMenu = &_pGUIM->gmNetworkOpenMenu;
	ChangeToMenu(&_pGUIM->gmSelectPlayersMenu);

	/*if (sam_strNetworkSettings=="")*/ {
		void StartNetworkSettingsMenu(void);
		StartNetworkSettingsMenu();
		_pGUIM->gmLoadSaveMenu.gm_bNoEscape = TRUE;
		_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmNetworkOpenMenu;
		_pGUIM->gmLoadSaveMenu.gm_pgmNextMenu = &_pGUIM->gmSelectPlayersMenu;
	}
}
void StartSelectPlayersMenuFromServers(void)
{
	_pGUIM->gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	_pGUIM->gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
	_pGUIM->gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
	_pGUIM->gmSelectPlayersMenu.gm_pgmParentMenu = &_pGUIM->gmServersMenu;
	ChangeToMenu(&_pGUIM->gmSelectPlayersMenu);

	/*if (sam_strNetworkSettings=="")*/ {
		void StartNetworkSettingsMenu(void);
		StartNetworkSettingsMenu();
		_pGUIM->gmLoadSaveMenu.gm_bNoEscape = TRUE;
		_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmServersMenu;
		_pGUIM->gmLoadSaveMenu.gm_pgmNextMenu = &_pGUIM->gmSelectPlayersMenu;
	}
}

void StartSelectServerLAN(void)
{
	_pGUIM->gmServersMenu.m_bInternet = FALSE;
	ChangeToMenu(&_pGUIM->gmServersMenu);
	_pGUIM->gmServersMenu.gm_pgmParentMenu = &_pGUIM->gmNetworkJoinMenu;
}

void StartSelectServerNET(void)
{
	_pGUIM->gmServersMenu.m_bInternet = TRUE;
	ChangeToMenu(&_pGUIM->gmServersMenu);
	_pGUIM->gmServersMenu.gm_pgmParentMenu = &_pGUIM->gmNetworkJoinMenu;
}

void StartSelectLevelFromSplit(void)
{
	FilterLevels(GetSpawnFlagsForGameType(_pGUIM->gmSplitStartMenu.gm_mgGameType.mg_iSelected));
	void StartSplitStartMenu(void);
	_pAfterLevelChosen = StartSplitStartMenu;
	ChangeToMenu(&_pGUIM->gmLevelsMenu);
	_pGUIM->gmLevelsMenu.gm_pgmParentMenu = &_pGUIM->gmSplitStartMenu;
}

void StartSelectLevelFromNetwork(void)
{
	FilterLevels(GetSpawnFlagsForGameType(_pGUIM->gmNetworkStartMenu.gm_mgGameType.mg_iSelected));
	void StartNetworkStartMenu(void);
	_pAfterLevelChosen = StartNetworkStartMenu;
	ChangeToMenu(&_pGUIM->gmLevelsMenu);
	_pGUIM->gmLevelsMenu.gm_pgmParentMenu = &_pGUIM->gmNetworkStartMenu;
}

#define CMENU _pGUIM->gmSelectPlayersMenu

void StartSelectPlayersMenuFromSplitScreen(void)
{
	CMENU.gm_bAllowDedicated = FALSE;
	CMENU.gm_bAllowObserving = FALSE;
	//CMENU.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
	CMENU.gm_pgmParentMenu = &_pGUIM->gmSplitScreenMenu;
	ChangeToMenu(&CMENU);
}

void StartSelectPlayersMenuFromNetworkLoad(void)
{
	CMENU.gm_bAllowDedicated = FALSE;
	CMENU.gm_bAllowObserving = TRUE;
	CMENU.gm_mgStart.mg_pActivatedFunction = &StartNetworkLoadGame;
	CMENU.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
	ChangeToMenu(&CMENU);
}

void StartSelectPlayersMenuFromSplitScreenLoad(void)
{
	CMENU.gm_bAllowDedicated = FALSE;
	CMENU.gm_bAllowObserving = FALSE;
	CMENU.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGameLoad;
	CMENU.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
	ChangeToMenu(&CMENU);
}

#undef CMENU

// save/load menu calling functions
#define CMENU _pGUIM->gmLoadSaveMenu

void StartPlayerModelLoadMenu(void)
{
	CMENU.gm_mgTitle.mg_strText = TRANS("CHOOSE MODEL");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_FILEUP;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = FALSE;
	CMENU.gm_fnmDirectory = CTString("Models\\Player\\");
	CMENU.gm_fnmSelected = _strLastPlayerAppearance;
	CMENU.gm_fnmExt = CTString(".amc");
	CMENU.gm_pAfterFileChosen = &LSLoadPlayerModel;
	CMENU.gm_mgNotes.mg_strText = "";

	CMENU.gm_pgmParentMenu = &_pGUIM->gmPlayerProfile;
	ChangeToMenu(&CMENU);
}

void StartControlsLoadMenu(void)
{
	CMENU.gm_mgTitle.mg_strText = TRANS("LOAD CONTROLS");
	CMENU.gm_bAllowThumbnails = FALSE;
	CMENU.gm_iSortType = LSSORT_FILEUP;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = FALSE;
	CMENU.gm_fnmDirectory = CTString("Controls\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".ctl");
	CMENU.gm_pAfterFileChosen = &LSLoadControls;
	CMENU.gm_mgNotes.mg_strText = "";

	CMENU.gm_pgmParentMenu = &_pGUIM->gmControls;
	ChangeToMenu(&CMENU);
}

void StartCustomLoadMenu(void)
{
	CMENU.gm_mgTitle.mg_strText = TRANS("ADVANCED OPTIONS");
	CMENU.gm_bAllowThumbnails = FALSE;
	CMENU.gm_iSortType = LSSORT_NAMEUP;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = FALSE;
	CMENU.gm_fnmDirectory = CTString("Scripts\\CustomOptions\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".cfg");
	CMENU.gm_pAfterFileChosen = &LSLoadCustom;
	CMENU.gm_mgNotes.mg_strText = "";

	CMENU.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
	ChangeToMenu(&CMENU);
}

void StartAddonsLoadMenu(void)
{
	CMENU.gm_mgTitle.mg_strText = TRANS("EXECUTE ADDON");
	CMENU.gm_bAllowThumbnails = FALSE;
	CMENU.gm_iSortType = LSSORT_NAMEUP;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = FALSE;
	CMENU.gm_fnmDirectory = CTString("Scripts\\Addons\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".ini");
	CMENU.gm_pAfterFileChosen = &LSLoadAddon;
	CMENU.gm_mgNotes.mg_strText = "";

	CMENU.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
	ChangeToMenu(&CMENU);
}

void StartModsLoadMenu(void)
{
	CMENU.gm_mgTitle.mg_strText = TRANS("CHOOSE MOD");
	CMENU.gm_bAllowThumbnails = TRUE;
	CMENU.gm_iSortType = LSSORT_NAMEUP;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = FALSE;
	CMENU.gm_fnmDirectory = CTString("Mods\\");
	CMENU.gm_fnmSelected = CTString("");
	CMENU.gm_fnmExt = CTString(".des");
	CMENU.gm_pAfterFileChosen = &LSLoadMod;

	CMENU.gm_pgmParentMenu = &_pGUIM->gmMainMenu;
	ChangeToMenu(&CMENU);
}

void StartNetworkSettingsMenu(void)
{
	CMENU.gm_mgTitle.mg_strText = TRANS("CONNECTION SETTINGS");
	CMENU.gm_bAllowThumbnails = FALSE;
	CMENU.gm_iSortType = LSSORT_FILEUP;
	CMENU.gm_bSave = FALSE;
	CMENU.gm_bManage = FALSE;
	CMENU.gm_fnmDirectory = CTString("Scripts\\NetSettings\\");
	CMENU.gm_fnmSelected = sam_strNetworkSettings;
	CMENU.gm_fnmExt = CTString(".ini");
	CMENU.gm_pAfterFileChosen = &LSLoadNetSettings;
	if (sam_strNetworkSettings == "") {
		CMENU.gm_mgNotes.mg_strText = TRANS(
			"Before joining a network game,\n"
			"you have to adjust your connection parameters.\n"
			"Choose one option from the list.\n"
			"If you have problems with connection, you can adjust\n"
			"these parameters again from the Options menu.\n"
			);
	} else {
		CMENU.gm_mgNotes.mg_strText = "";
	}

	CMENU.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
	ChangeToMenu(&CMENU);
}

#undef CMENU