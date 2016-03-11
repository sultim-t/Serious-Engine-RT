/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGRENAMECONTROLS_H__3F147D64_39B2_11D2_8383_004095812ACC__INCLUDED_)
#define AFX_DLGRENAMECONTROLS_H__3F147D64_39B2_11D2_8383_004095812ACC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgRenameControls.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRenameControls dialog

class CDlgRenameControls : public CDialog
{
// Construction
public:
	CDlgRenameControls(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRenameControls)
	enum { IDD = IDD_RENAME_CONTROLS };
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRenameControls)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRenameControls)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRENAMECONTROLS_H__3F147D64_39B2_11D2_8383_004095812ACC__INCLUDED_)
