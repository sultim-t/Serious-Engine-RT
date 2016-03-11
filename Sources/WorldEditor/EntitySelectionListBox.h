/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// EntitySelectionListBox.h : header file
//
#ifndef ENTITISELECTIONLISTBOX_H
#define ENTITISELECTIONLISTBOX_H 1

/////////////////////////////////////////////////////////////////////////////
// CEntitySelectionListBox window

class CEntitySelectionListBox : public CCheckListBox
{
// Construction
public:
	CEntitySelectionListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEntitySelectionListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEntitySelectionListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEntitySelectionListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // ENTITISELECTIONLISTBOX_H
