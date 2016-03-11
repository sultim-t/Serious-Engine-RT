/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_TEXTBOX_H__F08BF4CB_B32A_11D5_8AB4_00C0262D9BFE__INCLUDED_)
#define AFX_TEXTBOX_H__F08BF4CB_B32A_11D5_8AB4_00C0262D9BFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextBox window

class CTextBox : public CEdit
{
// Construction
public:
	CTextBox();
  void ValueChanged();

  CTString m_strID; // ID of control (base float)
  void SetDataPtr(FLOAT *pFloat);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTextBox();


	// Generated message map functions
protected:
	//{{AFX_MSG(CTextBox)
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTBOX_H__F08BF4CB_B32A_11D5_8AB4_00C0262D9BFE__INCLUDED_)
