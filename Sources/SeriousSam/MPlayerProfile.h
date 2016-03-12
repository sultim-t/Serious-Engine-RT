/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_PLAYERPROFILE_H
#define SE_INCL_GAME_MENU_PLAYERPROFILE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CPlayerProfileMenu : public CGameMenu {
public:
	INDEX *gm_piCurrentPlayer;
	void Initialize_t(void);
	INDEX ComboFromPlayer(INDEX iPlayer);
	INDEX PlayerFromCombo(INDEX iCombo);
	void SelectPlayer(INDEX iPlayer);
	void ApplyComboPlayer(INDEX iPlayer);
	void StartMenu(void);
	void EndMenu(void);
};

#endif  /* include-once check. */