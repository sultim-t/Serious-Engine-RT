/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CtrlAxisRadio.h : header file
//

#ifndef CTRLAXISRADIO_H
#define CTRLAXISRADIO_H 1

class CPropertyComboBar;
/////////////////////////////////////////////////////////////////////////////
// CCtrlAxisRadio window

class CCtrlAxisRadio : public CButton
{
// Construction
public:
	CCtrlAxisRadio();

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
	//{{AFX_VIRTUAL(CCtrlAxisRadio)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtrlAxisRadio();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtrlAxisRadio)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif    // CTRLAXISRADIO_H
