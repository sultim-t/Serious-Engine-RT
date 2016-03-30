/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Build.h>

#include "MenuManager.h"
#include "MenuStarters.h"
#include "MenuStuff.h"
#include "GUI/Components/MenuGadget.h"
#include "LevelInfo.h"
#include "VarList.h"

#define VOLUME_STEPS  50

extern CMenuGadget *_pmgLastActivatedGadget;
extern CMenuGadget *_pmgUnderCursor;

// functions to activate when user chose 'yes/no' on confirmation
void(*_pConfimedYes)(void) = NULL;
void(*_pConfimedNo)(void) = NULL;

INDEX         _ctAdapters = 0;
CTString     * _astrAdapterTexts = NULL;
INDEX         _ctResolutions = 0;
CTString     * _astrResolutionTexts = NULL;
CDisplayMode *_admResolutionModes = NULL;

extern BOOL _bMouseRight = FALSE;
extern BOOL _bMouseUsedLast;
ENGINE_API extern INDEX snd_iFormat;

// make description for a given resolution
CTString GetResolutionDescription(CDisplayMode &dm)
{
	CTString str;
	// if dual head
	if (dm.IsDualHead()) {
		str.PrintF(TRANS("%dx%d double"), dm.dm_pixSizeI / 2, dm.dm_pixSizeJ);
		// if widescreen
	} else if (dm.IsWideScreen()) {
		str.PrintF(TRANS("%dx%d wide"), dm.dm_pixSizeI, dm.dm_pixSizeJ);
		// otherwise it is normal
	} else {
		str.PrintF("%dx%d", dm.dm_pixSizeI, dm.dm_pixSizeJ);
	}
	return str;
}

// make description for a given resolution
void SetResolutionInList(INDEX iRes, PIX pixSizeI, PIX pixSizeJ)
{
	ASSERT(iRes >= 0 && iRes<_ctResolutions);

	CTString &str = _astrResolutionTexts[iRes];
	CDisplayMode &dm = _admResolutionModes[iRes];
	dm.dm_pixSizeI = pixSizeI;
	dm.dm_pixSizeJ = pixSizeJ;
	str = GetResolutionDescription(dm);
}

static void ResolutionToSize(INDEX iRes, PIX &pixSizeI, PIX &pixSizeJ)
{
	ASSERT(iRes >= 0 && iRes<_ctResolutions);
	CDisplayMode &dm = _admResolutionModes[iRes];
	pixSizeI = dm.dm_pixSizeI;
	pixSizeJ = dm.dm_pixSizeJ;
}

static void SizeToResolution(PIX pixSizeI, PIX pixSizeJ, INDEX &iRes)
{
	for (iRes = 0; iRes<_ctResolutions; iRes++) {
		CDisplayMode &dm = _admResolutionModes[iRes];
		if (dm.dm_pixSizeI == pixSizeI && dm.dm_pixSizeJ == pixSizeJ) {
			return;
		}
	}
	// if none was found, search for default 
	for (iRes = 0; iRes<_ctResolutions; iRes++) {
		CDisplayMode &dm = _admResolutionModes[iRes];
		if (dm.dm_pixSizeI == 640 && dm.dm_pixSizeJ == 480) {
			return;
		}
	}
	// if still none found
	ASSERT(FALSE);  // this should never happen
	// return first one
	iRes = 0;
}

// ------------------------ CConfirmMenu implementation
CTFileName _fnmModSelected;
CTString _strModURLSelected;
CTString _strModServerSelected;

void ExitGame(void)
{
	_bRunning = FALSE;
	_bQuitScreen = TRUE;
}

void ExitConfirm(void)
{
	_pConfimedYes = &ExitGame;
	_pConfimedNo = NULL;
	_pGUIM->gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("ARE YOU SERIOUS?");
	_pGUIM->gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
	_pGUIM->gmConfirmMenu.BeLarge();
	ChangeToMenu(&_pGUIM->gmConfirmMenu);
}

void StopCurrentGame(void)
{
	_pGame->StopGame();
	_gmRunningGameMode = GM_NONE;
	StopMenus(TRUE);
	StartMenus("");
}

void StopConfirm(void)
{
	_pConfimedYes = &StopCurrentGame;
	_pConfimedNo = NULL;
	_pGUIM->gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("ARE YOU SERIOUS?");
	_pGUIM->gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
	_pGUIM->gmConfirmMenu.BeLarge();
	ChangeToMenu(&_pGUIM->gmConfirmMenu);
}

void ModLoadYes(void)
{
	extern CTFileName _fnmModToLoad;
	_fnmModToLoad = _fnmModSelected;
}

void ModConnect(void)
{
	extern CTFileName _fnmModToLoad;
	extern CTString _strModServerJoin;
	_fnmModToLoad = _fnmModSelected;
	_strModServerJoin = _strModServerSelected;
}

extern void ModConnectConfirm(void)
{
	if (_fnmModSelected == " ") {
		_fnmModSelected = CTString("SeriousSam");
	}
	CTFileName fnmModPath = "Mods\\" + _fnmModSelected + "\\";
	if (!FileExists(fnmModPath + "BaseWriteInclude.lst")
		&& !FileExists(fnmModPath + "BaseWriteExclude.lst")
		&& !FileExists(fnmModPath + "BaseBrowseInclude.lst")
		&& !FileExists(fnmModPath + "BaseBrowseExclude.lst")) {
		extern void ModNotInstalled(void);
		ModNotInstalled();
		return;
	}

	CPrintF(TRANS("Server is running a different MOD (%s).\nYou need to reload to connect.\n"), _fnmModSelected);
	_pConfimedYes = &ModConnect;
	_pConfimedNo = NULL;
	_pGUIM->gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("CHANGE THE MOD?");
	_pGUIM->gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
	_pGUIM->gmConfirmMenu.BeLarge();
	ChangeToMenu(&_pGUIM->gmConfirmMenu);
}

void SaveConfirm(void)
{
	extern void OnFileSaveOK(void);
	_pConfimedYes = &OnFileSaveOK;
	_pConfimedNo = NULL;
	_pGUIM->gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("OVERWRITE?");
	_pGUIM->gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
	_pGUIM->gmConfirmMenu.BeLarge();
	ChangeToMenu(&_pGUIM->gmConfirmMenu);
}

void ExitAndSpawnExplorer(void)
{
	_bRunning = FALSE;
	_bQuitScreen = FALSE;
	extern CTString _strURLToVisit;
	_strURLToVisit = _strModURLSelected;
}

