/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_VIEWTEXTURE_H__00278257_91BC_11D2_8478_004095812ACC__INCLUDED_)
#define AFX_VIEWTEXTURE_H__00278257_91BC_11D2_8478_004095812ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewTexture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewTexture window

class CViewTexture : public CWnd
{
// Construction
public:
	CViewTexture();
// Attributes
public:
  CTString m_strTexture;
  COleDataSource m_DataSource;

  CViewPort *m_pViewPort;
  CDrawPort *m_pDrawPort;
 
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewTexture)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CViewTexture();

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewTexture)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRecreateTexture();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWTEXTURE_H__00278257_91BC_11D2_8478_004095812ACC__INCLUDED_)
