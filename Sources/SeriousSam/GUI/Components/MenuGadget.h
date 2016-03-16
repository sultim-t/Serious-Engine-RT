/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENU_GADGET_H
#define SE_INCL_MENU_GADGET_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif


#define DOING_NOTHING 0
#define PRESS_KEY_WAITING 1
#define RELEASE_RETURN_WAITING 2

#define EMPTYSLOTSTRING TRANS("<save a new one>")

class CMenuGadget {
public:
	CListNode mg_lnNode;
	FLOATaabbox2D mg_boxOnScreen;
	BOOL mg_bVisible;
	BOOL mg_bEnabled;
	BOOL mg_bLabel;
	BOOL mg_bFocused;
	INDEX mg_iInList; // for scrollable gadget lists

	CTString mg_strTip;
	CMenuGadget *mg_pmgLeft;
	CMenuGadget *mg_pmgRight;
	CMenuGadget *mg_pmgUp;
	CMenuGadget *mg_pmgDown;

	CMenuGadget(void);
	// return TRUE if handled
	virtual BOOL OnKeyDown(int iVKey);
	virtual BOOL OnChar(MSG msg);
	virtual void OnActivate(void);
	virtual void OnSetFocus(void);
	virtual void OnKillFocus(void);
	virtual void Appear(void);
	virtual void Disappear(void);
	virtual void Think(void);
	virtual void OnMouseOver(PIX pixI, PIX pixJ);

	virtual COLOR GetCurrentColor(void);
	virtual void  Render(CDrawPort *pdp);
	virtual BOOL  IsSeparator(void) { return FALSE; };
};

enum ButtonFontSize {
	BFS_SMALL = 0,
	BFS_MEDIUM = 1,
	BFS_LARGE = 2,
};

#endif  /* include-once check. */