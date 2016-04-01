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
#include <Engine/Build.h>

#include "MenuManager.h"
#include "MenuStarters.h"
#include "MenuStuff.h"
#include "GUI/Components/MenuGadget.h"
#include "LevelInfo.h"
#include "VarList.h"

ENGINE_API extern INDEX snd_iFormat;
extern BOOL _bMouseUsedLast;

extern CMenuGadget *_pmgLastActivatedGadget;
extern CMenuGadget *_pmgUnderCursor;

static INDEX         _ctAdapters = 0;
static CTString     * _astrAdapterTexts = NULL;
static INDEX         _ctResolutions = 0;
static CTString     * _astrResolutionTexts = NULL;
static CDisplayMode *_admResolutionModes = NULL;

#define VOLUME_STEPS  50


// make description for a given resolution
static CTString GetResolutionDescription(CDisplayMode &dm)
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
static void SetResolutionInList(INDEX iRes, PIX pixSizeI, PIX pixSizeJ)
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
extern CTFileName _fnmModToLoad;
extern CTString _strModServerJoin;

CTFileName _fnmModSelected;
CTString _strModURLSelected;
CTString _strModServerSelected;

static void ExitGame(void)
{
  _bRunning = FALSE;
  _bQuitScreen = TRUE;
}

static void ExitConfirm(void)
{
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &ExitGame;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.gm_mgConfirmLabel.mg_strText = TRANS("ARE YOU SERIOUS?");
  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

static void StopCurrentGame(void)
{
  _pGame->StopGame();
  _gmRunningGameMode = GM_NONE;
  StopMenus(TRUE);
  StartMenus("");
}

static void StopConfirm(void)
{
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &StopCurrentGame;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.gm_mgConfirmLabel.mg_strText = TRANS("ARE YOU SERIOUS?");
  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

static void ModLoadYes(void)
{
  _fnmModToLoad = _fnmModSelected;
}

static void ModConnect(void)
{
  _fnmModToLoad = _fnmModSelected;
  _strModServerJoin = _strModServerSelected;
}

extern void ModConnectConfirm(void)
{
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

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
  gmCurrent._pConfimedYes = &ModConnect;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.gm_mgConfirmLabel.mg_strText = TRANS("CHANGE THE MOD?");
  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

void SaveConfirm(void)
{
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  extern void OnFileSaveOK(void);
  gmCurrent._pConfimedYes = &OnFileSaveOK;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.gm_mgConfirmLabel.mg_strText = TRANS("OVERWRITE?");
  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
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
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &ExitAndSpawnExplorer;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.gm_mgConfirmLabel.mg_strText.PrintF(
    TRANS("You don't have MOD '%s' installed.\nDo you want to visit its web site?"), (const char*)_fnmModSelected);
  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  gmCurrent.BeSmall();
  ChangeToMenu(&gmCurrent);
}

extern void ModConfirm(void)
{
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent._pConfimedYes = &ModLoadYes;
  gmCurrent._pConfimedNo = NULL;
  gmCurrent.gm_mgConfirmLabel.mg_strText = TRANS("LOAD THIS MOD?");
  gmCurrent.gm_pgmParentMenu = &_pGUIM->gmLoadSaveMenu;
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

static void RevertVideoSettings(void);

void VideoConfirm(void)
{
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  // FIXUP: keyboard focus lost when going from full screen to window mode
  // due to WM_MOUSEMOVE being sent
  _bMouseUsedLast = FALSE;
  _pmgUnderCursor = gmCurrent.gm_pmgSelectedByDefault;

  gmCurrent._pConfimedYes = NULL;
  gmCurrent._pConfimedNo = RevertVideoSettings;

  gmCurrent.gm_mgConfirmLabel.mg_strText = TRANS("KEEP THIS SETTING?");
  gmCurrent.gm_pgmParentMenu = pgmCurrentMenu;
  gmCurrent.BeLarge();
  ChangeToMenu(&gmCurrent);
}

static void ConfirmYes(void)
{
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  if (gmCurrent._pConfimedYes != NULL) {
    gmCurrent._pConfimedYes();
  }
  void MenuGoToParent(void);
  MenuGoToParent();
}

static void ConfirmNo(void)
{
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  if (gmCurrent._pConfimedNo != NULL) {
    gmCurrent._pConfimedNo();
  }
  void MenuGoToParent(void);
  MenuGoToParent();
}

void InitActionsForConfirmMenu() {
  CConfirmMenu &gmCurrent = _pGUIM->gmConfirmMenu;

  gmCurrent.gm_mgConfirmYes.mg_pActivatedFunction = &ConfirmYes;
  gmCurrent.gm_mgConfirmNo.mg_pActivatedFunction = &ConfirmNo;
}

// ------------------------ CMainMenu implementation
void InitActionsForMainMenu() {
  CMainMenu &gmCurrent = _pGUIM->gmMainMenu;

  gmCurrent.gm_mgSingle.mg_pActivatedFunction = &StartSinglePlayerMenu;
  gmCurrent.gm_mgNetwork.mg_pActivatedFunction = StartNetworkMenu;
  gmCurrent.gm_mgSplitScreen.mg_pActivatedFunction = &StartSplitScreenMenu;
  gmCurrent.gm_mgDemo.mg_pActivatedFunction = &StartDemoLoadMenu;
  gmCurrent.gm_mgMods.mg_pActivatedFunction = &StartModsLoadMenu;
  gmCurrent.gm_mgHighScore.mg_pActivatedFunction = &StartHighScoreMenu;
  gmCurrent.gm_mgOptions.mg_pActivatedFunction = &StartOptionsMenu;
  gmCurrent.gm_mgQuit.mg_pActivatedFunction = &ExitConfirm;
}

// ------------------------ CInGameMenu implementation
// start load/save menus depending on type of game running
static void QuickSaveFromMenu()
{
  _pShell->SetINDEX("gam_bQuickSave", 2); // force save with reporting
  StopMenus(TRUE);
}

static void StopRecordingDemo(void)
{
  _pNetwork->StopDemoRec();
  void SetDemoStartStopRecText(void);
  SetDemoStartStopRecText();
}

void InitActionsForInGameMenu()
{
  CInGameMenu &gmCurrent = _pGUIM->gmInGameMenu;

  gmCurrent.gm_mgQuickLoad.mg_pActivatedFunction = &StartCurrentQuickLoadMenu;
  gmCurrent.gm_mgQuickSave.mg_pActivatedFunction = &QuickSaveFromMenu;
  gmCurrent.gm_mgLoad.mg_pActivatedFunction = &StartCurrentLoadMenu;
  gmCurrent.gm_mgSave.mg_pActivatedFunction = &StartCurrentSaveMenu;
  gmCurrent.gm_mgHighScore.mg_pActivatedFunction = &StartHighScoreMenu;
  gmCurrent.gm_mgOptions.mg_pActivatedFunction = &StartOptionsMenu;
  gmCurrent.gm_mgStop.mg_pActivatedFunction = &StopConfirm;
  gmCurrent.gm_mgQuit.mg_pActivatedFunction = &ExitConfirm;
}

extern void SetDemoStartStopRecText(void)
{
  CInGameMenu &gmCurrent = _pGUIM->gmInGameMenu;

  if (_pNetwork->IsRecordingDemo())
  {
    gmCurrent.gm_mgDemoRec.SetText(TRANS("STOP RECORDING"));
    gmCurrent.gm_mgDemoRec.mg_strTip = TRANS("stop current recording");
    gmCurrent.gm_mgDemoRec.mg_pActivatedFunction = &StopRecordingDemo;
  } else {
    gmCurrent.gm_mgDemoRec.SetText(TRANS("RECORD DEMO"));
    gmCurrent.gm_mgDemoRec.mg_strTip = TRANS("start recording current game");
    gmCurrent.gm_mgDemoRec.mg_pActivatedFunction = &StartDemoSaveMenu;
  }
}

// ------------------------ CSinglePlayerMenu implementation
extern CTString sam_strTechTestLevel;
extern CTString sam_strTrainingLevel;

static void StartSinglePlayerGame_Normal(void);
static void StartTechTest(void)
{
  _pGUIM->gmSinglePlayerNewMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
  _pGame->gam_strCustomLevel = sam_strTechTestLevel;
  StartSinglePlayerGame_Normal();
}

static void StartTraining(void)
{
  _pGUIM->gmSinglePlayerNewMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
  _pGame->gam_strCustomLevel = sam_strTrainingLevel;
  ChangeToMenu(&_pGUIM->gmSinglePlayerNewMenu);
}

void InitActionsForSinglePlayerMenu()
{
  CSinglePlayerMenu &gmCurrent = _pGUIM->gmSinglePlayerMenu;

  gmCurrent.gm_mgNewGame.mg_pActivatedFunction = &StartSinglePlayerNewMenu;
  gmCurrent.gm_mgCustom.mg_pActivatedFunction = &StartSelectLevelFromSingle;
  gmCurrent.gm_mgQuickLoad.mg_pActivatedFunction = &StartSinglePlayerQuickLoadMenu;
  gmCurrent.gm_mgLoad.mg_pActivatedFunction = &StartSinglePlayerLoadMenu;
  gmCurrent.gm_mgTraining.mg_pActivatedFunction = &StartTraining;
  gmCurrent.gm_mgTechTest.mg_pActivatedFunction = &StartTechTest;
  gmCurrent.gm_mgPlayersAndControls.mg_pActivatedFunction = &StartChangePlayerMenuFromSinglePlayer;
  gmCurrent.gm_mgOptions.mg_pActivatedFunction = &StartSinglePlayerGameOptions;
}

// ------------------------ CSinglePlayerNewMenu implementation
void StartSinglePlayerGame(void)
{
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
  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

static void StartSinglePlayerGame_Tourist(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_TOURIST);
  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
  StartSinglePlayerGame();
}

static void StartSinglePlayerGame_Easy(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_EASY);
  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
  StartSinglePlayerGame();
}

static void StartSinglePlayerGame_Normal(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_NORMAL);
  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
  StartSinglePlayerGame();
}

static void StartSinglePlayerGame_Hard(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_HARD);
  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
  StartSinglePlayerGame();
}

static void StartSinglePlayerGame_Serious(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_EXTREME);
  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
  StartSinglePlayerGame();
}

