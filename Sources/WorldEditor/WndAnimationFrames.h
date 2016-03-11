/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// WndAnimationFrames.h : header file
//

#ifndef WNDANIMATIONFRAMES_H
#define WNDANIMATIONFRAMES_H 1

class CDlgLightAnimationEditor;

/////////////////////////////////////////////////////////////////////////////
// CWndAnimationFrames window

class CWndAnimationFrames : public CWnd
{
// Construction
public:
	CWndAnimationFrames();
	inline void SetParentDlg( CDlgLightAnimationEditor *pParentDlg) {m_pParentDlg=pParentDlg;};
	void DeleteSelectedFrame( void);
  INDEX GetFrame( INDEX iFramePosition);
  BOOL IsFrameVisible(INDEX iFrame);
  BOOL IsSelectedFrameKeyFrame(void);
  void ScrollLeft(void);
  void ScrollRight(void);
  void ScrollPgLeft(void);
  void ScrollPgRight(void);

// Attributes
public:
  INDEX m_iFramesInLine;
  INDEX m_iStartingFrame;
  INDEX m_iSelectedFrame;
  CDlgLightAnimationEditor *m_pParentDlg;
  CDrawPort *m_pDrawPort;
  CViewPort *m_pViewPort;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndAnimationFrames)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndAnimationFrames();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndAnimationFrames)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // WNDANIMATIONFRAMES_H
