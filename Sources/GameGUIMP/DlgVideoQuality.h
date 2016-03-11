/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgVideoQuality.h : header file
//
#ifndef DLGVIDEOQUALITY_H
#define DLGVIDEOQUALITY_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgVideoQuality dialog

class CDlgVideoQuality : public CDialog
{
// Construction
public:
	CDlgVideoQuality(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgVideoQuality)
	enum { IDD = IDD_VIDEO_QUALITY };
	int		m_radioObjectShadowQuality;
	int		m_radioTextureQuality;
	int		m_radioWorldShadowQuality;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVideoQuality)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgVideoQuality)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGVIDEOQUALITY_H