static void StartSinglePlayerGame_Mental(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_EXTREME + 1);
  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
  StartSinglePlayerGame();
}

void InitActionsForSinglePlayerNewMenu() {
  CSinglePlayerNewMenu &gmCurrent = _pGUIM->gmSinglePlayerNewMenu;

  gmCurrent.gm_mgTourist.mg_pActivatedFunction = &StartSinglePlayerGame_Tourist;
  gmCurrent.gm_mgEasy.mg_pActivatedFunction = &StartSinglePlayerGame_Easy;
  gmCurrent.gm_mgMedium.mg_pActivatedFunction = &StartSinglePlayerGame_Normal;
  gmCurrent.gm_mgHard.mg_pActivatedFunction = &StartSinglePlayerGame_Hard;
  gmCurrent.gm_mgSerious.mg_pActivatedFunction = &StartSinglePlayerGame_Serious;
  gmCurrent.gm_mgMental.mg_pActivatedFunction = &StartSinglePlayerGame_Mental;
}

// ------------------------ CPlayerProfileMenu implementation
static void ChangeCrosshair(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  pps->ps_iCrossHairType = iNew - 1;
}

static void ChangeWeaponSelect(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  pps->ps_iWeaponAutoSelect = iNew;
}

static void ChangeWeaponHide(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_HIDEWEAPON;
  } else {
    pps->ps_ulFlags &= ~PSF_HIDEWEAPON;
  }
}

