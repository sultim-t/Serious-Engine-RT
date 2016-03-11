/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_BRUSHPALETTEWND_H__E0E5728A_563A_43BA_AD10_187D83BE8E55__INCLUDED_)
#define AFX_BRUSHPALETTEWND_H__E0E5728A_563A_43BA_AD10_187D83BE8E55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrushPaletteWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBrushPaletteWnd window

class CBrushPaletteWnd : public CWnd
{
// Construction
public:
	CBrushPaletteWnd();

// Attributes
public:
  CDrawPort *m_pDrawPort;
  CViewPort *m_pViewPort;
  int m_iTimerID;

// Operations
public:
  // calculate given color's box in pixels
  PIXaabbox2D GetBrushBBox( INDEX iBrush);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrushPaletteWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBrushPaletteWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrushPaletteWnd)
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRUSHPALETTEWND_H__E0E5728A_563A_43BA_AD10_187D83BE8E55__INCLUDED_)
