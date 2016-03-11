/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_CTLTIPOFTHEDAYTEXT_H__F7006AF5_44F1_11D4_93A2_004095812ACC__INCLUDED_)
#define AFX_CTLTIPOFTHEDAYTEXT_H__F7006AF5_44F1_11D4_93A2_004095812ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CtlTipOfTheDayText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCtlTipOfTheDayText window

class CCtlTipOfTheDayText : public CStatic
{
// Construction
public:
	CCtlTipOfTheDayText();

// Attributes
public:
  CString m_strTipText;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtlTipOfTheDayText)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtlTipOfTheDayText();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtlTipOfTheDayText)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTLTIPOFTHEDAYTEXT_H__F7006AF5_44F1_11D4_93A2_004095812ACC__INCLUDED_)
