/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_BUTTON_H
#define SE_INCL_MENU_GADGET_BUTTON_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MenuGadget.h"


class CMGButton : public CMenuGadget {
public:
	CTString mg_strLabel;   // for those that have labels separately from main text
	CTString mg_strText;
	INDEX mg_iCenterI;
	enum  ButtonFontSize mg_bfsFontSize;
	BOOL  mg_bEditing;
	BOOL  mg_bHighlighted;
	BOOL  mg_bRectangle;
	BOOL  mg_bMental;
	INDEX mg_iTextMode;
	INDEX mg_iCursorPos;

	INDEX mg_iIndex;
	void(*mg_pActivatedFunction)(void);
	CMGButton(void);
	void SetText(CTString strNew);
	void OnActivate(void);
	void Render(CDrawPort *pdp);
	PIX  GetCharOffset(CDrawPort *pdp, INDEX iCharNo);
};

#endif  /* include-once check. */