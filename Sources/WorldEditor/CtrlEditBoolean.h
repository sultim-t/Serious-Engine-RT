/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CtrlEditBoolean.h : header file
//
#ifndef CTRLEDITBOOLEAN_H
#define CTRLEDITBOOLEAN_H 1

class CPropertyComboBar;
/////////////////////////////////////////////////////////////////////////////
// CCtrlEditBoolean window

class CCtrlEditBoolean : public CButton
{
// Construction
public:
	CCtrlEditBoolean();

// Attributes
public:
  // ptr to parent dialog
  CWnd *m_pDialog;

// Operations
public:
  // sets ptr to parent dialog
  void SetDialogPtr( CWnd *pDialog);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlEditBoolean)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtrlEditBoolean();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtrlEditBoolean)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  // CTRLEDITBOOLEAN_H