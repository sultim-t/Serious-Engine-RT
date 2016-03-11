/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */


// DlgInfoFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoFrame frame

class CDlgInfoFrame : public CMiniFrameWnd
{
// Constructor
public:
	CDlgInfoFrame();
  ~CDlgInfoFrame();
  void SetSizes();

// Attributes
public:
  int m_PageWidth;
  int m_PageHeight;
	CDlgInfoSheet *m_pInfoSheet;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInfoFrame)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Handlers
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgInfoFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
