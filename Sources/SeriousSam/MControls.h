/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_CONTROLS_H
#define SE_INCL_GAME_MENU_CONTROLS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CControlsMenu : public CGameMenu {
public:
	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
	void ObtainActionSettings(void);
	void ApplyActionSettings(void);
};

#endif  /* include-once check. */