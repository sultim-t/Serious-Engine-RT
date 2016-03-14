/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_SPLITSCREEN_H
#define SE_INCL_GAME_MENU_SPLITSCREEN_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "MGButton.h"
#include "MGTitle.h"

class CSplitScreenMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGButton gm_mgStart;
	CMGButton gm_mgQuickLoad;
	CMGButton gm_mgLoad;

	void Initialize_t(void);
	void StartMenu(void);
};

#endif  /* include-once check. */