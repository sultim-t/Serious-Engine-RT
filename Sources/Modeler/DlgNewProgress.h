/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgNewProgress.h : header file
//
#ifndef DLGNEWPROGRESS_H
#define DLGNEWPROGRESS_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgNewProgress dialog

class CDlgNewProgress : public CDialog
{
// Construction
public:
	CTString m_strNewMessage;
	CDlgNewProgress(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewProgress)
	enum { IDD = IDD_NEW_PROGRESS };
	CProgressCtrl	m_NewProgressLine;
	CString	m_ProgressMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewProgress)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGNEWPROGRESS_H
