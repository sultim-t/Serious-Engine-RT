/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_VIDEOOPTIONS_H
#define SE_INCL_GAME_MENU_VIDEOOPTIONS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CVideoOptionsMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGTrigger gm_mgDisplayAPITrigger;
	CMGTrigger gm_mgDisplayAdaptersTrigger;
	CMGTrigger gm_mgFullScreenTrigger;
	CMGTrigger gm_mgResolutionsTrigger;
	CMGTrigger gm_mgDisplayPrefsTrigger;
	CMGButton gm_mgVideoRendering;
	CMGTrigger gm_mgBitsPerPixelTrigger;
	CMGButton gm_mgApply;

	void StartMenu(void);
	void Initialize_t(void);
};

#endif  /* include-once check. */