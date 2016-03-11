/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGPGSHADOW_H__A1A8F835_D928_11D2_8513_004095812ACC__INCLUDED_)
#define AFX_DLGPGSHADOW_H__A1A8F835_D928_11D2_8513_004095812ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPgShadow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPgShadow dialog

class CDlgPgShadow : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgPgShadow)

// Construction
public:
	CDlgPgShadow();
	~CDlgPgShadow();
  BOOL OnIdle(LONG lCount);
  void InitComboBoxes(void);

  CUpdateableRT m_udPolygonSelection;

// Dialog Data
	//{{AFX_DATA(CDlgPgShadow)
	enum { IDD = IDD_PG_SHADOW };
	CComboBox	m_ctrlComboGradient;
	CCtrlEditBoolean	m_bDarkCorners;
	CCtrlEditBoolean	m_bNoDynamicLights;
	CCtrlEditBoolean	m_bDontReceiveShadows;
	CCtrlEditBoolean	m_bDynamicLightsOnly;
	CCtrlEditBoolean	m_bHasDirectionalAmbient;
	CCtrlEditBoolean	m_bHasPreciseShadows;
	CCtrlEditBoolean	m_bNoPlaneDiffusion;
	CCtrlEditBoolean	m_bHasDirectionalShadows;
	CCtrlEditBoolean	m_NoShadow;
	CCtrlEditBoolean	m_IsLightBeamPassable;
	CColoredButton	m_ctrlShadowColor;
	CComboBox	m_comboShadowBlend;
	CComboBox	m_ctrlComboClusterSize;
	CComboBox	m_ComboIllumination;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgShadow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgPgShadow)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeShadowClusterSizeCombo();
	afx_msg void OnSelchangeIlluminationCombo();
	afx_msg void OnDropdownIlluminationCombo();
	afx_msg void OnSelchangeShadowBlendCombo();
	afx_msg void OnDropdownShadowBlendCombo();
	afx_msg void OnDropdownClusterSizeCombo();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDropdownGradientCombo();
	afx_msg void OnSelchangeGradientCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPGSHADOW_H__A1A8F835_D928_11D2_8513_004095812ACC__INCLUDED_)