static void Change3rdPerson(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_PREFER3RDPERSON;
  } else {
    pps->ps_ulFlags &= ~PSF_PREFER3RDPERSON;
  }
}

static void ChangeQuotes(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_NOQUOTES;
  } else {
    pps->ps_ulFlags |= PSF_NOQUOTES;
  }
}

static void ChangeAutoSave(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_AUTOSAVE;
  } else {
    pps->ps_ulFlags &= ~PSF_AUTOSAVE;
  }
}

static void ChangeCompDoubleClick(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_COMPSINGLECLICK;
  } else {
    pps->ps_ulFlags |= PSF_COMPSINGLECLICK;
  }
}

static void ChangeViewBobbing(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_NOBOBBING;
  } else {
    pps->ps_ulFlags |= PSF_NOBOBBING;
  }
}

static void ChangeSharpTurning(INDEX iNew)
{
  INDEX iPlayer = *_pGUIM->gmPlayerProfile.gm_piCurrentPlayer;
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
  CPlayerProfileMenu &gmCurrent = _pGUIM->gmPlayerProfile;

  gmCurrent.gm_mgCrosshair.mg_pOnTriggerChange = ChangeCrosshair;
  gmCurrent.gm_mgWeaponSelect.mg_pOnTriggerChange = ChangeWeaponSelect;
  gmCurrent.gm_mgWeaponHide.mg_pOnTriggerChange = ChangeWeaponHide;
  gmCurrent.gm_mg3rdPerson.mg_pOnTriggerChange = Change3rdPerson;
  gmCurrent.gm_mgQuotes.mg_pOnTriggerChange = ChangeQuotes;
  gmCurrent.gm_mgAutoSave.mg_pOnTriggerChange = ChangeAutoSave;
  gmCurrent.gm_mgCompDoubleClick.mg_pOnTriggerChange = ChangeCompDoubleClick;
  gmCurrent.gm_mgSharpTurning.mg_pOnTriggerChange = ChangeSharpTurning;
  gmCurrent.gm_mgViewBobbing.mg_pOnTriggerChange = ChangeViewBobbing;
  gmCurrent.gm_mgCustomizeControls.mg_pActivatedFunction = &StartControlsMenuFromPlayer;
  gmCurrent.gm_mgModel.mg_pActivatedFunction = &StartPlayerModelLoadMenu;
}

// ------------------------ CControlsMenu implementation
void InitActionsForControlsMenu()
{
  CControlsMenu &gmCurrent = _pGUIM->gmControls;

  gmCurrent.gm_mgButtons.mg_pActivatedFunction = &StartCustomizeKeyboardMenu;
  gmCurrent.gm_mgAdvanced.mg_pActivatedFunction = &StartCustomizeAxisMenu;
  gmCurrent.gm_mgPredefined.mg_pActivatedFunction = &StartControlsLoadMenu;
}

// ------------------------ CCustomizeAxisMenu implementation
void PreChangeAxis(INDEX iDummy)
{
  _pGUIM->gmCustomizeAxisMenu.ApplyActionSettings();
}

void PostChangeAxis(INDEX iDummy)
{
  _pGUIM->gmCustomizeAxisMenu.ObtainActionSettings();
}

void InitActionsForCustomizeAxisMenu()
{
  CCustomizeAxisMenu &gmCurrent = _pGUIM->gmCustomizeAxisMenu;

  gmCurrent.gm_mgActionTrigger.mg_pPreTriggerChange = PreChangeAxis;
  gmCurrent.gm_mgActionTrigger.mg_pOnTriggerChange = PostChangeAxis;
}

