/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// AxisListCtrl.h : header file
//

#ifndef AXISLISTCTRL_H
#define AXISLISTCTRL_H 1

/////////////////////////////////////////////////////////////////////////////
// CAxisListCtrl window

class CAxisListCtrl : public CListCtrl
{
// Construction
public:
	CAxisListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAxisListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAxisListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAxisListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  // AXISLISTCTRL_H