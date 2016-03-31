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
#include "MenuStuff.h"
#include "MPlayerProfile.h"
#include "GUI/Menus/MenuManager.h"

#define ADD_SELECT_PLAYER_MG( index, mg, mgprev, mgnext, me)\
  mg.mg_iIndex = index; \
  mg.mg_bfsFontSize = BFS_MEDIUM; \
  mg.mg_boxOnScreen = BoxNoUp(index); \
  mg.mg_bRectangle = TRUE; \
  mg.mg_pmgLeft = &mgprev; \
  mg.mg_pmgRight = &mgnext; \
  mg.mg_pmgUp = &gm_mgCustomizeControls; \
  mg.mg_pmgDown = &gm_mgNameField; \
  mg.mg_pActivatedFunction = &PPOnPlayerSelect; \
  mg.mg_strText = #index; \
  mg.mg_strTip = TRANS("select new currently active player"); \
  gm_lhGadgets.AddTail(mg.mg_lnNode);

extern BOOL  _bPlayerMenuFromSinglePlayer;
extern CTString _strLastPlayerAppearance;
extern void PPOnPlayerSelect(void);


void CPlayerProfileMenu::Initialize_t(void)
{
  // intialize player and controls menu
  _bPlayerMenuFromSinglePlayer = FALSE;
  gm_mgProfileTitle.mg_boxOnScreen = BoxTitle();
  gm_mgProfileTitle.mg_strText = TRANS("PLAYER PROFILE");
  gm_lhGadgets.AddTail(gm_mgProfileTitle.mg_lnNode);

  gm_mgNoLabel.mg_strText = TRANS("PROFILE:");
  gm_mgNoLabel.mg_boxOnScreen = BoxMediumLeft(0.0f);
  gm_mgNoLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgNoLabel.mg_iCenterI = -1;
  gm_lhGadgets.AddTail(gm_mgNoLabel.mg_lnNode);

  ADD_SELECT_PLAYER_MG(0, gm_mgNumber[0], gm_mgNumber[7], gm_mgNumber[1], gm_mgNumber[0]);
  ADD_SELECT_PLAYER_MG(1, gm_mgNumber[1], gm_mgNumber[0], gm_mgNumber[2], gm_mgNumber[1]);
  ADD_SELECT_PLAYER_MG(2, gm_mgNumber[2], gm_mgNumber[1], gm_mgNumber[3], gm_mgNumber[2]);
  ADD_SELECT_PLAYER_MG(3, gm_mgNumber[3], gm_mgNumber[2], gm_mgNumber[4], gm_mgNumber[3]);
  ADD_SELECT_PLAYER_MG(4, gm_mgNumber[4], gm_mgNumber[3], gm_mgNumber[5], gm_mgNumber[4]);
  ADD_SELECT_PLAYER_MG(5, gm_mgNumber[5], gm_mgNumber[4], gm_mgNumber[6], gm_mgNumber[5]);
  ADD_SELECT_PLAYER_MG(6, gm_mgNumber[6], gm_mgNumber[5], gm_mgNumber[7], gm_mgNumber[6]);
  ADD_SELECT_PLAYER_MG(7, gm_mgNumber[7], gm_mgNumber[6], gm_mgNumber[0], gm_mgNumber[7]);
  gm_mgNumber[7].mg_pmgRight = &gm_mgModel;

  gm_mgNameLabel.mg_strText = TRANS("NAME:");
  gm_mgNameLabel.mg_boxOnScreen = BoxMediumLeft(1.25f);
  gm_mgNameLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgNameLabel.mg_iCenterI = -1;
  gm_lhGadgets.AddTail(gm_mgNameLabel.mg_lnNode);

  // setup of player name button is done on start menu
  gm_mgNameField.mg_strText = "<???>";
  gm_mgNameField.mg_ctMaxStringLen = 25;
  gm_mgNameField.mg_boxOnScreen = BoxPlayerEdit(1.25);
  gm_mgNameField.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgNameField.mg_iCenterI = -1;
  gm_mgNameField.mg_pmgUp = &gm_mgNumber[0];
  gm_mgNameField.mg_pmgDown = &gm_mgTeam;
  gm_mgNameField.mg_pmgRight = &gm_mgModel;
  gm_mgNameField.mg_strTip = TRANS("rename currently active player");
  gm_lhGadgets.AddTail(gm_mgNameField.mg_lnNode);

  gm_mgTeamLabel.mg_strText = TRANS("TEAM:");
  gm_mgTeamLabel.mg_boxOnScreen = BoxMediumLeft(2.25f);
  gm_mgTeamLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgTeamLabel.mg_iCenterI = -1;
  gm_lhGadgets.AddTail(gm_mgTeamLabel.mg_lnNode);

  // setup of player name button is done on start menu
  gm_mgTeam.mg_strText = "<???>";
  gm_mgTeam.mg_ctMaxStringLen = 25;
  gm_mgTeam.mg_boxOnScreen = BoxPlayerEdit(2.25f);
  gm_mgTeam.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgTeam.mg_iCenterI = -1;
  gm_mgTeam.mg_pmgUp = gm_mgNameField.mg_pmgUp = &gm_mgNumber[0];

  gm_mgTeam.mg_pmgDown = &gm_mgCrosshair;
  gm_mgTeam.mg_pmgRight = &gm_mgModel;
  //gm_mgTeam.mg_strTip = TRANS("teamplay is disabled in this version");
  gm_mgTeam.mg_strTip = TRANS("enter team name, if playing in team");
  gm_lhGadgets.AddTail(gm_mgTeam.mg_lnNode);

  TRIGGER_MG(gm_mgCrosshair, 4.0, gm_mgTeam, gm_mgWeaponSelect, TRANS("CROSSHAIR"), astrCrosshair);
  gm_mgCrosshair.mg_bVisual = TRUE;
  gm_mgCrosshair.mg_boxOnScreen = BoxPlayerSwitch(5.0f);
  gm_mgCrosshair.mg_iCenterI = -1;
  gm_mgCrosshair.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgWeaponSelect, 4.0, gm_mgCrosshair, gm_mgWeaponHide, TRANS("AUTO SELECT WEAPON"), astrWeapon);
  gm_mgWeaponSelect.mg_boxOnScreen = BoxPlayerSwitch(6.0f);
  gm_mgWeaponSelect.mg_iCenterI = -1;
  gm_mgWeaponSelect.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgWeaponHide, 4.0, gm_mgWeaponSelect, gm_mg3rdPerson, TRANS("HIDE WEAPON MODEL"), astrNoYes);
  gm_mgWeaponHide.mg_boxOnScreen = BoxPlayerSwitch(7.0f);
  gm_mgWeaponHide.mg_iCenterI = -1;
  gm_mgWeaponHide.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mg3rdPerson, 4.0, gm_mgWeaponHide, gm_mgQuotes, TRANS("PREFER 3RD PERSON VIEW"), astrNoYes);
  gm_mg3rdPerson.mg_boxOnScreen = BoxPlayerSwitch(8.0f);
  gm_mg3rdPerson.mg_iCenterI = -1;
  gm_mg3rdPerson.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgQuotes, 4.0, gm_mg3rdPerson, gm_mgAutoSave, TRANS("VOICE QUOTES"), astrNoYes);
  gm_mgQuotes.mg_boxOnScreen = BoxPlayerSwitch(9.0f);
  gm_mgQuotes.mg_iCenterI = -1;
  gm_mgQuotes.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgAutoSave, 4.0, gm_mgQuotes, gm_mgCompDoubleClick, TRANS("AUTO SAVE"), astrNoYes);
  gm_mgAutoSave.mg_boxOnScreen = BoxPlayerSwitch(10.0f);
  gm_mgAutoSave.mg_iCenterI = -1;
  gm_mgAutoSave.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgCompDoubleClick, 4.0, gm_mgAutoSave, gm_mgSharpTurning, TRANS("INVOKE COMPUTER"), astrComputerInvoke);
  gm_mgCompDoubleClick.mg_boxOnScreen = BoxPlayerSwitch(11.0f);
  gm_mgCompDoubleClick.mg_iCenterI = -1;
  gm_mgCompDoubleClick.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgSharpTurning, 4.0, gm_mgCompDoubleClick, gm_mgViewBobbing, TRANS("SHARP TURNING"), astrNoYes);
  gm_mgSharpTurning.mg_boxOnScreen = BoxPlayerSwitch(12.0f);
  gm_mgSharpTurning.mg_iCenterI = -1;
  gm_mgSharpTurning.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgViewBobbing, 4.0, gm_mgSharpTurning, gm_mgCustomizeControls, TRANS("VIEW BOBBING"), astrNoYes);
  gm_mgViewBobbing.mg_boxOnScreen = BoxPlayerSwitch(13.0f);
  gm_mgViewBobbing.mg_iCenterI = -1;
  gm_mgViewBobbing.mg_pOnTriggerChange = NULL;

  gm_mgCustomizeControls.mg_strText = TRANS("CUSTOMIZE CONTROLS");
  gm_mgCustomizeControls.mg_boxOnScreen = BoxMediumLeft(14.5f);
  gm_mgCustomizeControls.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgCustomizeControls.mg_iCenterI = -1;
  gm_mgCustomizeControls.mg_pmgUp = &gm_mgViewBobbing;
  gm_mgCustomizeControls.mg_pActivatedFunction = NULL;
  gm_mgCustomizeControls.mg_pmgDown = &gm_mgNumber[0];
  gm_mgCustomizeControls.mg_pmgRight = &gm_mgModel;
  gm_mgCustomizeControls.mg_strTip = TRANS("customize controls for this player");
  gm_lhGadgets.AddTail(gm_mgCustomizeControls.mg_lnNode);

  gm_mgModel.mg_boxOnScreen = BoxPlayerModel();
  gm_mgModel.mg_pmgLeft = &gm_mgNameField;
  gm_mgModel.mg_pActivatedFunction = NULL;
  gm_mgModel.mg_pmgDown = &gm_mgNameField;
  gm_mgModel.mg_pmgLeft = &gm_mgNameField;
  gm_mgModel.mg_strTip = TRANS("change model for this player");
  gm_lhGadgets.AddTail(gm_mgModel.mg_lnNode);
}

