/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_EDITMIPSWITCHDISTANCE_H__2D4950D3_5FC1_11D4_84EC_000021291DC7__INCLUDED_)
#define AFX_EDITMIPSWITCHDISTANCE_H__2D4950D3_5FC1_11D4_84EC_000021291DC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditMipSwitchDistance.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditMipSwitchDistance window

class CEditMipSwitchDistance : public CEdit
{
// Construction
public:
  BOOL OnIdle(LONG lCount);
	CEditMipSwitchDistance();
  CBrushMip *m_pbrmBrushMipSelected;
  FLOAT m_fLastValue;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditMipSwitchDistance)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditMipSwitchDistance();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditMipSwitchDistance)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITMIPSWITCHDISTANCE_H__2D4950D3_5FC1_11D4_84EC_000021291DC7__INCLUDED_)
