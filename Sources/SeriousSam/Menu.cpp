/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Build.h>
#include <sys/timeb.h>
#include <time.h>
#include <io.h>
#include "MainWindow.h"
#include <Engine/CurrentVersion.h>
#include <Engine/Templates/Stock_CSoundData.h>
#include <GameMP/LCDDrawing.h>
#include "MenuPrinting.h"
#include "LevelInfo.h"
#include "VarList.h"
#include "FileInfo.h"

#include "MGArrow.h"
#include "MGButton.h"
#include "MGChangePlayer.h"
#include "MGEdit.h"
#include "MGFileButton.h"
#include "MGHighScore.h"
#include "MGKeyDefinition.h"
#include "MGLevelButton.h"
#include "MGModel.h"
#include "MGServerList.h"
#include "MGSlider.h"
#include "MGTitle.h"
#include "MGTrigger.h"
#include "MGVarButton.h"

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

#include "MenuStuff.h"

// macros for translating radio button text arrays
#define TRANSLATERADIOARRAY(array) TranslateRadioTexts(array, ARRAYCOUNT(array))

extern CMenuGadget *_pmgLastActivatedGadget;
extern BOOL bMenuActive;
extern BOOL bMenuRendering;
extern CTextureObject *_ptoLogoCT;
extern CTextureObject *_ptoLogoODI;
extern CTextureObject *_ptoLogoEAX;

INDEX _iLocalPlayer = -1;
extern BOOL  _bPlayerMenuFromSinglePlayer = FALSE;


GameMode _gmMenuGameMode = GM_NONE;
GameMode _gmRunningGameMode = GM_NONE;
CListHead _lhServers;

static INDEX sam_old_bFullScreenActive;
static INDEX sam_old_iScreenSizeI;
static INDEX sam_old_iScreenSizeJ;
static INDEX sam_old_iDisplayDepth;
static INDEX sam_old_iDisplayAdapter;
static INDEX sam_old_iGfxAPI;
static INDEX sam_old_iVideoSetup;  // 0==speed, 1==normal, 2==quality, 3==custom

ENGINE_API extern INDEX snd_iFormat;

extern BOOL IsCDInDrive(void);

void OnPlayerSelect(void);

// last tick done
TIME _tmMenuLastTickDone = -1;
// all possible menu entities
CListHead lhMenuEntities;
// controls that are currently customized
CTFileName _fnmControlsToCustomize = CTString("");

extern CTString _strLastPlayerAppearance = "";
extern CTString sam_strNetworkSettings;

// function to activate when level is chosen
void (*_pAfterLevelChosen)(void);

// functions for init actions
void InitActionsForAudioOptionsMenu();
void InitActionsForConfirmMenu();
void InitActionsForControlsMenu();
void InitActionsForCustomizeAxisMenu();
void InitActionsForMainMenu();
void InitActionsForInGameMenu();
void InitActionsForNetworkMenu();
void InitActionsForNetworkJoinMenu();
void InitActionsForNetworkOpenMenu();
void InitActionsForNetworkStartMenu();
void InitActionsForOptionsMenu();
void InitActionsForPlayerProfileMenu();
void InitActionsForSelectPlayersMenu();
void InitActionsForServersMenu();
void InitActionsForSinglePlayerMenu();
void InitActionsForSinglePlayerNewMenu();
void InitActionsForSplitScreenMenu();
void InitActionsForSplitStartMenu();
void InitActionsForVideoOptionsMenu();
void InitActionsForVarMenu();

// functions to activate when user chose 'yes/no' on confirmation
void (*_pConfimedYes)(void) = NULL;
void (*_pConfimedNo)(void) = NULL;

void FixupBackButton(CGameMenu *pgm);

void ConfirmYes(void)
{
  if (_pConfimedYes!=NULL) {
    _pConfimedYes();
  }
  void MenuGoToParent(void);
  MenuGoToParent();
}
void ConfirmNo(void)
{
  if (_pConfimedNo!=NULL) {
    _pConfimedNo();
  }
  void MenuGoToParent(void);
  MenuGoToParent();
}


extern void ControlsMenuOn()
{
  _pGame->SavePlayersAndControls();
  try {
    _pGame->gm_ctrlControlsExtra.Load_t(_fnmControlsToCustomize);
  } catch( char *strError) {
    WarningMessage(strError);
  }
}

extern void ControlsMenuOff()
{
  try {
    if (_pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions.Count()>0) {
      _pGame->gm_ctrlControlsExtra.Save_t(_fnmControlsToCustomize);
    }
  } catch( char *strError) {
    FatalError(strError);
  }
  FORDELETELIST( CButtonAction, ba_lnNode, _pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions, itAct) {
    delete &itAct.Current();
  }
  _pGame->LoadPlayersAndControls();
}

// mouse cursor position
extern PIX _pixCursorPosI = 0;
extern PIX _pixCursorPosJ = 0;
extern PIX _pixCursorExternPosI = 0;
extern PIX _pixCursorExternPosJ = 0;
extern BOOL _bMouseUsedLast = FALSE;
extern CMenuGadget *_pmgUnderCursor =  NULL;
extern BOOL _bDefiningKey;
extern BOOL _bEditingString;
extern BOOL _bMouseRight = FALSE;

// thumbnail for showing in menu
CTextureObject _toThumbnail;
BOOL _bThumbnailOn = FALSE;

CFontData _fdBig;
CFontData _fdMedium;
CFontData _fdSmall;
CFontData _fdTitle;


CSoundData *_psdSelect = NULL;
CSoundData *_psdPress = NULL;
CSoundObject *_psoMenuSound = NULL;

static CTextureObject _toPointer;
static CTextureObject _toLogoMenuA;
static CTextureObject _toLogoMenuB;

// -------------- All possible menu entities
#define BIG_BUTTONS_CT 6

#define CHANGETRIGGERARRAY(ltbmg, astr) \
  ltbmg.mg_astrTexts = astr;\
  ltbmg.mg_ctTexts = sizeof( astr)/sizeof( astr[0]);\
  ltbmg.mg_iSelected = 0;\
  ltbmg.mg_strText = astr[ltbmg.mg_iSelected];

#define PLACEMENT(x,y,z) CPlacement3D( FLOAT3D( x, y, z), \
  ANGLE3D( AngleDeg(0.0f), AngleDeg(0.0f), AngleDeg(0.0f)))

// ptr to current menu
CGameMenu *pgmCurrentMenu = NULL;

// global back button
CMGButton mgBack;

// -------- Confirm menu
CConfirmMenu gmConfirmMenu;

// -------- Main menu
CMainMenu gmMainMenu;

// -------- InGame menu
CInGameMenu gmInGameMenu;

// -------- Single player menu
CSinglePlayerMenu gmSinglePlayerMenu;

// -------- New single player menu
CSinglePlayerNewMenu gmSinglePlayerNewMenu;

// -------- Disabled menu
CDisabledMenu gmDisabledFunction;

// -------- Manual levels menu
CLevelsMenu gmLevelsMenu;

// -------- console variable adjustment menu
extern BOOL _bVarChanged = FALSE;
CVarMenu gmVarMenu;

// -------- Player profile menu
CPlayerProfileMenu gmPlayerProfile;

// -------- Controls menu
CControlsMenu gmControls;

// -------- Load/Save menu
CLoadSaveMenu gmLoadSaveMenu;

// -------- High-score menu
CHighScoreMenu gmHighScoreMenu;

// -------- Customize keyboard menu
CCustomizeKeyboardMenu gmCustomizeKeyboardMenu;

// -------- Choose servers menu
CServersMenu gmServersMenu;

// -------- Customize axis menu
CCustomizeAxisMenu gmCustomizeAxisMenu;

// -------- Options menu
COptionsMenu gmOptionsMenu;

// -------- Video options menu
CVideoOptionsMenu gmVideoOptionsMenu;

INDEX         _ctResolutions = 0;
CTString     * _astrResolutionTexts = NULL;
CDisplayMode *_admResolutionModes  = NULL;
INDEX         _ctAdapters = 0;
CTString     * _astrAdapterTexts = NULL;


// -------- Audio options menu
CAudioOptionsMenu gmAudioOptionsMenu;

// -------- Network menu
CNetworkMenu gmNetworkMenu;

// -------- Network join menu
CNetworkJoinMenu gmNetworkJoinMenu;

// -------- Network start menu
CNetworkStartMenu gmNetworkStartMenu;

// -------- Network open menu
CNetworkOpenMenu gmNetworkOpenMenu;

// -------- Split screen menu
CSplitScreenMenu gmSplitScreenMenu;

// -------- Split screen start menu
CSplitStartMenu gmSplitStartMenu;

// -------- Select players menu
CSelectPlayersMenu gmSelectPlayersMenu;

extern void PlayMenuSound(CSoundData *psd)
{
  if (_psoMenuSound!=NULL && !_psoMenuSound->IsPlaying()) {
    _psoMenuSound->Play(psd, SOF_NONGAME);
  }
}

CModelObject *AddAttachment_t(CModelObject *pmoParent, INDEX iPosition,
   const CTFileName &fnmModel, INDEX iAnim,
   const CTFileName &fnmTexture,
   const CTFileName &fnmReflection,
   const CTFileName &fnmSpecular)
{
  CAttachmentModelObject *pamo = pmoParent->AddAttachmentModel(iPosition);
  ASSERT(pamo!=NULL);
  pamo->amo_moModelObject.SetData_t(fnmModel);
  pamo->amo_moModelObject.PlayAnim(iAnim, AOF_LOOPING);
  pamo->amo_moModelObject.mo_toTexture.SetData_t(fnmTexture);
  pamo->amo_moModelObject.mo_toReflection.SetData_t(fnmReflection);
  pamo->amo_moModelObject.mo_toSpecular.SetData_t(fnmSpecular);
  return &pamo->amo_moModelObject;
}

// translate all texts in array for one radio button
void TranslateRadioTexts(CTString astr[], INDEX ct)
{
  for (INDEX i=0; i<ct; i++) {
    astr[i] = TranslateConst(astr[i], 4);
  }
}

