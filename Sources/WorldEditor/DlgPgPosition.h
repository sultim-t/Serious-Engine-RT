/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPgPosition.h : header file
//
#ifndef DLGPGPOSITION_H
#define DLGPGPOSITION_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPgPosition dialog

class CDlgPgPosition : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgPgPosition)

// Construction
public:
	CDlgPgPosition();
	~CDlgPgPosition();
  BOOL OnIdle(LONG lCount);
  CUpdateableRT m_udSelection;

// Dialog Data
	//{{AFX_DATA(CDlgPgPosition)
	enum { IDD = IDD_PG_POSITION };
	float	m_fBanking;
	float	m_fHeading;
	float	m_fPitch;
	float	m_fX;
	float	m_fY;
	float	m_fZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgPosition)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgPgPosition)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // DLGPGPOSITION_H
