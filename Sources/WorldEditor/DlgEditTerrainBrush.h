/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLGEDITTERRAINBRUSH_H__A349A7AF_EA7F_4EC3_8B44_7C191475F493__INCLUDED_)
#define AFX_DLGEDITTERRAINBRUSH_H__A349A7AF_EA7F_4EC3_8B44_7C191475F493__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditTerrainBrush.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEditTerrainBrush dialog

class CDlgEditTerrainBrush : public CDialog
{
// Construction
public:
	CDlgEditTerrainBrush(CWnd* pParent = NULL);   // standard constructor
  INDEX m_iBrush;

// Dialog Data
	//{{AFX_DATA(CDlgEditTerrainBrush)
	enum { IDD = IDD_EDIT_TERRAIN_BRUSH };
	float	m_fFallOff;
	float	m_fHotSpot;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditTerrainBrush)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditTerrainBrush)
	afx_msg void OnGenerateTerrainBrush();
	afx_msg void OnImportTerrainBrush();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITTERRAINBRUSH_H__A349A7AF_EA7F_4EC3_8B44_7C191475F493__INCLUDED_)
