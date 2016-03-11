/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgInfoPgRendering.h : header file 
//
#ifndef DLGINFOPGRENDERING
#define DLGINFOPGRENDERING 1

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoPgRendering dialog

class CDlgInfoPgRendering : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgInfoPgRendering)

// Construction
public:
	CDlgInfoPgRendering();
	~CDlgInfoPgRendering();
	BOOL OnIdle(LONG lCount);
  
  CUpdateable m_udAllValues;

// Dialog Data
	//{{AFX_DATA(CDlgInfoPgRendering)
	enum { IDD = IDD_INFO_RENDERING };
	CComboBox	m_comboTranslucency;
	CComboBox	m_comboShading;
	CColoredButton	m_colorSpecular;
	CColoredButton	m_colorReflections;
	CColoredButton	m_colorDiffuse;
	CColoredButton	m_colorBump;
	CString	m_strMipModel;
	CString	m_strSurfaceName;
	CCtrlEditBoolean		m_IsDoubleSided;
	CCtrlEditBoolean		m_IsInvisible;
	CCtrlEditBoolean		m_IsDiffuse;
	CCtrlEditBoolean		m_IsReflections;
	CCtrlEditBoolean		m_IsSpecular;
	CCtrlEditBoolean		m_IsBump;
	CCtrlEditBoolean	  m_IsDetail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgInfoPgRendering)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgInfoPgRendering)
	afx_msg void OnSelchangeShading();
	afx_msg void OnSelchangeTranslucency();
	afx_msg void OnSelectAllSurfaces();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // DLGINFOPGRENDERING
