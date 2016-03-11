/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgInfoPgNone.h : header file
//
#ifndef DLGINFOPGNONE
#define DLGINFOPGNONE 1

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoPgNone dialog

class CDlgInfoPgNone : public CPropertyPage
{
// Construction
public:
	CDlgInfoPgNone();   // standard constructor
  BOOL OnIdle(LONG lCount);

// Dialog Data
	//{{AFX_DATA(CDlgInfoPgNone)
	enum { IDD = IDD_INFO_NONE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInfoPgNone)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInfoPgNone)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGINFOPGNONE
