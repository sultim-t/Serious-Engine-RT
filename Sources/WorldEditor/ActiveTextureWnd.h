/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ActiveTextureWnd.h : header file
//
#ifndef ACTIVETEXTUREWND_H
#define ACTIVETEXTUREWND_H 1

/////////////////////////////////////////////////////////////////////////////
// CActiveTextureWnd window

class CActiveTextureWnd : public CWnd
{
// Construction
public:
	CActiveTextureWnd();

// Attributes
public:
  CDrawPort *m_pDrawPort;
  CViewPort *m_pViewPort;
  COleDataSource m_DataSource;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActiveTextureWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CActiveTextureWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CActiveTextureWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // ACTIVETEXTUREWND_H
