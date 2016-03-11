/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgCreateVirtualDirectory.h : header file
//
#ifndef DLGCREATEVIRTUALDIRECTORY_H
#define DLGCREATEVIRTUALDIRECTORY_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgCreateVirtualDirectory dialog

class CDlgCreateVirtualDirectory : public CDialog
{
// Construction
public:
	CDlgCreateVirtualDirectory(CTString strOldName = CTString(""),
    CTString strTitle = CTString("Create virtual directory"), CWnd* pParent = NULL);

  CTString m_strTitle;
  CTString m_strCreatedDirName;
  CImageList m_IconsImageList;
  INDEX m_iSelectedIconType;

// Dialog Data
	//{{AFX_DATA(CDlgCreateVirtualDirectory)
	enum { IDD = IDD_CREATE_VIRTUAL_DIRECTORY };
	CListCtrl	m_DirectoryIconsList;
	CString	m_strDirectoryName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCreateVirtualDirectory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCreateVirtualDirectory)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkDirectoryIconList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGCREATEVIRTUALDIRECTORY_H
