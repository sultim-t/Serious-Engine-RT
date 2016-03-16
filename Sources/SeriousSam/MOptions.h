/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_OPTIONS_H
#define SE_INCL_GAME_MENU_OPTIONS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGTitle.h"


class COptionsMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGButton gm_mgVideoOptions;
	CMGButton gm_mgAudioOptions;
	CMGButton gm_mgPlayerProfileOptions;
	CMGButton gm_mgNetworkOptions;
	CMGButton gm_mgCustomOptions;
	CMGButton gm_mgAddonOptions;

	void Initialize_t(void);
};

#endif  /* include-once check. */