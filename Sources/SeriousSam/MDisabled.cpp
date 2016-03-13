/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include "MDisabled.h"


void CDisabledMenu::Initialize_t(void)
{
	gm_mgTitle.mg_boxOnScreen = BoxTitle();
	gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

	gm_mgButton.mg_bfsFontSize = BFS_MEDIUM;
	gm_mgButton.mg_boxOnScreen = BoxBigRow(0.0f);
	gm_lhGadgets.AddTail(gm_mgButton.mg_lnNode);
	gm_mgButton.mg_pActivatedFunction = NULL;
}