void ModNotInstalled(void)
{
	_pConfimedYes = &ExitAndSpawnExplorer;
	_pConfimedNo = NULL;
	_pGUIM->gmConfirmMenu.gm_mgConfirmLabel.mg_strText.PrintF(
		TRANS("You don't have MOD '%s' installed.\nDo you want to visit its web site?"), (const char*)_fnmModSelected);
	_pGUIM->gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
	_pGUIM->gmConfirmMenu.BeSmall();
	ChangeToMenu(&_pGUIM->gmConfirmMenu);

	/*
	gmDisabledFunction.gm_pgmParentMenu = pgmCurrentMenu;
	mgDisabledMenuButton.mg_strText.PrintF(
	TRANS("You don't have MOD '%s' installed.\nPlease visit Croteam website for updates."), _fnmModSelected);
	mgDisabledTitle.mg_strText = TRANS("MOD REQUIRED");
	_strModURLSelected
	ChangeToMenu( &gmDisabledFunction);
	*/
}


extern void ModConfirm(void)
{
	_pConfimedYes = &ModLoadYes;
	_pConfimedNo = NULL;
	_pGUIM->gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("LOAD THIS MOD?");
	_pGUIM->gmConfirmMenu.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
	_pGUIM->gmConfirmMenu.BeLarge();
	ChangeToMenu(&_pGUIM->gmConfirmMenu);
}

void VideoConfirm(void)
{
	// FIXUP: keyboard focus lost when going from full screen to window mode
	// due to WM_MOUSEMOVE being sent
	_bMouseUsedLast = FALSE;
	_pmgUnderCursor = _pGUIM->gmConfirmMenu.gm_pmgSelectedByDefault;

	_pConfimedYes = NULL;
	void RevertVideoSettings(void);
	_pConfimedNo = RevertVideoSettings;

	_pGUIM->gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("KEEP THIS SETTING?");
	_pGUIM->gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
	_pGUIM->gmConfirmMenu.BeLarge();
	ChangeToMenu(&_pGUIM->gmConfirmMenu);
}

void CDConfirm(void(*pOk)(void))
{
	_pConfimedYes = pOk;
	_pConfimedNo = NULL;
	_pGUIM->gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("PLEASE INSERT GAME CD?");
	if (pgmCurrentMenu != &_pGUIM->gmConfirmMenu) {
		_pGUIM->gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
		_pGUIM->gmConfirmMenu.BeLarge();
		ChangeToMenu(&_pGUIM->gmConfirmMenu);
	}
}

void ConfirmYes(void)
{
	if (_pConfimedYes != NULL) {
		_pConfimedYes();
	}
	void MenuGoToParent(void);
	MenuGoToParent();
}

void ConfirmNo(void)
{
	if (_pConfimedNo != NULL) {
		_pConfimedNo();
	}
	void MenuGoToParent(void);
	MenuGoToParent();
}

void InitActionsForConfirmMenu() {
	_pGUIM->gmConfirmMenu.gm_mgConfirmYes.mg_pActivatedFunction = &ConfirmYes;
	_pGUIM->gmConfirmMenu.gm_mgConfirmNo.mg_pActivatedFunction = &ConfirmNo;
}

// return TRUE if handled
BOOL CConfirmMenu::OnKeyDown(int iVKey)
{
	if (iVKey == VK_ESCAPE || iVKey == VK_RBUTTON) {
		ConfirmNo();
		return TRUE;
	}
	return CGameMenu::OnKeyDown(iVKey);
}

// ------------------------ CMainMenu implementation
#define CMENU _pGUIM->gmMainMenu

void InitActionsForMainMenu() {
	CMENU.gm_mgSingle.mg_pActivatedFunction = &StartSinglePlayerMenu;
	CMENU.gm_mgNetwork.mg_pActivatedFunction = StartNetworkMenu;
	CMENU.gm_mgSplitScreen.mg_pActivatedFunction = &StartSplitScreenMenu;
	CMENU.gm_mgDemo.mg_pActivatedFunction = &StartDemoLoadMenu;
#if TECHTESTONLY
	CMENU.gm_mgMods.mg_pActivatedFunction = &DisabledFunction;
#else
	CMENU.gm_mgMods.mg_pActivatedFunction = &StartModsLoadMenu;
#endif
	CMENU.gm_mgHighScore.mg_pActivatedFunction = &StartHighScoreMenu;
	CMENU.gm_mgOptions.mg_pActivatedFunction = &StartOptionsMenu;
	CMENU.gm_mgQuit.mg_pActivatedFunction = &ExitConfirm;
}

#undef CMENU

// ------------------------ CInGameMenu implementation
// start load/save menus depending on type of game running
void QuickSaveFromMenu()
{
	_pShell->SetINDEX("gam_bQuickSave", 2); // force save with reporting
	StopMenus(TRUE);
}

void StopRecordingDemo(void)
{
	_pNetwork->StopDemoRec();
	void SetDemoStartStopRecText(void);
	SetDemoStartStopRecText();
}

#define CMENU _pGUIM->gmInGameMenu

void InitActionsForInGameMenu() {

	CMENU.gm_mgQuickLoad.mg_pActivatedFunction = &StartCurrentQuickLoadMenu;
	CMENU.gm_mgQuickSave.mg_pActivatedFunction = &QuickSaveFromMenu;
	CMENU.gm_mgLoad.mg_pActivatedFunction = &StartCurrentLoadMenu;
	CMENU.gm_mgSave.mg_pActivatedFunction = &StartCurrentSaveMenu;
	CMENU.gm_mgHighScore.mg_pActivatedFunction = &StartHighScoreMenu;
	CMENU.gm_mgOptions.mg_pActivatedFunction = &StartOptionsMenu;
#if TECHTESTONLY
	CMENU.gm_mgStop.mg_pActivatedFunction = &ExitConfirm;
#else
	CMENU.gm_mgStop.mg_pActivatedFunction = &StopConfirm;
#endif
	CMENU.gm_mgQuit.mg_pActivatedFunction = &ExitConfirm;
}

extern void SetDemoStartStopRecText(void)
{
	if (_pNetwork->IsRecordingDemo())
	{
		CMENU.gm_mgDemoRec.SetText(TRANS("STOP RECORDING"));
		CMENU.gm_mgDemoRec.mg_strTip = TRANS("stop current recording");
		CMENU.gm_mgDemoRec.mg_pActivatedFunction = &StopRecordingDemo;
	}
	else
	{
		CMENU.gm_mgDemoRec.SetText(TRANS("RECORD DEMO"));
		CMENU.gm_mgDemoRec.mg_strTip = TRANS("start recording current game");
		CMENU.gm_mgDemoRec.mg_pActivatedFunction = &StartDemoSaveMenu;
	}
}

#undef CMENU

// ------------------------ CSinglePlayerMenu implementation
void StartSinglePlayerGame_Normal(void);
void StartTechTest(void)
{
	_pGUIM->gmSinglePlayerNewMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
	extern CTString sam_strTechTestLevel;
	_pGame->gam_strCustomLevel = sam_strTechTestLevel;
	StartSinglePlayerGame_Normal();
}

