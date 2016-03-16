/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_RENDERINGOPTIONS_H
#define SE_INCL_GAME_MENU_RENDERINGOPTIONS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"


class CRenderingOptionsMenu : public CGameMenu {
public:
	void StartMenu(void);
	void EndMenu(void);
	void Initialize_t(void);
};
#endif  /* include-once check. */