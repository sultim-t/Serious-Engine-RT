/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPreferences.h : header file
//
#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPreferences dialog

class CDlgPreferences : public CDialog
{
// Construction
public:
	CDlgPreferences(CWnd* pParent = NULL);   // standard constructor
  void InitTextureCombos();
  CAppPrefs m_Prefs;

// Dialog Data
	//{{AFX_DATA(CDlgPreferences)
	enum { IDD = IDD_PREFERENCES };
	CComboBox	m_ctrlGfxApi;
	CColoredButton	m_colorDefaultAmbientColor;
	CColoredButton	m_MappingWinBcgColor;
	CButton	m_OkButton;
	CComboBox	m_ComboWinBcgTexture;
	CColoredButton	m_MappingPaper;
	CColoredButton	m_ModelPaper;
	CColoredButton	m_ModelInk;
	CColoredButton	m_MappingInactiveInk;
	CColoredButton	m_MappingActiveInk;
	BOOL	m_AllwaysLamp;
	BOOL	m_PrefsCopy;
	BOOL	m_AutoMaximize;
	BOOL	m_SetDefaultColors;
	BOOL	m_WindowFit;
	BOOL	m_bIsFloorVisibleByDefault;
	float	m_fDefaultBanking;
	float	m_fDefaultHeading;
	float	m_fDefaultPitch;
	float	m_fDefaultFOW;
	BOOL	m_bIsBcgVisibleByDefault;
	BOOL	m_bAllowSoundLock;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPreferences)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPreferences)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddWorkingTexture();
	afx_msg void OnRemoveWorkingTexture();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGPREFERENCES_H