void StartTraining(void)
{
	_pGUIM->gmSinglePlayerNewMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
	extern CTString sam_strTrainingLevel;
	_pGame->gam_strCustomLevel = sam_strTrainingLevel;
	ChangeToMenu(&_pGUIM->gmSinglePlayerNewMenu);
}

#define CMENU _pGUIM->gmSinglePlayerMenu

void InitActionsForSinglePlayerMenu() {
	CMENU.gm_mgNewGame.mg_pActivatedFunction = &StartSinglePlayerNewMenu;
#if _SE_DEMO || TECHTESTONLY
	CMENU.gm_mgCustom.mg_pActivatedFunction = &DisabledFunction;
#else
	CMENU.gm_mgCustom.mg_pActivatedFunction = &StartSelectLevelFromSingle;
#endif
	CMENU.gm_mgQuickLoad.mg_pActivatedFunction = &StartSinglePlayerQuickLoadMenu;
	CMENU.gm_mgLoad.mg_pActivatedFunction = &StartSinglePlayerLoadMenu;
	CMENU.gm_mgTraining.mg_pActivatedFunction = &StartTraining;
	CMENU.gm_mgTechTest.mg_pActivatedFunction = &StartTechTest;
	CMENU.gm_mgPlayersAndControls.mg_pActivatedFunction = &StartChangePlayerMenuFromSinglePlayer;
	CMENU.gm_mgOptions.mg_pActivatedFunction = &StartSinglePlayerGameOptions;
}

#undef CMENU

// ------------------------ CSinglePlayerNewMenu implementation
void StartSinglePlayerGame(void)
{
	/*  if (!IsCDInDrive()) {
	CDConfirm(StartSinglePlayerGame);
	return;
	}
	*/

	_pGame->gm_StartSplitScreenCfg = CGame::SSC_PLAY1;

	_pGame->gm_aiStartLocalPlayers[0] = _pGame->gm_iSinglePlayer;
	_pGame->gm_aiStartLocalPlayers[1] = -1;
	_pGame->gm_aiStartLocalPlayers[2] = -1;
	_pGame->gm_aiStartLocalPlayers[3] = -1;

	_pGame->gm_strNetworkProvider = "Local";
	CUniversalSessionProperties sp;
	_pGame->SetSinglePlayerSession(sp);

	if (_pGame->NewGame(_pGame->gam_strCustomLevel, _pGame->gam_strCustomLevel, sp))
	{
		StopMenus();
		_gmRunningGameMode = GM_SINGLE_PLAYER;
	}
	else {
		_gmRunningGameMode = GM_NONE;
	}
}

void StartSinglePlayerGame_Tourist(void)
{
	_pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_TOURIST);
	_pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
	StartSinglePlayerGame();
}

void StartSinglePlayerGame_Easy(void)
{
	_pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_EASY);
	_pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
	StartSinglePlayerGame();
}

void StartSinglePlayerGame_Normal(void)
{
	_pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_NORMAL);
	_pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
	StartSinglePlayerGame();
}

void StartSinglePlayerGame_Hard(void)
{
	_pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_HARD);
	_pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
	StartSinglePlayerGame();
}

void StartSinglePlayerGame_Serious(void)
{
	_pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_EXTREME);
	_pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
	StartSinglePlayerGame();
}

void StartSinglePlayerGame_Mental(void)
{
	_pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_EXTREME + 1);
	_pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
	StartSinglePlayerGame();
}


#define CMENU _pGUIM->gmSinglePlayerNewMenu

void InitActionsForSinglePlayerNewMenu() {
	CMENU.gm_mgTourist.mg_pActivatedFunction = &StartSinglePlayerGame_Tourist;
	CMENU.gm_mgEasy.mg_pActivatedFunction = &StartSinglePlayerGame_Easy;
	CMENU.gm_mgMedium.mg_pActivatedFunction = &StartSinglePlayerGame_Normal;
	CMENU.gm_mgHard.mg_pActivatedFunction = &StartSinglePlayerGame_Hard;
	CMENU.gm_mgSerious.mg_pActivatedFunction = &StartSinglePlayerGame_Serious;
	CMENU.gm_mgMental.mg_pActivatedFunction = &StartSinglePlayerGame_Mental;
}

#undef CMENU

// ------------------------ CPlayerProfileMenu implementation
#define CMENU _pGUIM->gmPlayerProfile

void ChangeCrosshair(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	pps->ps_iCrossHairType = iNew - 1;
}

void ChangeWeaponSelect(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	pps->ps_iWeaponAutoSelect = iNew;
}

void ChangeWeaponHide(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	if (iNew) {
		pps->ps_ulFlags |= PSF_HIDEWEAPON;
	} else {
		pps->ps_ulFlags &= ~PSF_HIDEWEAPON;
	}
}

void Change3rdPerson(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	if (iNew) {
		pps->ps_ulFlags |= PSF_PREFER3RDPERSON;
	} else {
		pps->ps_ulFlags &= ~PSF_PREFER3RDPERSON;
	}
}

void ChangeQuotes(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	if (iNew) {
		pps->ps_ulFlags &= ~PSF_NOQUOTES;
	} else {
		pps->ps_ulFlags |= PSF_NOQUOTES;
	}
}

void ChangeAutoSave(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	if (iNew) {
		pps->ps_ulFlags |= PSF_AUTOSAVE;
	} else {
		pps->ps_ulFlags &= ~PSF_AUTOSAVE;
	}
}

void ChangeCompDoubleClick(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	if (iNew) {
		pps->ps_ulFlags &= ~PSF_COMPSINGLECLICK;
	} else {
		pps->ps_ulFlags |= PSF_COMPSINGLECLICK;
	}
}

void ChangeViewBobbing(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	if (iNew) {
		pps->ps_ulFlags &= ~PSF_NOBOBBING;
	} else {
		pps->ps_ulFlags |= PSF_NOBOBBING;
	}
}

void ChangeSharpTurning(INDEX iNew)
{
	INDEX iPlayer = *CMENU.gm_piCurrentPlayer;
	CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
	if (iNew) {
		pps->ps_ulFlags |= PSF_SHARPTURNING;
	} else {
		pps->ps_ulFlags &= ~PSF_SHARPTURNING;
	}
}

extern void PPOnPlayerSelect(void)
{
	ASSERT(_pmgLastActivatedGadget != NULL);
	if (_pmgLastActivatedGadget->mg_bEnabled) {
		_pGUIM->gmPlayerProfile.SelectPlayer(((CMGButton *)_pmgLastActivatedGadget)->mg_iIndex);
	}
}

