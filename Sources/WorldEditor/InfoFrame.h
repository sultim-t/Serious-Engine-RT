/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// InfoFrame.h : header file
//
#ifndef INFOFRAME_H
#define INFOFRAME_H 1

/////////////////////////////////////////////////////////////////////////////
// CInfoFrame frame

class CInfoFrame : public CMiniFrameWnd
{
	DECLARE_DYNCREATE(CInfoFrame)
protected:

// Attributes
public:
  CInfoSheet *m_pInfoSheet;
  int m_PageWidth;
  int m_PageHeight;

// Operations
public:
	CInfoFrame();           // protected constructor used by dynamic creation
	virtual ~CInfoFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
public:

	// Generated message map functions
	//{{AFX_MSG(CInfoFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // INFOFRAME_H
