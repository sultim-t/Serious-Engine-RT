/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_CUSTOMCOMBOWND_H__6C892711_24F7_4340_8D42_6AA0869A266B__INCLUDED_)
#define AFX_CUSTOMCOMBOWND_H__6C892711_24F7_4340_8D42_6AA0869A266B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomComboWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomComboWnd window

class CComboLine
{
public:
  CTFileName cl_fnmTexture;
  PIXaabbox2D cl_boxIcon;
  CTString cl_strText;
  ULONG cl_ulValue;
  COLOR cl_colText;
};

class CCustomComboWnd : public CWnd
{
// Construction
public:
	CCustomComboWnd();

// Attributes
public:
  CDynamicContainer<CComboLine> m_dcComboLines;
  CDrawPort *m_pDrawPort;
  CViewPort *m_pViewPort;
  int m_iTimerID;
  void (*m_pOnSelect)(INDEX iSelected);

  FLOAT *m_pfResult;

// Operations
public:
  // calculate given line box in pixels
  PIXaabbox2D GetLineBBox( INDEX iLine);
  void RenderOneLine( INDEX iLine, PIXaabbox2D rectLine, CDrawPort *pdp, COLOR colFill);
  void GetComboLineSize(PIX &pixMaxWidth, PIX &pixMaxHeight);
  BOOL Initialize(FLOAT *pfResult, void (*pOnSelect)(INDEX iSelected),
    PIX pixX, PIX pixY, BOOL bDown=FALSE);
  INDEX InsertItem( CTString strText, CTFileName fnIcons=CTString(""), 
    MEXaabbox2D boxIcon=MEXaabbox2D( MEX2D(0,0), MEX2D(0,0)));
  void SetItemValue(INDEX iItem, ULONG ulValue);
  void SetItemColor(INDEX iItem, COLOR col);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomComboWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCustomComboWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomComboWnd)
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
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

#endif // !defined(AFX_CUSTOMCOMBOWND_H__6C892711_24F7_4340_8D42_6AA0869A266B__INCLUDED_)
