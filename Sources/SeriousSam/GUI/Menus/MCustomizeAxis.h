/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_CUSTOMIZEAXIS_H
#define SE_INCL_GAME_MENU_CUSTOMIZEAXIS_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGSlider.h"
#include "GUI/Components/MGTitle.h"
#include "GUI/Components/MGTrigger.h"


class CCustomizeAxisMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGTrigger gm_mgActionTrigger;
	CMGTrigger gm_mgMountedTrigger;
	CMGSlider gm_mgSensitivity;
	CMGSlider gm_mgDeadzone;
	CMGTrigger gm_mgInvertTrigger;
	CMGTrigger gm_mgRelativeTrigger;
	CMGTrigger gm_mgSmoothTrigger;

	~CCustomizeAxisMenu(void);
	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
	void ObtainActionSettings(void);
	void ApplyActionSettings(void);
};

#endif  /* include-once check. */