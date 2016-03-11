/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_TOOLTIPWND_H__93C6AAC8_0090_11D3_8573_004095812ACC__INCLUDED_)
#define AFX_TOOLTIPWND_H__93C6AAC8_0090_11D3_8573_004095812ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTipWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolTipWnd window

class CToolTipWnd : public CWnd
{
// Construction
public:
	CToolTipWnd();

// Attributes
public:
  POINT m_ptMouse;
  BOOL m_bManualControl;
  PIX m_pixManualX;
  PIX m_pixManualY;
  CTString m_strText;
  CTString GetLine( INDEX iLine);
  INDEX GetLinesCount( void);
  void ObtainTextSize(PIX &pixMaxWidth, PIX &pixMaxHeight);
  void SetupWindowSizeAndPosition(void);
  void ManualUpdate( void);
  void ManualOff(void);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolTipWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPWND_H__93C6AAC8_0090_11D3_8573_004095812ACC__INCLUDED_)
