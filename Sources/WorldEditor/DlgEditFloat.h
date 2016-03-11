/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGEDITFLOAT_H__A6E6E355_868D_4D46_B2A5_0C3458E0FB1B__INCLUDED_)
#define AFX_DLGEDITFLOAT_H__A6E6E355_868D_4D46_B2A5_0C3458E0FB1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditFloat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEditFloat dialog

class CDlgEditFloat : public CDialog
{
// Construction
public:
	CDlgEditFloat(CWnd* pParent = NULL);   // standard constructor
  CTString m_strTitle;

// Dialog Data
	//{{AFX_DATA(CDlgEditFloat)
	enum { IDD = IDD_EDIT_FLOAT };
	float	m_fEditFloat;
	CString	m_strVarName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditFloat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditFloat)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITFLOAT_H__A6E6E355_868D_4D46_B2A5_0C3458E0FB1B__INCLUDED_)
