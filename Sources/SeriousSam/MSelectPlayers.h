/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_SELECTPLAYERS_H
#define SE_INCL_GAME_MENU_SELECTPLAYERS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGChangePlayer.h"
#include "GUI/Components/MGTitle.h"
#include "GUI/Components/MGTrigger.h"


class CSelectPlayersMenu : public CGameMenu {
public:
	BOOL gm_bAllowDedicated;
	BOOL gm_bAllowObserving;

	CMGTitle gm_mgTitle;

	CMGTrigger gm_mgDedicated;
	CMGTrigger gm_mgObserver;
	CMGTrigger gm_mgSplitScreenCfg;

	CMGChangePlayer gm_mgPlayer0Change;
	CMGChangePlayer gm_mgPlayer1Change;
	CMGChangePlayer gm_mgPlayer2Change;
	CMGChangePlayer gm_mgPlayer3Change;

	CMGButton gm_mgNotes;

	CMGButton gm_mgStart;

	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
};

#endif  /* include-once check. */