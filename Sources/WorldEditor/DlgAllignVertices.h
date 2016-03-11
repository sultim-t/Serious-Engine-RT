/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGALLIGNVERTICES_H__BE198008_81A4_11D5_871A_00002103143B__INCLUDED_)
#define AFX_DLGALLIGNVERTICES_H__BE198008_81A4_11D5_871A_00002103143B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAllignVertices.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAllignVertices dialog

class CDlgAllignVertices : public CDialog
{
// Construction
public:
	CDlgAllignVertices(CWnd* pParent = NULL);   // standard constructor
  DOUBLE3D GetLastSelectedVertex(void);

// Dialog Data
	//{{AFX_DATA(CDlgAllignVertices)
	enum { IDD = IDD_ALLIGN_VERTICES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAllignVertices)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAllignVertices)
	afx_msg void OnAllignX();
	afx_msg void OnAllignY();
	afx_msg void OnAllignZ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGALLIGNVERTICES_H__BE198008_81A4_11D5_871A_00002103143B__INCLUDED_)
