/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_CUSTOMIZEKEYBOARD_H
#define SE_INCL_GAME_MENU_CUSTOMIZEKEYBOARD_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CCustomizeKeyboardMenu : public CGameMenu {
public:
	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
	void FillListItems(void);
};

#endif  /* include-once check. */