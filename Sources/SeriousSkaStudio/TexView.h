/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_TEXVIEW_H__F222519C_8EA7_41DC_B346_5A788032B2C2__INCLUDED_)
#define AFX_TEXVIEW_H__F222519C_8EA7_41DC_B346_5A788032B2C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TexView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTexView window

class CTexView : public CWnd
{
// Construction
public:
	CTexView();

// Attributes
public:

// Operations
public:
  CDrawPort *m_pDrawPort;
  CViewPort *m_pViewPort;
  CTextureObject m_ptoPreview;
  void ChangeTexture(CTString strNewTexObject);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTexView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTexView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTexView)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXVIEW_H__F222519C_8EA7_41DC_B346_5A788032B2C2__INCLUDED_)
