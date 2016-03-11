/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGMIRRORANDSTRETCH_H__74A2E0E6_311B_11D3_8611_004095812ACC__INCLUDED_)
#define AFX_DLGMIRRORANDSTRETCH_H__74A2E0E6_311B_11D3_8611_004095812ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMirrorAndStretch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMirrorAndStretch dialog

class CDlgMirrorAndStretch : public CDialog
{
// Construction
public:
	CDlgMirrorAndStretch(CWnd* pParent = NULL);   // standard constructor
  CTString m_strName;

// Dialog Data
	//{{AFX_DATA(CDlgMirrorAndStretch)
	enum { IDD = IDD_MIRROR_AND_STRETCH };
	float	m_fStretch;
	int		m_iMirror;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMirrorAndStretch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMirrorAndStretch)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMIRRORANDSTRETCH_H__74A2E0E6_311B_11D3_8611_004095812ACC__INCLUDED_)
