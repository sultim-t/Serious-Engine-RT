/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_GAME_MENU_AUDIOOPTIONS_H
#define SE_INCL_GAME_MENU_AUDIOOPTIONS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGButton.h"
#include "GUI/Components/MGSlider.h"
#include "GUI/Components/MGTrigger.h"
#include "GUI/Components/MGTitle.h"


class CAudioOptionsMenu : public CGameMenu {
public:
	CMGTitle gm_mgTitle;
	CMGTrigger gm_mgAudioAutoTrigger;
	CMGTrigger gm_mgAudioAPITrigger;
	CMGTrigger gm_mgFrequencyTrigger;
	CMGSlider gm_mgWaveVolume;
	CMGSlider gm_mgMPEGVolume;
	CMGButton gm_mgApply;

	void StartMenu(void);
	void Initialize_t(void);
};

#endif  /* include-once check. */