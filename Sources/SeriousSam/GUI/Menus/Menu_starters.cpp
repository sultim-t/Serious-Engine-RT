/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Build.h>

#include "Menu_starters.h"
#include "MenuStuff.h"
#include "LevelInfo.h"

#include "MAudioOptions.h"
#include "MConfirm.h"
#include "MControls.h"
#include "MCustomizeAxis.h"
#include "MCustomizeKeyboard.h"
#include "MCredits.h"
#include "MDisabled.h"
#include "MHighScore.h"
#include "MInGame.h"
#include "MLevels.h"
#include "MMain.h"
#include "MNetwork.h"
#include "MNetworkJoin.h"
#include "MNetworkOpen.h"
#include "MNetworkStart.h"
#include "MOptions.h"
#include "MRenderingOptions.h"
#include "MServers.h"
#include "MSinglePlayer.h"
#include "MSinglePlayerNew.h"
#include "MSplitScreen.h"
#include "MSplitStart.h"
#include "MVar.h"
#include "MVideoOptions.h"

extern void(*_pAfterLevelChosen)(void);
extern BOOL _bPlayerMenuFromSinglePlayer;

extern CTString _strLastPlayerAppearance;
extern CTString sam_strNetworkSettings;

extern CTFileName _fnmModSelected;
extern CTString _strModURLSelected;
extern CTString _strModServerSelected;

CTFileName _fnDemoToPlay;
CTFileName _fnGameToLoad;

extern CConfirmMenu gmConfirmMenu;
extern CMainMenu gmMainMenu;
extern CInGameMenu gmInGameMenu;
extern CSinglePlayerMenu gmSinglePlayerMenu;
extern CSinglePlayerNewMenu gmSinglePlayerNewMenu;
extern CDisabledMenu gmDisabledFunction;
extern CLevelsMenu gmLevelsMenu;
extern CVarMenu gmVarMenu;
extern CPlayerProfileMenu gmPlayerProfile;
extern CControlsMenu gmControls;
extern CLoadSaveMenu gmLoadSaveMenu;
extern CHighScoreMenu gmHighScoreMenu;
extern CCustomizeKeyboardMenu gmCustomizeKeyboardMenu;
extern CServersMenu gmServersMenu;
extern CCustomizeAxisMenu gmCustomizeAxisMenu;
extern COptionsMenu gmOptionsMenu;
extern CVideoOptionsMenu gmVideoOptionsMenu;
extern CAudioOptionsMenu gmAudioOptionsMenu;
extern CNetworkMenu gmNetworkMenu;
extern CNetworkJoinMenu gmNetworkJoinMenu;
extern CNetworkStartMenu gmNetworkStartMenu;
extern CNetworkOpenMenu gmNetworkOpenMenu;
extern CSplitScreenMenu gmSplitScreenMenu;
extern CSplitStartMenu gmSplitStartMenu;
extern CSelectPlayersMenu gmSelectPlayersMenu;


void StartVideoOptionsMenu(void)
{
	ChangeToMenu(&gmVideoOptionsMenu);
}

void StartAudioOptionsMenu(void)
{
	ChangeToMenu(&gmAudioOptionsMenu);
}

void StartNetworkMenu(void)
{
	ChangeToMenu(&gmNetworkMenu);
}

void StartNetworkJoinMenu(void)
{
	ChangeToMenu(&gmNetworkJoinMenu);
}

void StartNetworkStartMenu(void)
{
	ChangeToMenu(&gmNetworkStartMenu);
}

void StartNetworkOpenMenu(void)
{
	ChangeToMenu(&gmNetworkOpenMenu);
}

void StartSplitScreenMenu(void)
{
	ChangeToMenu(&gmSplitScreenMenu);
}

void StartSplitStartMenu(void)
{
	ChangeToMenu(&gmSplitStartMenu);
}

void StartSinglePlayerNewMenuCustom(void)
{
	gmSinglePlayerNewMenu.gm_pgmParentMenu = &gmLevelsMenu;
	ChangeToMenu(&gmSinglePlayerNewMenu);
}

