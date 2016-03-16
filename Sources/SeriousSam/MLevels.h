/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_LEVELS_H
#define SE_INCL_GAME_MENU_LEVELS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGArrow.h"
#include "GUI/Components/MGLevelButton.h"
#include "GUI/Components/MGTitle.h"


class CLevelsMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGLevelButton gm_mgManualLevel[LEVELS_ON_SCREEN];
	CMGArrow gm_mgArrowUp;
	CMGArrow gm_mgArrowDn;

	void Initialize_t(void);
	void FillListItems(void);
	void StartMenu(void);
};

#endif  /* include-once check. */