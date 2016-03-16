/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_SINGLEPLAYER_H
#define SE_INCL_GAME_MENU_SINGLEPLAYER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGTitle.h"


class CSinglePlayerMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGButton gm_mgPlayerLabel;
	CMGButton gm_mgNewGame;
	CMGButton gm_mgCustom;
	CMGButton gm_mgQuickLoad;
	CMGButton gm_mgLoad;
	CMGButton gm_mgTraining;
	CMGButton gm_mgTechTest;
	CMGButton gm_mgPlayersAndControls;
	CMGButton gm_mgOptions;

	void Initialize_t(void);
	void StartMenu(void);
};

#endif  /* include-once check. */