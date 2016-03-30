/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Build.h>

#include "MenuManager.h"
#include "Menu_starters.h"
#include "MenuStuff.h"
#include "LevelInfo.h"

extern void(*_pAfterLevelChosen)(void);
extern BOOL _bPlayerMenuFromSinglePlayer;

extern CTString _strLastPlayerAppearance;
extern CTString sam_strNetworkSettings;

extern CTFileName _fnmModSelected;
extern CTString _strModURLSelected;
extern CTString _strModServerSelected;

CTFileName _fnDemoToPlay;
CTFileName _fnGameToLoad;

void StartVideoOptionsMenu(void)
{
	ChangeToMenu(&_pGUIM->gmVideoOptionsMenu);
}

void StartAudioOptionsMenu(void)
{
	ChangeToMenu(&_pGUIM->gmAudioOptionsMenu);
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

BOOL LSLoadSinglePlayer(const CTFileName &fnm)
{
	_pGame->gm_StartSplitScreenCfg = CGame::SSC_PLAY1;

	_pGame->gm_aiStartLocalPlayers[0] = _pGame->gm_iSinglePlayer;
	_pGame->gm_aiStartLocalPlayers[1] = -1;
	_pGame->gm_aiStartLocalPlayers[2] = -1;
	_pGame->gm_aiStartLocalPlayers[3] = -1;
	_pGame->gm_strNetworkProvider = "Local";
	if (_pGame->LoadGame(fnm)) {
		StopMenus();
		_gmRunningGameMode = GM_SINGLE_PLAYER;
	} else {
		_gmRunningGameMode = GM_NONE;
	}
	return TRUE;
}

BOOL LSLoadNetwork(const CTFileName &fnm)
{
	// call local players menu
	_fnGameToLoad = fnm;
	StartSelectPlayersMenuFromNetworkLoad();
	return TRUE;
}

BOOL LSLoadSplitScreen(const CTFileName &fnm)
{
	// call local players menu
	_fnGameToLoad = fnm;
	StartSelectPlayersMenuFromSplitScreenLoad();
	return TRUE;
}

void StartDemoPlay(void)
{
	_pGame->gm_StartSplitScreenCfg = CGame::SSC_OBSERVER;
	// play the demo
	_pGame->gm_strNetworkProvider = "Local";
	if (_pGame->StartDemoPlay(_fnDemoToPlay))
	{
		// exit menu and pull up the console
		StopMenus();
		if (_pGame->gm_csConsoleState != CS_OFF) _pGame->gm_csConsoleState = CS_TURNINGOFF;
		_gmRunningGameMode = GM_DEMO;
	} else {
		_gmRunningGameMode = GM_NONE;
	}
}

extern BOOL LSLoadDemo(const CTFileName &fnm)
{
	// call local players menu
	_fnDemoToPlay = fnm;
	StartDemoPlay();
	return TRUE;
}

BOOL LSLoadPlayerModel(const CTFileName &fnm)
{
	// get base filename
	CTString strBaseName = fnm.FileName();
	// set it for current player
	CPlayerCharacter &pc = _pGame->gm_apcPlayers[*_pGUIM->gmPlayerProfile.gm_piCurrentPlayer];
	CPlayerSettings *pps = (CPlayerSettings *)pc.pc_aubAppearance;
	memset(pps->ps_achModelFile, 0, sizeof(pps->ps_achModelFile));
	strncpy(pps->ps_achModelFile, strBaseName, sizeof(pps->ps_achModelFile));

	void MenuGoToParent(void);
	MenuGoToParent();
	return TRUE;
}

extern void ControlsMenuOn();
extern void ControlsMenuOff();

BOOL LSLoadControls(const CTFileName &fnm)
{
	try {
		ControlsMenuOn();
		_pGame->gm_ctrlControlsExtra.Load_t(fnm);
		ControlsMenuOff();
	} catch (char *strError) {
		CPrintF("%s", strError);
	}

	void MenuGoToParent(void);
	MenuGoToParent();
	return TRUE;
}

BOOL LSLoadAddon(const CTFileName &fnm)
{
	extern INDEX _iAddonExecState;
	extern CTFileName _fnmAddonToExec;
	_iAddonExecState = 1;
	_fnmAddonToExec = fnm;
	return TRUE;
}

BOOL LSLoadMod(const CTFileName &fnm)
{
	_fnmModSelected = fnm;
	extern void ModConfirm(void);
	ModConfirm();
	return TRUE;
}

BOOL LSLoadCustom(const CTFileName &fnm)
{
	_pGUIM->gmVarMenu.gm_mgTitle.mg_strText = TRANS("ADVANCED OPTIONS");
	//  LoadStringVar(fnm.NoExt()+".des", mgVarTitle.mg_strText);
	//  mgVarTitle.mg_strText.OnlyFirstLine();
	_pGUIM->gmVarMenu.gm_fnmMenuCFG = fnm;
	_pGUIM->gmVarMenu.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
	ChangeToMenu(&_pGUIM->gmVarMenu);
	return TRUE;
}

BOOL LSLoadNetSettings(const CTFileName &fnm)
{
	sam_strNetworkSettings = fnm;
	CTString strCmd;
	strCmd.PrintF("include \"%s\"", (const char*)sam_strNetworkSettings);
	_pShell->Execute(strCmd);

	void MenuGoToParent(void);
	MenuGoToParent();
	return TRUE;
}

// same function for saving in singleplay, network and splitscreen
BOOL LSSaveAnyGame(const CTFileName &fnm)
{
	if (_pGame->SaveGame(fnm)) {
		StopMenus();
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL LSSaveDemo(const CTFileName &fnm)
{
	// save the demo
	if (_pGame->StartDemoRec(fnm)) {
		StopMenus();
		return TRUE;
	} else {
		return FALSE;
	}
}

void StartSinglePlayerQuickLoadMenu(void)
{
	_gmMenuGameMode = GM_SINGLE_PLAYER;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\Quick\\", _pGame->gm_iSinglePlayer);
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadSinglePlayer;
	SetQuickLoadNotes();

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartSinglePlayerLoadMenu(void)
{
	_gmMenuGameMode = GM_SINGLE_PLAYER;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("LOAD");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", _pGame->gm_iSinglePlayer);
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadSinglePlayer;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
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
	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("SAVE");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", _pGame->gm_iSinglePlayer);
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmBaseName = CTString("SaveGame");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSSaveAnyGame;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	_pGUIM->gmLoadSaveMenu.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartDemoLoadMenu(void)
{
	_gmMenuGameMode = GM_DEMO;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("PLAY DEMO");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("Demos\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".dem");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadDemo;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartDemoSaveMenu(void)
{
	if (_gmRunningGameMode == GM_NONE) return;
	_gmMenuGameMode = GM_DEMO;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("RECORD DEMO");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEUP;
	_pGUIM->gmLoadSaveMenu.gm_bSave = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("Demos\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmBaseName = CTString("Demo");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".dem");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSSaveDemo;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	_pGUIM->gmLoadSaveMenu.gm_strSaveDes = _pGame->GetDefaultGameDescription(FALSE);

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartNetworkQuickLoadMenu(void)
{
	_gmMenuGameMode = GM_NETWORK;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\Network\\Quick\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadNetwork;
	SetQuickLoadNotes();

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartNetworkLoadMenu(void)
{
	_gmMenuGameMode = GM_NETWORK;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("LOAD");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\Network\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadNetwork;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartNetworkSaveMenu(void)
{
	if (_gmRunningGameMode != GM_NETWORK) return;
	_gmMenuGameMode = GM_NETWORK;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("SAVE");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\Network\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmBaseName = CTString("SaveGame");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSSaveAnyGame;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	_pGUIM->gmLoadSaveMenu.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartSplitScreenQuickLoadMenu(void)
{
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\Quick\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadSplitScreen;
	SetQuickLoadNotes();

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartSplitScreenLoadMenu(void)
{
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("LOAD");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadSplitScreen;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartSplitScreenSaveMenu(void)
{
	if (_gmRunningGameMode != GM_SPLIT_SCREEN) return;
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("SAVE");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	_pGUIM->gmLoadSaveMenu.gm_bSave = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmBaseName = CTString("SaveGame");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSSaveAnyGame;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	_pGUIM->gmLoadSaveMenu.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

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

void StartSelectPlayersMenuFromSplitScreen(void)
{
	_pGUIM->gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	_pGUIM->gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
	//  gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
	_pGUIM->gmSelectPlayersMenu.gm_pgmParentMenu = &_pGUIM->gmSplitScreenMenu;
	ChangeToMenu(&_pGUIM->gmSelectPlayersMenu);
}

void StartNetworkLoadGame(void)
{
	//  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) mgSplitScreenCfg.mg_iSelected;
	_pGame->gm_StartSplitScreenCfg = _pGame->gm_MenuSplitScreenCfg;

	_pGame->gm_aiStartLocalPlayers[0] = _pGame->gm_aiMenuLocalPlayers[0];
	_pGame->gm_aiStartLocalPlayers[1] = _pGame->gm_aiMenuLocalPlayers[1];
	_pGame->gm_aiStartLocalPlayers[2] = _pGame->gm_aiMenuLocalPlayers[2];
	_pGame->gm_aiStartLocalPlayers[3] = _pGame->gm_aiMenuLocalPlayers[3];

	_pGame->gm_strNetworkProvider = "TCP/IP Server";
	if (_pGame->LoadGame(_fnGameToLoad))
	{
		StopMenus();
		_gmRunningGameMode = GM_NETWORK;
	} else {
		_gmRunningGameMode = GM_NONE;
	}
}

void StartSelectPlayersMenuFromNetworkLoad(void)
{
	_pGUIM->gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	_pGUIM->gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
	_pGUIM->gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartNetworkLoadGame;
	_pGUIM->gmSelectPlayersMenu.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
	ChangeToMenu(&_pGUIM->gmSelectPlayersMenu);
}

void StartSplitScreenGameLoad(void)
{
	//  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) mgSplitScreenCfg.mg_iSelected;
	_pGame->gm_StartSplitScreenCfg = _pGame->gm_MenuSplitScreenCfg;

	_pGame->gm_aiStartLocalPlayers[0] = _pGame->gm_aiMenuLocalPlayers[0];
	_pGame->gm_aiStartLocalPlayers[1] = _pGame->gm_aiMenuLocalPlayers[1];
	_pGame->gm_aiStartLocalPlayers[2] = _pGame->gm_aiMenuLocalPlayers[2];
	_pGame->gm_aiStartLocalPlayers[3] = _pGame->gm_aiMenuLocalPlayers[3];

	_pGame->gm_strNetworkProvider = "Local";
	if (_pGame->LoadGame(_fnGameToLoad)) {
		StopMenus();
		_gmRunningGameMode = GM_SPLIT_SCREEN;
	} else {
		_gmRunningGameMode = GM_NONE;
	}
}

void StartSelectPlayersMenuFromSplitScreenLoad(void)
{
	_pGUIM->gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	_pGUIM->gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
	_pGUIM->gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGameLoad;
	_pGUIM->gmSelectPlayersMenu.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
	ChangeToMenu(&_pGUIM->gmSelectPlayersMenu);
}

// save/load menu calling functions
void StartPlayerModelLoadMenu(void)
{
	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("CHOOSE MODEL");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEUP;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("Models\\Player\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = _strLastPlayerAppearance;
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".amc");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadPlayerModel;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmPlayerProfile;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartControlsLoadMenu(void)
{
	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("LOAD CONTROLS");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEUP;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("Controls\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".ctl");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadControls;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmControls;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartCustomLoadMenu(void)
{
	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("ADVANCED OPTIONS");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_NAMEUP;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("Scripts\\CustomOptions\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".cfg");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadCustom;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartAddonsLoadMenu(void)
{
	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("EXECUTE ADDON");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_NAMEUP;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("Scripts\\Addons\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".ini");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadAddon;
	_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartModsLoadMenu(void)
{
	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("CHOOSE MOD");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_NAMEUP;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("Mods\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = CTString("");
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".des");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadMod;

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmMainMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartNetworkSettingsMenu(void)
{
	_pGUIM->gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("CONNECTION SETTINGS");
	_pGUIM->gmLoadSaveMenu.gm_bAllowThumbnails = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_iSortType = LSSORT_FILEUP;
	_pGUIM->gmLoadSaveMenu.gm_bSave = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_bManage = FALSE;
	_pGUIM->gmLoadSaveMenu.gm_fnmDirectory = CTString("Scripts\\NetSettings\\");
	_pGUIM->gmLoadSaveMenu.gm_fnmSelected = sam_strNetworkSettings;
	_pGUIM->gmLoadSaveMenu.gm_fnmExt = CTString(".ini");
	_pGUIM->gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadNetSettings;
	if (sam_strNetworkSettings == "") {
		_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = TRANS(
			"Before joining a network game,\n"
			"you have to adjust your connection parameters.\n"
			"Choose one option from the list.\n"
			"If you have problems with connection, you can adjust\n"
			"these parameters again from the Options menu.\n"
			);
	} else {
		_pGUIM->gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	}

	_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu = &_pGUIM->gmOptionsMenu;
	ChangeToMenu(&_pGUIM->gmLoadSaveMenu);
}

void StartSinglePlayerMenu(void)
{
	ChangeToMenu(&_pGUIM->gmSinglePlayerMenu);
}