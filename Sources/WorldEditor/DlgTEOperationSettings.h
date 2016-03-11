/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGTEOPERATIONSETTINGS_H__81F7BA10_AB5C_4B29_B028_F45E397EAF40__INCLUDED_)
#define AFX_DLGTEOPERATIONSETTINGS_H__81F7BA10_AB5C_4B29_B028_F45E397EAF40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTEOperationSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTEOperationSettings dialog

CTString GetFilterName(INDEX iFilter);

class CDlgTEOperationSettings : public CDialog
{
// Construction
public:
	CDlgTEOperationSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTEOperationSettings)
	enum { IDD = IDD_TE_OPTION_SETTINGS };
	CComboBox	m_ctrlGenerationMethod;
	CComboBox	m_ctrlFilter;
	float	m_fClampAltitude;
	float	m_fNoiseAltitude;
	float	m_fPaintPower;
	float	m_fPosterizeStep;
	float	m_fSmoothPower;
	float	m_fFilterPower;
	CString	m_strContinousNoiseTexture;
	CString	m_strDistributionNoiseTexture;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTEOperationSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTEOperationSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnViewNoiseTexture();
	afx_msg void OnBrowseContinousNoise();
	afx_msg void OnBrowseDistributionNoise();
	afx_msg void OnViewDistributionNoiseTexture();
	afx_msg void OnGenerationSettings();
	afx_msg void OnDropdownGenerationAlgorithm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTEOPERATIONSETTINGS_H__81F7BA10_AB5C_4B29_B028_F45E397EAF40__INCLUDED_)
