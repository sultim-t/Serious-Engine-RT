/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// StainsComboBox.h : header file
//
#ifndef STAINSCOMBOBOX_H
#define STAINSCOMBOBOX_H 1

/////////////////////////////////////////////////////////////////////////////
// CStainsComboBox window

class CStainsComboBox : public CComboBox
{
// Construction
public:
	CStainsComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStainsComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStainsComboBox();
  void Refresh();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStainsComboBox)
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // STAINSCOMBOBOX_H
