/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include "MHighScore.h"


void CHighScoreMenu::Initialize_t(void)
{
	gm_mgHScore.mg_boxOnScreen = FLOATaabbox2D(FLOAT2D(0, 0), FLOAT2D(1, 0.5));
	gm_lhGadgets.AddTail(gm_mgHScore.mg_lnNode);

	gm_mgTitle.mg_strText = TRANS("HIGH SCORE TABLE");
	gm_mgTitle.mg_boxOnScreen = BoxTitle();
	gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);
}

void CHighScoreMenu::StartMenu(void)
{
	gm_pgmParentMenu = pgmCurrentMenu;
	CGameMenu::StartMenu();
}