// make description for a given resolution
CTString GetResolutionDescription(CDisplayMode &dm)
{
  CTString str;
  // if dual head
  if (dm.IsDualHead()) {
    str.PrintF(TRANS("%dx%d double"), dm.dm_pixSizeI/2, dm.dm_pixSizeJ);
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
  ASSERT(iRes>=0 && iRes<_ctResolutions);

  CTString &str    = _astrResolutionTexts[iRes];
  CDisplayMode &dm = _admResolutionModes[iRes];
  dm.dm_pixSizeI = pixSizeI;
  dm.dm_pixSizeJ = pixSizeJ;
  str = GetResolutionDescription(dm);
}

// set new thumbnail
void SetThumbnail(CTFileName fn)
{
  _bThumbnailOn = TRUE;
  try {
    _toThumbnail.SetData_t(fn.NoExt()+"Tbn.tex");
  } catch(char *strError) {
    (void)strError;
    try {
      _toThumbnail.SetData_t(fn.NoExt()+".tbn");
    } catch(char *strError) {
      (void)strError;
      _toThumbnail.SetData(NULL);
    }
  }
}

// remove thumbnail
void ClearThumbnail(void)
{
  _bThumbnailOn = FALSE;
  _toThumbnail.SetData(NULL);
  _pShell->Execute( "FreeUnusedStock();");
}

// start load/save menus depending on type of game running

void QuickSaveFromMenu()
{
  _pShell->SetINDEX("gam_bQuickSave", 2); // force save with reporting
  StopMenus(TRUE);
}

void StartCurrentLoadMenu()
{
  if (_gmRunningGameMode==GM_NETWORK) {
    void StartNetworkLoadMenu(void);
    StartNetworkLoadMenu();
  } else if (_gmRunningGameMode==GM_SPLIT_SCREEN) {
    void StartSplitScreenLoadMenu(void);
    StartSplitScreenLoadMenu();
  } else {
    void StartSinglePlayerLoadMenu(void);
    StartSinglePlayerLoadMenu();
  }
}
void StartCurrentSaveMenu()
{
  if (_gmRunningGameMode==GM_NETWORK) {
    void StartNetworkSaveMenu(void);
    StartNetworkSaveMenu();
  } else if (_gmRunningGameMode==GM_SPLIT_SCREEN) {
    void StartSplitScreenSaveMenu(void);
    StartSplitScreenSaveMenu();
  } else {
    void StartSinglePlayerSaveMenu(void);
    StartSinglePlayerSaveMenu();
  }
}
void StartCurrentQuickLoadMenu()
{
  if (_gmRunningGameMode==GM_NETWORK) {
    void StartNetworkQuickLoadMenu(void);
    StartNetworkQuickLoadMenu();
  } else if (_gmRunningGameMode==GM_SPLIT_SCREEN) {
    void StartSplitScreenQuickLoadMenu(void);
    StartSplitScreenQuickLoadMenu();
  } else {
    void StartSinglePlayerQuickLoadMenu(void);
    StartSinglePlayerQuickLoadMenu();
  }
}

void StartMenus(char *str)
{
  _tmMenuLastTickDone=_pTimer->GetRealTimeTick();
  // disable printing of last lines
  CON_DiscardLastLineTimes();

  // stop all IFeel effects
  IFeel_StopEffect(NULL);
  if (pgmCurrentMenu==&gmMainMenu || pgmCurrentMenu==&gmInGameMenu) {
    if (_gmRunningGameMode==GM_NONE) {
      pgmCurrentMenu = &gmMainMenu;
    } else {
      pgmCurrentMenu = &gmInGameMenu;
    }
  }

  // start main menu, or last active one
  if (pgmCurrentMenu!=NULL) {
    ChangeToMenu(pgmCurrentMenu);
  } else {
    if (_gmRunningGameMode==GM_NONE) {
      ChangeToMenu(&gmMainMenu);
    } else {
      ChangeToMenu(&gmInGameMenu);
    }
  }
  if (CTString(str)=="load") {
    StartCurrentLoadMenu();
    gmLoadSaveMenu.gm_pgmParentMenu = NULL;
  }
  if (CTString(str)=="save") {
    StartCurrentSaveMenu();
    gmLoadSaveMenu.gm_pgmParentMenu = NULL;
    FixupBackButton(&gmLoadSaveMenu);
  }
  if (CTString(str)=="controls") {
    void StartControlsMenuFromOptions(void);
    StartControlsMenuFromOptions();
    gmControls.gm_pgmParentMenu = NULL;
    FixupBackButton(&gmControls);
  }
  if (CTString(str)=="join") {
    void StartSelectPlayersMenuFromOpen(void);
    StartSelectPlayersMenuFromOpen();
    gmSelectPlayersMenu.gm_pgmParentMenu = &gmMainMenu;
    FixupBackButton(&gmSelectPlayersMenu);
  }
  if (CTString(str)=="hiscore") {
    ChangeToMenu( &gmHighScoreMenu);
    gmHighScoreMenu.gm_pgmParentMenu = &gmMainMenu;
    FixupBackButton(&gmHighScoreMenu);
  }
  bMenuActive = TRUE;
  bMenuRendering = TRUE;
}


void StopMenus( BOOL bGoToRoot /*=TRUE*/)
{
  ClearThumbnail();
  if (pgmCurrentMenu!=NULL && bMenuActive) {
    pgmCurrentMenu->EndMenu();
  }
  bMenuActive = FALSE;
  if (bGoToRoot) {
    if (_gmRunningGameMode==GM_NONE) {
      pgmCurrentMenu = &gmMainMenu;
    } else {
      pgmCurrentMenu = &gmInGameMenu;
    }
  }
}


BOOL IsMenusInRoot(void)
{
  return pgmCurrentMenu==NULL || pgmCurrentMenu==&gmMainMenu || pgmCurrentMenu==&gmInGameMenu;
}

// ---------------------- When activated functions 
void StartSinglePlayerMenu(void)
{
  ChangeToMenu( &gmSinglePlayerMenu);
}

void ExitGame(void)
{
  _bRunning = FALSE;
  _bQuitScreen = TRUE;
}

CTFileName _fnmModSelected;
CTString _strModURLSelected;
CTString _strModServerSelected;

void ExitAndSpawnExplorer(void)
{
  _bRunning = FALSE;
  _bQuitScreen = FALSE;
  extern CTString _strURLToVisit;
  _strURLToVisit = _strModURLSelected;
}

void ExitConfirm(void)
{
  _pConfimedYes = &ExitGame;
  _pConfimedNo = NULL;
  gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("ARE YOU SERIOUS?");
  gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
  gmConfirmMenu.BeLarge();
  ChangeToMenu( &gmConfirmMenu);
}

void StopConfirm(void)
{
  extern void StopCurrentGame(void);
  _pConfimedYes = &StopCurrentGame;
  _pConfimedNo = NULL;
  gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("ARE YOU SERIOUS?");
  gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
  gmConfirmMenu.BeLarge();
  ChangeToMenu( &gmConfirmMenu);
}

void ModConnect(void)
{
  extern CTFileName _fnmModToLoad;
  extern CTString _strModServerJoin;
  _fnmModToLoad = _fnmModSelected;
  _strModServerJoin = _strModServerSelected;
}

void ModConnectConfirm(void)
{
  if (_fnmModSelected==" ") {
    _fnmModSelected = CTString("SeriousSam");
  }
  CTFileName fnmModPath = "Mods\\"+_fnmModSelected+"\\";
  if (!FileExists(fnmModPath+"BaseWriteInclude.lst")
    &&!FileExists(fnmModPath+"BaseWriteExclude.lst")
    &&!FileExists(fnmModPath+"BaseBrowseInclude.lst")
    &&!FileExists(fnmModPath+"BaseBrowseExclude.lst")) {
    extern void ModNotInstalled(void);
    ModNotInstalled();
    return;
  }

  CPrintF(TRANS("Server is running a different MOD (%s).\nYou need to reload to connect.\n"), _fnmModSelected);
  _pConfimedYes = &ModConnect;
  _pConfimedNo = NULL;
  gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("CHANGE THE MOD?");
  gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
  gmConfirmMenu.BeLarge();
  ChangeToMenu( &gmConfirmMenu);
}

void SaveConfirm(void)
{
  extern void OnFileSaveOK(void);
  _pConfimedYes = &OnFileSaveOK;
  _pConfimedNo = NULL;
  gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("OVERWRITE?");
  gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
  gmConfirmMenu.BeLarge();
  ChangeToMenu( &gmConfirmMenu);
}


void ModLoadYes(void)
{
  extern CTFileName _fnmModToLoad;
  _fnmModToLoad = _fnmModSelected;
}

void ModConfirm(void)
{
  _pConfimedYes = &ModLoadYes;
  _pConfimedNo = NULL;
  gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("LOAD THIS MOD?");
  gmConfirmMenu.gm_pgmParentMenu = &gmLoadSaveMenu;
  gmConfirmMenu.BeLarge();
  ChangeToMenu( &gmConfirmMenu);
}

void VideoConfirm(void)
{
  // FIXUP: keyboard focus lost when going from full screen to window mode
  // due to WM_MOUSEMOVE being sent
  _bMouseUsedLast = FALSE;
  _pmgUnderCursor = gmConfirmMenu.gm_pmgSelectedByDefault;

  _pConfimedYes = NULL;
  void RevertVideoSettings(void);
  _pConfimedNo = RevertVideoSettings;

  gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("KEEP THIS SETTING?");
  gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
  gmConfirmMenu.BeLarge();
  ChangeToMenu( &gmConfirmMenu);
}

void CDConfirm(void (*pOk)(void))
{
  _pConfimedYes = pOk;
  _pConfimedNo = NULL;
  gmConfirmMenu.gm_mgConfirmLabel.mg_strText = TRANS("PLEASE INSERT GAME CD?");
  if (pgmCurrentMenu!=&gmConfirmMenu) {
    gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
    gmConfirmMenu.BeLarge();
    ChangeToMenu( &gmConfirmMenu);
  }
}

void StopCurrentGame(void)
{
  _pGame->StopGame();
  _gmRunningGameMode=GM_NONE;
  StopMenus(TRUE);
  StartMenus("");
}
void StartSinglePlayerNewMenuCustom(void)
{
  gmSinglePlayerNewMenu.gm_pgmParentMenu = &gmLevelsMenu;
  ChangeToMenu( &gmSinglePlayerNewMenu);
}
void StartSinglePlayerNewMenu(void)
{
  gmSinglePlayerNewMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;
  extern CTString sam_strFirstLevel;
  _pGame->gam_strCustomLevel = sam_strFirstLevel;
  ChangeToMenu( &gmSinglePlayerNewMenu);
}

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

  if (_pGame->NewGame( _pGame->gam_strCustomLevel, _pGame->gam_strCustomLevel, sp))
  {
    StopMenus();
    _gmRunningGameMode = GM_SINGLE_PLAYER;
  } else {
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
  _pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_EXTREME+1);
  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
  StartSinglePlayerGame();
}

void StartTraining(void)
{
  gmSinglePlayerNewMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;
  extern CTString sam_strTrainingLevel;
  _pGame->gam_strCustomLevel = sam_strTrainingLevel;
  ChangeToMenu( &gmSinglePlayerNewMenu);
}
void StartTechTest(void)
{
  gmSinglePlayerNewMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;
  extern CTString sam_strTechTestLevel; 
  _pGame->gam_strCustomLevel = sam_strTechTestLevel;
  StartSinglePlayerGame_Normal();
}

void StartChangePlayerMenuFromOptions(void)
{
  _bPlayerMenuFromSinglePlayer = FALSE;
  gmPlayerProfile.gm_piCurrentPlayer = &_pGame->gm_iSinglePlayer;
  gmPlayerProfile.gm_pgmParentMenu = &gmOptionsMenu;
  ChangeToMenu( &gmPlayerProfile);
}

void StartChangePlayerMenuFromSinglePlayer(void)
{
  _iLocalPlayer = -1;
  _bPlayerMenuFromSinglePlayer = TRUE;
  gmPlayerProfile.gm_piCurrentPlayer = &_pGame->gm_iSinglePlayer;
  gmPlayerProfile.gm_pgmParentMenu = &gmSinglePlayerMenu;
  ChangeToMenu( &gmPlayerProfile);
}

void StartControlsMenuFromPlayer(void)
{
  gmControls.gm_pgmParentMenu = &gmPlayerProfile;
  ChangeToMenu( &gmControls);
}
void StartControlsMenuFromOptions(void)
{
  gmControls.gm_pgmParentMenu = &gmOptionsMenu;
  ChangeToMenu( &gmControls);
}

void DisabledFunction(void)
{
  gmDisabledFunction.gm_pgmParentMenu = pgmCurrentMenu;
  gmDisabledFunction.gm_mgButton.mg_strText = TRANS("The feature is not available in this version!");
  gmDisabledFunction.gm_mgTitle.mg_strText = TRANS("DISABLED");
  ChangeToMenu( &gmDisabledFunction);
}

void ModNotInstalled(void)
{
  _pConfimedYes = &ExitAndSpawnExplorer;
  _pConfimedNo = NULL;
  gmConfirmMenu.gm_mgConfirmLabel.mg_strText.PrintF(
    TRANS("You don't have MOD '%s' installed.\nDo you want to visit its web site?"), (const char*)_fnmModSelected);
  gmConfirmMenu.gm_pgmParentMenu = pgmCurrentMenu;
  gmConfirmMenu.BeSmall();
  ChangeToMenu( &gmConfirmMenu);

/*
  gmDisabledFunction.gm_pgmParentMenu = pgmCurrentMenu;
  mgDisabledMenuButton.mg_strText.PrintF(
    TRANS("You don't have MOD '%s' installed.\nPlease visit Croteam website for updates."), _fnmModSelected);
  mgDisabledTitle.mg_strText = TRANS("MOD REQUIRED");
  _strModURLSelected
  ChangeToMenu( &gmDisabledFunction);
  */
}

