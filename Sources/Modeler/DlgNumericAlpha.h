/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGNUMERICALPHA_H__2B26F794_E15C_11D2_8532_004095812ACC__INCLUDED_)
#define AFX_DLGNUMERICALPHA_H__2B26F794_E15C_11D2_8532_004095812ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNumericAlpha.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNumericAlpha dialog

class CDlgNumericAlpha : public CDialog
{
// Construction
public:
	CDlgNumericAlpha(int iAlpha = 0, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNumericAlpha)
	enum { IDD = IDD_NUMERIC_ALPHA };
	int		m_iAlpha;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNumericAlpha)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNumericAlpha)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNUMERICALPHA_H__2B26F794_E15C_11D2_8532_004095812ACC__INCLUDED_)