void InitActionsForPlayerProfileMenu()
{
	CMENU.gm_mgCrosshair.mg_pOnTriggerChange = ChangeCrosshair;
	CMENU.gm_mgWeaponSelect.mg_pOnTriggerChange = ChangeWeaponSelect;
	CMENU.gm_mgWeaponHide.mg_pOnTriggerChange = ChangeWeaponHide;
	CMENU.gm_mg3rdPerson.mg_pOnTriggerChange = Change3rdPerson;
	CMENU.gm_mgQuotes.mg_pOnTriggerChange = ChangeQuotes;
	CMENU.gm_mgAutoSave.mg_pOnTriggerChange = ChangeAutoSave;
	CMENU.gm_mgCompDoubleClick.mg_pOnTriggerChange = ChangeCompDoubleClick;
	CMENU.gm_mgSharpTurning.mg_pOnTriggerChange = ChangeSharpTurning;
	CMENU.gm_mgViewBobbing.mg_pOnTriggerChange = ChangeViewBobbing;
	CMENU.gm_mgCustomizeControls.mg_pActivatedFunction = &StartControlsMenuFromPlayer;
	CMENU.gm_mgModel.mg_pActivatedFunction = &StartPlayerModelLoadMenu;
}

#undef CMENU

// ------------------------ CControlsMenu implementation
#define CMENU _pGUIM->gmControls

void InitActionsForControlsMenu()
{
	CMENU.gm_mgButtons.mg_pActivatedFunction = &StartCustomizeKeyboardMenu;
	CMENU.gm_mgAdvanced.mg_pActivatedFunction = &StartCustomizeAxisMenu;
	CMENU.gm_mgPredefined.mg_pActivatedFunction = &StartControlsLoadMenu;
}

#undef CMENU

// ------------------------ CCustomizeAxisMenu implementation
#define CMENU _pGUIM->gmCustomizeAxisMenu

void PreChangeAxis(INDEX iDummy)
{
	CMENU.ApplyActionSettings();
}

void PostChangeAxis(INDEX iDummy)
{
	CMENU.ObtainActionSettings();
}

void InitActionsForCustomizeAxisMenu() {
	CMENU.gm_mgActionTrigger.mg_pPreTriggerChange = PreChangeAxis;
	CMENU.gm_mgActionTrigger.mg_pOnTriggerChange = PostChangeAxis;
}

#undef CMENU

// ------------------------ COptionsMenu implementation
#define CMENU _pGUIM->gmOptionsMenu

void InitActionsForOptionsMenu()
{
	CMENU.gm_mgVideoOptions.mg_pActivatedFunction = &StartVideoOptionsMenu;
	CMENU.gm_mgAudioOptions.mg_pActivatedFunction = &StartAudioOptionsMenu;
	CMENU.gm_mgPlayerProfileOptions.mg_pActivatedFunction = &StartChangePlayerMenuFromOptions;
	CMENU.gm_mgNetworkOptions.mg_pActivatedFunction = &StartNetworkSettingsMenu;
	CMENU.gm_mgCustomOptions.mg_pActivatedFunction = &StartCustomLoadMenu;
	CMENU.gm_mgAddonOptions.mg_pActivatedFunction = &StartAddonsLoadMenu;
}

#undef CMENU

// ------------------------ CVideoOptionsMenu implementation
static INDEX sam_old_bFullScreenActive;
static INDEX sam_old_iScreenSizeI;
static INDEX sam_old_iScreenSizeJ;
static INDEX sam_old_iDisplayDepth;
static INDEX sam_old_iDisplayAdapter;
static INDEX sam_old_iGfxAPI;
static INDEX sam_old_iVideoSetup;  // 0==speed, 1==normal, 2==quality, 3==custom


#define CMENU _pGUIM->gmVideoOptionsMenu

static void FillResolutionsList(void)
{
	// free resolutions
	if (_astrResolutionTexts != NULL) {
		delete[] _astrResolutionTexts;
	}
	if (_admResolutionModes != NULL) {
		delete[] _admResolutionModes;
	}
	_ctResolutions = 0;

	// if window
	if (CMENU.gm_mgFullScreenTrigger.mg_iSelected == 0) {
		// always has fixed resolutions, but not greater than desktop

		_ctResolutions = ARRAYCOUNT(apixWidths);
		_astrResolutionTexts = new CTString[_ctResolutions];
		_admResolutionModes = new CDisplayMode[_ctResolutions];
		extern PIX _pixDesktopWidth;
		INDEX iRes = 0;
		for (; iRes<_ctResolutions; iRes++) {
			if (apixWidths[iRes][0]>_pixDesktopWidth) break;
			SetResolutionInList(iRes, apixWidths[iRes][0], apixWidths[iRes][1]);
		}
		_ctResolutions = iRes;

		// if fullscreen
	} else {
		// get resolutions list from engine
		CDisplayMode *pdm = _pGfx->EnumDisplayModes(_ctResolutions,
			SwitchToAPI(CMENU.gm_mgDisplayAPITrigger.mg_iSelected), CMENU.gm_mgDisplayAdaptersTrigger.mg_iSelected);
		// allocate that much
		_astrResolutionTexts = new CTString[_ctResolutions];
		_admResolutionModes = new CDisplayMode[_ctResolutions];
		// for each resolution
		for (INDEX iRes = 0; iRes<_ctResolutions; iRes++) {
			// add it to list
			SetResolutionInList(iRes, pdm[iRes].dm_pixSizeI, pdm[iRes].dm_pixSizeJ);
		}
	}
	CMENU.gm_mgResolutionsTrigger.mg_astrTexts = _astrResolutionTexts;
	CMENU.gm_mgResolutionsTrigger.mg_ctTexts = _ctResolutions;
}

static void FillAdaptersList(void)
{
	if (_astrAdapterTexts != NULL) {
		delete[] _astrAdapterTexts;
	}
	_ctAdapters = 0;

	INDEX iApi = SwitchToAPI(CMENU.gm_mgDisplayAPITrigger.mg_iSelected);
	_ctAdapters = _pGfx->gl_gaAPI[iApi].ga_ctAdapters;
	_astrAdapterTexts = new CTString[_ctAdapters];
	for (INDEX iAdapter = 0; iAdapter<_ctAdapters; iAdapter++) {
		_astrAdapterTexts[iAdapter] = _pGfx->gl_gaAPI[iApi].ga_adaAdapter[iAdapter].da_strRenderer;
	}
	CMENU.gm_mgDisplayAdaptersTrigger.mg_astrTexts = _astrAdapterTexts;
	CMENU.gm_mgDisplayAdaptersTrigger.mg_ctTexts = _ctAdapters;
}

