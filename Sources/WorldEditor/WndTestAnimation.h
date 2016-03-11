/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// WndTestAnimation.h : header file
//
#ifndef WNDTESTANIMATION_H
#define WNDTESTANIMATION_H 1

class CDlgLightAnimationEditor;

/////////////////////////////////////////////////////////////////////////////
// CWndTestAnimation window

class CWndTestAnimation : public CWnd
{
// Construction 
public:
	CWndTestAnimation();
	inline void SetParentDlg( CDlgLightAnimationEditor *pParentDlg) {m_pParentDlg=pParentDlg;};

// Attributes
public:
  CAnimObject m_aoAnimObject;
  CDlgLightAnimationEditor *m_pParentDlg;
  int m_iTimerID;
  CDrawPort *m_pDrawPort;
  CViewPort *m_pViewPort;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndTestAnimation)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndTestAnimation();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndTestAnimation)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // WNDTESTANIMATION_H
