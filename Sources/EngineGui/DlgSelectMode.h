/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGSELECTMODE_H__F71966B3_C31A_11D1_8231_000000000000__INCLUDED_)
#define AFX_DLGSELECTMODE_H__F71966B3_C31A_11D1_8231_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgSelectMode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectMode dialog

class CDlgSelectMode : public CDialog
{
public:
  CDisplayMode *m_pdm;
  CDisplayMode *m_pdmAvailableModes;
  INDEX m_ctAvailableDisplayModes;
  enum GfxAPIType *m_pGfxAPI;
// Construction
public:
	CDlgSelectMode( CDisplayMode &dm, enum GfxAPIType &gfxAPI, CWnd* pParent = NULL);
  ~CDlgSelectMode();
  void ApplySettings( CDisplayMode *pdm, enum GfxAPIType *pGfxAPI);

// Dialog Data
	//{{AFX_DATA(CDlgSelectMode)
	enum { IDD = IDD_SELECT_MODE_DIALOG };
	CComboBox	m_ctrlResCombo;
	CComboBox	m_ctrlDriverCombo;
	CString	m_strCurrentMode;
	CString	m_strCurrentDriver;
	int		m_iColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectMode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectMode)
	afx_msg void OnTestButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTMODE_H__F71966B3_C31A_11D1_8231_000000000000__INCLUDED_)