// ------------------------ COptionsMenu implementation
void InitActionsForOptionsMenu()
{
  COptionsMenu &gmCurrent = _pGUIM->gmOptionsMenu;

  gmCurrent.gm_mgVideoOptions.mg_pActivatedFunction = &StartVideoOptionsMenu;
  gmCurrent.gm_mgAudioOptions.mg_pActivatedFunction = &StartAudioOptionsMenu;
  gmCurrent.gm_mgPlayerProfileOptions.mg_pActivatedFunction = &StartChangePlayerMenuFromOptions;
  gmCurrent.gm_mgNetworkOptions.mg_pActivatedFunction = &StartNetworkSettingsMenu;
  gmCurrent.gm_mgCustomOptions.mg_pActivatedFunction = &StartCustomLoadMenu;
  gmCurrent.gm_mgAddonOptions.mg_pActivatedFunction = &StartAddonsLoadMenu;
}

// ------------------------ CVideoOptionsMenu implementation
static INDEX sam_old_bFullScreenActive;
static INDEX sam_old_iScreenSizeI;
static INDEX sam_old_iScreenSizeJ;
static INDEX sam_old_iDisplayDepth;
static INDEX sam_old_iDisplayAdapter;
static INDEX sam_old_iGfxAPI;
static INDEX sam_old_iVideoSetup;  // 0==speed, 1==normal, 2==quality, 3==custom

static void FillResolutionsList(void)
{
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  // free resolutions
  if (_astrResolutionTexts != NULL) {
    delete[] _astrResolutionTexts;
  }
  if (_admResolutionModes != NULL) {
    delete[] _admResolutionModes;
  }
  _ctResolutions = 0;

  // if window
  if (gmCurrent.gm_mgFullScreenTrigger.mg_iSelected == 0) {
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
      SwitchToAPI(gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected), gmCurrent.gm_mgDisplayAdaptersTrigger.mg_iSelected);
    // allocate that much
    _astrResolutionTexts = new CTString[_ctResolutions];
    _admResolutionModes = new CDisplayMode[_ctResolutions];
    // for each resolution
    for (INDEX iRes = 0; iRes<_ctResolutions; iRes++) {
      // add it to list
      SetResolutionInList(iRes, pdm[iRes].dm_pixSizeI, pdm[iRes].dm_pixSizeJ);
    }
  }

  gmCurrent.gm_mgResolutionsTrigger.mg_astrTexts = _astrResolutionTexts;
  gmCurrent.gm_mgResolutionsTrigger.mg_ctTexts = _ctResolutions;
}

static void FillAdaptersList(void)
{
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  if (_astrAdapterTexts != NULL) {
    delete[] _astrAdapterTexts;
  }

  _ctAdapters = 0;

  INDEX iApi = SwitchToAPI(gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected);
  _ctAdapters = _pGfx->gl_gaAPI[iApi].ga_ctAdapters;
  _astrAdapterTexts = new CTString[_ctAdapters];
  for (INDEX iAdapter = 0; iAdapter<_ctAdapters; iAdapter++) {
    _astrAdapterTexts[iAdapter] = _pGfx->gl_gaAPI[iApi].ga_adaAdapter[iAdapter].da_strRenderer;
  }

  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_astrTexts = _astrAdapterTexts;
  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_ctTexts = _ctAdapters;
}

extern void UpdateVideoOptionsButtons(INDEX iSelected)
{
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  const BOOL _bVideoOptionsChanged = (iSelected != -1);

  const BOOL bOGLEnabled = _pGfx->HasAPI(GAT_OGL);
#ifdef SE1_D3D
  const BOOL bD3DEnabled = _pGfx->HasAPI(GAT_D3D);
  ASSERT(bOGLEnabled || bD3DEnabled);
#else // 
  ASSERT(bOGLEnabled);
#endif // SE1_D3D

  CDisplayAdapter &da = _pGfx->gl_gaAPI[SwitchToAPI(gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected)]
    .ga_adaAdapter[gmCurrent.gm_mgDisplayAdaptersTrigger.mg_iSelected];

  // number of available preferences is higher if video setup is custom
  gmCurrent.gm_mgDisplayPrefsTrigger.mg_ctTexts = 3;
  if (sam_iVideoSetup == 3) gmCurrent.gm_mgDisplayPrefsTrigger.mg_ctTexts++;

  // enumerate adapters
  FillAdaptersList();

  // show or hide buttons
  gmCurrent.gm_mgDisplayAPITrigger.mg_bEnabled = bOGLEnabled
#ifdef SE1_D3D
    && bD3DEnabled
#endif // SE1_D3D
    ;
  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_bEnabled = _ctAdapters>1;
  gmCurrent.gm_mgApply.mg_bEnabled = _bVideoOptionsChanged;
  // determine which should be visible

  gmCurrent.gm_mgFullScreenTrigger.mg_bEnabled = TRUE;
  if (da.da_ulFlags&DAF_FULLSCREENONLY) {
    gmCurrent.gm_mgFullScreenTrigger.mg_bEnabled = FALSE;
    gmCurrent.gm_mgFullScreenTrigger.mg_iSelected = 1;
    gmCurrent.gm_mgFullScreenTrigger.ApplyCurrentSelection();
  }

  gmCurrent.gm_mgBitsPerPixelTrigger.mg_bEnabled = TRUE;
  if (gmCurrent.gm_mgFullScreenTrigger.mg_iSelected == 0) {
    gmCurrent.gm_mgBitsPerPixelTrigger.mg_bEnabled = FALSE;
    gmCurrent.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch(DD_DEFAULT);
    gmCurrent.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
  } else if (da.da_ulFlags&DAF_16BITONLY) {
    gmCurrent.gm_mgBitsPerPixelTrigger.mg_bEnabled = FALSE;
    gmCurrent.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch(DD_16BIT);
    gmCurrent.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
  }

  // remember current selected resolution
  PIX pixSizeI, pixSizeJ;
  ResolutionToSize(gmCurrent.gm_mgResolutionsTrigger.mg_iSelected, pixSizeI, pixSizeJ);

  // select same resolution again if possible
  FillResolutionsList();
  SizeToResolution(pixSizeI, pixSizeJ, gmCurrent.gm_mgResolutionsTrigger.mg_iSelected);

  // apply adapter and resolutions
  gmCurrent.gm_mgDisplayAdaptersTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgResolutionsTrigger.ApplyCurrentSelection();
}

