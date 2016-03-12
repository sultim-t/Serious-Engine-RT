/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_VAR_H
#define SE_INCL_GAME_MENU_VAR_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CVarMenu : public CGameMenu {
public:
	CTFileName gm_fnmMenuCFG;
	void Initialize_t(void);
	void FillListItems(void);
	void StartMenu(void);
	void EndMenu(void);
	void Think(void);
};

#endif  /* include-once check. */