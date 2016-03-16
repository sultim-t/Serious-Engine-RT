/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_HIGHSCORE_H
#define SE_INCL_GAME_MENU_HIGHSCORE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGHighScore.h"
#include "GUI/Components/MGTitle.h"


class CHighScoreMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGHighScore gm_mgHScore;

	void Initialize_t(void);
	void StartMenu(void);
};

#endif  /* include-once check. */