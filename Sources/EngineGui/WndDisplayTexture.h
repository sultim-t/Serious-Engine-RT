/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_WNDDISPLAYTEXTURE_H__4B489BC1_FAD9_11D1_82EA_000000000000__INCLUDED_)
#define AFX_WNDDISPLAYTEXTURE_H__4B489BC1_FAD9_11D1_82EA_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WndDisplayTexture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndDisplayTexture window

class CWndDisplayTexture : public CWnd
{
// Construction
public:
	CWndDisplayTexture();

  // function that is called when lmb is clicked
  inline void SetLeftMouseButtonClicked( void(*pLeftMouseButtonClicked)( PIX pixX, PIX pixY))
    {m_pLeftMouseButtonClicked=pLeftMouseButtonClicked;};
  // function that is called when lmb is released
  inline void SetLeftMouseButtonReleased( void(*pLeftMouseButtonReleased)( PIX pixX, PIX pixY))
    {m_pLeftMouseButtonReleased=pLeftMouseButtonReleased;};
  // function that is called when rmb is clicked
  inline void SetRightMouseButtonClicked( void(*pRightMouseButtonClicked)( PIX pixX, PIX pixY))
    {m_pRightMouseButtonClicked=pRightMouseButtonClicked;};
  // function that is called when rmb is moved
  inline void SetRightMouseButtonMoved( void(*pRightMouseButtonMoved)( PIX pixX, PIX pixY))
    {m_pRightMouseButtonMoved=pRightMouseButtonMoved;};

  void (*m_pLeftMouseButtonClicked)( PIX pixX, PIX pixY);
  void (*m_pLeftMouseButtonReleased)( PIX pixX, PIX pixY);
  void (*m_pRightMouseButtonClicked)( PIX pixX, PIX pixY);
  void (*m_pRightMouseButtonMoved)( PIX pixX, PIX pixY);
  CTextureObject m_toTexture;
  CDrawPort *m_pDrawPort;
  CViewPort *m_pViewPort;
  int m_iTimerID;
  BOOL m_bChequeredAlpha;
  BOOL m_bForce32;
  FLOAT m_fWndTexRatio;
  PIX m_pixWinWidth; 
  PIX m_pixWinHeight;
  PIX m_pixWinOffsetU; 
  PIX m_pixWinOffsetV;
public:
  BOOL m_bDrawLine;
  PIX m_pixLineStartU;
  PIX m_pixLineStartV;
  PIX m_pixLineStopU;
  PIX m_pixLineStopV;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndDisplayTexture)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndDisplayTexture();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndDisplayTexture)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDDISPLAYTEXTURE_H__4B489BC1_FAD9_11D1_82EA_000000000000__INCLUDED_)