CTFileName _fnDemoToPlay;
void StartDemoPlay(void)
{
  _pGame->gm_StartSplitScreenCfg = CGame::SSC_OBSERVER;
  // play the demo
  _pGame->gm_strNetworkProvider = "Local";
  if( _pGame->StartDemoPlay( _fnDemoToPlay))
  {
    // exit menu and pull up the console
    StopMenus();
    if( _pGame->gm_csConsoleState!=CS_OFF) _pGame->gm_csConsoleState = CS_TURNINGOFF;
    _gmRunningGameMode = GM_DEMO;
  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

void StartSelectLevelFromSingle(void)
{
  FilterLevels(GetSpawnFlagsForGameType(-1));
  _pAfterLevelChosen = StartSinglePlayerNewMenuCustom;
  ChangeToMenu( &gmLevelsMenu);
  gmLevelsMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;
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
  if (_pGame->NewGame( _pGame->gam_strSessionName, fnWorld, sp))
  {
    StopMenus();
    _gmRunningGameMode = GM_NETWORK;
    // if starting a dedicated server
    if (_pGame->gm_MenuSplitScreenCfg==CGame::SSC_DEDICATED) {
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
  if (_pGame->JoinGame( CNetworkSession( _pGame->gam_strJoinAddress)))
  {
    StopMenus();
    _gmRunningGameMode = GM_NETWORK;
  } else {
    if (_pNetwork->ga_strRequiredMod != "") {
      extern CTFileName _fnmModToLoad;
      extern CTString _strModServerJoin;
      char strModName[256] = {0};
      char strModURL[256] = {0};
      _pNetwork->ga_strRequiredMod.ScanF("%250[^\\]\\%s", &strModName, &strModURL);
      _fnmModSelected = CTString(strModName);
      _strModURLSelected = strModURL;
      if (_strModURLSelected="") {
        _strModURLSelected = "http://www.croteam.com/mods/Old";
      }
      _strModServerSelected.PrintF("%s:%s", _pGame->gam_strJoinAddress, _pShell->GetValue("net_iPort"));
      ModConnectConfirm();
    }
    _gmRunningGameMode = GM_NONE;
  }
}
void StartHighScoreMenu(void)
{
  gmHighScoreMenu.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu( &gmHighScoreMenu);
}
CTFileName _fnGameToLoad;
void StartNetworkLoadGame(void)
{

//  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) mgSplitScreenCfg.mg_iSelected;
  _pGame->gm_StartSplitScreenCfg = _pGame->gm_MenuSplitScreenCfg;

  _pGame->gm_aiStartLocalPlayers[0] = _pGame->gm_aiMenuLocalPlayers[0];
  _pGame->gm_aiStartLocalPlayers[1] = _pGame->gm_aiMenuLocalPlayers[1];
  _pGame->gm_aiStartLocalPlayers[2] = _pGame->gm_aiMenuLocalPlayers[2];
  _pGame->gm_aiStartLocalPlayers[3] = _pGame->gm_aiMenuLocalPlayers[3];

  _pGame->gm_strNetworkProvider = "TCP/IP Server";
  if (_pGame->LoadGame( _fnGameToLoad))
  {
    StopMenus();
    _gmRunningGameMode = GM_NETWORK;
  } else {
    _gmRunningGameMode = GM_NONE;
  }
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
  if (_pGame->NewGame( fnWorld.FileName(), fnWorld, sp))
  {
    StopMenus();
    _gmRunningGameMode = GM_SPLIT_SCREEN;
  } else {
    _gmRunningGameMode = GM_NONE;
  }
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
  if (_pGame->LoadGame( _fnGameToLoad))
  {
    StopMenus();
    _gmRunningGameMode = GM_SPLIT_SCREEN;
  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

void StartSelectPlayersMenuFromSplit(void)
{
  gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
  gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
  gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
  gmSelectPlayersMenu.gm_pgmParentMenu = &gmSplitStartMenu;
  ChangeToMenu( &gmSelectPlayersMenu);
}

void StartSelectPlayersMenuFromNetwork(void)
{
  gmSelectPlayersMenu.gm_bAllowDedicated = TRUE;
  gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
  gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartNetworkGame;
  gmSelectPlayersMenu.gm_pgmParentMenu = &gmNetworkStartMenu;
  ChangeToMenu( &gmSelectPlayersMenu);
}

void StartSelectPlayersMenuFromOpen(void)
{
  gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
  gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
  gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &JoinNetworkGame;
  gmSelectPlayersMenu.gm_pgmParentMenu = &gmNetworkOpenMenu;
  ChangeToMenu( &gmSelectPlayersMenu);

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
  ChangeToMenu( &gmSelectPlayersMenu);

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
  ChangeToMenu( &gmServersMenu);
  gmServersMenu.gm_pgmParentMenu = &gmNetworkJoinMenu;
}
void StartSelectServerNET(void)
{
  gmServersMenu.m_bInternet = TRUE;
  ChangeToMenu( &gmServersMenu);
  gmServersMenu.gm_pgmParentMenu = &gmNetworkJoinMenu;
}

void StartSelectLevelFromSplit(void)
{
  FilterLevels(GetSpawnFlagsForGameType(gmSplitStartMenu.gm_mgGameType.mg_iSelected));
  void StartSplitStartMenu(void);
  _pAfterLevelChosen = StartSplitStartMenu;
  ChangeToMenu( &gmLevelsMenu);
  gmLevelsMenu.gm_pgmParentMenu = &gmSplitStartMenu;
}
void StartSelectLevelFromNetwork(void)
{
  FilterLevels(GetSpawnFlagsForGameType(gmNetworkStartMenu.gm_mgGameType.mg_iSelected));
  void StartNetworkStartMenu(void);
  _pAfterLevelChosen = StartNetworkStartMenu;
  ChangeToMenu( &gmLevelsMenu);
  gmLevelsMenu.gm_pgmParentMenu = &gmNetworkStartMenu;
}

void StartSelectPlayersMenuFromSplitScreen(void)
{
  gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
  gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
//  gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGame;
  gmSelectPlayersMenu.gm_pgmParentMenu = &gmSplitScreenMenu;
  ChangeToMenu( &gmSelectPlayersMenu);
}
void StartSelectPlayersMenuFromNetworkLoad(void)
{
  gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
  gmSelectPlayersMenu.gm_bAllowObserving = TRUE;
  gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartNetworkLoadGame;
  gmSelectPlayersMenu.gm_pgmParentMenu = &gmLoadSaveMenu;
  ChangeToMenu( &gmSelectPlayersMenu);
}

void StartSelectPlayersMenuFromSplitScreenLoad(void)
{
  gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
  gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
  gmSelectPlayersMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitScreenGameLoad;
  gmSelectPlayersMenu.gm_pgmParentMenu = &gmLoadSaveMenu;
  ChangeToMenu( &gmSelectPlayersMenu);
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
  ChangeToMenu( &gmVarMenu);
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
  if( _pGame->SaveGame( fnm)) {
    StopMenus();
    return TRUE;
  } else {
    return FALSE;
  }
}

BOOL LSSaveDemo(const CTFileName &fnm)
{
  // save the demo
  if(_pGame->StartDemoRec(fnm)) {
    StopMenus();
    return TRUE;
  } else {
    return FALSE;
  }
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  if (sam_strNetworkSettings=="") {
    gmLoadSaveMenu.gm_mgNotes.mg_strText = TRANS(
      "Before joining a network game,\n"
      "you have to adjust your connection parameters.\n"
      "Choose one option from the list.\n"
      "If you have problems with connection, you can adjust\n"
      "these parameters again from the Options menu.\n"
      );
  } else {
    gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
  }

  gmLoadSaveMenu.gm_pgmParentMenu = &gmOptionsMenu;
  ChangeToMenu( &gmLoadSaveMenu);
}

void SetQuickLoadNotes(void)
{
  if (_pShell->GetINDEX("gam_iQuickSaveSlots")<=8) {
    gmLoadSaveMenu.gm_mgNotes.mg_strText = TRANS(
      "In-game QuickSave shortcuts:\n"
      "F6 - save a new QuickSave\n"
      "F9 - load the last QuickSave\n");
  } else {
    gmLoadSaveMenu.gm_mgNotes.mg_strText = "";
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
}
void StartSinglePlayerSaveMenu(void)
{
  if( _gmRunningGameMode != GM_SINGLE_PLAYER) return;
  // if no live players
  if (_pGame->GetPlayersCount()>0 && _pGame->GetLivePlayersCount()<=0) {
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
}
void StartDemoSaveMenu(void)
{
  if( _gmRunningGameMode == GM_NONE) return;
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
}

void StartNetworkSaveMenu(void)
{
  if( _gmRunningGameMode != GM_NETWORK) return;
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
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
  ChangeToMenu( &gmLoadSaveMenu);
}
void StartSplitScreenSaveMenu(void)
{
  if( _gmRunningGameMode != GM_SPLIT_SCREEN) return;
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
  ChangeToMenu( &gmLoadSaveMenu);
}

// game options var settings
void StartVarGameOptions(void)
{
  gmVarMenu.gm_mgTitle.mg_strText = TRANS("GAME OPTIONS");
  gmVarMenu.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\GameOptions.cfg");
  ChangeToMenu( &gmVarMenu);
}
void StartSinglePlayerGameOptions(void)
{
  gmVarMenu.gm_mgTitle.mg_strText = TRANS("GAME OPTIONS");
  gmVarMenu.gm_fnmMenuCFG = CTFILENAME("Scripts\\Menu\\SPOptions.cfg");
  ChangeToMenu( &gmVarMenu);
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
  ChangeToMenu( &gmVarMenu);
}

void StartCustomizeKeyboardMenu(void)
{
  ChangeToMenu( &gmCustomizeKeyboardMenu);
}
void StartCustomizeAxisMenu(void)
{
  ChangeToMenu( &gmCustomizeAxisMenu);
}
void StopRecordingDemo(void)
{
  _pNetwork->StopDemoRec();
  void SetDemoStartStopRecText(void);
  SetDemoStartStopRecText();
}
void StartOptionsMenu(void)
{
  gmOptionsMenu.gm_pgmParentMenu = pgmCurrentMenu;
  ChangeToMenu( &gmOptionsMenu);
}
static void ResolutionToSize(INDEX iRes, PIX &pixSizeI, PIX &pixSizeJ)
{
  ASSERT(iRes>=0 && iRes<_ctResolutions);
  CDisplayMode &dm = _admResolutionModes[iRes];
  pixSizeI = dm.dm_pixSizeI;
  pixSizeJ = dm.dm_pixSizeJ;
}
static void SizeToResolution(PIX pixSizeI, PIX pixSizeJ, INDEX &iRes)
{
  for(iRes=0; iRes<_ctResolutions; iRes++) {
    CDisplayMode &dm = _admResolutionModes[iRes];
    if (dm.dm_pixSizeI==pixSizeI && dm.dm_pixSizeJ==pixSizeJ) {
      return;
    }
  }
  // if none was found, search for default 
  for(iRes=0; iRes<_ctResolutions; iRes++) {
    CDisplayMode &dm = _admResolutionModes[iRes];
    if (dm.dm_pixSizeI==640 && dm.dm_pixSizeJ==480) {
      return;
    }
  }
  // if still none found
  ASSERT(FALSE);  // this should never happen
  // return first one
  iRes = 0;
}


static INDEX APIToSwitch(enum GfxAPIType gat)
{
  switch(gat) {
  case GAT_OGL: return 0;
#ifdef SE1_D3D
  case GAT_D3D: return 1;
#endif // SE1_D3D
  default: ASSERT(FALSE); return 0;
  }
}
static enum GfxAPIType SwitchToAPI(INDEX i)
{
  switch(i) {
  case 0: return GAT_OGL;
#ifdef SE1_D3D
  case 1: return GAT_D3D;
#endif // SE1_D3D
  default: ASSERT(FALSE); return GAT_OGL;
  }
}

static INDEX DepthToSwitch(enum DisplayDepth dd)
{
  switch(dd) {
  case DD_DEFAULT: return 0;
  case DD_16BIT  : return 1;
  case DD_32BIT  : return 2;
  default: ASSERT(FALSE); return 0;
  }
}
static enum DisplayDepth SwitchToDepth(INDEX i)
{
  switch(i) {
  case 0: return DD_DEFAULT;
  case 1: return DD_16BIT;
  case 2: return DD_32BIT;
  default: ASSERT(FALSE); return DD_DEFAULT;
  }
}

extern void InitVideoOptionsButtons(void);
extern void UpdateVideoOptionsButtons(INDEX i);

void RevertVideoSettings(void)
{
  // restore previous variables
  sam_bFullScreenActive = sam_old_bFullScreenActive;
  sam_iScreenSizeI      = sam_old_iScreenSizeI;
  sam_iScreenSizeJ      = sam_old_iScreenSizeJ;
  sam_iDisplayDepth     = sam_old_iDisplayDepth;
  sam_iDisplayAdapter   = sam_old_iDisplayAdapter;
  sam_iGfxAPI           = sam_old_iGfxAPI;
  sam_iVideoSetup       = sam_old_iVideoSetup;

  // update the video mode
  extern void ApplyVideoMode(void);
  ApplyVideoMode();

  // refresh buttons
  InitVideoOptionsButtons();
  UpdateVideoOptionsButtons(-1);
}

void ApplyVideoOptions(void)
{
  sam_old_bFullScreenActive = sam_bFullScreenActive;
  sam_old_iScreenSizeI      = sam_iScreenSizeI;
  sam_old_iScreenSizeJ      = sam_iScreenSizeJ;
  sam_old_iDisplayDepth     = sam_iDisplayDepth;
  sam_old_iDisplayAdapter   = sam_iDisplayAdapter;
  sam_old_iGfxAPI           = sam_iGfxAPI;
  sam_old_iVideoSetup       = sam_iVideoSetup;

  BOOL bFullScreenMode = gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_iSelected == 1;
  PIX pixWindowSizeI, pixWindowSizeJ;
  ResolutionToSize(gmVideoOptionsMenu.gm_mgResolutionsTrigger.mg_iSelected, pixWindowSizeI, pixWindowSizeJ);
  enum GfxAPIType gat = SwitchToAPI(gmVideoOptionsMenu.gm_mgDisplayAPITrigger.mg_iSelected);
  enum DisplayDepth dd = SwitchToDepth(gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.mg_iSelected);
  const INDEX iAdapter = gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.mg_iSelected;

  // setup preferences
  extern INDEX _iLastPreferences;
  if( sam_iVideoSetup==3) _iLastPreferences = 3;
  sam_iVideoSetup = gmVideoOptionsMenu.gm_mgDisplayPrefsTrigger.mg_iSelected;

  // force fullscreen mode if needed
  CDisplayAdapter &da = _pGfx->gl_gaAPI[gat].ga_adaAdapter[iAdapter];
  if( da.da_ulFlags & DAF_FULLSCREENONLY) bFullScreenMode = TRUE;
  if( da.da_ulFlags & DAF_16BITONLY) dd = DD_16BIT;
  // force window to always be in default colors
  if( !bFullScreenMode) dd = DD_DEFAULT;

  // (try to) set mode
  StartNewMode( gat, iAdapter, pixWindowSizeI, pixWindowSizeJ, dd, bFullScreenMode);

  // refresh buttons
  InitVideoOptionsButtons();
  UpdateVideoOptionsButtons(-1);
  
  // ask user to keep or restore
  if( bFullScreenMode) VideoConfirm();
}

#define VOLUME_STEPS  50

extern void RefreshSoundFormat( void)
{
  switch( _pSound->GetFormat())
  {
  case CSoundLibrary::SF_NONE:     {gmAudioOptionsMenu.gm_mgFrequencyTrigger.mg_iSelected = 0; break; }
  case CSoundLibrary::SF_11025_16: {gmAudioOptionsMenu.gm_mgFrequencyTrigger.mg_iSelected = 1; break; }
  case CSoundLibrary::SF_22050_16: {gmAudioOptionsMenu.gm_mgFrequencyTrigger.mg_iSelected = 2; break; }
  case CSoundLibrary::SF_44100_16: {gmAudioOptionsMenu.gm_mgFrequencyTrigger.mg_iSelected = 3; break; }
  default:                          gmAudioOptionsMenu.gm_mgFrequencyTrigger.mg_iSelected = 0;
  }

  gmAudioOptionsMenu.gm_mgAudioAutoTrigger.mg_iSelected = Clamp(sam_bAutoAdjustAudio, 0, 1);
  gmAudioOptionsMenu.gm_mgAudioAPITrigger.mg_iSelected = Clamp(_pShell->GetINDEX("snd_iInterface"), 0L, 2L);

  gmAudioOptionsMenu.gm_mgWaveVolume.mg_iMinPos = 0;
  gmAudioOptionsMenu.gm_mgWaveVolume.mg_iMaxPos = VOLUME_STEPS;
  gmAudioOptionsMenu.gm_mgWaveVolume.mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fSoundVolume")*VOLUME_STEPS + 0.5f);
  gmAudioOptionsMenu.gm_mgWaveVolume.ApplyCurrentPosition();

  gmAudioOptionsMenu.gm_mgMPEGVolume.mg_iMinPos = 0;
  gmAudioOptionsMenu.gm_mgMPEGVolume.mg_iMaxPos = VOLUME_STEPS;
  gmAudioOptionsMenu.gm_mgMPEGVolume.mg_iCurPos = (INDEX)(_pShell->GetFLOAT("snd_fMusicVolume")*VOLUME_STEPS + 0.5f);
  gmAudioOptionsMenu.gm_mgMPEGVolume.ApplyCurrentPosition();

  gmAudioOptionsMenu.gm_mgAudioAutoTrigger.ApplyCurrentSelection();
  gmAudioOptionsMenu.gm_mgAudioAPITrigger.ApplyCurrentSelection();
  gmAudioOptionsMenu.gm_mgFrequencyTrigger.ApplyCurrentSelection();
}

void ApplyAudioOptions(void)
{
  sam_bAutoAdjustAudio = gmAudioOptionsMenu.gm_mgAudioAutoTrigger.mg_iSelected;
  if (sam_bAutoAdjustAudio) {
    _pShell->Execute("include \"Scripts\\Addons\\SFX-AutoAdjust.ini\"");
  } else {
	_pShell->SetINDEX("snd_iInterface", gmAudioOptionsMenu.gm_mgAudioAPITrigger.mg_iSelected);

	switch (gmAudioOptionsMenu.gm_mgFrequencyTrigger.mg_iSelected)
    {
    case 0: {_pSound->SetFormat(CSoundLibrary::SF_NONE)    ;break;}
    case 1: {_pSound->SetFormat(CSoundLibrary::SF_11025_16);break;}
    case 2: {_pSound->SetFormat(CSoundLibrary::SF_22050_16);break;}
    case 3: {_pSound->SetFormat(CSoundLibrary::SF_44100_16);break;}
    default: _pSound->SetFormat(CSoundLibrary::SF_NONE);
    }
  }

  RefreshSoundFormat();
  snd_iFormat = _pSound->GetFormat();
}

void StartVideoOptionsMenu(void)
{
  ChangeToMenu( &gmVideoOptionsMenu);
}

void StartAudioOptionsMenu(void)
{
  ChangeToMenu( &gmAudioOptionsMenu);
}

void StartNetworkMenu(void)
{
  ChangeToMenu( &gmNetworkMenu);
}
void StartNetworkJoinMenu(void)
{
  ChangeToMenu( &gmNetworkJoinMenu);
}
void StartNetworkStartMenu(void)
{
  ChangeToMenu( &gmNetworkStartMenu);
}

void StartNetworkOpenMenu(void)
{
  ChangeToMenu( &gmNetworkOpenMenu);
}

void StartSplitScreenMenu(void)
{
  ChangeToMenu( &gmSplitScreenMenu);
}
void StartSplitStartMenu(void)
{
  ChangeToMenu( &gmSplitStartMenu);
}

// initialize game type strings table
void InitGameTypes(void)
{
  // get function that will provide us the info about gametype
  CShellSymbol *pss = _pShell->GetSymbol("GetGameTypeNameSS", /*bDeclaredOnly=*/ TRUE);
  // if none
  if (pss==NULL) {
    // error
    astrGameTypeRadioTexts[0] = "<???>";
    ctGameTypeRadioTexts = 1;
    return;
  }

  // for each mode
  for(ctGameTypeRadioTexts=0; ctGameTypeRadioTexts<ARRAYCOUNT(astrGameTypeRadioTexts); ctGameTypeRadioTexts++) {
    // get the text
    CTString (*pFunc)(INDEX) = (CTString (*)(INDEX))pss->ss_pvValue;
    CTString strMode = pFunc(ctGameTypeRadioTexts);
    // if no mode modes
    if (strMode=="") {
      // stop
      break;
    }
    // add that mode
    astrGameTypeRadioTexts[ctGameTypeRadioTexts] = strMode;
  }
}


// ------------------------ Global menu function implementation
void InitializeMenus(void)
{
  try {
    // initialize and load corresponding fonts
    _fdSmall.Load_t(  CTFILENAME( "Fonts\\Display3-narrow.fnt"));
    _fdMedium.Load_t( CTFILENAME( "Fonts\\Display3-normal.fnt"));
    _fdBig.Load_t(    CTFILENAME( "Fonts\\Display3-caps.fnt"));
    _fdTitle.Load_t(  CTFILENAME( "Fonts\\Title2.fnt"));
    _fdSmall.SetCharSpacing(-1);
    _fdSmall.SetLineSpacing( 0);
    _fdSmall.SetSpaceWidth(0.4f);
    _fdMedium.SetCharSpacing(+1);
    _fdMedium.SetLineSpacing( 0);
    _fdMedium.SetSpaceWidth(0.4f);
    _fdBig.SetCharSpacing(+1);
    _fdBig.SetLineSpacing( 0);
    _fdTitle.SetCharSpacing(+1);
    _fdTitle.SetLineSpacing( 0);

    // load menu sounds
    _psdSelect = _pSoundStock->Obtain_t( CTFILENAME("Sounds\\Menu\\Select.wav"));
    _psdPress  = _pSoundStock->Obtain_t( CTFILENAME("Sounds\\Menu\\Press.wav"));
    _psoMenuSound = new CSoundObject;

    // initialize and load menu textures
    _toPointer.SetData_t( CTFILENAME( "Textures\\General\\Pointer.tex"));
#if _SE_DEMO || TECHTESTONLY
    _toLogoMenuA.SetData_t(  CTFILENAME( "Textures\\Logo\\sam_menulogo256a_demo.tex"));
    _toLogoMenuB.SetData_t(  CTFILENAME( "Textures\\Logo\\sam_menulogo256b_demo.tex"));
#else
    _toLogoMenuA.SetData_t(  CTFILENAME( "Textures\\Logo\\sam_menulogo256a.tex"));
    _toLogoMenuB.SetData_t(  CTFILENAME( "Textures\\Logo\\sam_menulogo256b.tex"));
#endif
  }
  catch( char *strError) {
    FatalError( strError);
  }
  // force logo textures to be of maximal size
  ((CTextureData*)_toLogoMenuA.GetData())->Force(TEX_CONSTANT);
  ((CTextureData*)_toLogoMenuB.GetData())->Force(TEX_CONSTANT);

  // menu's relative placement
  CPlacement3D plRelative = CPlacement3D( FLOAT3D( 0.0f, 0.0f, -9.0f), 
                            ANGLE3D( AngleDeg(0.0f), AngleDeg(0.0f), AngleDeg(0.0f)));
  try
  {
    TRANSLATERADIOARRAY(astrNoYes);
    TRANSLATERADIOARRAY(astrComputerInvoke);
    TRANSLATERADIOARRAY(astrDisplayAPIRadioTexts);
    TRANSLATERADIOARRAY(astrDisplayPrefsRadioTexts);
    TRANSLATERADIOARRAY(astrBitsPerPixelRadioTexts);
    TRANSLATERADIOARRAY(astrFrequencyRadioTexts);
    TRANSLATERADIOARRAY(astrSoundAPIRadioTexts);
    TRANSLATERADIOARRAY(astrDifficultyRadioTexts);
    TRANSLATERADIOARRAY(astrMaxPlayersRadioTexts);
    TRANSLATERADIOARRAY(astrWeapon);
    TRANSLATERADIOARRAY(astrSplitScreenRadioTexts);

    // initialize game type strings table
    InitGameTypes();

    // ------------------- Initialize menus
    gmConfirmMenu.Initialize_t();
    gmConfirmMenu.gm_strName="Confirm";
	gmConfirmMenu.gm_pmgSelectedByDefault = &gmConfirmMenu.gm_mgConfirmYes;
    gmConfirmMenu.gm_pgmParentMenu = NULL;
	InitActionsForConfirmMenu();

    gmMainMenu.Initialize_t();
    gmMainMenu.gm_strName="Main";
	gmMainMenu.gm_pmgSelectedByDefault = &gmMainMenu.gm_mgSingle;
    gmMainMenu.gm_pgmParentMenu = NULL;
	InitActionsForMainMenu();

    gmInGameMenu.Initialize_t();
    gmInGameMenu.gm_strName="InGame";
    gmInGameMenu.gm_pmgSelectedByDefault = &gmInGameMenu.gm_mgQuickLoad;
    gmInGameMenu.gm_pgmParentMenu = NULL;
	InitActionsForInGameMenu();

    gmSinglePlayerMenu.Initialize_t();
    gmSinglePlayerMenu.gm_strName="SinglePlayer";
	gmSinglePlayerMenu.gm_pmgSelectedByDefault = &gmSinglePlayerMenu.gm_mgNewGame;
    gmSinglePlayerMenu.gm_pgmParentMenu = &gmMainMenu;
	InitActionsForSinglePlayerMenu();

    gmSinglePlayerNewMenu.Initialize_t();
    gmSinglePlayerNewMenu.gm_strName="SinglePlayerNew";
	gmSinglePlayerNewMenu.gm_pmgSelectedByDefault = &gmSinglePlayerNewMenu.gm_mgMedium;
    gmSinglePlayerNewMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;
	InitActionsForSinglePlayerNewMenu();

    gmDisabledFunction.Initialize_t();
    gmDisabledFunction.gm_strName="DisabledFunction";
	gmDisabledFunction.gm_pmgSelectedByDefault = &gmDisabledFunction.gm_mgButton;
    gmDisabledFunction.gm_pgmParentMenu = NULL;

    gmPlayerProfile.Initialize_t();
    gmPlayerProfile.gm_strName="PlayerProfile";
	gmPlayerProfile.gm_pmgSelectedByDefault = &gmPlayerProfile.gm_mgNameField;
	InitActionsForPlayerProfileMenu();

    gmControls.Initialize_t();
    gmControls.gm_strName="Controls";
	gmControls.gm_pmgSelectedByDefault = &gmControls.gm_mgButtons;
	InitActionsForControlsMenu();

    // warning! parent menu has to be set inside button activate function from where
    // Load/Save menu is called
    gmLoadSaveMenu.Initialize_t();
    gmLoadSaveMenu.gm_strName="LoadSave";
	gmLoadSaveMenu.gm_pmgSelectedByDefault = &gmLoadSaveMenu.gm_amgButton[0];

    gmHighScoreMenu.Initialize_t();
    gmHighScoreMenu.gm_strName="HighScore";
    gmHighScoreMenu.gm_pmgSelectedByDefault = &mgBack;

    gmCustomizeKeyboardMenu.Initialize_t();
    gmCustomizeKeyboardMenu.gm_strName="CustomizeKeyboard";
	gmCustomizeKeyboardMenu.gm_pmgSelectedByDefault = &gmCustomizeKeyboardMenu.gm_mgKey[0];
    gmCustomizeKeyboardMenu.gm_pgmParentMenu = &gmControls;

    gmCustomizeAxisMenu.Initialize_t();
    gmCustomizeAxisMenu.gm_strName="CustomizeAxis";
	gmCustomizeAxisMenu.gm_pmgSelectedByDefault = &gmCustomizeAxisMenu.gm_mgActionTrigger;
    gmCustomizeAxisMenu.gm_pgmParentMenu = &gmControls;
	InitActionsForCustomizeAxisMenu();

    gmOptionsMenu.Initialize_t();
    gmOptionsMenu.gm_strName="Options";
	gmOptionsMenu.gm_pmgSelectedByDefault = &gmOptionsMenu.gm_mgVideoOptions;
    gmOptionsMenu.gm_pgmParentMenu = &gmMainMenu;
	InitActionsForOptionsMenu();

    gmVideoOptionsMenu.Initialize_t();
    gmVideoOptionsMenu.gm_strName="VideoOptions";
	gmVideoOptionsMenu.gm_pmgSelectedByDefault = &gmVideoOptionsMenu.gm_mgDisplayAPITrigger;
    gmVideoOptionsMenu.gm_pgmParentMenu = &gmOptionsMenu;
	InitActionsForVideoOptionsMenu();

    gmAudioOptionsMenu.Initialize_t();
    gmAudioOptionsMenu.gm_strName="AudioOptions";
	gmAudioOptionsMenu.gm_pmgSelectedByDefault = &gmAudioOptionsMenu.gm_mgFrequencyTrigger;
    gmAudioOptionsMenu.gm_pgmParentMenu = &gmOptionsMenu;
	InitActionsForAudioOptionsMenu();

    gmLevelsMenu.Initialize_t();
    gmLevelsMenu.gm_strName="Levels";
	gmLevelsMenu.gm_pmgSelectedByDefault = &gmLevelsMenu.gm_mgManualLevel[0];
    gmLevelsMenu.gm_pgmParentMenu = &gmSinglePlayerMenu;

    gmVarMenu.Initialize_t();
    gmVarMenu.gm_strName="Var";
	gmVarMenu.gm_pmgSelectedByDefault = &gmVarMenu.gm_mgVar[0];
    gmVarMenu.gm_pgmParentMenu = &gmNetworkStartMenu;
	InitActionsForVarMenu();

    gmServersMenu.Initialize_t();
    gmServersMenu.gm_strName="Servers";
	gmServersMenu.gm_pmgSelectedByDefault = &gmServersMenu.gm_mgList;
    gmServersMenu.gm_pgmParentMenu = &gmNetworkOpenMenu;
	InitActionsForServersMenu();

    gmNetworkMenu.Initialize_t();
    gmNetworkMenu.gm_strName="Network";
	gmNetworkMenu.gm_pmgSelectedByDefault = &gmNetworkMenu.gm_mgJoin;
    gmNetworkMenu.gm_pgmParentMenu = &gmMainMenu;
	InitActionsForNetworkMenu();

    gmNetworkStartMenu.Initialize_t();
    gmNetworkStartMenu.gm_strName="NetworkStart";
	gmNetworkStartMenu.gm_pmgSelectedByDefault = &gmNetworkStartMenu.gm_mgStart;
    gmNetworkStartMenu.gm_pgmParentMenu = &gmNetworkMenu;
	InitActionsForNetworkStartMenu();

    gmNetworkJoinMenu.Initialize_t();
    gmNetworkJoinMenu.gm_strName="NetworkJoin";
	gmNetworkJoinMenu.gm_pmgSelectedByDefault = &gmNetworkJoinMenu.gm_mgLAN;
    gmNetworkJoinMenu.gm_pgmParentMenu = &gmNetworkMenu;
	InitActionsForNetworkJoinMenu();

    gmSelectPlayersMenu.gm_bAllowDedicated = FALSE;
    gmSelectPlayersMenu.gm_bAllowObserving = FALSE;
    gmSelectPlayersMenu.Initialize_t();
    gmSelectPlayersMenu.gm_strName="SelectPlayers";
    gmSelectPlayersMenu.gm_pmgSelectedByDefault = &gmSelectPlayersMenu.gm_mgStart;
	InitActionsForSelectPlayersMenu();

    gmNetworkOpenMenu.Initialize_t();
    gmNetworkOpenMenu.gm_strName="NetworkOpen";
	gmNetworkOpenMenu.gm_pmgSelectedByDefault = &gmNetworkOpenMenu.gm_mgJoin;
    gmNetworkOpenMenu.gm_pgmParentMenu = &gmNetworkJoinMenu;
	InitActionsForNetworkOpenMenu();

    gmSplitScreenMenu.Initialize_t();
    gmSplitScreenMenu.gm_strName="SplitScreen";
    gmSplitScreenMenu.gm_pmgSelectedByDefault = &gmSplitScreenMenu.gm_mgStart;
    gmSplitScreenMenu.gm_pgmParentMenu = &gmMainMenu;
	InitActionsForSplitScreenMenu();

    gmSplitStartMenu.Initialize_t();
    gmSplitStartMenu.gm_strName="SplitStart";
	gmSplitStartMenu.gm_pmgSelectedByDefault = &gmSplitStartMenu.gm_mgStart;
    gmSplitStartMenu.gm_pgmParentMenu = &gmSplitScreenMenu;
	InitActionsForSplitStartMenu();
  }
  catch( char *strError)
  {
    FatalError( strError);
  }
}


void DestroyMenus( void)
{
  gmMainMenu.Destroy();
  pgmCurrentMenu = NULL;
  _pSoundStock->Release(_psdSelect);
  _pSoundStock->Release(_psdPress);
  delete _psoMenuSound;
  _psdSelect = NULL;
  _psdPress = NULL;
  _psoMenuSound = NULL;
}

// go to parent menu if possible
void MenuGoToParent(void)
{
  // if there is no parent menu
  if( pgmCurrentMenu->gm_pgmParentMenu == NULL) {
    // if in game
    if (_gmRunningGameMode!=GM_NONE) {
      // exit menus
      StopMenus();
    // if no game is running
    } else {
      // go to main menu
      ChangeToMenu( &gmMainMenu);
    }
  // if there is some parent menu
  } else {
    // go to parent menu
    ChangeToMenu( pgmCurrentMenu->gm_pgmParentMenu);
  }
}

void MenuOnKeyDown( int iVKey)
{

  // check if mouse buttons used
  _bMouseUsedLast = (iVKey==VK_LBUTTON || iVKey==VK_RBUTTON || iVKey==VK_MBUTTON 
    || iVKey==10 || iVKey==11);

  // ignore mouse when editing
  if (_bEditingString && _bMouseUsedLast) {
    _bMouseUsedLast = FALSE;
    return;
  }

  // initially the message is not handled
  BOOL bHandled = FALSE;

  // if not a mouse button, or mouse is over some gadget
  if (!_bMouseUsedLast || _pmgUnderCursor!=NULL) {
    // ask current menu to handle the key
    bHandled = pgmCurrentMenu->OnKeyDown( iVKey);
  }

  // if not handled
  if(!bHandled) {
    // if escape or right mouse pressed
    if(iVKey==VK_ESCAPE || iVKey==VK_RBUTTON) {
      if (pgmCurrentMenu==&gmLoadSaveMenu && gmLoadSaveMenu.gm_bNoEscape) {
        return;
      }
      // go to parent menu if possible
      MenuGoToParent();
    }
  }
}

void MenuOnChar(MSG msg)
{
  // check if mouse buttons used
  _bMouseUsedLast = FALSE;

  // ask current menu to handle the key
  pgmCurrentMenu->OnChar(msg);
}

void MenuOnMouseMove(PIX pixI, PIX pixJ)
{
  static PIX pixLastI = 0;
  static PIX pixLastJ = 0;
  if (pixLastI==pixI && pixLastJ==pixJ) {
    return;
  }
  pixLastI = pixI;
  pixLastJ = pixJ;
  _bMouseUsedLast = !_bEditingString && !_bDefiningKey && !_pInput->IsInputEnabled();
}

void MenuUpdateMouseFocus(void)
{
  // get real cursor position
  POINT pt;
  GetCursorPos(&pt);
  ScreenToClient(_hwndMain, &pt);
  extern INDEX sam_bWideScreen;
  extern CDrawPort *pdp;
  if( sam_bWideScreen) {
    const PIX pixHeight = pdp->GetHeight();
    pt.y -= (pixHeight/0.75f-pixHeight)/2;
  }
  _pixCursorPosI += pt.x-_pixCursorExternPosI;
  _pixCursorPosJ  = _pixCursorExternPosJ;
  _pixCursorExternPosI = pt.x;
  _pixCursorExternPosJ = pt.y;

  // if mouse not used last
  if (!_bMouseUsedLast||_bDefiningKey||_bEditingString) {
    // do nothing
    return;
  }

  CMenuGadget *pmgActive = NULL;
  // for all gadgets in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    CMenuGadget &mg = *itmg;
    // if focused
    if( itmg->mg_bFocused) {
      // remember it
      pmgActive = &itmg.Current();
    }
  }

  // if there is some under cursor
  if (_pmgUnderCursor!=NULL) {
    _pmgUnderCursor->OnMouseOver(_pixCursorPosI, _pixCursorPosJ);
    // if the one under cursor has no neighbours
    if (_pmgUnderCursor->mg_pmgLeft ==NULL 
      &&_pmgUnderCursor->mg_pmgRight==NULL 
      &&_pmgUnderCursor->mg_pmgUp   ==NULL 
      &&_pmgUnderCursor->mg_pmgDown ==NULL) {
      // it cannot be focused
      _pmgUnderCursor = NULL;
      return;
    }

    // if the one under cursor is not active and not disappearing
    if (pmgActive!=_pmgUnderCursor && _pmgUnderCursor->mg_bVisible) {
      // change focus
      if (pmgActive!=NULL) {
        pmgActive->OnKillFocus();
      }
      _pmgUnderCursor->OnSetFocus();
    }
  }
}

static CTimerValue _tvInitialization;
static TIME _tmInitializationTick = -1;
extern TIME _tmMenuLastTickDone;

void SetMenuLerping(void)
{
  CTimerValue tvNow = _pTimer->GetHighPrecisionTimer();
  
  // if lerping was never set before
  if (_tmInitializationTick<0) {
    // initialize it
    _tvInitialization = tvNow;
    _tmInitializationTick = _tmMenuLastTickDone;
  }

  // get passed time from session state starting in precise time and in ticks
  FLOAT tmRealDelta = FLOAT((tvNow-_tvInitialization).GetSeconds());
  FLOAT tmTickDelta = _tmMenuLastTickDone-_tmInitializationTick;
  // calculate factor
  FLOAT fFactor = 1.0f-(tmTickDelta-tmRealDelta)/_pTimer->TickQuantum;

  // if the factor starts getting below zero
  if (fFactor<0) {
    // clamp it
    fFactor = 0.0f;
    // readjust timers so that it gets better
    _tvInitialization = tvNow;
    _tmInitializationTick = _tmMenuLastTickDone-_pTimer->TickQuantum;
  }
  if (fFactor>1) {
    // clamp it
    fFactor = 1.0f;
    // readjust timers so that it gets better
    _tvInitialization = tvNow;
    _tmInitializationTick = _tmMenuLastTickDone;
  }
  // set lerping factor and timer
  _pTimer->SetCurrentTick(_tmMenuLastTickDone);
  _pTimer->SetLerp(fFactor);
}


// render mouse cursor if needed
void RenderMouseCursor(CDrawPort *pdp)
{
  // if mouse not used last
  if (!_bMouseUsedLast|| _bDefiningKey || _bEditingString) {
    // don't render cursor
    return;
  }
  LCDSetDrawport(pdp);
  LCDDrawPointer(_pixCursorPosI, _pixCursorPosJ);
}


BOOL DoMenu( CDrawPort *pdp)
{
  pdp->Unlock();
  CDrawPort dpMenu(pdp, TRUE);
  dpMenu.Lock();

  MenuUpdateMouseFocus();

  // if in fullscreen
  CDisplayMode dmCurrent;
  _pGfx->GetCurrentDisplayMode(dmCurrent);
  if (dmCurrent.IsFullScreen()) {
    // clamp mouse pointer
    _pixCursorPosI = Clamp(_pixCursorPosI, 0L, dpMenu.GetWidth());
    _pixCursorPosJ = Clamp(_pixCursorPosJ, 0L, dpMenu.GetHeight());
  // if in window
  } else {
    // use same mouse pointer as windows
    _pixCursorPosI = _pixCursorExternPosI;
    _pixCursorPosJ = _pixCursorExternPosJ;
  }

  pgmCurrentMenu->Think();

  TIME tmTickNow = _pTimer->GetRealTimeTick();

  while( _tmMenuLastTickDone<tmTickNow)
  {
    _pTimer->SetCurrentTick(_tmMenuLastTickDone);
    // call think for all gadgets in menu
    FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
      itmg->Think();
    }
    _tmMenuLastTickDone+=_pTimer->TickQuantum;
  }

  SetMenuLerping();

  PIX pixW = dpMenu.GetWidth();
  PIX pixH = dpMenu.GetHeight();

  // blend background if menu is on
  if( bMenuActive)
  {
    // get current time
    TIME  tmNow = _pTimer->GetLerpedCurrentTick();
    UBYTE ubH1  = (INDEX)(tmNow*08.7f) & 255;
    UBYTE ubH2  = (INDEX)(tmNow*27.6f) & 255;
    UBYTE ubH3  = (INDEX)(tmNow*16.5f) & 255;
    UBYTE ubH4  = (INDEX)(tmNow*35.4f) & 255;

    // clear screen with background texture
    LCDPrepare(1.0f);
    LCDSetDrawport(&dpMenu);
    // do not allow game to show through
    dpMenu.Fill(C_BLACK|255);
    LCDRenderClouds1();
    LCDRenderGrid();
    LCDRenderClouds2();

    FLOAT fScaleW = (FLOAT)pixW / 640.0f;
    FLOAT fScaleH = (FLOAT)pixH / 480.0f;
    PIX   pixI0, pixJ0, pixI1, pixJ1;
    // put logo(s) to main menu (if logos exist)
    if( pgmCurrentMenu==&gmMainMenu)
    {
      if( _ptoLogoODI!=NULL) {
        CTextureData &td = (CTextureData&)*_ptoLogoODI->GetData();
        #define LOGOSIZE 50
        const PIX pixLogoWidth  = LOGOSIZE * dpMenu.dp_fWideAdjustment;
        const PIX pixLogoHeight = LOGOSIZE* td.GetHeight() / td.GetWidth();
        pixI0 = (640-pixLogoWidth -16)*fScaleW;
        pixJ0 = (480-pixLogoHeight-16)*fScaleH;
        pixI1 = pixI0+ pixLogoWidth *fScaleW;
        pixJ1 = pixJ0+ pixLogoHeight*fScaleH;
        dpMenu.PutTexture( _ptoLogoODI, PIXaabbox2D( PIX2D( pixI0, pixJ0),PIX2D( pixI1, pixJ1)));
        #undef LOGOSIZE
      }  
      if( _ptoLogoCT!=NULL) {
        CTextureData &td = (CTextureData&)*_ptoLogoCT->GetData();
        #define LOGOSIZE 50
        const PIX pixLogoWidth  = LOGOSIZE * dpMenu.dp_fWideAdjustment;
        const PIX pixLogoHeight = LOGOSIZE* td.GetHeight() / td.GetWidth();
        pixI0 = 12*fScaleW;
        pixJ0 = (480-pixLogoHeight-16)*fScaleH;
        pixI1 = pixI0+ pixLogoWidth *fScaleW;
        pixJ1 = pixJ0+ pixLogoHeight*fScaleH;
        dpMenu.PutTexture( _ptoLogoCT, PIXaabbox2D( PIX2D( pixI0, pixJ0),PIX2D( pixI1, pixJ1)));
        #undef LOGOSIZE
      } 
      
      {
        FLOAT fResize = Min(dpMenu.GetWidth()/640.0f, dpMenu.GetHeight()/480.0f);
        PIX pixSizeI = 256*fResize;
        PIX pixSizeJ = 64*fResize;
        PIX pixCenterI = dpMenu.GetWidth()/2;
        PIX pixHeightJ = 10*fResize;
        dpMenu.PutTexture(&_toLogoMenuA, PIXaabbox2D( 
          PIX2D( pixCenterI-pixSizeI, pixHeightJ),PIX2D( pixCenterI, pixHeightJ+pixSizeJ)));
        dpMenu.PutTexture(&_toLogoMenuB, PIXaabbox2D( 
          PIX2D( pixCenterI, pixHeightJ),PIX2D( pixCenterI+pixSizeI, pixHeightJ+pixSizeJ)));
      }
    } else if (pgmCurrentMenu==&gmAudioOptionsMenu) {
      if( _ptoLogoEAX!=NULL) {
        CTextureData &td = (CTextureData&)*_ptoLogoEAX->GetData();
        const INDEX iSize = 95;
        const PIX pixLogoWidth  = iSize * dpMenu.dp_fWideAdjustment;
        const PIX pixLogoHeight = iSize * td.GetHeight() / td.GetWidth();
        pixI0 =  (640-pixLogoWidth - 35)*fScaleW;
        pixJ0 = (480-pixLogoHeight - 7)*fScaleH;
        pixI1 = pixI0+ pixLogoWidth *fScaleW;
        pixJ1 = pixJ0+ pixLogoHeight*fScaleH;
        dpMenu.PutTexture( _ptoLogoEAX, PIXaabbox2D( PIX2D( pixI0, pixJ0),PIX2D( pixI1, pixJ1)));
      }
    }

#define THUMBW 96
#define THUMBH 96
    // if there is a thumbnail
    if( _bThumbnailOn) {
      const FLOAT fThumbScaleW = fScaleW * dpMenu.dp_fWideAdjustment;
      PIX pixOfs = 8*fScaleW;
      pixI0 = 8*fScaleW;
      pixJ0 = (240-THUMBW/2)*fScaleH;
      pixI1 = pixI0+ THUMBW*fThumbScaleW;
      pixJ1 = pixJ0+ THUMBH*fScaleH;
      if( _toThumbnail.GetData()!=NULL)
      { // show thumbnail with shadow and border
        dpMenu.Fill( pixI0+pixOfs, pixJ0+pixOfs, THUMBW*fThumbScaleW, THUMBH*fScaleH, C_BLACK|128);
        dpMenu.PutTexture( &_toThumbnail, PIXaabbox2D( PIX2D( pixI0, pixJ0), PIX2D( pixI1, pixJ1)), C_WHITE|255);
        dpMenu.DrawBorder( pixI0,pixJ0, THUMBW*fThumbScaleW,THUMBH*fScaleH, LCDGetColor(C_mdGREEN|255, "thumbnail border"));
      } else {
        dpMenu.SetFont( _pfdDisplayFont);
        dpMenu.SetTextScaling( fScaleW);
        dpMenu.SetTextAspect( 1.0f);
        dpMenu.PutTextCXY( TRANS("no thumbnail"), (pixI0+pixI1)/2, (pixJ0+pixJ1)/2, LCDGetColor(C_GREEN|255, "no thumbnail"));
      }
    }

    // assure we can listen to non-3d sounds
    _pSound->UpdateSounds();
  }

  // if this is popup menu
  if (pgmCurrentMenu->gm_bPopup) {

    // render parent menu first
    if (pgmCurrentMenu->gm_pgmParentMenu!=NULL) {
      _pGame->MenuPreRenderMenu(pgmCurrentMenu->gm_pgmParentMenu->gm_strName);
      FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_pgmParentMenu->gm_lhGadgets, itmg) {
        if( itmg->mg_bVisible) {
          itmg->Render( &dpMenu);
        }
      }
      _pGame->MenuPostRenderMenu(pgmCurrentMenu->gm_pgmParentMenu->gm_strName);
    }

    // gray it out
    dpMenu.Fill(C_BLACK|128);

    // clear popup box
    dpMenu.Unlock();
    PIXaabbox2D box = FloatBoxToPixBox(&dpMenu, BoxPopup());
    CDrawPort dpPopup(pdp, box);
    dpPopup.Lock();
    LCDSetDrawport(&dpPopup);
    dpPopup.Fill(C_BLACK|255);
    LCDRenderClouds1();
    LCDRenderGrid();
  //LCDRenderClouds2();
    LCDScreenBox(LCDGetColor(C_GREEN|255, "popup box"));
    dpPopup.Unlock();
    dpMenu.Lock();
  }

  // no entity is under cursor initially
  _pmgUnderCursor = NULL;

  BOOL bStilInMenus = FALSE;
  _pGame->MenuPreRenderMenu(pgmCurrentMenu->gm_strName);
  // for each menu gadget
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if gadget is visible
    if( itmg->mg_bVisible) {
      bStilInMenus = TRUE;
      itmg->Render( &dpMenu);
      if (FloatBoxToPixBox(&dpMenu, itmg->mg_boxOnScreen)>=PIX2D(_pixCursorPosI, _pixCursorPosJ)) {
        _pmgUnderCursor = itmg;
      }
    }
  }
  _pGame->MenuPostRenderMenu(pgmCurrentMenu->gm_strName);

  // no currently active gadget initially
  CMenuGadget *pmgActive = NULL;
  // if mouse was not active last
  if (!_bMouseUsedLast) {
    // find focused gadget
    FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
      CMenuGadget &mg = *itmg;
      // if focused
      if( itmg->mg_bFocused) {
        // it is active
        pmgActive = &itmg.Current();
        break;
      }
    }
  // if mouse was active last
  } else {
    // gadget under cursor is active
    pmgActive = _pmgUnderCursor;
  }

  // if editing
  if (_bEditingString && pmgActive!=NULL) {
    // dim the menu  bit
    dpMenu.Fill(C_BLACK|0x40);
    // render the edit gadget again
    pmgActive->Render(&dpMenu);
  }
  
  // if there is some active gadget and it has tips
  if (pmgActive!=NULL && (pmgActive->mg_strTip!="" || _bEditingString)) {
    CTString strTip = pmgActive->mg_strTip;
    if (_bEditingString) {
      strTip = TRANS("Enter - OK, Escape - Cancel");
    }
    // print the tip
    SetFontMedium(&dpMenu);
    dpMenu.PutTextC(strTip, 
      pixW*0.5f, pixH*0.92f, LCDGetColor(C_WHITE|255, "tool tip"));
  }

  _pGame->ConsolePrintLastLines(&dpMenu);

  RenderMouseCursor(&dpMenu);

  dpMenu.Unlock();
  pdp->Lock();

  return bStilInMenus;
}

void MenuBack(void)
{
  MenuGoToParent();
}

extern void FixupBackButton(CGameMenu *pgm)
{
  BOOL bResume = FALSE;

  if (mgBack.mg_lnNode.IsLinked()) {
    mgBack.mg_lnNode.Remove();
  }

  BOOL bHasBack = TRUE;

  if (pgm->gm_bPopup) {
    bHasBack = FALSE;
  }

  if (pgm->gm_pgmParentMenu==NULL) {
    if (_gmRunningGameMode==GM_NONE) {
      bHasBack = FALSE;
    } else {
      bResume = TRUE;
    }
  }
  if (!bHasBack) {
    mgBack.Disappear();
    return;
  }

  if (bResume) {
    mgBack.mg_strText = TRANS("RESUME");
    mgBack.mg_strTip = TRANS("return to game");
  } else {
    if (_bVarChanged) {
      mgBack.mg_strText = TRANS("CANCEL");
      mgBack.mg_strTip = TRANS("cancel changes");
    } else {
      mgBack.mg_strText = TRANS("BACK");
      mgBack.mg_strTip = TRANS("return to previous menu");
    }
  }

  mgBack.mg_iCenterI = -1;
  mgBack.mg_bfsFontSize = BFS_LARGE;
  mgBack.mg_boxOnScreen = BoxBack();
  mgBack.mg_boxOnScreen = BoxLeftColumn(16.5f);
  pgm->gm_lhGadgets.AddTail( mgBack.mg_lnNode);

  mgBack.mg_pmgLeft = 
  mgBack.mg_pmgRight = 
  mgBack.mg_pmgUp = 
  mgBack.mg_pmgDown = pgm->gm_pmgSelectedByDefault;

  mgBack.mg_pActivatedFunction = &MenuBack;

  mgBack.Appear();
}

void ChangeToMenu( CGameMenu *pgmNewMenu)
{
  // auto-clear old thumbnail when going out of menu
  ClearThumbnail();

  if( pgmCurrentMenu != NULL) {
    if (!pgmNewMenu->gm_bPopup) {
      pgmCurrentMenu->EndMenu();
    } else {
      FOREACHINLIST(CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
        itmg->OnKillFocus();
      }
    }
  }
  pgmNewMenu->StartMenu();
  if (pgmNewMenu->gm_pmgSelectedByDefault) {
    if (mgBack.mg_bFocused) {
      mgBack.OnKillFocus();
    }
    pgmNewMenu->gm_pmgSelectedByDefault->OnSetFocus();
  }
  FixupBackButton(pgmNewMenu);
  pgmCurrentMenu = pgmNewMenu;
}

// ------------------------ SGameMenu implementation
CGameMenu::CGameMenu( void)
{
  gm_pgmParentMenu = NULL;
  gm_pmgSelectedByDefault = NULL;
  gm_pmgArrowUp = NULL;
  gm_pmgArrowDn = NULL;
  gm_pmgListTop = NULL;
  gm_pmgListBottom = NULL;
  gm_iListOffset = 0;
  gm_ctListVisible = 0;
  gm_ctListTotal = 0;
  gm_bPopup = FALSE;
}

void CGameMenu::Initialize_t( void)
{
}

void CGameMenu::Destroy(void)
{
}
void CGameMenu::FillListItems(void)
{
  ASSERT(FALSE);  // must be implemented to scroll up/down
}

// +-1 -> hit top/bottom when pressing up/down on keyboard
// +-2 -> pressed pageup/pagedown on keyboard
// +-3 -> pressed arrow up/down  button in menu
// +-4 -> scrolling with mouse wheel
void CGameMenu::ScrollList(INDEX iDir)
{
  // if not valid for scrolling
  if (gm_ctListTotal<=0
    || gm_pmgArrowUp == NULL || gm_pmgArrowDn == NULL
    || gm_pmgListTop == NULL || gm_pmgListBottom == NULL) {
    // do nothing
    return;
  }

  INDEX iOldTopKey = gm_iListOffset;
  // change offset
  switch(iDir) {
    case -1:
      gm_iListOffset -= 1;
      break;
    case -4:
      gm_iListOffset -= 3;
      break;
    case -2:
    case -3:
      gm_iListOffset -= gm_ctListVisible;
      break;
    case +1:
      gm_iListOffset += 1;
      break;
    case +4:
      gm_iListOffset += 3;
      break;
    case +2:
    case +3:
      gm_iListOffset += gm_ctListVisible;
      break;
    default:
      ASSERT(FALSE);
      return;
  }
  if (gm_ctListTotal<=gm_ctListVisible) {
    gm_iListOffset = 0;
  } else {
    gm_iListOffset = Clamp(gm_iListOffset, INDEX(0), INDEX(gm_ctListTotal-gm_ctListVisible));
  }

  // set new names
  FillListItems();

  // if scroling with wheel
  if (iDir==+4 || iDir==-4) {
    // no focus changing
    return;
  }

  // delete all focuses
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    itmg->OnKillFocus();
  }

  // set new focus
  const INDEX iFirst = 0;
  const INDEX iLast = gm_ctListVisible-1;
  switch(iDir) {
    case +1:
      gm_pmgListBottom->OnSetFocus();
      break;
    case +2:
      if (gm_iListOffset!=iOldTopKey) {
        gm_pmgListTop->OnSetFocus();
      } else {
        gm_pmgListBottom->OnSetFocus();
      }
    break;
    case +3:
      gm_pmgArrowDn->OnSetFocus();
      break;
    case -1:
      gm_pmgListTop->OnSetFocus();
      break;
    case -2:
      gm_pmgListTop->OnSetFocus();
      break;
    case -3:
      gm_pmgArrowUp->OnSetFocus();
      break;
  }
}

void CGameMenu::KillAllFocuses(void)
{
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg) {
    itmg->mg_bFocused = FALSE;
  }
}

void CGameMenu::StartMenu(void)
{
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg)
  {
    itmg->mg_bFocused = FALSE;
    // call appear
    itmg->Appear();
  }

  // if there is a list
  if (gm_pmgListTop!=NULL) {
    // scroll it so that the wanted tem is centered
    gm_iListOffset = gm_iListWantedItem-gm_ctListVisible/2;
    // clamp the scrolling
    gm_iListOffset = Clamp(gm_iListOffset, 0L, Max(0L, gm_ctListTotal-gm_ctListVisible));

    // fill the list
    FillListItems();

    // for each menu gadget in menu
    FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg) {
      // if in list, but disabled
      if (itmg->mg_iInList==-2) {
        // hide it
        itmg->mg_bVisible = FALSE;
      // if in list
      } else if (itmg->mg_iInList>=0) {
        // show it
        itmg->mg_bVisible = TRUE;
      }
      // if wanted
      if (itmg->mg_iInList==gm_iListWantedItem) {
        // focus it
        itmg->OnSetFocus();
        gm_pmgSelectedByDefault = itmg;
      }
    }
  }
}

