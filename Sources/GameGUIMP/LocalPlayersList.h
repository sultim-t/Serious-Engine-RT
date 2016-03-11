/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// LocalPlayersList.h : header file
//
#ifndef LOCALPLAYERSLIST_H
#define LOCALPLAYERSLIST_H 1

/////////////////////////////////////////////////////////////////////////////
// CLocalPlayersList window

class CLocalPlayersList : public CCheckListBox
{
// Construction
public:
	CLocalPlayersList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocalPlayersList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLocalPlayersList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLocalPlayersList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // LOCALPLAYERSLIST_H