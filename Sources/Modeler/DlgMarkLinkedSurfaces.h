/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgMarkLinkedSurfaces.h : header file
//

#ifndef DLGMARKLINKEDSURFACES_H
#define DLGMARKLINKEDSURFACES_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgMarkLinkedSurfaces dialog

class CDlgMarkLinkedSurfaces : public CDialog
{
// Construction
public:
  CDlgMarkLinkedSurfaces( CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMarkLinkedSurfaces)
	enum { IDD = IDD_LINKED_SURFACES };
	CLinkedSurfaceList	m_listSurfaces;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMarkLinkedSurfaces)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMarkLinkedSurfaces)
	virtual BOOL OnInitDialog();
	afx_msg void OnClearSelection();
	afx_msg void OnSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif //DLGMARKLINKEDSURFACES_H