extern void InitVideoOptionsButtons(void)
{
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  if (sam_bFullScreenActive) {
    gmCurrent.gm_mgFullScreenTrigger.mg_iSelected = 1;
  } else {
    gmCurrent.gm_mgFullScreenTrigger.mg_iSelected = 0;
  }

  gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected = APIToSwitch((GfxAPIType)(INDEX)sam_iGfxAPI);
  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_iSelected = sam_iDisplayAdapter;
  gmCurrent.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch((enum DisplayDepth)(INDEX)sam_iDisplayDepth);

  FillResolutionsList();
  SizeToResolution(sam_iScreenSizeI, sam_iScreenSizeJ, gmCurrent.gm_mgResolutionsTrigger.mg_iSelected);
  gmCurrent.gm_mgDisplayPrefsTrigger.mg_iSelected = Clamp(int(sam_iVideoSetup), 0, 3);

  gmCurrent.gm_mgFullScreenTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgDisplayPrefsTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgDisplayAPITrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgDisplayAdaptersTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgResolutionsTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
}

static void ApplyVideoOptions(void)
{
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  // Remember old video settings
  sam_old_bFullScreenActive = sam_bFullScreenActive;
  sam_old_iScreenSizeI = sam_iScreenSizeI;
  sam_old_iScreenSizeJ = sam_iScreenSizeJ;
  sam_old_iDisplayDepth = sam_iDisplayDepth;
  sam_old_iDisplayAdapter = sam_iDisplayAdapter;
  sam_old_iGfxAPI = sam_iGfxAPI;
  sam_old_iVideoSetup = sam_iVideoSetup;

  BOOL bFullScreenMode = gmCurrent.gm_mgFullScreenTrigger.mg_iSelected == 1;
  PIX pixWindowSizeI, pixWindowSizeJ;
  ResolutionToSize(gmCurrent.gm_mgResolutionsTrigger.mg_iSelected, pixWindowSizeI, pixWindowSizeJ);
  enum GfxAPIType gat = SwitchToAPI(gmCurrent.gm_mgDisplayAPITrigger.mg_iSelected);
  enum DisplayDepth dd = SwitchToDepth(gmCurrent.gm_mgBitsPerPixelTrigger.mg_iSelected);
  const INDEX iAdapter = gmCurrent.gm_mgDisplayAdaptersTrigger.mg_iSelected;

  // setup preferences
  extern INDEX _iLastPreferences;
  if (sam_iVideoSetup == 3) _iLastPreferences = 3;
  sam_iVideoSetup = gmCurrent.gm_mgDisplayPrefsTrigger.mg_iSelected;

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

static void RevertVideoSettings(void)
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
  CVideoOptionsMenu &gmCurrent = _pGUIM->gmVideoOptionsMenu;

  gmCurrent.gm_mgDisplayPrefsTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgDisplayAPITrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgDisplayAdaptersTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgFullScreenTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgResolutionsTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgBitsPerPixelTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
  gmCurrent.gm_mgVideoRendering.mg_pActivatedFunction = &StartRenderingOptionsMenu;
  gmCurrent.gm_mgApply.mg_pActivatedFunction = &ApplyVideoOptions;
}

