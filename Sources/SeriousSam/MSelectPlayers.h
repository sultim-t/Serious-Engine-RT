/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_SELECTPLAYERS_H
#define SE_INCL_GAME_MENU_SELECTPLAYERS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CSelectPlayersMenu : public CGameMenu {
public:
	BOOL gm_bAllowDedicated;
	BOOL gm_bAllowObserving;
	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
};

#endif  /* include-once check. */