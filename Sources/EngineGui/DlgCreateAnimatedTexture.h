/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGCREATEANIMATEDTEXTURE_H__C517CED3_FA6C_11D1_82E9_000000000000__INCLUDED_)
#define AFX_DLGCREATEANIMATEDTEXTURE_H__C517CED3_FA6C_11D1_82E9_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgCreateAnimatedTexture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCreateAnimatedTexture dialog

class CDlgCreateAnimatedTexture : public CDialog
{
// Construction
public:
	CDlgCreateAnimatedTexture(CDynamicArray<CTFileName> &afnPictures, CWnd* pParent = NULL);   // standard constructor
	~CDlgCreateAnimatedTexture();
  void ReleaseCreatedTexture(void);
  void InitAnimationsCombo(void);
  void RefreshTexture(void);
  
  CDynamicArray<CTFileName> *m_pafnPictures;
  BOOL m_bPreviewWindowsCreated;
  PIX m_pixSourceWidth;
  PIX m_pixSourceHeight;
  CTFileName m_fnSourceFileName;
  CTFileName m_fnCreatedFileName;
  CWndDisplayTexture m_wndViewDetailTexture;
  CWndDisplayTexture m_wndViewCreatedTexture;
  CTextureData *m_ptdCreated;

// Dialog Data
	//{{AFX_DATA(CDlgCreateAnimatedTexture)
	enum { IDD = IDD_CREATE_ANIMATED_TEXTURE };
	CButton	m_ctrlCheckButton;
	CComboBox	m_ctrlAnimationsCombo;
	CString	m_strEditScript;
	CString	m_strSizeInPixels;
	CString	m_strCreatedTextureName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCreateAnimatedTexture)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCreateAnimatedTexture)
	afx_msg void OnPaint();
	afx_msg void OnChequeredAlpha();
	afx_msg void OnBrowseDetail();
	afx_msg void OnDetailNone();
	afx_msg void OnCreateTexture();
	afx_msg void OnRefreshTexture();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTextureAnimations();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCREATEANIMATEDTEXTURE_H__C517CED3_FA6C_11D1_82E9_000000000000__INCLUDED_)
