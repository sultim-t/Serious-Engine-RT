/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_EDIT_H
#define SE_INCL_MENU_GADGET_EDIT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MGButton.h"


class CMGEdit : public CMGButton {
public:
	INDEX mg_ctMaxStringLen;
	CTString *mg_pstrToChange;
	CMGEdit(void);
	// return TRUE if handled
	BOOL OnKeyDown(int iVKey);
	BOOL OnChar(MSG msg);
	void Clear(void);
	void OnActivate(void);
	void OnKillFocus(void);
	void Render(CDrawPort *pdp);
	virtual void OnStringChanged(void);
	virtual void OnStringCanceled(void);
};

#endif  /* include-once check. */