extern void UpdateVideoOptionsButtons(INDEX iSelected)
{
	const BOOL _bVideoOptionsChanged = (iSelected != -1);

	const BOOL bOGLEnabled = _pGfx->HasAPI(GAT_OGL);
#ifdef SE1_D3D
	const BOOL bD3DEnabled = _pGfx->HasAPI(GAT_D3D);
	ASSERT(bOGLEnabled || bD3DEnabled);
#else // 
	ASSERT(bOGLEnabled);
#endif // SE1_D3D
	CDisplayAdapter &da = _pGfx->gl_gaAPI[SwitchToAPI(CMENU.gm_mgDisplayAPITrigger.mg_iSelected)]
		.ga_adaAdapter[CMENU.gm_mgDisplayAdaptersTrigger.mg_iSelected];

	// number of available preferences is higher if video setup is custom
	CMENU.gm_mgDisplayPrefsTrigger.mg_ctTexts = 3;
	if (sam_iVideoSetup == 3) CMENU.gm_mgDisplayPrefsTrigger.mg_ctTexts++;

	// enumerate adapters
	FillAdaptersList();

	// show or hide buttons
	CMENU.gm_mgDisplayAPITrigger.mg_bEnabled = bOGLEnabled
#ifdef SE1_D3D
		&& bD3DEnabled
#endif // SE1_D3D
		;
	CMENU.gm_mgDisplayAdaptersTrigger.mg_bEnabled = _ctAdapters>1;
	CMENU.gm_mgApply.mg_bEnabled = _bVideoOptionsChanged;
	// determine which should be visible
	CMENU.gm_mgFullScreenTrigger.mg_bEnabled = TRUE;
	if (da.da_ulFlags&DAF_FULLSCREENONLY) {
		CMENU.gm_mgFullScreenTrigger.mg_bEnabled = FALSE;
		CMENU.gm_mgFullScreenTrigger.mg_iSelected = 1;
		CMENU.gm_mgFullScreenTrigger.ApplyCurrentSelection();
	}
	CMENU.gm_mgBitsPerPixelTrigger.mg_bEnabled = TRUE;
	if (CMENU.gm_mgFullScreenTrigger.mg_iSelected == 0) {
		CMENU.gm_mgBitsPerPixelTrigger.mg_bEnabled = FALSE;
		CMENU.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch(DD_DEFAULT);
		CMENU.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
	} else if (da.da_ulFlags&DAF_16BITONLY) {
		CMENU.gm_mgBitsPerPixelTrigger.mg_bEnabled = FALSE;
		CMENU.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch(DD_16BIT);
		CMENU.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
	}

	// remember current selected resolution
	PIX pixSizeI, pixSizeJ;
	ResolutionToSize(CMENU.gm_mgResolutionsTrigger.mg_iSelected, pixSizeI, pixSizeJ);

	// select same resolution again if possible
	FillResolutionsList();
	SizeToResolution(pixSizeI, pixSizeJ, CMENU.gm_mgResolutionsTrigger.mg_iSelected);

	// apply adapter and resolutions
	CMENU.gm_mgDisplayAdaptersTrigger.ApplyCurrentSelection();
	CMENU.gm_mgResolutionsTrigger.ApplyCurrentSelection();
}


extern void InitVideoOptionsButtons(void)
{
	if (sam_bFullScreenActive) {
		CMENU.gm_mgFullScreenTrigger.mg_iSelected = 1;
	} else {
		CMENU.gm_mgFullScreenTrigger.mg_iSelected = 0;
	}

	CMENU.gm_mgDisplayAPITrigger.mg_iSelected = APIToSwitch((GfxAPIType)(INDEX)sam_iGfxAPI);
	CMENU.gm_mgDisplayAdaptersTrigger.mg_iSelected = sam_iDisplayAdapter;
	CMENU.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch((enum DisplayDepth)(INDEX)sam_iDisplayDepth);

	FillResolutionsList();
	SizeToResolution(sam_iScreenSizeI, sam_iScreenSizeJ, CMENU.gm_mgResolutionsTrigger.mg_iSelected);
	CMENU.gm_mgDisplayPrefsTrigger.mg_iSelected = Clamp(int(sam_iVideoSetup), 0, 3);

	CMENU.gm_mgFullScreenTrigger.ApplyCurrentSelection();
	CMENU.gm_mgDisplayPrefsTrigger.ApplyCurrentSelection();
	CMENU.gm_mgDisplayAPITrigger.ApplyCurrentSelection();
	CMENU.gm_mgDisplayAdaptersTrigger.ApplyCurrentSelection();
	CMENU.gm_mgResolutionsTrigger.ApplyCurrentSelection();
	CMENU.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
}

void ApplyVideoOptions(void)
{
	sam_old_bFullScreenActive = sam_bFullScreenActive;
	sam_old_iScreenSizeI = sam_iScreenSizeI;
	sam_old_iScreenSizeJ = sam_iScreenSizeJ;
	sam_old_iDisplayDepth = sam_iDisplayDepth;
	sam_old_iDisplayAdapter = sam_iDisplayAdapter;
	sam_old_iGfxAPI = sam_iGfxAPI;
	sam_old_iVideoSetup = sam_iVideoSetup;

	BOOL bFullScreenMode = CMENU.gm_mgFullScreenTrigger.mg_iSelected == 1;
	PIX pixWindowSizeI, pixWindowSizeJ;
	ResolutionToSize(CMENU.gm_mgResolutionsTrigger.mg_iSelected, pixWindowSizeI, pixWindowSizeJ);
	enum GfxAPIType gat = SwitchToAPI(CMENU.gm_mgDisplayAPITrigger.mg_iSelected);
	enum DisplayDepth dd = SwitchToDepth(CMENU.gm_mgBitsPerPixelTrigger.mg_iSelected);
	const INDEX iAdapter = CMENU.gm_mgDisplayAdaptersTrigger.mg_iSelected;

	// setup preferences
	extern INDEX _iLastPreferences;
	if (sam_iVideoSetup == 3) _iLastPreferences = 3;
	sam_iVideoSetup = CMENU.gm_mgDisplayPrefsTrigger.mg_iSelected;

	// force fullscreen mode if needed
	CDisplayAdapter &da = _pGfx->gl_gaAPI[gat].ga_adaAdapter[iAdapter];
	if (da.da_ulFlags & DAF_FULLSCREENONLY) bFullScreenMode = TRUE;
	if (da.da_ulFlags & DAF_16BITONLY) dd = DD_16BIT;
	// force window to always be in default colors
	if (!bFullScreenMode) dd = DD_DEFAULT;

	// (try to) set mode
	StartNewMode(gat, iAdapter, pixWindowSizeI, pixWindowSizeJ, dd, bFullScreenMode);

	// refresh buttons
	InitVideoOptionsButtons();
	UpdateVideoOptionsButtons(-1);

	// ask user to keep or restore
	if (bFullScreenMode) VideoConfirm();
}

