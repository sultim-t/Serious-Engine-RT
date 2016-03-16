/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_NETWORKSTART_H
#define SE_INCL_GAME_MENU_NETWORKSTART_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGEdit.h"
#include "GUI/Components/MGTitle.h"
#include "GUI/Components/MGTrigger.h"


class CNetworkStartMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGEdit gm_mgSessionName;
	CMGTrigger gm_mgGameType;
	CMGTrigger gm_mgDifficulty;
	CMGButton gm_mgLevel;
	CMGTrigger gm_mgMaxPlayers;
	CMGTrigger gm_mgWaitAllPlayers;
	CMGTrigger gm_mgVisible;
	CMGButton gm_mgGameOptions;
	CMGButton gm_mgStart;

	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
};

#endif  /* include-once check. */