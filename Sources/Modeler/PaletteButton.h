/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// PaletteButton.h : header file
//
#ifndef COLORS_PALLETE_BUTTON_H
#define COLORS_PALLETE_BUTTON_H 1

/////////////////////////////////////////////////////////////////////////////
// CPaletteButton window

class CPaletteButton : public CButton
{
// Construction
public:
	CPaletteButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPaletteButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPaletteButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // COLORS_PALLETE_BUTTON_H
