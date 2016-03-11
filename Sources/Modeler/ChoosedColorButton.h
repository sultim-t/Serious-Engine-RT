/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ChoosedColorButton.h : header file
//

#ifndef CHOOSED_COLOR_BUTTON_H
#define CHOOSED_COLOR_BUTTON_H 1

/////////////////////////////////////////////////////////////////////////////
// CChoosedColorButton window

class CChoosedColorButton : public CButton
{
// Construction
public:
	CChoosedColorButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChoosedColorButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChoosedColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChoosedColorButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // CHOOSED_COLOR_BUTTON_H
