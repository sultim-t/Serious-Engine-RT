/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPleaseWait.h : header file
//
#ifndef DLGPLEASEWAIT_H
#define DLGPLEASEWAIT_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPleaseWait dialog

class CDlgPleaseWait : public CDialog
{
// Construction
public:
	CDlgPleaseWait( const CTString &fnMessage, const CTString &strArgument,
                  CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CDlgPleaseWait)
	enum { IDD = IDD_PLEASE_WAIT };
	CString	m_MessageArgument;
	CString	m_Message;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPleaseWait)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPleaseWait)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGPLEASEWAIT_H
