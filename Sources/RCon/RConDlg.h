/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// RConDlg.h : header file
//

#if !defined(AFX_RCONDLG_H__2FCD4619_96D7_11D5_9918_000021211E76__INCLUDED_)
#define AFX_RCONDLG_H__2FCD4619_96D7_11D5_9918_000021211E76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRConDlg dialog

class CRConDlg : public CDialog
{
// Construction
public:
	CRConDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRConDlg)
	enum { IDD = IDD_RCON_DIALOG };
	CString	m_strLog;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRConDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRConDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RCONDLG_H__2FCD4619_96D7_11D5_9918_000021211E76__INCLUDED_)