INDEX CPlayerProfileMenu::ComboFromPlayer(INDEX iPlayer)
{
  return iPlayer;
}

INDEX CPlayerProfileMenu::PlayerFromCombo(INDEX iCombo)
{
  return iCombo;
}

void CPlayerProfileMenu::SelectPlayer(INDEX iPlayer)
{
  CPlayerCharacter &pc = _pGame->gm_apcPlayers[iPlayer];

  for (INDEX iPl = 0; iPl<8; iPl++)
  {
    gm_mgNumber[iPl].mg_bHighlighted = FALSE;
  }

  gm_mgNumber[iPlayer].mg_bHighlighted = TRUE;

  iPlayer = Clamp(iPlayer, INDEX(0), INDEX(7));

  if (_iLocalPlayer >= 0 && _iLocalPlayer<4) {
    _pGame->gm_aiMenuLocalPlayers[_iLocalPlayer] = iPlayer;
  } else {
    _pGame->gm_iSinglePlayer = iPlayer;
  }
  gm_mgNameField.mg_pstrToChange = &pc.pc_strName;
  gm_mgNameField.SetText(*gm_mgNameField.mg_pstrToChange);
  gm_mgTeam.mg_pstrToChange = &pc.pc_strTeam;
  gm_mgTeam.SetText(*gm_mgTeam.mg_pstrToChange);

  CPlayerSettings *pps = (CPlayerSettings *)pc.pc_aubAppearance;

  gm_mgCrosshair.mg_iSelected = pps->ps_iCrossHairType + 1;
  gm_mgCrosshair.ApplyCurrentSelection();

  gm_mgWeaponSelect.mg_iSelected = pps->ps_iWeaponAutoSelect;
  gm_mgWeaponSelect.ApplyCurrentSelection();

  gm_mgWeaponHide.mg_iSelected = (pps->ps_ulFlags&PSF_HIDEWEAPON) ? 1 : 0;
  gm_mgWeaponHide.ApplyCurrentSelection();

  gm_mg3rdPerson.mg_iSelected = (pps->ps_ulFlags&PSF_PREFER3RDPERSON) ? 1 : 0;
  gm_mg3rdPerson.ApplyCurrentSelection();

  gm_mgQuotes.mg_iSelected = (pps->ps_ulFlags&PSF_NOQUOTES) ? 0 : 1;
  gm_mgQuotes.ApplyCurrentSelection();

  gm_mgAutoSave.mg_iSelected = (pps->ps_ulFlags&PSF_AUTOSAVE) ? 1 : 0;
  gm_mgAutoSave.ApplyCurrentSelection();

  gm_mgCompDoubleClick.mg_iSelected = (pps->ps_ulFlags&PSF_COMPSINGLECLICK) ? 0 : 1;
  gm_mgCompDoubleClick.ApplyCurrentSelection();

  gm_mgViewBobbing.mg_iSelected = (pps->ps_ulFlags&PSF_NOBOBBING) ? 0 : 1;
  gm_mgViewBobbing.ApplyCurrentSelection();

  gm_mgSharpTurning.mg_iSelected = (pps->ps_ulFlags&PSF_SHARPTURNING) ? 1 : 0;
  gm_mgSharpTurning.ApplyCurrentSelection();

  // get function that will set player appearance
  CShellSymbol *pss = _pShell->GetSymbol("SetPlayerAppearance", /*bDeclaredOnly=*/ TRUE);
  // if none
  if (pss == NULL) {
    // no model
    gm_mgModel.mg_moModel.SetData(NULL);
    // if there is some
  } else {
    // set the model
    BOOL(*pFunc)(CModelObject *, CPlayerCharacter *, CTString &, BOOL) =
      (BOOL(*)(CModelObject *, CPlayerCharacter *, CTString &, BOOL))pss->ss_pvValue;
    CTString strName;
    BOOL bSet;
    if (_gmRunningGameMode != GM_SINGLE_PLAYER && !_bPlayerMenuFromSinglePlayer) {
      bSet = pFunc(&gm_mgModel.mg_moModel, &pc, strName, TRUE);
      gm_mgModel.mg_strTip = TRANS("change model for this player");
      gm_mgModel.mg_bEnabled = TRUE;
    }
    else {
      // cannot change player appearance in single player mode
      bSet = pFunc(&gm_mgModel.mg_moModel, NULL, strName, TRUE);
      gm_mgModel.mg_strTip = TRANS("cannot change model for single-player game");
      gm_mgModel.mg_bEnabled = FALSE;
    }
    // ignore gender flags, if any
    strName.RemovePrefix("#female#");
    strName.RemovePrefix("#male#");
    gm_mgModel.mg_plModel = CPlacement3D(FLOAT3D(0.1f, -1.0f, -3.5f), ANGLE3D(150, 0, 0));
    gm_mgModel.mg_strText = strName;
    CPlayerSettings *pps = (CPlayerSettings *)pc.pc_aubAppearance;
    _strLastPlayerAppearance = pps->GetModelFilename();
    try {
      gm_mgModel.mg_moFloor.SetData_t(CTFILENAME("Models\\Computer\\Floor.mdl"));
      gm_mgModel.mg_moFloor.mo_toTexture.SetData_t(CTFILENAME("Models\\Computer\\Floor.tex"));
    }
    catch (char *strError) {
      (void)strError;
    }
  }
}