// ------------------------ CAudioOptionsMenu implementation
extern void RefreshSoundFormat(void)
{
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  switch (_pSound->GetFormat())
  {
    case CSoundLibrary::SF_NONE:     {gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 0; break; }
    case CSoundLibrary::SF_11025_16: {gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 1; break; }
    case CSoundLibrary::SF_22050_16: {gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 2; break; }
    case CSoundLibrary::SF_44100_16: {gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 3; break; }
    default:                          gmCurrent.gm_mgFrequencyTrigger.mg_iSelected = 0;
  }

  gmCurrent.gm_mgAudioAutoTrigger.mg_iSelected = Clamp(sam_bAutoAdjustAudio, 0, 1);
  gmCurrent.gm_mgAudioAPITrigger.mg_iSelected = Clamp(_pShell->GetINDEX("snd_iInterface"), 0L, 2L);

  gmCurrent.gm_mgWaveVolume.mg_iMinPos = 0;
  gmCurrent.gm_mgWaveVolume.mg_iMaxPos = VOLUME_STEPS;
  gmCurrent.gm_mgWaveVolume.mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fSoundVolume")*VOLUME_STEPS + 0.5f);
  gmCurrent.gm_mgWaveVolume.ApplyCurrentPosition();

  gmCurrent.gm_mgMPEGVolume.mg_iMinPos = 0;
  gmCurrent.gm_mgMPEGVolume.mg_iMaxPos = VOLUME_STEPS;
  gmCurrent.gm_mgMPEGVolume.mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fMusicVolume")*VOLUME_STEPS + 0.5f);
  gmCurrent.gm_mgMPEGVolume.ApplyCurrentPosition();

  gmCurrent.gm_mgAudioAutoTrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgAudioAPITrigger.ApplyCurrentSelection();
  gmCurrent.gm_mgFrequencyTrigger.ApplyCurrentSelection();
}

