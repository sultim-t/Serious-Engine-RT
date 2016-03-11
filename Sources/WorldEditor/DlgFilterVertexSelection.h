/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGFILTERVERTEXSELECTION_H__BE198003_81A4_11D5_871A_00002103143B__INCLUDED_)
#define AFX_DLGFILTERVERTEXSELECTION_H__BE198003_81A4_11D5_871A_00002103143B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFilterVertexSelection.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFilterVertexSelection dialog

class CDlgFilterVertexSelection : public CDialog
{
// Construction
public:
	CDlgFilterVertexSelection(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFilterVertexSelection)
	enum { IDD = IDD_FILTER_VERTEX_SELECTION };
	float	m_fMaxY;
	float	m_fMaxX;
	float	m_fMaxZ;
	float	m_fMinX;
	float	m_fMinY;
	float	m_fMinZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFilterVertexSelection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFilterVertexSelection)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFILTERVERTEXSELECTION_H__BE198003_81A4_11D5_871A_00002103143B__INCLUDED_)
