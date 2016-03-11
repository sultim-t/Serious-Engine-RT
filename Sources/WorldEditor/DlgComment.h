/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGCOMMENT_H__AC7EDCA5_C9D0_11D4_85BB_000021291DC7__INCLUDED_)
#define AFX_DLGCOMMENT_H__AC7EDCA5_C9D0_11D4_85BB_000021291DC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgComment.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgComment dialog

class CDlgComment : public CDialog
{
// Construction
public:
	CDlgComment(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgComment)
	enum { IDD = IDD_COMMENT };
	CString	m_strComment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgComment)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgComment)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOMMENT_H__AC7EDCA5_C9D0_11D4_85BB_000021291DC7__INCLUDED_)
