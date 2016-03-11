/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CtrlEditString.h : header file
//
#ifndef CTRLEDITSTRING_H
#define CTRLEDITSTRING_H 1

class CPropertyComboBar;
/////////////////////////////////////////////////////////////////////////////
// CCtrlEditString window

class CCtrlEditString : public CEdit
{
// Construction
public:
	CCtrlEditString();

// Attributes
public:
  // ptr to parent dialog
  CPropertyComboBar *m_pDialog;

// Operations
public:
  // sets ptr to parent dialog
  void SetDialogPtr( CPropertyComboBar *pDialog);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlEditString)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtrlEditString();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtrlEditString)
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  // CTRLEDITSTRING_H
