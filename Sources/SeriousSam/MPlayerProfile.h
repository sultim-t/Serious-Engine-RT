/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_PLAYERPROFILE_H
#define SE_INCL_GAME_MENU_PLAYERPROFILE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGEdit.h"
#include "GUI/Components/MGModel.h"
#include "GUI/Components/MGTitle.h"
#include "GUI/Components/MGTrigger.h"


class CPlayerProfileMenu : public CGameMenu {
public:
	INDEX *gm_piCurrentPlayer;

	CMGTitle gm_mgProfileTitle;
	CMGButton gm_mgNoLabel;
	CMGButton gm_mgNumber[8];
	CMGButton gm_mgNameLabel;
	CMGEdit gm_mgNameField;
	CMGButton gm_mgTeamLabel;
	CMGEdit gm_mgTeam;
	CMGButton gm_mgCustomizeControls;
	CMGTrigger gm_mgCrosshair;
	CMGTrigger gm_mgWeaponSelect;
	CMGTrigger gm_mgWeaponHide;
	CMGTrigger gm_mg3rdPerson;
	CMGTrigger gm_mgQuotes;
	CMGTrigger gm_mgAutoSave;
	CMGTrigger gm_mgCompDoubleClick;
	CMGTrigger gm_mgViewBobbing;
	CMGTrigger gm_mgSharpTurning;
	CMGModel gm_mgModel;

	void Initialize_t(void);
	INDEX ComboFromPlayer(INDEX iPlayer);
	INDEX PlayerFromCombo(INDEX iCombo);
	void SelectPlayer(INDEX iPlayer);
	void ApplyComboPlayer(INDEX iPlayer);
	void StartMenu(void);
	void EndMenu(void);
};

#endif  /* include-once check. */