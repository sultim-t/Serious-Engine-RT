/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_CONFIRM_H
#define SE_INCL_GAME_MENU_CONFIRM_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"


class CConfirmMenu : public CGameMenu {
public:
	CMGButton gm_mgConfirmLabel;
	CMGButton gm_mgConfirmYes;
	CMGButton gm_mgConfirmNo;

	void Initialize_t(void);
	// return TRUE if handled
	BOOL OnKeyDown(int iVKey);

	void BeLarge(void);
	void BeSmall(void);
};

#endif  /* include-once check. */