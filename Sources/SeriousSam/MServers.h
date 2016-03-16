/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_SERVERS_H
#define SE_INCL_GAME_MENU_SERVERS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGEdit.h"
#include "GUI/Components/MGServerList.h"
#include "GUI/Components/MGTitle.h"


class CServersMenu : public CGameMenu {
public:
	BOOL m_bInternet;

	CMGTitle gm_mgTitle;
	CMGServerList gm_mgList;
	CMGButton gm_mgRefresh;

	void Initialize_t(void);
	void StartMenu(void);
	void Think(void);
};

#endif  /* include-once check. */