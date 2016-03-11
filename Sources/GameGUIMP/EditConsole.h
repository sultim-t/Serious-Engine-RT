/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_EDITCONSOLE_H__833B8174_E923_11D1_82A7_000000000000__INCLUDED_)
#define AFX_EDITCONSOLE_H__833B8174_E923_11D1_82A7_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EditConsole.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditConsole window

class CEditConsole : public CEdit
{
// Construction
public:
	CEditConsole();

// Attributes
public:

// Operations
public:
  void SetTextFromConsole(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditConsole)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditConsole();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditConsole)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCONSOLE_H__833B8174_E923_11D1_82A7_000000000000__INCLUDED_)
