/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_NETWORK_H
#define SE_INCL_GAME_MENU_NETWORK_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CNetworkMenu : public CGameMenu {
public:
	void Initialize_t(void);
	void StartMenu(void);
};

#endif  /* include-once check. */