void CGameMenu::EndMenu(void)
{
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, gm_lhGadgets, itmg)
  {
    // call disappear
    itmg->Disappear();
  }
}

// return TRUE if handled
BOOL CGameMenu::OnKeyDown( int iVKey)
{
  // find curently active gadget
  CMenuGadget *pmgActive = NULL;
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if focused
    if( itmg->mg_bFocused) {
      // remember as active
      pmgActive = &itmg.Current();
    }
  }

  // if none focused
  if( pmgActive == NULL) {
    // do nothing
    return FALSE;
  }

  // if active gadget handles it
  if( pmgActive->OnKeyDown( iVKey)) {
    // key is handled
    return TRUE;
  }

  // process normal in menu movement
  switch( iVKey) {
  case VK_PRIOR:
    ScrollList(-2);
    return TRUE;
  case VK_NEXT:
    ScrollList(+2);
    return TRUE;

  case 11:
    ScrollList(-4);
    return TRUE;
  case 10:
    ScrollList(+4);
    return TRUE;

  case VK_UP:
    // if this is top button in list
    if (pmgActive==gm_pmgListTop) {
      // scroll list up
      ScrollList(-1);
      // key is handled
      return TRUE;
    }
    // if we can go up
    if(pmgActive->mg_pmgUp != NULL && pmgActive->mg_pmgUp->mg_bVisible) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      pmgActive = pmgActive->mg_pmgUp;
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_DOWN:
    // if this is bottom button in list
    if (pmgActive==gm_pmgListBottom) {
      // scroll list down
      ScrollList(+1);
      // key is handled
      return TRUE;
    }
    // if we can go down
    if(pmgActive->mg_pmgDown != NULL && pmgActive->mg_pmgDown->mg_bVisible) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      pmgActive = pmgActive->mg_pmgDown;
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_LEFT:
    // if we can go left
    if(pmgActive->mg_pmgLeft != NULL) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      if (!pmgActive->mg_pmgLeft->mg_bVisible && gm_pmgSelectedByDefault!=NULL) {
        pmgActive = gm_pmgSelectedByDefault;
      } else {
        pmgActive = pmgActive->mg_pmgLeft;
      }
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  case VK_RIGHT:
    // if we can go right
    if(pmgActive->mg_pmgRight != NULL) {
      // call lose focus to still active gadget and
      pmgActive->OnKillFocus();
      // set focus to new one
      if (!pmgActive->mg_pmgRight->mg_bVisible && gm_pmgSelectedByDefault!=NULL) {
        pmgActive = gm_pmgSelectedByDefault;
      } else {
        pmgActive = pmgActive->mg_pmgRight;
      }
      pmgActive->OnSetFocus();
      // key is handled
      return TRUE;
    }
    break;
  }

  // key is not handled
  return FALSE;
}

