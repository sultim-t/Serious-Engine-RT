/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgInfoPgAnim.h : header file
//
#ifndef DLGINFOPGANIM
#define DLGINFOPGANIM 1

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoPgAnim dialog

class CDlgInfoPgAnim : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgInfoPgAnim)

// Construction
public:
	CDlgInfoPgAnim();
	~CDlgInfoPgAnim();
  void SetAnimPageFromView(CModelerView* pModelerView);
	BOOL OnIdle(LONG lCount);
  CUpdateable m_udAllValues;
  
// Dialog Data
	//{{AFX_DATA(CDlgInfoPgAnim)
	enum { IDD = IDD_INFO_ANIMATION };
	CString	m_strCurrentFrame;
	CString	m_strFramesInAnim;
	CString	m_strAnimName;
	CString	m_strTimePassed;
	CString	m_strAnimState;
	CString	m_strAnimationLenght;
	CString	m_strNoOfAnimations;
	CString	m_strNoOfFrames;
	float	m_fAnimSpeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgInfoPgAnim)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgInfoPgAnim)
	afx_msg void OnChangeAnimSpeed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // DLGINFOPGANIM
