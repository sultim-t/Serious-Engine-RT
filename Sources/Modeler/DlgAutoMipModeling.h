/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGAUTOMIPMODELING_H__B9701A13_2491_11D2_835E_004095812ACC__INCLUDED_)
#define AFX_DLGAUTOMIPMODELING_H__B9701A13_2491_11D2_835E_004095812ACC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgAutoMipModeling.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoMipModeling dialog

class CDlgAutoMipModeling : public CDialog
{
// Construction
public:
	CDlgAutoMipModeling(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAutoMipModeling)
	enum { IDD = IDD_AUTO_MIP_MODELING };
	int		m_iVerticesToRemove;
	int		m_iSurfacePreservingFactor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAutoMipModeling)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAutoMipModeling)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAUTOMIPMODELING_H__B9701A13_2491_11D2_835E_004095812ACC__INCLUDED_)
