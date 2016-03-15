/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include "MConfirm.h"

void CConfirmMenu::Initialize_t(void)
{
	gm_bPopup = TRUE;

	gm_mgConfirmLabel.mg_strText = "";
	gm_lhGadgets.AddTail(gm_mgConfirmLabel.mg_lnNode);
	gm_mgConfirmLabel.mg_boxOnScreen = BoxPopupLabel();
	gm_mgConfirmLabel.mg_iCenterI = 0;
	gm_mgConfirmLabel.mg_bfsFontSize = BFS_LARGE;

	gm_mgConfirmYes.mg_strText = TRANS("YES");
	gm_lhGadgets.AddTail(gm_mgConfirmYes.mg_lnNode);
	gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesLarge();
	gm_mgConfirmYes.mg_pActivatedFunction = NULL;
	gm_mgConfirmYes.mg_pmgLeft =
		gm_mgConfirmYes.mg_pmgRight = &gm_mgConfirmNo;
	gm_mgConfirmYes.mg_iCenterI = 1;
	gm_mgConfirmYes.mg_bfsFontSize = BFS_LARGE;

	gm_mgConfirmNo.mg_strText = TRANS("NO");
	gm_lhGadgets.AddTail(gm_mgConfirmNo.mg_lnNode);
	gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoLarge();
	gm_mgConfirmNo.mg_pActivatedFunction = NULL;
	gm_mgConfirmNo.mg_pmgLeft =
		gm_mgConfirmNo.mg_pmgRight = &gm_mgConfirmYes;
	gm_mgConfirmNo.mg_iCenterI = -1;
	gm_mgConfirmNo.mg_bfsFontSize = BFS_LARGE;
}

void CConfirmMenu::BeLarge(void)
{
	gm_mgConfirmLabel.mg_bfsFontSize = BFS_LARGE;
	gm_mgConfirmYes.mg_bfsFontSize = BFS_LARGE;
	gm_mgConfirmNo.mg_bfsFontSize = BFS_LARGE;

	gm_mgConfirmLabel.mg_iCenterI = 0;
	gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesLarge();
	gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoLarge();
}

void CConfirmMenu::BeSmall(void)
{
	gm_mgConfirmLabel.mg_bfsFontSize = BFS_MEDIUM;
	gm_mgConfirmYes.mg_bfsFontSize = BFS_MEDIUM;
	gm_mgConfirmNo.mg_bfsFontSize = BFS_MEDIUM;

	gm_mgConfirmLabel.mg_iCenterI = -1;
	gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesSmall();
	gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoSmall();
}