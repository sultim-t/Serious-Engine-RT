/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// PatchPaletteButton.h : header file
//
#ifndef PATCH_PALLETE_BUTTON_H
#define PATCH_PALLETE_BUTTON_H 1

/////////////////////////////////////////////////////////////////////////////
// CPatchPaletteButton window

class CPatchPaletteButton : public CButton
{
// Construction
public:
	CPatchPaletteButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchPaletteButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPatchPaletteButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPatchPaletteButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // PATCH_PALLETE_BUTTON_H
