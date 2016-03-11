/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_CONSOLESYMBOLSCOMBO_H__E417EA91_1B23_11D2_834D_004095812ACC__INCLUDED_)
#define AFX_CONSOLESYMBOLSCOMBO_H__E417EA91_1B23_11D2_834D_004095812ACC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConsoleSymbolsCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConsoleSymbolsCombo window

class CConsoleSymbolsCombo : public CComboBox
{
// Construction
public:
	CConsoleSymbolsCombo();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConsoleSymbolsCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConsoleSymbolsCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CConsoleSymbolsCombo)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONSOLESYMBOLSCOMBO_H__E417EA91_1B23_11D2_834D_004095812ACC__INCLUDED_)
