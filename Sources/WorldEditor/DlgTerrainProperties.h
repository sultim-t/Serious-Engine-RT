/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGTERRAINPROPERTIES_H__44370841_18E0_4F93_8BF8_C0640205AE46__INCLUDED_)
#define AFX_DLGTERRAINPROPERTIES_H__44370841_18E0_4F93_8BF8_C0640205AE46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTerrainProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTerrainProperties dialog

class CDlgTerrainProperties : public CDialog
{
// Construction
public:
	CDlgTerrainProperties(CWnd* pParent = NULL);   // standard constructor
  void PrepareGlobalPretenderCombo(void);
  void PrepareTilePretenderCombo(void);
  void InitComboBoxes(void);

// Dialog Data
	//{{AFX_DATA(CDlgTerrainProperties)
	enum { IDD = IDD_TERRAIN_PROPERTIES };
	CComboBox	m_ctrlGlobalPretenderTexture;
	CComboBox	m_ctrlTilePretender;
	CSliderCtrl	m_ctrlShadingMapSlider;
	CComboBox	m_ctrlQuadsPerTile;
	CComboBox	m_ctrlHeightMapWidth;
	CComboBox	m_ctrlHeightMapHeight;
	CSliderCtrl	m_ctrlShadowMapSlider;
	CString	m_strHeightmapSize;
	CString	m_strShadowMapSize;
	CString	m_strTerrainPretender;
	CString	m_strTilePretender;
	float	m_fTerrainLength;
	float	m_fTerrainHeight;
	float	m_fTerrainWidth;
	float	m_fLODSwitch;
	CString	m_strShadingMapSize;
	CString	m_strMemoryConsumption;
	CString	m_strLayerMemory;
	CString	m_strEdgeMap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTerrainProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTerrainProperties)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTerrainHmWidth();
	afx_msg void OnSelchangeTerrainHmHeight();
	virtual void OnOK();
	afx_msg void OnSelchangeTilePretender();
	afx_msg void OnSelchangeGlobalPretender();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTERRAINPROPERTIES_H__44370841_18E0_4F93_8BF8_C0640205AE46__INCLUDED_)
