/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// PressKeyEditControl.h : header file
//
#ifndef PRESSKEYEDITCONTROL_H
#define PRESSKEYEDITCONTROL_H 1

/////////////////////////////////////////////////////////////////////////////
// CPressKeyEditControl window

class CPressKeyEditControl : public CEdit
{
// Construction
public:
	CPressKeyEditControl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPressKeyEditControl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPressKeyEditControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPressKeyEditControl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // PRESSKEYEDITCONTROL_H 1
