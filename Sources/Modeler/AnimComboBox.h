/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// AnimComboBox.h : header file
//
#ifndef ANIMCOMBOBOX_H
#define ANIMCOMBOBOX_H 1

/////////////////////////////////////////////////////////////////////////////
// CAnimComboBox window

class CAnimComboBox : public CComboBox
{
// Construction
public:
	BOOL OnIdle(LONG lCount);
	CModelerView *m_pvLastUpdatedView;
	CAnimComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnimComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnimComboBox)
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // ANIMCOMBOBOX_H
