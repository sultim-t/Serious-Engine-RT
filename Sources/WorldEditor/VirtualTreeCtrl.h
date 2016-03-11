/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// VirtualTreeCtrl.h : header file
//
#ifndef VIRTUALTREECTRL_H
#define VIRTUALTREECTRL_H 1

/////////////////////////////////////////////////////////////////////////////
// CVirtualTreeCtrl window

class CBrowser;

class CVirtualTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CVirtualTreeCtrl();

// Attributes
public:
  CBrowser *m_pBrowser;
  COleDataSource m_DataSource;
  BOOL m_bIsOpen;

// Operations
public:
  void OpenTreeCtrl(void);
  void CloseTreeCtrl(void);
  void SetBrowserPtr( CBrowser *pBrowser);
	void OnContextMenu( CPoint point);
  CVirtualTreeNode *ItemForCoordinate(CPoint pt);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVirtualTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVirtualTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVirtualTreeCtrl)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // VIRTUALTREECTRL_H