void CPlayerProfileMenu::StartMenu(void)
{
  _pGUIM->gmPlayerProfile.gm_pmgSelectedByDefault = &gm_mgNameField;

  if (_gmRunningGameMode == GM_NONE || _gmRunningGameMode == GM_DEMO) {
    for (INDEX i = 0; i<8; i++) {
      gm_mgNumber[i].mg_bEnabled = TRUE;
    }
  } else {
    for (INDEX i = 0; i<8; i++) {
      gm_mgNumber[i].mg_bEnabled = FALSE;
    }
    INDEX iFirstEnabled = 0;
    {for (INDEX ilp = 0; ilp<4; ilp++) {
      CLocalPlayer &lp = _pGame->gm_lpLocalPlayers[ilp];
      if (lp.lp_bActive) {
        gm_mgNumber[lp.lp_iPlayer].mg_bEnabled = TRUE;
        if (iFirstEnabled == 0) {
          iFirstEnabled = lp.lp_iPlayer;
        }
      }
    }}
    // backup to first player in case current player is disabled
    if (!gm_mgNumber[*gm_piCurrentPlayer].mg_bEnabled) *gm_piCurrentPlayer = iFirstEnabled;
  }
  // done
  SelectPlayer(*gm_piCurrentPlayer);
  CGameMenu::StartMenu();
}

void CPlayerProfileMenu::EndMenu(void)
{
  _pGame->SavePlayersAndControls();
  CGameMenu::EndMenu();
}