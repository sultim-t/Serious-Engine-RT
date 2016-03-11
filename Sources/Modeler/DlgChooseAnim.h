/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DChooseAnim.h : header file
//
#ifndef DLGCHOOSEANIM_H
#define DLGCHOOSEANIM_H 1

/////////////////////////////////////////////////////////////////////////////
// CDChooseAnim dialog

class CDChooseAnim : public CDialog
{
// Construction
public:
	CAnimObject *m_pAnimObject;
	CDChooseAnim(CAnimObject *pAO, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDChooseAnim)
	enum { IDD = IDD_CHOOSEANIMATION };
	CListBox	m_ListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDChooseAnim)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDChooseAnim)
	afx_msg void OnDblclkList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // DLGCHOOSEANIM_H
