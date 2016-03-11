/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPgGlobal.h : header file
//
#ifndef DLGPGGLOBAL_H
#define DLGPGGLOBAL_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPgGlobal dialog

class CDlgPgGlobal : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgPgGlobal)

// Construction
public:
	CDlgPgGlobal();
	~CDlgPgGlobal();
  BOOL OnIdle(LONG lCount);
	CActiveTextureWnd	m_wndActiveTexture;

  CUpdateableRT m_udSelectionCounts;

// Dialog Data
	//{{AFX_DATA(CDlgPgGlobal)
	enum { IDD = IDD_PG_GLOBAL };
	CString	m_strTextureInfo;
	CString	m_strSelectedEntitiesCt;
	CString	m_strSelectedPolygonsCt;
	CString	m_strSelectedSectorsCt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgGlobal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgPgGlobal)
	virtual BOOL OnInitDialog();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // DLGPGGLOBAL_H
