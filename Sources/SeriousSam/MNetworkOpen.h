/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_NETWORKOPEN_H
#define SE_INCL_GAME_MENU_NETWORKOPEN_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGEdit.h"
#include "GUI/Components/MGTitle.h"


class CNetworkOpenMenu : public CGameMenu {
public:
	CTString gm_strPort;

	CMGTitle gm_mgTitle;
	CMGButton gm_mgAddressLabel;
	CMGEdit gm_mgAddress;
	CMGButton gm_mgPortLabel;
	CMGEdit gm_mgPort;
	CMGButton gm_mgJoin;

	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
};

#endif  /* include-once check. */