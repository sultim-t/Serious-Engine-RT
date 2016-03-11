/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGSNAPVERTEX_H__BE198005_81A4_11D5_871A_00002103143B__INCLUDED_)
#define AFX_DLGSNAPVERTEX_H__BE198005_81A4_11D5_871A_00002103143B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSnapVertex.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSnapVertex dialog

class CDlgSnapVertex : public CDialog
{
// Construction
public:
	CDlgSnapVertex(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSnapVertex)
	enum { IDD = IDD_SNAP_VERTEX };
	float	m_fX;
	float	m_fY;
	float	m_fZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSnapVertex)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSnapVertex)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSNAPVERTEX_H__BE198005_81A4_11D5_871A_00002103143B__INCLUDED_)
