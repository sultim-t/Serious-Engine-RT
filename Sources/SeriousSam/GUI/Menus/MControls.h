/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_CONTROLS_H
#define SE_INCL_GAME_MENU_CONTROLS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGSlider.h"
#include "GUI/Components/MGTitle.h"
#include "GUI/Components/MGTrigger.h"


class CControlsMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGButton gm_mgNameLabel;
	CMGButton gm_mgButtons;
	CMGSlider gm_mgSensitivity;
	CMGTrigger gm_mgInvertTrigger;
	CMGTrigger gm_mgSmoothTrigger;
	CMGTrigger gm_mgAccelTrigger;
	CMGTrigger gm_mgIFeelTrigger;
	CMGButton gm_mgPredefined;
	CMGButton gm_mgAdvanced;

	void Initialize_t(void);
	void StartMenu(void);
	void EndMenu(void);
	void ObtainActionSettings(void);
	void ApplyActionSettings(void);
};

#endif  /* include-once check. */