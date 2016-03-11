/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgMultiplyMapping.h : header file
//
#ifndef DLGMULTIPLYMAPPING_H
#define DLGMULTIPLYMAPPING_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgMultiplyMapping dialog

class CDlgMultiplyMapping : public CDialog
{
// Construction
public:
  FLOAT3D m_f3MultiplyValues;
  CDlgMultiplyMapping(FLOAT3D f3InitialValues = FLOAT3D(1.0f, 1.0f, 1.0f),
    CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMultiplyMapping)
	enum { IDD = IDD_MULTIPLY_MAPPING };
	float	m_fMultiplyXOffset;
	float	m_fMultiplyYOffset;
	float	m_fMultiplyZoom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMultiplyMapping)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMultiplyMapping)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGMULTIPLYMAPPING_H
