/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPgCollision.h : header file
//
#ifndef DLGPGCOLLISION_H
#define DLGPGCOLLISION_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPgCollision dialog

class CDlgPgCollision : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgPgCollision)

// Construction
public:
	CUpdateable m_udAllValues;
	CDlgPgCollision();
	~CDlgPgCollision();
	BOOL OnIdle(LONG lCount);

// Dialog Data
	//{{AFX_DATA(CDlgPgCollision)
	enum { IDD = IDD_INFO_COLLISION };
	float	m_fWidth;
	float	m_fHeight;
	float	m_fLenght;
	float	m_fXCenter;
	float	m_fYDown;
	float	m_fZCenter;
	int		m_EqualityRadio;
	CString	m_strCollisionBoxName;
	CString	m_strCollisionBoxIndex;
	BOOL	m_bCollideAsBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgCollision)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgPgCollision)
	afx_msg void OnChangeEditWidth();
	afx_msg void OnChangeEditHeight();
	afx_msg void OnChangeEditLenght();
	afx_msg void OnChangeEditXCenter();
	afx_msg void OnChangeEditYDown();
	afx_msg void OnChangeEditZCenter();
	afx_msg void OnHEqW();
	afx_msg void OnLEqW();
	afx_msg void OnLEqH();
	afx_msg void OnAddCollisionBox();
	afx_msg void OnChangeCollisionBoxName();
	afx_msg void OnNextCollisionBox();
	afx_msg void OnPreviousCollisionBox();
	afx_msg void OnRemoveCollisionBox();
	afx_msg void OnCollideAsBox();
	afx_msg void OnAllignToSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // DLGPGCOLLISION_H
