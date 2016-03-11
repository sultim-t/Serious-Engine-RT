/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGCONSOLE_H__833B8175_E923_11D1_82A7_000000000000__INCLUDED_)
#define AFX_DLGCONSOLE_H__833B8175_E923_11D1_82A7_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgConsole.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgConsole dialog

class CDlgConsole : public CDialog
{
// Construction
public:
	CDlgConsole(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgConsole)
	enum { IDD = IDD_CONSOLE };
	CConsoleSymbolsCombo	m_ctrConsoleSymbolsCombo;
	CEditConsole	m_ctrlEditConsole;
	CString	m_strConsoleOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConsole)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConsole)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONSOLE_H__833B8175_E923_11D1_82A7_000000000000__INCLUDED_)