void CGameMenu::Think(void)
{
}

BOOL CGameMenu::OnChar(MSG msg)
{
  // find curently active gadget
  CMenuGadget *pmgActive = NULL;
  // for each menu gadget in menu
  FOREACHINLIST( CMenuGadget, mg_lnNode, pgmCurrentMenu->gm_lhGadgets, itmg) {
    // if focused
    if( itmg->mg_bFocused) {
      // remember as active
      pmgActive = &itmg.Current();
    }
  }

  // if none focused
  if( pmgActive == NULL) {
    // do nothing
    return FALSE;
  }

  // if active gadget handles it
  if( pmgActive->OnChar(msg)) {
    // key is handled
    return TRUE;
  }

  // key is not handled
  return FALSE;
}

// ------------------------ CConfirmMenu implementation
void InitActionsForConfirmMenu() {
	gmConfirmMenu.gm_mgConfirmYes.mg_pActivatedFunction = &ConfirmYes;
	gmConfirmMenu.gm_mgConfirmNo.mg_pActivatedFunction = &ConfirmNo;
}

// return TRUE if handled
BOOL CConfirmMenu::OnKeyDown(int iVKey)
{
  if (iVKey==VK_ESCAPE || iVKey==VK_RBUTTON) {
    ConfirmNo();
    return TRUE;
  }
  return CGameMenu::OnKeyDown(iVKey);
}

// ------------------------ CMainMenu implementation
void InitActionsForMainMenu() {
	gmMainMenu.gm_mgSingle.mg_pActivatedFunction = &StartSinglePlayerMenu;
	gmMainMenu.gm_mgNetwork.mg_pActivatedFunction = StartNetworkMenu;
	gmMainMenu.gm_mgSplitScreen.mg_pActivatedFunction = &StartSplitScreenMenu;
	gmMainMenu.gm_mgDemo.mg_pActivatedFunction = &StartDemoLoadMenu;
#if TECHTESTONLY
	gmMainMenu.gm_mgMods.mg_pActivatedFunction = &DisabledFunction;
#else
	gmMainMenu.gm_mgMods.mg_pActivatedFunction = &StartModsLoadMenu;
#endif
	gmMainMenu.gm_mgHighScore.mg_pActivatedFunction = &StartHighScoreMenu;
	gmMainMenu.gm_mgOptions.mg_pActivatedFunction = &StartOptionsMenu;
	gmMainMenu.gm_mgQuit.mg_pActivatedFunction = &ExitConfirm;
}

// ------------------------ CInGameMenu implementation
void InitActionsForInGameMenu() {
	
	gmInGameMenu.gm_mgQuickLoad.mg_pActivatedFunction = &StartCurrentQuickLoadMenu;
	gmInGameMenu.gm_mgQuickSave.mg_pActivatedFunction = &QuickSaveFromMenu;
	gmInGameMenu.gm_mgLoad.mg_pActivatedFunction = &StartCurrentLoadMenu;
	gmInGameMenu.gm_mgSave.mg_pActivatedFunction = &StartCurrentSaveMenu;
	gmInGameMenu.gm_mgHighScore.mg_pActivatedFunction = &StartHighScoreMenu;
	gmInGameMenu.gm_mgOptions.mg_pActivatedFunction = &StartOptionsMenu;
#if TECHTESTONLY
	gmInGameMenu.gm_mgStop.mg_pActivatedFunction = &ExitConfirm;
#else
	gmInGameMenu.gm_mgStop.mg_pActivatedFunction = &StopConfirm;
#endif
	gmInGameMenu.gm_mgQuit.mg_pActivatedFunction = &ExitConfirm;
}

extern void SetDemoStartStopRecText(void)
{
  if( _pNetwork->IsRecordingDemo())
  {
	gmInGameMenu.gm_mgDemoRec.SetText(TRANS("STOP RECORDING"));
	gmInGameMenu.gm_mgDemoRec.mg_strTip = TRANS("stop current recording");
	gmInGameMenu.gm_mgDemoRec.mg_pActivatedFunction = &StopRecordingDemo;
  }
  else
  {
    gmInGameMenu.gm_mgDemoRec.SetText(TRANS("RECORD DEMO"));
    gmInGameMenu.gm_mgDemoRec.mg_strTip = TRANS("start recording current game");
    gmInGameMenu.gm_mgDemoRec.mg_pActivatedFunction = &StartDemoSaveMenu;
  }
}


// ------------------------ CSinglePlayerMenu implementation
void InitActionsForSinglePlayerMenu() {
	gmSinglePlayerMenu.gm_mgNewGame.mg_pActivatedFunction = &StartSinglePlayerNewMenu;
#if _SE_DEMO || TECHTESTONLY
	gmSinglePlayerMenu.gm_mgCustom.mg_pActivatedFunction = &DisabledFunction;
#else
	gmSinglePlayerMenu.gm_mgCustom.mg_pActivatedFunction = &StartSelectLevelFromSingle;
#endif
	gmSinglePlayerMenu.gm_mgQuickLoad.mg_pActivatedFunction = &StartSinglePlayerQuickLoadMenu;
	gmSinglePlayerMenu.gm_mgLoad.mg_pActivatedFunction = &StartSinglePlayerLoadMenu;
	gmSinglePlayerMenu.gm_mgTraining.mg_pActivatedFunction = &StartTraining;
	gmSinglePlayerMenu.gm_mgTechTest.mg_pActivatedFunction = &StartTechTest;
	gmSinglePlayerMenu.gm_mgPlayersAndControls.mg_pActivatedFunction = &StartChangePlayerMenuFromSinglePlayer;
	gmSinglePlayerMenu.gm_mgOptions.mg_pActivatedFunction = &StartSinglePlayerGameOptions;
}

