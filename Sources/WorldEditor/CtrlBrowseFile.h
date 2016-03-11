/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CtrlBrowseFile.h : header file
//
#ifndef CTRLBROWSEFILE_H
#define CTRLBROWSEFILE_H 1

class CPropertyComboBar;
class CWorldEditorDoc;
/////////////////////////////////////////////////////////////////////////////
// CCtrlBrowseFile window

class CCtrlBrowseFile : public CButton
{
// Construction
public:
	CCtrlBrowseFile();

// Attributes
public:
  // ptr to parent dialog
  CPropertyComboBar *m_pDialog;
  BOOL m_bFileNameNoDep;

// Operations
public:
  // sets ptr to parent dialog
  void SetDialogPtr( CPropertyComboBar *pDialog);
  CTFileName GetIntersectingFile();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlBrowseFile)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtrlBrowseFile();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtrlBrowseFile)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // CTRLBROWSEFILE_H