void RevertVideoSettings(void)
{
	// restore previous variables
	sam_bFullScreenActive = sam_old_bFullScreenActive;
	sam_iScreenSizeI = sam_old_iScreenSizeI;
	sam_iScreenSizeJ = sam_old_iScreenSizeJ;
	sam_iDisplayDepth = sam_old_iDisplayDepth;
	sam_iDisplayAdapter = sam_old_iDisplayAdapter;
	sam_iGfxAPI = sam_old_iGfxAPI;
	sam_iVideoSetup = sam_old_iVideoSetup;

	// update the video mode
	extern void ApplyVideoMode(void);
	ApplyVideoMode();

	// refresh buttons
	InitVideoOptionsButtons();
	UpdateVideoOptionsButtons(-1);
}

void InitActionsForVideoOptionsMenu()
{
	CMENU.gm_mgDisplayPrefsTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	CMENU.gm_mgDisplayAPITrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	CMENU.gm_mgDisplayAdaptersTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	CMENU.gm_mgFullScreenTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	CMENU.gm_mgResolutionsTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	CMENU.gm_mgBitsPerPixelTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	CMENU.gm_mgVideoRendering.mg_pActivatedFunction = &StartRenderingOptionsMenu;
	CMENU.gm_mgApply.mg_pActivatedFunction = &ApplyVideoOptions;
}

#undef CMENU

// ------------------------ CAudioOptionsMenu implementation
#define CMENU _pGUIM->gmAudioOptionsMenu

extern void RefreshSoundFormat(void)
{
	switch (_pSound->GetFormat())
	{
		case CSoundLibrary::SF_NONE:     {CMENU.gm_mgFrequencyTrigger.mg_iSelected = 0; break; }
		case CSoundLibrary::SF_11025_16: {CMENU.gm_mgFrequencyTrigger.mg_iSelected = 1; break; }
		case CSoundLibrary::SF_22050_16: {CMENU.gm_mgFrequencyTrigger.mg_iSelected = 2; break; }
		case CSoundLibrary::SF_44100_16: {CMENU.gm_mgFrequencyTrigger.mg_iSelected = 3; break; }
		default:                          CMENU.gm_mgFrequencyTrigger.mg_iSelected = 0;
	}

	CMENU.gm_mgAudioAutoTrigger.mg_iSelected = Clamp(sam_bAutoAdjustAudio, 0, 1);
	CMENU.gm_mgAudioAPITrigger.mg_iSelected = Clamp(_pShell->GetINDEX("snd_iInterface"), 0L, 2L);

	CMENU.gm_mgWaveVolume.mg_iMinPos = 0;
	CMENU.gm_mgWaveVolume.mg_iMaxPos = VOLUME_STEPS;
	CMENU.gm_mgWaveVolume.mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fSoundVolume")*VOLUME_STEPS + 0.5f);
	CMENU.gm_mgWaveVolume.ApplyCurrentPosition();

	CMENU.gm_mgMPEGVolume.mg_iMinPos = 0;
	CMENU.gm_mgMPEGVolume.mg_iMaxPos = VOLUME_STEPS;
	CMENU.gm_mgMPEGVolume.mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fMusicVolume")*VOLUME_STEPS + 0.5f);
	CMENU.gm_mgMPEGVolume.ApplyCurrentPosition();

	CMENU.gm_mgAudioAutoTrigger.ApplyCurrentSelection();
	CMENU.gm_mgAudioAPITrigger.ApplyCurrentSelection();
	CMENU.gm_mgFrequencyTrigger.ApplyCurrentSelection();
}

void ApplyAudioOptions(void)
{
	sam_bAutoAdjustAudio = CMENU.gm_mgAudioAutoTrigger.mg_iSelected;
	if (sam_bAutoAdjustAudio) {
		_pShell->Execute("include \"Scripts\\Addons\\SFX-AutoAdjust.ini\"");
	} else {
		_pShell->SetINDEX("snd_iInterface", CMENU.gm_mgAudioAPITrigger.mg_iSelected);

		switch (CMENU.gm_mgFrequencyTrigger.mg_iSelected)
		{
			case 0: {_pSound->SetFormat(CSoundLibrary::SF_NONE); break; }
			case 1: {_pSound->SetFormat(CSoundLibrary::SF_11025_16); break; }
			case 2: {_pSound->SetFormat(CSoundLibrary::SF_22050_16); break; }
			case 3: {_pSound->SetFormat(CSoundLibrary::SF_44100_16); break; }
			default: _pSound->SetFormat(CSoundLibrary::SF_NONE);
		}
	}

	RefreshSoundFormat();
	snd_iFormat = _pSound->GetFormat();
}

static void OnWaveVolumeChange(INDEX iCurPos)
{
	_pShell->SetFLOAT("snd_fSoundVolume", iCurPos / FLOAT(VOLUME_STEPS));
}

void WaveSliderChange(void)
{
	if (_bMouseRight) {
		CMENU.gm_mgWaveVolume.mg_iCurPos += 5;
	} else {
		CMENU.gm_mgWaveVolume.mg_iCurPos -= 5;
	}
	CMENU.gm_mgWaveVolume.ApplyCurrentPosition();
}

void FrequencyTriggerChange(INDEX iDummy)
{
	sam_bAutoAdjustAudio = 0;
	CMENU.gm_mgAudioAutoTrigger.mg_iSelected = 0;
	CMENU.gm_mgAudioAutoTrigger.ApplyCurrentSelection();
}

void MPEGSliderChange(void)
{
	if (_bMouseRight) {
		CMENU.gm_mgMPEGVolume.mg_iCurPos += 5;
	} else {
		CMENU.gm_mgMPEGVolume.mg_iCurPos -= 5;
	}
	CMENU.gm_mgMPEGVolume.ApplyCurrentPosition();
}

static void OnMPEGVolumeChange(INDEX iCurPos)
{
	_pShell->SetFLOAT("snd_fMusicVolume", iCurPos / FLOAT(VOLUME_STEPS));
}

void InitActionsForAudioOptionsMenu()
{
	CMENU.gm_mgFrequencyTrigger.mg_pOnTriggerChange = FrequencyTriggerChange;
	CMENU.gm_mgWaveVolume.mg_pOnSliderChange = &OnWaveVolumeChange;
	CMENU.gm_mgWaveVolume.mg_pActivatedFunction = WaveSliderChange;
	CMENU.gm_mgMPEGVolume.mg_pOnSliderChange = &OnMPEGVolumeChange;
	CMENU.gm_mgMPEGVolume.mg_pActivatedFunction = MPEGSliderChange;
	CMENU.gm_mgApply.mg_pActivatedFunction = &ApplyAudioOptions;
}

#undef CMENU

// ------------------------ CVarMenu implementation
#define CMENU _pGUIM->gmVarMenu

void VarApply(void)
{
	FlushVarSettings(TRUE);
	CMENU.EndMenu();
	CMENU.StartMenu();
}

void InitActionsForVarMenu() {
	CMENU.gm_mgApply.mg_pActivatedFunction = &VarApply;
}

#undef CMENU
// ------------------------ CServersMenu implementation
extern void RefreshServerList(void)
{
	_pNetwork->EnumSessions(_pGUIM->gmServersMenu.m_bInternet);
}

void RefreshServerListManually(void)
{
	ChangeToMenu(&_pGUIM->gmServersMenu); // this refreshes the list and sets focuses
}

void SortByColumn(int i)
{
	if (_pGUIM->gmServersMenu.gm_mgList.mg_iSort == i) {
		_pGUIM->gmServersMenu.gm_mgList.mg_bSortDown = !_pGUIM->gmServersMenu.gm_mgList.mg_bSortDown;
	}
	else {
		_pGUIM->gmServersMenu.gm_mgList.mg_bSortDown = FALSE;
	}
	_pGUIM->gmServersMenu.gm_mgList.mg_iSort = i;
}

void SortByServer(void) { SortByColumn(0); }
void SortByMap(void)    { SortByColumn(1); }
void SortByPing(void)   { SortByColumn(2); }
void SortByPlayers(void){ SortByColumn(3); }
void SortByGame(void)   { SortByColumn(4); }
void SortByMod(void)    { SortByColumn(5); }
void SortByVer(void)    { SortByColumn(6); }

extern CMGButton mgServerColumn[7];
extern CMGEdit mgServerFilter[7];

void InitActionsForServersMenu() {
	_pGUIM->gmServersMenu.gm_mgRefresh.mg_pActivatedFunction = &RefreshServerList;

	mgServerColumn[0].mg_pActivatedFunction = SortByServer;
	mgServerColumn[1].mg_pActivatedFunction = SortByMap;
	mgServerColumn[2].mg_pActivatedFunction = SortByPing;
	mgServerColumn[3].mg_pActivatedFunction = SortByPlayers;
	mgServerColumn[4].mg_pActivatedFunction = SortByGame;
	mgServerColumn[5].mg_pActivatedFunction = SortByMod;
	mgServerColumn[6].mg_pActivatedFunction = SortByVer;
}

// ------------------------ CNetworkMenu implementation
#define CMENU _pGUIM->gmNetworkMenu

void InitActionsForNetworkMenu()
{
	CMENU.gm_mgJoin.mg_pActivatedFunction = &StartNetworkJoinMenu;
	CMENU.gm_mgStart.mg_pActivatedFunction = &StartNetworkStartMenu;
	CMENU.gm_mgQuickLoad.mg_pActivatedFunction = &StartNetworkQuickLoadMenu;
	CMENU.gm_mgLoad.mg_pActivatedFunction = &StartNetworkLoadMenu;
}

#undef CMENU

// ------------------------ CNetworkJoinMenu implementation
#define CMENU _pGUIM->gmNetworkJoinMenu

void InitActionsForNetworkJoinMenu()
{
	CMENU.gm_mgLAN.mg_pActivatedFunction = &StartSelectServerLAN;
	CMENU.gm_mgNET.mg_pActivatedFunction = &StartSelectServerNET;
	CMENU.gm_mgOpen.mg_pActivatedFunction = &StartNetworkOpenMenu;
}

#undef CMENU

// ------------------------ CNetworkStartMenu implementation
extern void UpdateNetworkLevel(INDEX iDummy)
{
	ValidateLevelForFlags(_pGame->gam_strCustomLevel,
		GetSpawnFlagsForGameType(_pGUIM->gmNetworkStartMenu.gm_mgGameType.mg_iSelected));
	_pGUIM->gmNetworkStartMenu.gm_mgLevel.mg_strText = FindLevelByFileName(_pGame->gam_strCustomLevel).li_strName;
}

void InitActionsForNetworkStartMenu()
{
	_pGUIM->gmNetworkStartMenu.gm_mgLevel.mg_pActivatedFunction = &StartSelectLevelFromNetwork;
	_pGUIM->gmNetworkStartMenu.gm_mgGameOptions.mg_pActivatedFunction = &StartGameOptionsFromNetwork;
	_pGUIM->gmNetworkStartMenu.gm_mgStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromNetwork;
}

// ------------------------ CSelectPlayersMenu implementation
#define CMENU _pGUIM->gmSelectPlayersMenu

INDEX FindUnusedPlayer(void)
{
	INDEX *ai = _pGame->gm_aiMenuLocalPlayers;
	INDEX iPlayer = 0;
	for (; iPlayer<8; iPlayer++) {
		BOOL bUsed = FALSE;
		for (INDEX iLocal = 0; iLocal<4; iLocal++) {
			if (ai[iLocal] == iPlayer) {
				bUsed = TRUE;
				break;
			}
		}
		if (!bUsed) {
			return iPlayer;
		}
	}
	ASSERT(FALSE);
	return iPlayer;
}

extern void SelectPlayersFillMenu(void)
{
	INDEX *ai = _pGame->gm_aiMenuLocalPlayers;

	CMENU.gm_mgPlayer0Change.mg_iLocalPlayer = 0;
	CMENU.gm_mgPlayer1Change.mg_iLocalPlayer = 1;
	CMENU.gm_mgPlayer2Change.mg_iLocalPlayer = 2;
	CMENU.gm_mgPlayer3Change.mg_iLocalPlayer = 3;

	if (CMENU.gm_bAllowDedicated && _pGame->gm_MenuSplitScreenCfg == CGame::SSC_DEDICATED) {
		CMENU.gm_mgDedicated.mg_iSelected = 1;
	} else {
		CMENU.gm_mgDedicated.mg_iSelected = 0;
	}

	CMENU.gm_mgDedicated.ApplyCurrentSelection();

	if (CMENU.gm_bAllowObserving && _pGame->gm_MenuSplitScreenCfg == CGame::SSC_OBSERVER) {
		CMENU.gm_mgObserver.mg_iSelected = 1;
	} else {
		CMENU.gm_mgObserver.mg_iSelected = 0;
	}

	CMENU.gm_mgObserver.ApplyCurrentSelection();

	if (_pGame->gm_MenuSplitScreenCfg >= CGame::SSC_PLAY1) {
		CMENU.gm_mgSplitScreenCfg.mg_iSelected = _pGame->gm_MenuSplitScreenCfg;
		CMENU.gm_mgSplitScreenCfg.ApplyCurrentSelection();
	}

	BOOL bHasDedicated = CMENU.gm_bAllowDedicated;
	BOOL bHasObserver = CMENU.gm_bAllowObserving;
	BOOL bHasPlayers = TRUE;

	if (bHasDedicated && CMENU.gm_mgDedicated.mg_iSelected) {
		bHasObserver = FALSE;
		bHasPlayers = FALSE;
	}

	if (bHasObserver && CMENU.gm_mgObserver.mg_iSelected) {
		bHasPlayers = FALSE;
	}

	CMenuGadget *apmg[8];
	memset(apmg, 0, sizeof(apmg));
	INDEX i = 0;

	if (bHasDedicated) {
		CMENU.gm_mgDedicated.Appear();
		apmg[i++] = &CMENU.gm_mgDedicated;
	} else {
		CMENU.gm_mgDedicated.Disappear();
	}

	if (bHasObserver) {
		CMENU.gm_mgObserver.Appear();
		apmg[i++] = &CMENU.gm_mgObserver;
	} else {
		CMENU.gm_mgObserver.Disappear();
	}

	for (INDEX iLocal = 0; iLocal<4; iLocal++) {
		if (ai[iLocal]<0 || ai[iLocal]>7) {
			ai[iLocal] = 0;
		}
		for (INDEX iCopy = 0; iCopy<iLocal; iCopy++) {
			if (ai[iCopy] == ai[iLocal]) {
				ai[iLocal] = FindUnusedPlayer();
			}
		}
	}

	CMENU.gm_mgPlayer0Change.Disappear();
	CMENU.gm_mgPlayer1Change.Disappear();
	CMENU.gm_mgPlayer2Change.Disappear();
	CMENU.gm_mgPlayer3Change.Disappear();

	if (bHasPlayers) {
		CMENU.gm_mgSplitScreenCfg.Appear();
		apmg[i++] = &CMENU.gm_mgSplitScreenCfg;
		CMENU.gm_mgPlayer0Change.Appear();
		apmg[i++] = &CMENU.gm_mgPlayer0Change;
		if (CMENU.gm_mgSplitScreenCfg.mg_iSelected >= 1) {
			CMENU.gm_mgPlayer1Change.Appear();
			apmg[i++] = &CMENU.gm_mgPlayer1Change;
		}
		if (CMENU.gm_mgSplitScreenCfg.mg_iSelected >= 2) {
			CMENU.gm_mgPlayer2Change.Appear();
			apmg[i++] = &CMENU.gm_mgPlayer2Change;
		}
		if (CMENU.gm_mgSplitScreenCfg.mg_iSelected >= 3) {
			CMENU.gm_mgPlayer3Change.Appear();
			apmg[i++] = &CMENU.gm_mgPlayer3Change;
		}
	} else {
		CMENU.gm_mgSplitScreenCfg.Disappear();
	}
	apmg[i++] = &CMENU.gm_mgStart;

	// relink
	for (INDEX img = 0; img<8; img++) {
		if (apmg[img] == NULL) {
			continue;
		}
		INDEX imgPred = (img + 8 - 1) % 8;
		for (; imgPred != img; imgPred = (imgPred + 8 - 1) % 8) {
			if (apmg[imgPred] != NULL) {
				break;
			}
		}
		INDEX imgSucc = (img + 1) % 8;
		for (; imgSucc != img; imgSucc = (imgSucc + 1) % 8) {
			if (apmg[imgSucc] != NULL) {
				break;
			}
		}
		apmg[img]->mg_pmgUp = apmg[imgPred];
		apmg[img]->mg_pmgDown = apmg[imgSucc];
	}

	CMENU.gm_mgPlayer0Change.SetPlayerText();
	CMENU.gm_mgPlayer1Change.SetPlayerText();
	CMENU.gm_mgPlayer2Change.SetPlayerText();
	CMENU.gm_mgPlayer3Change.SetPlayerText();

	if (bHasPlayers && CMENU.gm_mgSplitScreenCfg.mg_iSelected >= 1) {
		CMENU.gm_mgNotes.mg_strText = TRANS("Make sure you set different controls for each player!");
	}
	else {
		CMENU.gm_mgNotes.mg_strText = "";
	}
}

extern void SelectPlayersApplyMenu(void)
{
	if (CMENU.gm_bAllowDedicated && CMENU.gm_mgDedicated.mg_iSelected) {
		_pGame->gm_MenuSplitScreenCfg = CGame::SSC_DEDICATED;
		return;
	}

	if (CMENU.gm_bAllowObserving && CMENU.gm_mgObserver.mg_iSelected) {
		_pGame->gm_MenuSplitScreenCfg = CGame::SSC_OBSERVER;
		return;
	}

	_pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) CMENU.gm_mgSplitScreenCfg.mg_iSelected;
}

void UpdateSelectPlayers(INDEX i)
{
	SelectPlayersApplyMenu();
	SelectPlayersFillMenu();
}

void InitActionsForSelectPlayersMenu()
{
	CMENU.gm_mgDedicated.mg_pOnTriggerChange = UpdateSelectPlayers;
	CMENU.gm_mgObserver.mg_pOnTriggerChange = UpdateSelectPlayers;
	CMENU.gm_mgSplitScreenCfg.mg_pOnTriggerChange = UpdateSelectPlayers;
}

#undef CMENU

// ------------------------ CNetworkOpenMenu implementation
void InitActionsForNetworkOpenMenu()
{
	_pGUIM->gmNetworkOpenMenu.gm_mgJoin.mg_pActivatedFunction = &StartSelectPlayersMenuFromOpen;
}

// ------------------------ CSplitScreenMenu implementation
#define CMENU _pGUIM->gmSplitScreenMenu

void InitActionsForSplitScreenMenu()
{
	CMENU.gm_mgStart.mg_pActivatedFunction = &StartSplitStartMenu;
	CMENU.gm_mgQuickLoad.mg_pActivatedFunction = &StartSplitScreenQuickLoadMenu;
	CMENU.gm_mgLoad.mg_pActivatedFunction = &StartSplitScreenLoadMenu;
}

#undef CMENU

// ------------------------ CSplitStartMenu implementation
#define CMENU _pGUIM->gmSplitStartMenu

void InitActionsForSplitStartMenu()
{
	CMENU.gm_mgLevel.mg_pActivatedFunction = &StartSelectLevelFromSplit;
	CMENU.gm_mgOptions.mg_pActivatedFunction = &StartGameOptionsFromSplitScreen;
	CMENU.gm_mgStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromSplit;
}

extern void UpdateSplitLevel(INDEX iDummy)
{
	ValidateLevelForFlags(_pGame->gam_strCustomLevel,
		GetSpawnFlagsForGameType(CMENU.gm_mgGameType.mg_iSelected));
	CMENU.gm_mgLevel.mg_strText = FindLevelByFileName(_pGame->gam_strCustomLevel).li_strName;
}

#undef CMENU