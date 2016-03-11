/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGAUTTEXTURIZE_H__17E4B1E3_B1A2_11D5_8748_00002103143B__INCLUDED_)
#define AFX_DLGAUTTEXTURIZE_H__17E4B1E3_B1A2_11D5_8748_00002103143B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAutTexturize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAutTexturize dialog

class CDlgAutTexturize : public CDialog
{
// Construction
public:
  PIX m_pixWidth;
  PIX m_pixHeight;
  INDEX m_iPretenderStyle;
  CDlgAutTexturize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAutTexturize)
	enum { IDD = IDD_AUTO_TEXTURIZE };
	CComboBox	m_ctrlPretenderTextureStyle;
	CColoredButton	m_colBcg;
	CComboBox	m_ctrPretenderTextureSize;
	BOOL	m_bExpandEdges;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAutTexturize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAutTexturize)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAUTTEXTURIZE_H__17E4B1E3_B1A2_11D5_8748_00002103143B__INCLUDED_)
