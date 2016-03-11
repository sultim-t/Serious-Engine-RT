/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgAutoDeltaCSG.h : header file
//

#ifndef DLGAUTODELTACSG_H
#define DLGAUTODELTACSG_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoDeltaCSG dialog

class CDlgAutoDeltaCSG : public CDialog
{
// Construction
public:
	CDlgAutoDeltaCSG(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAutoDeltaCSG)
	enum { IDD = IDD_AUTO_DELTA_CSG };
	UINT	m_ctNumberOfClones;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAutoDeltaCSG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAutoDeltaCSG)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // DLGAUTODELTACSG_H
