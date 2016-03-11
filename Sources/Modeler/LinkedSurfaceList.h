/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// LinkedSurfaceList.h : header file
//

#ifndef LINKEDSURFACELIST_H
#define LINKEDSURFACELIST_H 1

/////////////////////////////////////////////////////////////////////////////
// CLinkedSurfaceList window

class CLinkedSurfaceList : public CCheckListBox
{
// Construction
public:
	CLinkedSurfaceList();
  CDialog *m_pdlgParentDialog;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkedSurfaceList)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLinkedSurfaceList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLinkedSurfaceList)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // LINKEDSURFACELIST_H
