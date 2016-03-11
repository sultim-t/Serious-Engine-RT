/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CSGDesitnationCombo.h : header file
//
#if !defined(AFX_TRIANGULARISATIONCOMBO_H__1FB481F3_78B0_11D2_8437_004095812ACC__INCLUDED_)
#define AFX_TRIANGULARISATIONCOMBO_H__1FB481F3_78B0_11D2_8437_004095812ACC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TriangularisationCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTriangularisationCombo window

class CTriangularisationCombo : public CComboBox
{
// Construction
public:
	CTriangularisationCombo();
  BOOL OnIdle(LONG lCount);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTriangularisationCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTriangularisationCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTriangularisationCombo)
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIANGULARISATIONCOMBO_H__1FB481F3_78B0_11D2_8437_004095812ACC__INCLUDED_)
