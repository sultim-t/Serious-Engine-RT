/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGCHOOSETEXTURETYPE_H__C517CED1_FA6C_11D1_82E9_000000000000__INCLUDED_)
#define AFX_DLGCHOOSETEXTURETYPE_H__C517CED1_FA6C_11D1_82E9_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgChooseTextureType.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseTextureType dialog

class CDlgChooseTextureType : public CDialog
{
// Construction
public:
	CDlgChooseTextureType(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChooseTextureType)
	enum { IDD = IDD_CHOOSE_TEXTURE_TYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChooseTextureType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgChooseTextureType)
	afx_msg void OnAnimatedTexture();
	afx_msg void OnEffectTexture();
	afx_msg void OnNormalTexture();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOOSETEXTURETYPE_H__C517CED1_FA6C_11D1_82E9_000000000000__INCLUDED_)
