/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_KEYDEFINITION_H
#define SE_INCL_MENU_GADGET_KEYDEFINITION_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MenuGadget.h"


class CMGKeyDefinition : public CMenuGadget {
public:
	INDEX mg_iState;
	INDEX mg_iControlNumber;

	CTString mg_strLabel;
	CTString mg_strBinding;

	CMGKeyDefinition(void);
	void Appear(void);
	void Disappear(void);
	void OnActivate(void);
	// return TRUE if handled
	BOOL OnKeyDown(int iVKey);
	void Think(void);
	// set names for both key bindings
	void SetBindingNames(BOOL bDefining);
	void DefineKey(INDEX iDik);
	void Render(CDrawPort *pdp);
};

#endif  /* include-once check. */