/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CtrlEditFloat.h : header file
//
#ifndef CTRLEDITFLOAT_H
#define CTRLEDITFLOAT_H 1

/////////////////////////////////////////////////////////////////////////////
// CCtrlEditFloat window

class CCtrlEditFloat : public CEdit
{
// Construction
public:
	CCtrlEditFloat();

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
	//{{AFX_VIRTUAL(CCtrlEditFloat)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtrlEditFloat();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtrlEditFloat)
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  // CTRLEDITFLOAT_H