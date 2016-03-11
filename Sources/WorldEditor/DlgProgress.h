/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgProgress.h : header file
//

#ifndef DLGPROGRESS_H
#define DLGPROGRESS_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress dialog

class CDlgProgress : public CDialog
{
// Construction
public:
	CDlgProgress(CWnd* pParent = NULL, BOOL bCanCancel=FALSE);   // standard constructor
  void SetProgressMessageAndPosition( char *strProgressMessage, INDEX iCurrentPos);
  
  BOOL m_bCancelPressed;
  BOOL m_bHasCancel;

// Dialog Data
	//{{AFX_DATA(CDlgProgress)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CProgressCtrl	m_ctrlProgres;
	CString	m_strProgressMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProgress)
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif  // DLGPROGRESS_H
