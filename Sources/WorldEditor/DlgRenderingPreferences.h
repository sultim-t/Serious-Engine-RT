/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgRenderingPreferences.h : header file
//
#ifndef DLGRENDERINGPREFERENCES_H
#define DLGRENDERINGPREFERENCES_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgRenderingPreferences dialog

class CDlgRenderingPreferences : public CDialog
{
// Construction
public:
	CDlgRenderingPreferences(INDEX iBuffer, CWnd* pParent = NULL);   // standard constructor
  void UpdateEditRangeControl();
  INDEX m_iBuffer;

// Dialog Data
	//{{AFX_DATA(CDlgRenderingPreferences)
	enum { IDD = IDD_RENDERING_PREFERENCES };
	CComboBox	m_comboFlareFX;
	CColoredButton	m_SelectionColor;
	CColoredButton	m_GridColor;
	CColoredButton	m_PaperColor;
	CComboBox	m_TextureFillType;
	CComboBox	m_EdgesFillType;
	CComboBox	m_PolygonFillType;
	CComboBox	m_VertexFillType;
	CColoredButton	m_VertexColors;
	CColoredButton	m_PolygonColors;
	CColoredButton	m_EdgesColors;
	BOOL	m_bBoundingBox;
	BOOL	m_bHidenLines;
	BOOL	m_bShadows;
	BOOL	m_bWireFrame;
	float	m_fRenderingRange;
	BOOL	m_bAutoRenderingRange;
	BOOL	m_bRenderEditorModels;
	BOOL	m_bUseTextureForBcg;
	BOOL	m_bRenderFieldBrushes;
	BOOL	m_bRenderFog;
	BOOL	m_bRenderHaze;
	BOOL	m_bRenderMirrors;
	CString	m_strBcgTexture;
	float	m_fFarClipPlane;
	BOOL	m_bApplyFarClipInIsometricProjection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRenderingPreferences)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRenderingPreferences)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoadPreferences();
	afx_msg void OnSavePreferences();
	afx_msg void OnAutoRenderingRange();
	afx_msg void OnBrowseBcgPicture();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
#endif // DLGRENDERINGPREFERENCES_H
