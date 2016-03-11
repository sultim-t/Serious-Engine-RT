/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_VARBUTTON_H
#define SE_INCL_MENU_GADGET_VARBUTTON_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MGButton.h"


class CMGVarButton : public CMGButton {
public:
	class CVarSetting *mg_pvsVar;
	PIXaabbox2D GetSliderBox(void);
	BOOL OnKeyDown(int iVKey);
	void Render(CDrawPort *pdp);
	BOOL IsSeparator(void);
	BOOL IsEnabled(void);
};

#endif  /* include-once check. */