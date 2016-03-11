/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPlayerAppearance.h : header file
//
#ifndef DLGPLAYERAPPEARANCE_H
#define DLGPLAYERAPPEARANCE_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPlayerAppearance dialog

class CDlgPlayerAppearance : public CDialog
{
// Construction
public:
	CPlayerCharacter m_pcPlayerCharacter;
	CDlgPlayerAppearance(CPlayerCharacter &pcPlayerCharacter, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPlayerAppearance)
	enum { IDD = IDD_PLAYER_APPEARANCE };
	CComboBox	m_comboAvailableAppearances;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPlayerAppearance)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPlayerAppearance)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGPLAYERAPPEARANCE_H