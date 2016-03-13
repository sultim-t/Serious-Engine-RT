/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_MAIN_H
#define SE_INCL_GAME_MENU_MAIN_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "MGButton.h"


class CMainMenu : public CGameMenu {
public:
	CMGButton gm_mgVersionLabel;
	CMGButton gm_mgModLabel;
	CMGButton gm_mgSingle;
	CMGButton gm_mgNetwork;
	CMGButton gm_mgSplitScreen;
	CMGButton gm_mgDemo;
	CMGButton gm_mgMods;
	CMGButton gm_mgHighScore;
	CMGButton gm_mgOptions;
	CMGButton gm_mgQuit;

	void Initialize_t(void);
	void StartMenu(void);
};

#endif  /* include-once check. */