/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGDISPLACEMAPSIZE_H__E70A7D13_781F_11D4_851E_000021291DC7__INCLUDED_)
#define AFX_DLGDISPLACEMAPSIZE_H__E70A7D13_781F_11D4_851E_000021291DC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDisplaceMapSize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDisplaceMapSize dialog

class CDlgDisplaceMapSize : public CDialog
{
// Construction
public:
	CDlgDisplaceMapSize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDisplaceMapSize)
	enum { IDD = IDD_DISPLACE_MAP_SIZE };
	UINT	m_pixWidth;
	UINT	m_pixHeight;
	BOOL	m_bMidPixSample;
	BOOL	m_bHighResolution;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDisplaceMapSize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDisplaceMapSize)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDISPLACEMAPSIZE_H__E70A7D13_781F_11D4_851E_000021291DC7__INCLUDED_)
