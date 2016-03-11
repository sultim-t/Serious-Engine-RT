/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGINFOPGMIP_H__2B26F795_E15C_11D2_8532_004095812ACC__INCLUDED_)
#define AFX_DLGINFOPGMIP_H__2B26F795_E15C_11D2_8532_004095812ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInfoPgMip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoPgMip dialog

class CDlgInfoPgMip : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgInfoPgMip)

// Construction
public:
	CUpdateable m_udAllValues;
	BOOL OnIdle(LONG lCount);

  CDlgInfoPgMip();
	~CDlgInfoPgMip();

  void SetMipPageFromView(CModelerView* pModelerView);
  void SetViewFromMipPage(CModelerView* pModelerView);
  void ToggleMipFlag( ULONG ulFlag);

// Dialog Data
	//{{AFX_DATA(CDlgInfoPgMip)
	enum { IDD = IDD_INFO_MIP };
	CString	m_strCurrentMipModel;
	CString	m_strModelDistance;
	CString	m_strCurrentMipFactor;
	CString	m_strModelMipSwitchFactor;
	CString	m_strNoOfPolygons;
	CString	m_strNoOfVertices;
	CString	m_strNoOfTriangles;
	BOOL	m_bHasPatches;
	BOOL	m_bHasAttachedModels;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgInfoPgMip)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgInfoPgMip)
	afx_msg void OnHasPatches();
	afx_msg void OnHasAttachedModels();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINFOPGMIP_H__2B26F795_E15C_11D2_8532_004095812ACC__INCLUDED_)
