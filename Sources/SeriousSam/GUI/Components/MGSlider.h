/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_SLIDER_H
#define SE_INCL_MENU_GADGET_SLIDER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MGButton.h"


class CMGSlider : public CMGButton {
public:
	FLOAT mg_fFactor;
	INDEX mg_iMinPos;
	INDEX mg_iMaxPos;
	INDEX mg_iCurPos;

	CMGSlider();
	void ApplyCurrentPosition(void);
	void ApplyGivenPosition(INDEX iMin, INDEX iMax, INDEX iCur);
	// return TRUE if handled
	virtual BOOL OnKeyDown(int iVKey);
	void(*mg_pOnSliderChange)(INDEX iCurPos);
	PIXaabbox2D GetSliderBox(void);
	void Render(CDrawPort *pdp);
};

#endif  /* include-once check. */