static void ApplyAudioOptions(void)
{
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  sam_bAutoAdjustAudio = gmCurrent.gm_mgAudioAutoTrigger.mg_iSelected;
  if (sam_bAutoAdjustAudio) {
    _pShell->Execute("include \"Scripts\\Addons\\SFX-AutoAdjust.ini\"");
  } else {
    _pShell->SetINDEX("snd_iInterface", gmCurrent.gm_mgAudioAPITrigger.mg_iSelected);

    switch (gmCurrent.gm_mgFrequencyTrigger.mg_iSelected)
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

static void WaveSliderChange(void)
{
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  gmCurrent.gm_mgWaveVolume.mg_iCurPos -= 5;
  gmCurrent.gm_mgWaveVolume.ApplyCurrentPosition();
}

static void FrequencyTriggerChange(INDEX iDummy)
{
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  sam_bAutoAdjustAudio = 0;
  gmCurrent.gm_mgAudioAutoTrigger.mg_iSelected = 0;
  gmCurrent.gm_mgAudioAutoTrigger.ApplyCurrentSelection();
}

static void MPEGSliderChange(void)
{
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  gmCurrent.gm_mgMPEGVolume.mg_iCurPos -= 5;
  gmCurrent.gm_mgMPEGVolume.ApplyCurrentPosition();
}

static void OnMPEGVolumeChange(INDEX iCurPos)
{
  _pShell->SetFLOAT("snd_fMusicVolume", iCurPos / FLOAT(VOLUME_STEPS));
}

void InitActionsForAudioOptionsMenu()
{
  CAudioOptionsMenu &gmCurrent = _pGUIM->gmAudioOptionsMenu;

  gmCurrent.gm_mgFrequencyTrigger.mg_pOnTriggerChange = FrequencyTriggerChange;
  gmCurrent.gm_mgWaveVolume.mg_pOnSliderChange = &OnWaveVolumeChange;
  gmCurrent.gm_mgWaveVolume.mg_pActivatedFunction = WaveSliderChange;
  gmCurrent.gm_mgMPEGVolume.mg_pOnSliderChange = &OnMPEGVolumeChange;
  gmCurrent.gm_mgMPEGVolume.mg_pActivatedFunction = MPEGSliderChange;
  gmCurrent.gm_mgApply.mg_pActivatedFunction = &ApplyAudioOptions;
}

// ------------------------ CVarMenu implementation
static void VarApply(void)
{
  CVarMenu &gmCurrent = _pGUIM->gmVarMenu;

  FlushVarSettings(TRUE);
  gmCurrent.EndMenu();
  gmCurrent.StartMenu();
}

void InitActionsForVarMenu() {
  _pGUIM->gmVarMenu.gm_mgApply.mg_pActivatedFunction = &VarApply;
}

// ------------------------ CServersMenu implementation

extern CMGButton mgServerColumn[7];
extern CMGEdit mgServerFilter[7];

static void SortByColumn(int i)
{
  CServersMenu &gmCurrent = _pGUIM->gmServersMenu;

  if (gmCurrent.gm_mgList.mg_iSort == i) {
    gmCurrent.gm_mgList.mg_bSortDown = !gmCurrent.gm_mgList.mg_bSortDown;
  } else {
    gmCurrent.gm_mgList.mg_bSortDown = FALSE;
  }
  gmCurrent.gm_mgList.mg_iSort = i;
}

static void SortByServer(void) { SortByColumn(0); }
static void SortByMap(void)    { SortByColumn(1); }
static void SortByPing(void)   { SortByColumn(2); }
static void SortByPlayers(void){ SortByColumn(3); }
static void SortByGame(void)   { SortByColumn(4); }
static void SortByMod(void)    { SortByColumn(5); }
static void SortByVer(void)    { SortByColumn(6); }

extern void RefreshServerList(void)
{
  _pNetwork->EnumSessions(_pGUIM->gmServersMenu.m_bInternet);
}

void RefreshServerListManually(void)
{
  ChangeToMenu(&_pGUIM->gmServersMenu); // this refreshes the list and sets focuses
}

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
void InitActionsForNetworkMenu()
{
  CNetworkMenu &gmCurrent = _pGUIM->gmNetworkMenu;

  gmCurrent.gm_mgJoin.mg_pActivatedFunction = &StartNetworkJoinMenu;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartNetworkStartMenu;
  gmCurrent.gm_mgQuickLoad.mg_pActivatedFunction = &StartNetworkQuickLoadMenu;
  gmCurrent.gm_mgLoad.mg_pActivatedFunction = &StartNetworkLoadMenu;
}

// ------------------------ CNetworkJoinMenu implementation
void InitActionsForNetworkJoinMenu()
{
  CNetworkJoinMenu &gmCurrent = _pGUIM->gmNetworkJoinMenu;

  gmCurrent.gm_mgLAN.mg_pActivatedFunction = &StartSelectServerLAN;
  gmCurrent.gm_mgNET.mg_pActivatedFunction = &StartSelectServerNET;
  gmCurrent.gm_mgOpen.mg_pActivatedFunction = &StartNetworkOpenMenu;
}

// ------------------------ CNetworkStartMenu implementation
extern void UpdateNetworkLevel(INDEX iDummy)
{
  ValidateLevelForFlags(_pGame->gam_strCustomLevel,
    GetSpawnFlagsForGameType(_pGUIM->gmNetworkStartMenu.gm_mgGameType.mg_iSelected));
  _pGUIM->gmNetworkStartMenu.gm_mgLevel.mg_strText = FindLevelByFileName(_pGame->gam_strCustomLevel).li_strName;
}

void InitActionsForNetworkStartMenu()
{
  CNetworkStartMenu &gmCurrent = _pGUIM->gmNetworkStartMenu;

  gmCurrent.gm_mgLevel.mg_pActivatedFunction = &StartSelectLevelFromNetwork;
  gmCurrent.gm_mgGameOptions.mg_pActivatedFunction = &StartGameOptionsFromNetwork;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromNetwork;
}

// ------------------------ CSelectPlayersMenu implementation
static INDEX FindUnusedPlayer(void)
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
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  INDEX *ai = _pGame->gm_aiMenuLocalPlayers;

  gmCurrent.gm_mgPlayer0Change.mg_iLocalPlayer = 0;
  gmCurrent.gm_mgPlayer1Change.mg_iLocalPlayer = 1;
  gmCurrent.gm_mgPlayer2Change.mg_iLocalPlayer = 2;
  gmCurrent.gm_mgPlayer3Change.mg_iLocalPlayer = 3;

  if (gmCurrent.gm_bAllowDedicated && _pGame->gm_MenuSplitScreenCfg == CGame::SSC_DEDICATED) {
    gmCurrent.gm_mgDedicated.mg_iSelected = 1;
  } else {
    gmCurrent.gm_mgDedicated.mg_iSelected = 0;
  }

  gmCurrent.gm_mgDedicated.ApplyCurrentSelection();

  if (gmCurrent.gm_bAllowObserving && _pGame->gm_MenuSplitScreenCfg == CGame::SSC_OBSERVER) {
    gmCurrent.gm_mgObserver.mg_iSelected = 1;
  } else {
    gmCurrent.gm_mgObserver.mg_iSelected = 0;
  }

  gmCurrent.gm_mgObserver.ApplyCurrentSelection();

  if (_pGame->gm_MenuSplitScreenCfg >= CGame::SSC_PLAY1) {
    gmCurrent.gm_mgSplitScreenCfg.mg_iSelected = _pGame->gm_MenuSplitScreenCfg;
    gmCurrent.gm_mgSplitScreenCfg.ApplyCurrentSelection();
  }

  BOOL bHasDedicated = gmCurrent.gm_bAllowDedicated;
  BOOL bHasObserver = gmCurrent.gm_bAllowObserving;
  BOOL bHasPlayers = TRUE;

  if (bHasDedicated && gmCurrent.gm_mgDedicated.mg_iSelected) {
    bHasObserver = FALSE;
    bHasPlayers = FALSE;
  }

  if (bHasObserver && gmCurrent.gm_mgObserver.mg_iSelected) {
    bHasPlayers = FALSE;
  }

  CMenuGadget *apmg[8];
  memset(apmg, 0, sizeof(apmg));
  INDEX i = 0;

  if (bHasDedicated) {
    gmCurrent.gm_mgDedicated.Appear();
    apmg[i++] = &gmCurrent.gm_mgDedicated;
  } else {
    gmCurrent.gm_mgDedicated.Disappear();
  }

  if (bHasObserver) {
    gmCurrent.gm_mgObserver.Appear();
    apmg[i++] = &gmCurrent.gm_mgObserver;
  } else {
    gmCurrent.gm_mgObserver.Disappear();
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

  gmCurrent.gm_mgPlayer0Change.Disappear();
  gmCurrent.gm_mgPlayer1Change.Disappear();
  gmCurrent.gm_mgPlayer2Change.Disappear();
  gmCurrent.gm_mgPlayer3Change.Disappear();

  if (bHasPlayers) {
    gmCurrent.gm_mgSplitScreenCfg.Appear();
    apmg[i++] = &gmCurrent.gm_mgSplitScreenCfg;
    gmCurrent.gm_mgPlayer0Change.Appear();
    apmg[i++] = &gmCurrent.gm_mgPlayer0Change;
    if (gmCurrent.gm_mgSplitScreenCfg.mg_iSelected >= 1) {
      gmCurrent.gm_mgPlayer1Change.Appear();
      apmg[i++] = &gmCurrent.gm_mgPlayer1Change;
    }
    if (gmCurrent.gm_mgSplitScreenCfg.mg_iSelected >= 2) {
      gmCurrent.gm_mgPlayer2Change.Appear();
      apmg[i++] = &gmCurrent.gm_mgPlayer2Change;
    }
    if (gmCurrent.gm_mgSplitScreenCfg.mg_iSelected >= 3) {
      gmCurrent.gm_mgPlayer3Change.Appear();
      apmg[i++] = &gmCurrent.gm_mgPlayer3Change;
    }
  } else {
    gmCurrent.gm_mgSplitScreenCfg.Disappear();
  }
  apmg[i++] = &gmCurrent.gm_mgStart;

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

  gmCurrent.gm_mgPlayer0Change.SetPlayerText();
  gmCurrent.gm_mgPlayer1Change.SetPlayerText();
  gmCurrent.gm_mgPlayer2Change.SetPlayerText();
  gmCurrent.gm_mgPlayer3Change.SetPlayerText();

  if (bHasPlayers && gmCurrent.gm_mgSplitScreenCfg.mg_iSelected >= 1) {
    gmCurrent.gm_mgNotes.mg_strText = TRANS("Make sure you set different controls for each player!");
  } else {
    gmCurrent.gm_mgNotes.mg_strText = "";
  }
}

extern void SelectPlayersApplyMenu(void)
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  if (gmCurrent.gm_bAllowDedicated && gmCurrent.gm_mgDedicated.mg_iSelected) {
    _pGame->gm_MenuSplitScreenCfg = CGame::SSC_DEDICATED;
    return;
  }

  if (gmCurrent.gm_bAllowObserving && gmCurrent.gm_mgObserver.mg_iSelected) {
    _pGame->gm_MenuSplitScreenCfg = CGame::SSC_OBSERVER;
    return;
  }

  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) gmCurrent.gm_mgSplitScreenCfg.mg_iSelected;
}