// ------------------------ CSinglePlayerNewMenu implementation
void InitActionsForSinglePlayerNewMenu() {
	gmSinglePlayerNewMenu.gm_mgTourist.mg_pActivatedFunction = &StartSinglePlayerGame_Tourist;
	gmSinglePlayerNewMenu.gm_mgEasy.mg_pActivatedFunction = &StartSinglePlayerGame_Easy;
	gmSinglePlayerNewMenu.gm_mgMedium.mg_pActivatedFunction = &StartSinglePlayerGame_Normal;
	gmSinglePlayerNewMenu.gm_mgHard.mg_pActivatedFunction = &StartSinglePlayerGame_Hard;
	gmSinglePlayerNewMenu.gm_mgSerious.mg_pActivatedFunction = &StartSinglePlayerGame_Serious;
	gmSinglePlayerNewMenu.gm_mgMental.mg_pActivatedFunction = &StartSinglePlayerGame_Mental;
}

// ------------------------ CDisabledMenu implementation

void ChangeCrosshair(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  pps->ps_iCrossHairType = iNew-1;
}
void ChangeWeaponSelect(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  pps->ps_iWeaponAutoSelect = iNew;
}
void ChangeWeaponHide(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_HIDEWEAPON;
  } else {
    pps->ps_ulFlags &= ~PSF_HIDEWEAPON;
  }
}
void Change3rdPerson(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_PREFER3RDPERSON;
  } else {
    pps->ps_ulFlags &= ~PSF_PREFER3RDPERSON;
  }
}
void ChangeQuotes(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_NOQUOTES;
  } else {
    pps->ps_ulFlags |= PSF_NOQUOTES;
  }
}
void ChangeAutoSave(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_AUTOSAVE;
  } else {
    pps->ps_ulFlags &= ~PSF_AUTOSAVE;
  }
}
void ChangeCompDoubleClick(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_COMPSINGLECLICK;
  } else {
    pps->ps_ulFlags |= PSF_COMPSINGLECLICK;
  }
}

void ChangeViewBobbing(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags &= ~PSF_NOBOBBING;
  } else {
    pps->ps_ulFlags |= PSF_NOBOBBING;
  }
}

void ChangeSharpTurning(INDEX iNew)
{
  INDEX iPlayer = *gmPlayerProfile.gm_piCurrentPlayer;
  CPlayerSettings *pps = (CPlayerSettings *)_pGame->gm_apcPlayers[iPlayer].pc_aubAppearance;
  if (iNew) {
    pps->ps_ulFlags |= PSF_SHARPTURNING;
  } else {
    pps->ps_ulFlags &= ~PSF_SHARPTURNING;
  }
}

// ------------------------ CPlayerProfileMenu implementation
extern void PPOnPlayerSelect(void)
{
	ASSERT(_pmgLastActivatedGadget != NULL);
	if (_pmgLastActivatedGadget->mg_bEnabled) {
		gmPlayerProfile.SelectPlayer(((CMGButton *)_pmgLastActivatedGadget)->mg_iIndex);
	}
}

void InitActionsForPlayerProfileMenu()
{
	gmPlayerProfile.gm_mgCrosshair.mg_pOnTriggerChange = ChangeCrosshair;
	gmPlayerProfile.gm_mgWeaponSelect.mg_pOnTriggerChange = ChangeWeaponSelect;
	gmPlayerProfile.gm_mgWeaponHide.mg_pOnTriggerChange = ChangeWeaponHide;
	gmPlayerProfile.gm_mg3rdPerson.mg_pOnTriggerChange = Change3rdPerson;
	gmPlayerProfile.gm_mgQuotes.mg_pOnTriggerChange = ChangeQuotes;
	gmPlayerProfile.gm_mgAutoSave.mg_pOnTriggerChange = ChangeAutoSave;
	gmPlayerProfile.gm_mgCompDoubleClick.mg_pOnTriggerChange = ChangeCompDoubleClick;
	gmPlayerProfile.gm_mgSharpTurning.mg_pOnTriggerChange = ChangeSharpTurning;
	gmPlayerProfile.gm_mgViewBobbing.mg_pOnTriggerChange = ChangeViewBobbing;
	gmPlayerProfile.gm_mgCustomizeControls.mg_pActivatedFunction = &StartControlsMenuFromPlayer;
	gmPlayerProfile.gm_mgModel.mg_pActivatedFunction = &StartPlayerModelLoadMenu;
}

// ------------------------ CControlsMenu implementation
void InitActionsForControlsMenu()
{
	gmControls.gm_mgButtons.mg_pActivatedFunction = &StartCustomizeKeyboardMenu;
	gmControls.gm_mgAdvanced.mg_pActivatedFunction = &StartCustomizeAxisMenu;
	gmControls.gm_mgPredefined.mg_pActivatedFunction = &StartControlsLoadMenu;
}

// ------------------------ CCustomizeAxisMenu implementation
void PreChangeAxis(INDEX iDummy)
{
	gmCustomizeAxisMenu.ApplyActionSettings();
}
void PostChangeAxis(INDEX iDummy)
{
	gmCustomizeAxisMenu.ObtainActionSettings();
}

void InitActionsForCustomizeAxisMenu() {
	gmCustomizeAxisMenu.gm_mgActionTrigger.mg_pPreTriggerChange = PreChangeAxis;
	gmCustomizeAxisMenu.gm_mgActionTrigger.mg_pOnTriggerChange = PostChangeAxis;
}

// ------------------------ COptionsMenu implementation
void InitActionsForOptionsMenu()
{
	gmOptionsMenu.gm_mgVideoOptions.mg_pActivatedFunction = &StartVideoOptionsMenu;
	gmOptionsMenu.gm_mgAudioOptions.mg_pActivatedFunction = &StartAudioOptionsMenu;
	gmOptionsMenu.gm_mgPlayerProfileOptions.mg_pActivatedFunction = &StartChangePlayerMenuFromOptions;
	gmOptionsMenu.gm_mgNetworkOptions.mg_pActivatedFunction = &StartNetworkSettingsMenu;
	gmOptionsMenu.gm_mgCustomOptions.mg_pActivatedFunction = &StartCustomLoadMenu;
	gmOptionsMenu.gm_mgAddonOptions.mg_pActivatedFunction = &StartAddonsLoadMenu;
}

// ------------------------ CVideoOptionsMenu implementation
static void FillResolutionsList(void)
{
  // free resolutions
  if (_astrResolutionTexts!=NULL) {
    delete [] _astrResolutionTexts;
  }
  if (_admResolutionModes!=NULL) {
    delete [] _admResolutionModes;
  }
  _ctResolutions = 0;

  // if window
  if (gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_iSelected == 0) {
    // always has fixed resolutions, but not greater than desktop
    static PIX apixWidths[][2] = {
       320, 240,
       400, 300,
       512, 384,
       640, 240,
       640, 480,
       720, 540,
       800, 300,
       800, 600,
       960, 720,
      1024, 384,
      1024, 768,
      1152, 864,
      1280, 480,
      1280, 960,
      1600, 600,
      1600,1200,
      1920, 720,
      1920,1440,
      2048, 786,
      2048,1536,
    };
    _ctResolutions = ARRAYCOUNT(apixWidths);
    _astrResolutionTexts = new CTString    [_ctResolutions];
    _admResolutionModes  = new CDisplayMode[_ctResolutions];
    extern PIX _pixDesktopWidth;
    INDEX iRes=0;
    for( ; iRes<_ctResolutions; iRes++) {
      if( apixWidths[iRes][0]>_pixDesktopWidth) break;
      SetResolutionInList( iRes, apixWidths[iRes][0], apixWidths[iRes][1]);
    }
    _ctResolutions = iRes;

  // if fullscreen
  } else {
    // get resolutions list from engine
    CDisplayMode *pdm = _pGfx->EnumDisplayModes(_ctResolutions, 
		SwitchToAPI(gmVideoOptionsMenu.gm_mgDisplayAPITrigger.mg_iSelected), gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.mg_iSelected);
    // allocate that much
    _astrResolutionTexts = new CTString    [_ctResolutions];
    _admResolutionModes  = new CDisplayMode[_ctResolutions];
    // for each resolution
    for( INDEX iRes=0; iRes<_ctResolutions; iRes++) {
      // add it to list
      SetResolutionInList( iRes, pdm[iRes].dm_pixSizeI, pdm[iRes].dm_pixSizeJ);
    }
  }
  gmVideoOptionsMenu.gm_mgResolutionsTrigger.mg_astrTexts = _astrResolutionTexts;
  gmVideoOptionsMenu.gm_mgResolutionsTrigger.mg_ctTexts = _ctResolutions;
}


static void FillAdaptersList(void)
{
  if (_astrAdapterTexts!=NULL) {
    delete [] _astrAdapterTexts;
  }
  _ctAdapters = 0;

  INDEX iApi = SwitchToAPI(gmVideoOptionsMenu.gm_mgDisplayAPITrigger.mg_iSelected);
  _ctAdapters = _pGfx->gl_gaAPI[iApi].ga_ctAdapters;
  _astrAdapterTexts = new CTString[_ctAdapters];
  for(INDEX iAdapter = 0; iAdapter<_ctAdapters; iAdapter++) {
    _astrAdapterTexts[iAdapter] = _pGfx->gl_gaAPI[iApi].ga_adaAdapter[iAdapter].da_strRenderer;
  }
  gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.mg_astrTexts = _astrAdapterTexts;
  gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.mg_ctTexts = _ctAdapters;
}


extern void UpdateVideoOptionsButtons(INDEX iSelected)
{
  const BOOL _bVideoOptionsChanged = (iSelected != -1);

  const BOOL bOGLEnabled = _pGfx->HasAPI(GAT_OGL);
#ifdef SE1_D3D
  const BOOL bD3DEnabled = _pGfx->HasAPI(GAT_D3D);
  ASSERT( bOGLEnabled || bD3DEnabled); 
#else // 
  ASSERT( bOGLEnabled ); 
#endif // SE1_D3D
  CDisplayAdapter &da = _pGfx->gl_gaAPI[SwitchToAPI(gmVideoOptionsMenu.gm_mgDisplayAPITrigger.mg_iSelected)]
	  .ga_adaAdapter[gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.mg_iSelected];

  // number of available preferences is higher if video setup is custom
  gmVideoOptionsMenu.gm_mgDisplayPrefsTrigger.mg_ctTexts = 3;
  if (sam_iVideoSetup == 3) gmVideoOptionsMenu.gm_mgDisplayPrefsTrigger.mg_ctTexts++;

  // enumerate adapters
  FillAdaptersList();

  // show or hide buttons
  gmVideoOptionsMenu.gm_mgDisplayAPITrigger.mg_bEnabled = bOGLEnabled
#ifdef SE1_D3D
    && bD3DEnabled
#endif // SE1_D3D
    ;
  gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.mg_bEnabled = _ctAdapters>1;
  gmVideoOptionsMenu.gm_mgApply.mg_bEnabled = _bVideoOptionsChanged;
  // determine which should be visible
  gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_bEnabled = TRUE;
  if( da.da_ulFlags&DAF_FULLSCREENONLY) {
	gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_bEnabled = FALSE;
	gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_iSelected = 1;
	gmVideoOptionsMenu.gm_mgFullScreenTrigger.ApplyCurrentSelection();
  }
  gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.mg_bEnabled = TRUE;
  if (gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_iSelected == 0) {
	gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.mg_bEnabled = FALSE;
	gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch(DD_DEFAULT);
	gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
  } else if( da.da_ulFlags&DAF_16BITONLY) {
	gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.mg_bEnabled = FALSE;
	gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch(DD_16BIT);
    gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
  }

  // remember current selected resolution
  PIX pixSizeI, pixSizeJ;
  ResolutionToSize(gmVideoOptionsMenu.gm_mgResolutionsTrigger.mg_iSelected, pixSizeI, pixSizeJ);

  // select same resolution again if possible
  FillResolutionsList();
  SizeToResolution(pixSizeI, pixSizeJ, gmVideoOptionsMenu.gm_mgResolutionsTrigger.mg_iSelected);

  // apply adapter and resolutions
  gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.ApplyCurrentSelection();
  gmVideoOptionsMenu.gm_mgResolutionsTrigger.ApplyCurrentSelection();
}


extern void InitVideoOptionsButtons(void)
{
  if( sam_bFullScreenActive) {
	gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_iSelected = 1;
  } else {
	gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_iSelected = 0;
  }

  gmVideoOptionsMenu.gm_mgDisplayAPITrigger.mg_iSelected = APIToSwitch((GfxAPIType)(INDEX)sam_iGfxAPI);
  gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.mg_iSelected = sam_iDisplayAdapter;
  gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.mg_iSelected = DepthToSwitch((enum DisplayDepth)(INDEX)sam_iDisplayDepth);

  FillResolutionsList();
  SizeToResolution(sam_iScreenSizeI, sam_iScreenSizeJ, gmVideoOptionsMenu.gm_mgResolutionsTrigger.mg_iSelected);
  gmVideoOptionsMenu.gm_mgDisplayPrefsTrigger.mg_iSelected = Clamp(int(sam_iVideoSetup), 0, 3);

  gmVideoOptionsMenu.gm_mgFullScreenTrigger.ApplyCurrentSelection();
  gmVideoOptionsMenu.gm_mgDisplayPrefsTrigger.ApplyCurrentSelection();
  gmVideoOptionsMenu.gm_mgDisplayAPITrigger.ApplyCurrentSelection();
  gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.ApplyCurrentSelection();
  gmVideoOptionsMenu.gm_mgResolutionsTrigger.ApplyCurrentSelection();
  gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.ApplyCurrentSelection();
}

void InitActionsForVideoOptionsMenu()
{
	gmVideoOptionsMenu.gm_mgDisplayPrefsTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	gmVideoOptionsMenu.gm_mgDisplayAPITrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	gmVideoOptionsMenu.gm_mgDisplayAdaptersTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	gmVideoOptionsMenu.gm_mgFullScreenTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	gmVideoOptionsMenu.gm_mgResolutionsTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	gmVideoOptionsMenu.gm_mgBitsPerPixelTrigger.mg_pOnTriggerChange = &UpdateVideoOptionsButtons;
	gmVideoOptionsMenu.gm_mgVideoRendering.mg_pActivatedFunction = &StartRenderingOptionsMenu;
	gmVideoOptionsMenu.gm_mgApply.mg_pActivatedFunction = &ApplyVideoOptions;
}

// ------------------------ CAudioOptionsMenu implementation
static void OnWaveVolumeChange(INDEX iCurPos)
{
  _pShell->SetFLOAT("snd_fSoundVolume", iCurPos/FLOAT(VOLUME_STEPS));
}

void WaveSliderChange(void)
{
  if (_bMouseRight) {
    gmAudioOptionsMenu.gm_mgWaveVolume.mg_iCurPos += 5;
  } else {
    gmAudioOptionsMenu.gm_mgWaveVolume.mg_iCurPos -= 5;
  }
  gmAudioOptionsMenu.gm_mgWaveVolume.ApplyCurrentPosition();
}

