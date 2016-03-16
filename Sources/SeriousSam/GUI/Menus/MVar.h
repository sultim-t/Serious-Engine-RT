/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_VAR_H
#define SE_INCL_GAME_MENU_VAR_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGArrow.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGTitle.h"
#include "GUI/Components/MGVarButton.h"


class CVarMenu : public CGameMenu {
public:
	CTFileName gm_fnmMenuCFG;

	CMGTitle gm_mgTitle;
	CMGVarButton gm_mgVar[LEVELS_ON_SCREEN];
	CMGButton gm_mgApply;
	CMGArrow gm_mgArrowUp;
	CMGArrow gm_mgArrowDn;

	void Initialize_t(void);
	void FillListItems(void);
	void StartMenu(void);
	void EndMenu(void);
	void Think(void);
};

#endif  /* include-once check. */