static void SetQuickLoadNotes(void)
{
	if (_pShell->GetINDEX("gam_iQuickSaveSlots") <= 8) {
		gmLoadSaveMenu.gm_mgNotes.mg_strText = TRANS(
			"In-game QuickSave shortcuts:\n"
			"F6 - save a new QuickSave\n"
			"F9 - load the last QuickSave\n");
	} else {
		gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	}
}

void StartSinglePlayerNewMenu(void)
{
	gmSinglePlayerNewMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;
	extern CTString sam_strFirstLevel;
	_pGame->gam_strCustomLevel = sam_strFirstLevel;
	ChangeToMenu(&gmSinglePlayerNewMenu);
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
	}
	else {
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
	}
	else {
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
	CPlayerCharacter &pc = _pGame->gm_apcPlayers[*gmPlayerProfile.gm_piCurrentPlayer];
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
	gmVarMenu.gm_mgTitle.mg_strText = TRANS("ADVANCED OPTIONS");
	//  LoadStringVar(fnm.NoExt()+".des", mgVarTitle.mg_strText);
	//  mgVarTitle.mg_strText.OnlyFirstLine();
	gmVarMenu.gm_fnmMenuCFG = fnm;
	gmVarMenu.gm_pgmParentMenu = &gmLoadSaveMenu;
	ChangeToMenu(&gmVarMenu);
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

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\Quick\\", _pGame->gm_iSinglePlayer);
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadSinglePlayer;
	SetQuickLoadNotes();

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartSinglePlayerLoadMenu(void)
{
	_gmMenuGameMode = GM_SINGLE_PLAYER;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("LOAD");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", _pGame->gm_iSinglePlayer);
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadSinglePlayer;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
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
	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("SAVE");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = TRUE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory.PrintF("SaveGame\\Player%d\\", _pGame->gm_iSinglePlayer);
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmBaseName = CTString("SaveGame");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSSaveAnyGame;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	gmLoadSaveMenu.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartDemoLoadMenu(void)
{
	_gmMenuGameMode = GM_DEMO;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("PLAY DEMO");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("Demos\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".dem");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadDemo;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartDemoSaveMenu(void)
{
	if (_gmRunningGameMode == GM_NONE) return;
	_gmMenuGameMode = GM_DEMO;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("RECORD DEMO");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEUP;
	gmLoadSaveMenu.gm_bSave = TRUE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("Demos\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmBaseName = CTString("Demo");
	gmLoadSaveMenu.gm_fnmExt = CTString(".dem");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSSaveDemo;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	gmLoadSaveMenu.gm_strSaveDes = _pGame->GetDefaultGameDescription(FALSE);

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartNetworkQuickLoadMenu(void)
{
	_gmMenuGameMode = GM_NETWORK;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\Network\\Quick\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadNetwork;
	SetQuickLoadNotes();

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartNetworkLoadMenu(void)
{
	_gmMenuGameMode = GM_NETWORK;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("LOAD");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\Network\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadNetwork;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartNetworkSaveMenu(void)
{
	if (_gmRunningGameMode != GM_NETWORK) return;
	_gmMenuGameMode = GM_NETWORK;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("SAVE");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = TRUE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\Network\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmBaseName = CTString("SaveGame");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSSaveAnyGame;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	gmLoadSaveMenu.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartSplitScreenQuickLoadMenu(void)
{
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("QUICK LOAD");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\Quick\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadSplitScreen;
	SetQuickLoadNotes();

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartSplitScreenLoadMenu(void)
{
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("LOAD");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadSplitScreen;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartSplitScreenSaveMenu(void)
{
	if (_gmRunningGameMode != GM_SPLIT_SCREEN) return;
	_gmMenuGameMode = GM_SPLIT_SCREEN;

	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("SAVE");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEDN;
	gmLoadSaveMenu.gm_bSave = TRUE;
	gmLoadSaveMenu.gm_bManage = TRUE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("SaveGame\\SplitScreen\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmBaseName = CTString("SaveGame");
	gmLoadSaveMenu.gm_fnmExt = CTString(".sav");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSSaveAnyGame;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	gmLoadSaveMenu.gm_strSaveDes = _pGame->GetDefaultGameDescription(TRUE);

	gmLoadSaveMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

// game options var settings
void StartVarGameOptions(void)
{
	gmVarMenu.gm_mgTitle.mg_strText = TRANS("GAME OPTIONS");
	gmVarMenu.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\GameOptions.cfg");
	ChangeToMenu(&gmVarMenu);
}

void StartSinglePlayerGameOptions(void)
{
	gmVarMenu.gm_mgTitle.mg_strText = TRANS("GAME OPTIONS");
	gmVarMenu.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\SPOptions.cfg");
	ChangeToMenu(&gmVarMenu);
	gmVarMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;
}

void StartGameOptionsFromNetwork(void)
{
	StartVarGameOptions();
	gmVarMenu.gm_pgmParentMenu = &gmNetworkStartMenu;
}

void StartGameOptionsFromSplitScreen(void)
{
	StartVarGameOptions();
	gmVarMenu.gm_pgmParentMenu = &gmSplitStartMenu;
}

// rendering options var settings
void StartRenderingOptionsMenu(void)
{
	gmVarMenu.gm_mgTitle.mg_strText = TRANS("RENDERING OPTIONS");
	gmVarMenu.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\RenderingOptions.cfg");
	gmVarMenu.gm_pgmParentMenu = &gmVideoOptionsMenu;
	ChangeToMenu(&gmVarMenu);
}

void StartCustomizeKeyboardMenu(void)
{
	ChangeToMenu(&gmCustomizeKeyboardMenu);
}

void StartCustomizeAxisMenu(void)
{
	ChangeToMenu(&gmCustomizeAxisMenu);
}

void StartOptionsMenu(void)
{
	gmOptionsMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmOptionsMenu);
}

void StartCurrentLoadMenu()
{
	if (_gmRunningGameMode == GM_NETWORK) {
		void StartNetworkLoadMenu(void);
		StartNetworkLoadMenu();
	}
	else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
		void StartSplitScreenLoadMenu(void);
		StartSplitScreenLoadMenu();
	}
	else {
		void StartSinglePlayerLoadMenu(void);
		StartSinglePlayerLoadMenu();
	}
}

void StartCurrentSaveMenu()
{
	if (_gmRunningGameMode == GM_NETWORK) {
		void StartNetworkSaveMenu(void);
		StartNetworkSaveMenu();
	}
	else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
		void StartSplitScreenSaveMenu(void);
		StartSplitScreenSaveMenu();
	}
	else {
		void StartSinglePlayerSaveMenu(void);
		StartSinglePlayerSaveMenu();
	}
}

void StartCurrentQuickLoadMenu()
{
	if (_gmRunningGameMode == GM_NETWORK) {
		void StartNetworkQuickLoadMenu(void);
		StartNetworkQuickLoadMenu();
	}
	else if (_gmRunningGameMode == GM_SPLIT_SCREEN) {
		void StartSplitScreenQuickLoadMenu(void);
		StartSplitScreenQuickLoadMenu();
	}
	else {
		void StartSinglePlayerQuickLoadMenu(void);
		StartSinglePlayerQuickLoadMenu();
	}
}

void StartChangePlayerMenuFromOptions(void)
{
	_bPlayerMenuFromSinglePlayer = FALSE;
	gmPlayerProfile.gm_piCurrentPlayer = &_pGame->gm_iSinglePlayer;
	gmPlayerProfile.gm_pgmParentMenu = &gmOptionsMenu;
	ChangeToMenu(&gmPlayerProfile);
}

void StartChangePlayerMenuFromSinglePlayer(void)
{
	_iLocalPlayer = -1;
	_bPlayerMenuFromSinglePlayer = TRUE;
	gmPlayerProfile.gm_piCurrentPlayer = &_pGame->gm_iSinglePlayer;
	gmPlayerProfile.gm_pgmParentMenu = &gmSinglePlayerMenu;
	ChangeToMenu(&gmPlayerProfile);
}

void StartControlsMenuFromPlayer(void)
{
	gmControls.gm_pgmParentMenu = &gmPlayerProfile;
	ChangeToMenu(&gmControls);
}

void StartControlsMenuFromOptions(void)
{
	gmControls.gm_pgmParentMenu = &gmOptionsMenu;
	ChangeToMenu(&gmControls);
}

void StartSelectLevelFromSingle(void)
{
	FilterLevels(GetSpawnFlagsForGameType(-1));
	_pAfterLevelChosen = StartSinglePlayerNewMenuCustom;
	ChangeToMenu(&gmLevelsMenu);
	gmLevelsMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;
}

void StartHighScoreMenu(void)
{
	gmHighScoreMenu.gm_pgmParentMenu = pgmCurrentMenu;
	ChangeToMenu(&gmHighScoreMenu);
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
	}
	else {
		_gmRunningGameMode = GM_NONE;
	}
}

void StartSelectPlayersMenuFromSplit(void)
{
	gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
	gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
	gmSelectPlayersMenu.gm_pgmParentMenu = &gmSplitStartMenu;
	ChangeToMenu(&gmSelectPlayersMenu);
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
	}
	else {
		_gmRunningGameMode = GM_NONE;
	}
}

void StartSelectPlayersMenuFromNetwork(void)
{
	gmSelectPlayersMenu.gm_bAllowDedicated = TRUE;
	gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
	gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartNetworkGame;
	gmSelectPlayersMenu.gm_pgmParentMenu = &gmNetworkStartMenu;
	ChangeToMenu(&gmSelectPlayersMenu);
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
	}
	else {
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
	gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
	gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
	gmSelectPlayersMenu.gm_pgmParentMenu = &gmNetworkOpenMenu;
	ChangeToMenu(&gmSelectPlayersMenu);

	/*if (sam_strNetworkSettings=="")*/ {
		void StartNetworkSettingsMenu(void);
		StartNetworkSettingsMenu();
		gmLoadSaveMenu.gm_bNoEscape = TRUE;
		gmLoadSaveMenu.gm_pgmParentMenu = &gmNetworkOpenMenu;
		gmLoadSaveMenu.gm_pgmNextMenu = &gmSelectPlayersMenu;
	}
}
void StartSelectPlayersMenuFromServers(void)
{
	gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
	gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
	gmSelectPlayersMenu.gm_pgmParentMenu = &gmServersMenu;
	ChangeToMenu(&gmSelectPlayersMenu);

	/*if (sam_strNetworkSettings=="")*/ {
		void StartNetworkSettingsMenu(void);
		StartNetworkSettingsMenu();
		gmLoadSaveMenu.gm_bNoEscape = TRUE;
		gmLoadSaveMenu.gm_pgmParentMenu = &gmServersMenu;
		gmLoadSaveMenu.gm_pgmNextMenu = &gmSelectPlayersMenu;
	}
}

void StartSelectServerLAN(void)
{
	gmServersMenu.m_bInternet = FALSE;
	ChangeToMenu(&gmServersMenu);
	gmServersMenu.gm_pgmParentMenu = &gmNetworkJoinMenu;
}

void StartSelectServerNET(void)
{
	gmServersMenu.m_bInternet = TRUE;
	ChangeToMenu(&gmServersMenu);
	gmServersMenu.gm_pgmParentMenu = &gmNetworkJoinMenu;
}

void StartSelectLevelFromSplit(void)
{
	FilterLevels(GetSpawnFlagsForGameType(gmSplitStartMenu.gm_mgGameType.mg_iSelected));
	void StartSplitStartMenu(void);
	_pAfterLevelChosen = StartSplitStartMenu;
	ChangeToMenu(&gmLevelsMenu);
	gmLevelsMenu.gm_pgmParentMenu = &gmSplitStartMenu;
}

void StartSelectLevelFromNetwork(void)
{
	FilterLevels(GetSpawnFlagsForGameType(gmNetworkStartMenu.gm_mgGameType.mg_iSelected));
	void StartNetworkStartMenu(void);
	_pAfterLevelChosen = StartNetworkStartMenu;
	ChangeToMenu(&gmLevelsMenu);
	gmLevelsMenu.gm_pgmParentMenu = &gmNetworkStartMenu;
}

void StartSelectPlayersMenuFromSplitScreen(void)
{
	gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
	//  gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
	gmSelectPlayersMenu.gm_pgmParentMenu = &gmSplitScreenMenu;
	ChangeToMenu(&gmSelectPlayersMenu);
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
	}
	else {
		_gmRunningGameMode = GM_NONE;
	}
}

void StartSelectPlayersMenuFromNetworkLoad(void)
{
	gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
	gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartNetworkLoadGame;
	gmSelectPlayersMenu.gm_pgmParentMenu = &gmLoadSaveMenu;
	ChangeToMenu(&gmSelectPlayersMenu);
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
	gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
	gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
	gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGameLoad;
	gmSelectPlayersMenu.gm_pgmParentMenu = &gmLoadSaveMenu;
	ChangeToMenu(&gmSelectPlayersMenu);
}

// save/load menu calling functions
void StartPlayerModelLoadMenu(void)
{
	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("CHOOSE MODEL");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEUP;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = FALSE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("Models\\Player\\");
	gmLoadSaveMenu.gm_fnmSelected = _strLastPlayerAppearance;
	gmLoadSaveMenu.gm_fnmExt = CTString(".amc");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadPlayerModel;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	gmLoadSaveMenu.gm_pgmParentMenu = &gmPlayerProfile;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartControlsLoadMenu(void)
{
	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("LOAD CONTROLS");
	gmLoadSaveMenu.gm_bAllowThumbnails = FALSE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEUP;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = FALSE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("Controls\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".ctl");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadControls;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	gmLoadSaveMenu.gm_pgmParentMenu = &gmControls;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartCustomLoadMenu(void)
{
	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("ADVANCED OPTIONS");
	gmLoadSaveMenu.gm_bAllowThumbnails = FALSE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_NAMEUP;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = FALSE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("Scripts\\CustomOptions\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".cfg");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadCustom;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	gmLoadSaveMenu.gm_pgmParentMenu = &gmOptionsMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartAddonsLoadMenu(void)
{
	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("EXECUTE ADDON");
	gmLoadSaveMenu.gm_bAllowThumbnails = FALSE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_NAMEUP;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = FALSE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("Scripts\\Addons\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".ini");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadAddon;
	gmLoadSaveMenu.gm_mgNotes.mg_strText = "";

	gmLoadSaveMenu.gm_pgmParentMenu = &gmOptionsMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartModsLoadMenu(void)
{
	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("CHOOSE MOD");
	gmLoadSaveMenu.gm_bAllowThumbnails = TRUE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_NAMEUP;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = FALSE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("Mods\\");
	gmLoadSaveMenu.gm_fnmSelected = CTString("");
	gmLoadSaveMenu.gm_fnmExt = CTString(".des");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadMod;

	gmLoadSaveMenu.gm_pgmParentMenu = &gmMainMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartNetworkSettingsMenu(void)
{
	gmLoadSaveMenu.gm_mgTitle.mg_strText = TRANS("CONNECTION SETTINGS");
	gmLoadSaveMenu.gm_bAllowThumbnails = FALSE;
	gmLoadSaveMenu.gm_iSortType = LSSORT_FILEUP;
	gmLoadSaveMenu.gm_bSave = FALSE;
	gmLoadSaveMenu.gm_bManage = FALSE;
	gmLoadSaveMenu.gm_fnmDirectory = CTString("Scripts\\NetSettings\\");
	gmLoadSaveMenu.gm_fnmSelected = sam_strNetworkSettings;
	gmLoadSaveMenu.gm_fnmExt = CTString(".ini");
	gmLoadSaveMenu.gm_pAfterFileChosen = &LSLoadNetSettings;
	if (sam_strNetworkSettings == "") {
		gmLoadSaveMenu.gm_mgNotes.mg_strText = TRANS(
			"Before joining a network game,\n"
			"you have to adjust your connection parameters.\n"
			"Choose one option from the list.\n"
			"If you have problems with connection, you can adjust\n"
			"these parameters again from the Options menu.\n"
			);
	}
	else {
		gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
	}

	gmLoadSaveMenu.gm_pgmParentMenu = &gmOptionsMenu;
	ChangeToMenu(&gmLoadSaveMenu);
}

void StartSinglePlayerMenu(void)
{
	ChangeToMenu(&gmSinglePlayerMenu);
}