void FrequencyTriggerChange(INDEX iDummy)
{
  sam_bAutoAdjustAudio = 0;
  gmAudioOptionsMenu.gm_mgAudioAutoTrigger.mg_iSelected = 0;
  gmAudioOptionsMenu.gm_mgAudioAutoTrigger.ApplyCurrentSelection();
}

void MPEGSliderChange(void)
{
  if (_bMouseRight) {
	gmAudioOptionsMenu.gm_mgMPEGVolume.mg_iCurPos += 5;
  } else {
	gmAudioOptionsMenu.gm_mgMPEGVolume.mg_iCurPos -= 5;
  }
  gmAudioOptionsMenu.gm_mgMPEGVolume.ApplyCurrentPosition();
}

static void OnMPEGVolumeChange(INDEX iCurPos)
{
	_pShell->SetFLOAT("snd_fMusicVolume", iCurPos / FLOAT(VOLUME_STEPS));
}

void InitActionsForAudioOptionsMenu()
{
  gmAudioOptionsMenu.gm_mgFrequencyTrigger.mg_pOnTriggerChange = FrequencyTriggerChange;
  gmAudioOptionsMenu.gm_mgWaveVolume.mg_pOnSliderChange = &OnWaveVolumeChange;
  gmAudioOptionsMenu.gm_mgWaveVolume.mg_pActivatedFunction = WaveSliderChange;
  gmAudioOptionsMenu.gm_mgMPEGVolume.mg_pOnSliderChange = &OnMPEGVolumeChange;
  gmAudioOptionsMenu.gm_mgMPEGVolume.mg_pActivatedFunction = MPEGSliderChange;
  gmAudioOptionsMenu.gm_mgApply.mg_pActivatedFunction = &ApplyAudioOptions;
}

// ------------------------ CVarMenu implementation
void VarApply(void)
{
  FlushVarSettings(TRUE);
  gmVarMenu.EndMenu();
  gmVarMenu.StartMenu();
}

void InitActionsForVarMenu() {
	gmVarMenu.gm_mgApply.mg_pActivatedFunction = &VarApply;
}

// ------------------------ CServersMenu implementation
extern void RefreshServerList(void)
{
  _pNetwork->EnumSessions(gmServersMenu.m_bInternet);
}

void RefreshServerListManually(void)
{
  ChangeToMenu(&gmServersMenu); // this refreshes the list and sets focuses
}

void SortByColumn(int i)
{
  if (gmServersMenu.gm_mgList.mg_iSort==i) {
	gmServersMenu.gm_mgList.mg_bSortDown = !gmServersMenu.gm_mgList.mg_bSortDown;
  } else {
	gmServersMenu.gm_mgList.mg_bSortDown = FALSE;
  }
  gmServersMenu.gm_mgList.mg_iSort = i;
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
	gmServersMenu.gm_mgRefresh.mg_pActivatedFunction = &RefreshServerList;

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
	gmNetworkMenu.gm_mgJoin.mg_pActivatedFunction = &StartNetworkJoinMenu;
	gmNetworkMenu.gm_mgStart.mg_pActivatedFunction = &StartNetworkStartMenu;
	gmNetworkMenu.gm_mgQuickLoad.mg_pActivatedFunction = &StartNetworkQuickLoadMenu;
	gmNetworkMenu.gm_mgLoad.mg_pActivatedFunction = &StartNetworkLoadMenu;
}

// ------------------------ CNetworkJoinMenu implementation
void InitActionsForNetworkJoinMenu()
{
	gmNetworkJoinMenu.gm_mgLAN.mg_pActivatedFunction = &StartSelectServerLAN;
	gmNetworkJoinMenu.gm_mgNET.mg_pActivatedFunction = &StartSelectServerNET;
	gmNetworkJoinMenu.gm_mgOpen.mg_pActivatedFunction = &StartNetworkOpenMenu;
}

// ------------------------ CNetworkStartMenu implementation
extern void UpdateNetworkLevel(INDEX iDummy)
{
	ValidateLevelForFlags(_pGame->gam_strCustomLevel,
		GetSpawnFlagsForGameType(gmNetworkStartMenu.gm_mgGameType.mg_iSelected));
	gmNetworkStartMenu.gm_mgLevel.mg_strText = FindLevelByFileName(_pGame->gam_strCustomLevel).li_strName;
}

void InitActionsForNetworkStartMenu()
{
	gmNetworkStartMenu.gm_mgLevel.mg_pActivatedFunction = &StartSelectLevelFromNetwork;
	gmNetworkStartMenu.gm_mgGameOptions.mg_pActivatedFunction = &StartGameOptionsFromNetwork;
	gmNetworkStartMenu.gm_mgStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromNetwork;
}

//
#define ADD_GADGET( gd, box, up, dn, lf, rt, txt) \
  gd.mg_boxOnScreen = box;\
  gd.mg_pmgUp = up;\
  gd.mg_pmgDown = dn;\
  gd.mg_pmgLeft = lf;\
  gd.mg_pmgRight = rt;\
  gd.mg_strText = txt;\
  gm_lhGadgets.AddTail( gd.mg_lnNode);

#define SET_CHGPLR( gd, iplayer, bnone, bauto, pmgit) \
  gd.mg_pmgInfoTable = pmgit;\
  gd.mg_bResetToNone = bnone;\
  gd.mg_bAutomatic   = bauto;\
  gd.mg_iLocalPlayer = iplayer;

// ------------------------ CSelectPlayersMenu implementation
INDEX FindUnusedPlayer(void)
{
  INDEX *ai = _pGame->gm_aiMenuLocalPlayers;
  INDEX iPlayer=0;
  for(; iPlayer<8; iPlayer++) {
    BOOL bUsed = FALSE;
    for (INDEX iLocal=0; iLocal<4; iLocal++) {
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

  gmSelectPlayersMenu.gm_mgPlayer0Change.mg_iLocalPlayer = 0;
  gmSelectPlayersMenu.gm_mgPlayer1Change.mg_iLocalPlayer = 1;
  gmSelectPlayersMenu.gm_mgPlayer2Change.mg_iLocalPlayer = 2;
  gmSelectPlayersMenu.gm_mgPlayer3Change.mg_iLocalPlayer = 3;

  if (gmSelectPlayersMenu.gm_bAllowDedicated && _pGame->gm_MenuSplitScreenCfg==CGame::SSC_DEDICATED) {
	gmSelectPlayersMenu.gm_mgDedicated.mg_iSelected = 1;
  } else {
	gmSelectPlayersMenu.gm_mgDedicated.mg_iSelected = 0;
  }
  gmSelectPlayersMenu.gm_mgDedicated.ApplyCurrentSelection();

  if (gmSelectPlayersMenu.gm_bAllowObserving && _pGame->gm_MenuSplitScreenCfg==CGame::SSC_OBSERVER) {
	gmSelectPlayersMenu.gm_mgObserver.mg_iSelected = 1;
  } else {
	gmSelectPlayersMenu.gm_mgObserver.mg_iSelected = 0;
  }
  gmSelectPlayersMenu.gm_mgObserver.ApplyCurrentSelection();

  if (_pGame->gm_MenuSplitScreenCfg>=CGame::SSC_PLAY1) {
	gmSelectPlayersMenu.gm_mgSplitScreenCfg.mg_iSelected = _pGame->gm_MenuSplitScreenCfg;
	gmSelectPlayersMenu.gm_mgSplitScreenCfg.ApplyCurrentSelection();
  }

  BOOL bHasDedicated = gmSelectPlayersMenu.gm_bAllowDedicated;
  BOOL bHasObserver = gmSelectPlayersMenu.gm_bAllowObserving;
  BOOL bHasPlayers = TRUE;

  if (bHasDedicated && gmSelectPlayersMenu.gm_mgDedicated.mg_iSelected) {
    bHasObserver = FALSE;
    bHasPlayers = FALSE;
  }

  if (bHasObserver && gmSelectPlayersMenu.gm_mgObserver.mg_iSelected) {
    bHasPlayers = FALSE;
  }

  CMenuGadget *apmg[8];
  memset(apmg, 0, sizeof(apmg));
  INDEX i=0;

  if (bHasDedicated) {
	gmSelectPlayersMenu.gm_mgDedicated.Appear();
	apmg[i++] = &gmSelectPlayersMenu.gm_mgDedicated;
  } else {
	gmSelectPlayersMenu.gm_mgDedicated.Disappear();
  }
  if (bHasObserver) {
	gmSelectPlayersMenu.gm_mgObserver.Appear();
	apmg[i++] = &gmSelectPlayersMenu.gm_mgObserver;
  } else {
	gmSelectPlayersMenu.gm_mgObserver.Disappear();
  }

  for (INDEX iLocal=0; iLocal<4; iLocal++) {
    if (ai[iLocal]<0 || ai[iLocal]>7) {
      ai[iLocal] = 0;
    }
    for (INDEX iCopy=0; iCopy<iLocal; iCopy++) {
      if (ai[iCopy]==ai[iLocal]) {
        ai[iLocal] = FindUnusedPlayer();
      }
    }
  }

  gmSelectPlayersMenu.gm_mgPlayer0Change.Disappear();
  gmSelectPlayersMenu.gm_mgPlayer1Change.Disappear();
  gmSelectPlayersMenu.gm_mgPlayer2Change.Disappear();
  gmSelectPlayersMenu.gm_mgPlayer3Change.Disappear();

  if (bHasPlayers) {
	gmSelectPlayersMenu.gm_mgSplitScreenCfg.Appear();
	apmg[i++] = &gmSelectPlayersMenu.gm_mgSplitScreenCfg;
	gmSelectPlayersMenu.gm_mgPlayer0Change.Appear();
	apmg[i++] = &gmSelectPlayersMenu.gm_mgPlayer0Change;
	if (gmSelectPlayersMenu.gm_mgSplitScreenCfg.mg_iSelected >= 1) {
	  gmSelectPlayersMenu.gm_mgPlayer1Change.Appear();
	  apmg[i++] = &gmSelectPlayersMenu.gm_mgPlayer1Change;
    }
	if (gmSelectPlayersMenu.gm_mgSplitScreenCfg.mg_iSelected >= 2) {
	  gmSelectPlayersMenu.gm_mgPlayer2Change.Appear();
	  apmg[i++] = &gmSelectPlayersMenu.gm_mgPlayer2Change;
    }
	if (gmSelectPlayersMenu.gm_mgSplitScreenCfg.mg_iSelected >= 3) {
	  gmSelectPlayersMenu.gm_mgPlayer3Change.Appear();
	  apmg[i++] = &gmSelectPlayersMenu.gm_mgPlayer3Change;
    }
  } else {
	gmSelectPlayersMenu.gm_mgSplitScreenCfg.Disappear();
  }
  apmg[i++] = &gmSelectPlayersMenu.gm_mgStart;

  // relink
  for (INDEX img=0; img<8; img++) {
    if (apmg[img]==NULL) {
      continue;
    }
    INDEX imgPred=(img+8-1)%8;
    for (; imgPred!=img; imgPred = (imgPred+8-1)%8) {
      if (apmg[imgPred]!=NULL) {
        break;
      }
    }
    INDEX imgSucc=(img+1)%8;
    for (; imgSucc!=img; imgSucc = (imgSucc+1)%8) {
      if (apmg[imgSucc]!=NULL) {
        break;
      }
    }
    apmg[img]->mg_pmgUp   = apmg[imgPred];
    apmg[img]->mg_pmgDown = apmg[imgSucc];
  }

  gmSelectPlayersMenu.gm_mgPlayer0Change.SetPlayerText();
  gmSelectPlayersMenu.gm_mgPlayer1Change.SetPlayerText();
  gmSelectPlayersMenu.gm_mgPlayer2Change.SetPlayerText();
  gmSelectPlayersMenu.gm_mgPlayer3Change.SetPlayerText();

  if (bHasPlayers && gmSelectPlayersMenu.gm_mgSplitScreenCfg.mg_iSelected>=1) {
	gmSelectPlayersMenu.gm_mgNotes.mg_strText = TRANS("Make sure you set different controls for each player!");
  } else {
	gmSelectPlayersMenu.gm_mgNotes.mg_strText = "";
  }
}

extern void SelectPlayersApplyMenu(void)
{
	if (gmSelectPlayersMenu.gm_bAllowDedicated && gmSelectPlayersMenu.gm_mgDedicated.mg_iSelected) {
    _pGame->gm_MenuSplitScreenCfg = CGame::SSC_DEDICATED;
    return;
  }

  if (gmSelectPlayersMenu.gm_bAllowObserving && gmSelectPlayersMenu.gm_mgObserver.mg_iSelected) {
    _pGame->gm_MenuSplitScreenCfg = CGame::SSC_OBSERVER;
    return;
  }

  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) gmSelectPlayersMenu.gm_mgSplitScreenCfg.mg_iSelected;
}

void UpdateSelectPlayers(INDEX i)
{
  SelectPlayersApplyMenu();
  SelectPlayersFillMenu();
}

void InitActionsForSelectPlayersMenu()
{
	gmSelectPlayersMenu.gm_mgDedicated.mg_pOnTriggerChange = UpdateSelectPlayers;
	gmSelectPlayersMenu.gm_mgObserver.mg_pOnTriggerChange = UpdateSelectPlayers;
	gmSelectPlayersMenu.gm_mgSplitScreenCfg.mg_pOnTriggerChange = UpdateSelectPlayers;
}

// ------------------------ CNetworkOpenMenu implementation
void InitActionsForNetworkOpenMenu()
{
	gmNetworkOpenMenu.gm_mgJoin.mg_pActivatedFunction = &StartSelectPlayersMenuFromOpen;
}

// ------------------------ CSplitScreenMenu implementation
void InitActionsForSplitScreenMenu()
{
	gmSplitScreenMenu.gm_mgStart.mg_pActivatedFunction = &StartSplitStartMenu;
	gmSplitScreenMenu.gm_mgQuickLoad.mg_pActivatedFunction = &StartSplitScreenQuickLoadMenu;
	gmSplitScreenMenu.gm_mgLoad.mg_pActivatedFunction = &StartSplitScreenLoadMenu;
}

// ------------------------ CSplitStartMenu implementation
void InitActionsForSplitStartMenu()
{
	gmSplitStartMenu.gm_mgLevel.mg_pActivatedFunction = &StartSelectLevelFromSplit;
	gmSplitStartMenu.gm_mgOptions.mg_pActivatedFunction = &StartGameOptionsFromSplitScreen;
	gmSplitStartMenu.gm_mgStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromSplit;
}

extern void UpdateSplitLevel(INDEX iDummy)
{
	ValidateLevelForFlags(_pGame->gam_strCustomLevel,
		GetSpawnFlagsForGameType(gmSplitStartMenu.gm_mgGameType.mg_iSelected));
	gmSplitStartMenu.gm_mgLevel.mg_strText = FindLevelByFileName(_pGame->gam_strCustomLevel).li_strName;
}