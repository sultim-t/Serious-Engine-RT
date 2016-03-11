/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgInfoPgPos.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoPgPos dialog

class CDlgInfoPgPos : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgInfoPgPos)

// Construction
public:
	CUpdateable m_udAllValues;
	BOOL OnIdle(LONG lCount);
	CDlgInfoPgPos();
	~CDlgInfoPgPos();

// Dialog Data
	//{{AFX_DATA(CDlgInfoPgPos)
	enum { IDD = IDD_INFO_POSITION };
	float	m_fLightDist;
	float	m_fHeading;
	float	m_fPitch;
	float	m_fBanking;
	float	m_fX;
	float	m_fY;
	float	m_fZ;
	float	m_fFOW;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgInfoPgPos)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgInfoPgPos)
	afx_msg void OnChangeEditHeading();
	afx_msg void OnChangeEditBanking();
	afx_msg void OnChangeEditPitch();
	afx_msg void OnChangeEditX();
	afx_msg void OnChangeEditY();
	afx_msg void OnChangeEditZ();
	afx_msg void OnChangeEditLightDistance();
	afx_msg void OnChangeEditFow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
