/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_SERVERS_H
#define SE_INCL_GAME_MENU_SERVERS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CServersMenu : public CGameMenu {
public:
	BOOL m_bInternet;
	void Initialize_t(void);
	void StartMenu(void);
	void Think(void);

	BOOL OnKeyDown(int iVKey); // __Evolution
};

#endif  /* include-once check. */