/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_INGAME_H
#define SE_INCL_GAME_MENU_INGAME_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "MGButton.h"
#include "MGTitle.h"

class CInGameMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGButton gm_mgLabel1;
	CMGButton gm_mgLabel2;
	CMGButton gm_mgQuickLoad;
	CMGButton gm_mgQuickSave;
	CMGButton gm_mgLoad;
	CMGButton gm_mgSave;
	CMGButton gm_mgDemoRec;
	CMGButton gm_mgHighScore;
	CMGButton gm_mgOptions;
	CMGButton gm_mgStop;
	CMGButton gm_mgQuit;

	void Initialize_t(void);
	void StartMenu(void);
};

#endif  /* include-once check. */