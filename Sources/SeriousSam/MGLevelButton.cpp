/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Base/KeyNames.h>
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include <GameMP/LCDDrawing.h>
#include "MGLevelButton.h"

extern CSoundData *_psdPress;


void CMGLevelButton::OnActivate(void)
{
	PlayMenuSound(_psdPress);
	IFeel_PlayEffect("Menu_press");
	_pGame->gam_strCustomLevel = mg_fnmLevel;
	extern void(*_pAfterLevelChosen)(void);
	_pAfterLevelChosen();
}


void CMGLevelButton::OnSetFocus(void)
{
	SetThumbnail(mg_fnmLevel);
	CMGButton::OnSetFocus();
}