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

#include "GUI/Components/MGArrow.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGChangePlayer.h"
#include "GUI/Components/MGEdit.h"
#include "GUI/Components/MGFileButton.h"
#include "GUI/Components/MGHighScore.h"
#include "GUI/Components/MGKeyDefinition.h"
#include "GUI/Components/MGLevelButton.h"
#include "GUI/Components/MGModel.h"
#include "GUI/Components/MGServerList.h"
#include "GUI/Components/MGSlider.h"
#include "GUI/Components/MGTitle.h"
#include "GUI/Components/MGTrigger.h"
#include "GUI/Components/MGVarButton.h"

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
#include "Menu_starters.h"

// macros for translating radio button text arrays
#define TRANSLATERADIOARRAY(array) TranslateRadioTexts(array, ARRAYCOUNT(array))

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

void FixupBackButton(CGameMenu *pgm);

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

// Menus
CConfirmMenu gmConfirmMenu;
CMainMenu gmMainMenu;
CInGameMenu gmInGameMenu;
CSinglePlayerMenu gmSinglePlayerMenu;
CSinglePlayerNewMenu gmSinglePlayerNewMenu;
CDisabledMenu gmDisabledFunction;
CLevelsMenu gmLevelsMenu;
CVarMenu gmVarMenu;
CPlayerProfileMenu gmPlayerProfile;
CControlsMenu gmControls;
CLoadSaveMenu gmLoadSaveMenu;
CHighScoreMenu gmHighScoreMenu;
CCustomizeKeyboardMenu gmCustomizeKeyboardMenu;
CServersMenu gmServersMenu;
CCustomizeAxisMenu gmCustomizeAxisMenu;
COptionsMenu gmOptionsMenu;
CVideoOptionsMenu gmVideoOptionsMenu;
CAudioOptionsMenu gmAudioOptionsMenu;
CNetworkMenu gmNetworkMenu;
CNetworkJoinMenu gmNetworkJoinMenu;
CNetworkStartMenu gmNetworkStartMenu;
CNetworkOpenMenu gmNetworkOpenMenu;
CSplitScreenMenu gmSplitScreenMenu;
CSplitStartMenu gmSplitStartMenu;
CSelectPlayersMenu gmSelectPlayersMenu;

// -------- console variable adjustment menu
extern BOOL _bVarChanged = FALSE;

extern void PlayMenuSound(CSoundData *psd)
{
  if (_psoMenuSound!=NULL && !_psoMenuSound->IsPlaying()) {
    _psoMenuSound->Play(psd, SOF_NONGAME);
  }
}

// translate all texts in array for one radio button
void TranslateRadioTexts(CTString astr[], INDEX ct)
{
  for (INDEX i=0; i<ct; i++) {
    astr[i] = TranslateConst(astr[i], 4);
  }
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



void StopCurrentGame(void)
{
  _pGame->StopGame();
  _gmRunningGameMode=GM_NONE;
  StopMenus(TRUE);
  StartMenus("");
}

void DisabledFunction(void)
{
  gmDisabledFunction.gm_pgmParentMenu = pgmCurrentMenu;
  gmDisabledFunction.gm_mgButton.mg_strText = TRANS("The feature is not available in this version!");
  gmDisabledFunction.gm_mgTitle.mg_strText = TRANS("DISABLED");
  ChangeToMenu( &gmDisabledFunction);
}

extern void InitVideoOptionsButtons(void);
extern void UpdateVideoOptionsButtons(INDEX i);



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