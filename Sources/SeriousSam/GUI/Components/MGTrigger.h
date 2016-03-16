/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_TRIGGER_H
#define SE_INCL_MENU_GADGET_TRIGGER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MenuGadget.h"


class CMGTrigger : public CMenuGadget {
public:
	CTString mg_strLabel;
	CTString mg_strValue;
	CTString *mg_astrTexts;
	INDEX mg_ctTexts;
	INDEX mg_iSelected;
	INDEX mg_iCenterI;
	BOOL mg_bVisual;

	CMGTrigger(void);

	void ApplyCurrentSelection(void);
	void OnSetNextInList(int iVKey);
	void(*mg_pPreTriggerChange)(INDEX iCurrentlySelected);
	void(*mg_pOnTriggerChange)(INDEX iCurrentlySelected);

	// return TRUE if handled
	BOOL OnKeyDown(int iVKey);
	void Render(CDrawPort *pdp);
};

#endif  /* include-once check. */