/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPlayerControls.h : header file
//
#ifndef DLGPLAYERCONTROLS_H
#define DLGPLAYERCONTROLS_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPlayerControls dialog

class CDlgPlayerControls : public CDialog
{
// Construction
public:
  INDEX m_iSelectedAction;
  CControls &m_ctrlControls;
  
  CDlgPlayerControls( CControls &ctrlControls, CWnd* pParent = NULL);   // standard constructor
  void ActivatePressKey( char *pFirstOrSecond);
  void FillActionsList(void);
  void FillAxisList(void);
  void SetFirstAndSecondButtonNames(void);
  CButtonAction *GetSelectedButtonAction();

// Dialog Data
	//{{AFX_DATA(CDlgPlayerControls)
	enum { IDD = IDD_PLAYER_CONTROLS };
	CAxisListCtrl	m_listAxisActions;
	CActionsListControl	m_listButtonActions;
	CPressKeyEditControl	m_editSecondControl;
	CPressKeyEditControl	m_editFirstControl;
	CSliderCtrl	m_sliderControlerSensitivity;
	CComboBox	m_comboControlerAxis;
	BOOL	m_bInvertControler;
	int		m_iRelativeAbsoluteType;
	CString	m_strPressNewButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPlayerControls)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
//protected:
public:

	// Generated message map functions
	//{{AFX_MSG(CDlgPlayerControls)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusEditFirstControl();
	afx_msg void OnSetfocusEditSecondControl();
	afx_msg void OnFirstControlNone();
	afx_msg void OnSecondControlNone();
	afx_msg void OnDefault();
	afx_msg void OnSelchangeControlerAxis();
	afx_msg void OnMoveControlUp();
	afx_msg void OnMoveControlDown();
	afx_msg void OnButtonActionAdd();
	afx_msg void OnButtonActionEdit();
	afx_msg void OnButtonActionRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // DLGPLAYERCONTROLS_H