static void UpdateSelectPlayers(INDEX i)
{
  SelectPlayersApplyMenu();
  SelectPlayersFillMenu();
}

void InitActionsForSelectPlayersMenu()
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  gmCurrent.gm_mgDedicated.mg_pOnTriggerChange = UpdateSelectPlayers;
  gmCurrent.gm_mgObserver.mg_pOnTriggerChange = UpdateSelectPlayers;
  gmCurrent.gm_mgSplitScreenCfg.mg_pOnTriggerChange = UpdateSelectPlayers;
}

// ------------------------ CNetworkOpenMenu implementation
void InitActionsForNetworkOpenMenu()
{
  _pGUIM->gmNetworkOpenMenu.gm_mgJoin.mg_pActivatedFunction = &StartSelectPlayersMenuFromOpen;
}

// ------------------------ CSplitScreenMenu implementation
void InitActionsForSplitScreenMenu()
{
  CSplitScreenMenu &gmCurrent = _pGUIM->gmSplitScreenMenu;

  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSplitStartMenu;
  gmCurrent.gm_mgQuickLoad.mg_pActivatedFunction = &StartSplitScreenQuickLoadMenu;
  gmCurrent.gm_mgLoad.mg_pActivatedFunction = &StartSplitScreenLoadMenu;
}

// ------------------------ CSplitStartMenu implementation
void InitActionsForSplitStartMenu()
{
  CSplitStartMenu &gmCurrent = _pGUIM->gmSplitStartMenu;

  gmCurrent.gm_mgLevel.mg_pActivatedFunction = &StartSelectLevelFromSplit;
  gmCurrent.gm_mgOptions.mg_pActivatedFunction = &StartGameOptionsFromSplitScreen;
  gmCurrent.gm_mgStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromSplit;
}

extern void UpdateSplitLevel(INDEX iDummy)
{
  CSplitStartMenu &gmCurrent = _pGUIM->gmSplitStartMenu;

  ValidateLevelForFlags(_pGame->gam_strCustomLevel,
    GetSpawnFlagsForGameType(gmCurrent.gm_mgGameType.mg_iSelected));
  gmCurrent.gm_mgLevel.mg_strText = FindLevelByFileName(_pGame->gam_strCustomLevel).li_strName;
}