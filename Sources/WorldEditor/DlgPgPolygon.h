/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPgPolygon.h : header file
//
#ifndef DLGPGPOLYGON_H
#define DLGPGPOLYGON_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPgPolygon dialog

class CDlgPgPolygon : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgPgPolygon)

// Construction
public:
	CDlgPgPolygon();
	~CDlgPgPolygon();
  BOOL OnIdle(LONG lCount);
  void InitComboBoxes(void);
  FLOAT m_fPretenderDistance;
  BOOL m_bPretenderDistance;

  CUpdateableRT m_udPolygonSelection;

// Dialog Data
	//{{AFX_DATA(CDlgPgPolygon)
	enum { IDD = IDD_PG_POLYGON };
	CCtrlEditBoolean	m_IsDoubleSided;
	CCtrlEditBoolean	m_bShootThru;
	CCtrlEditBoolean	m_IsTransparent;
	CCtrlEditBoolean	m_bStairs;
	CCtrlEditBoolean	m_IsOccluder;
	CComboBox	m_ComboMirror;
	CCtrlEditBoolean	m_IsOldPortal;
	CCtrlEditBoolean	m_bIsDetail;
	CCtrlEditBoolean	m_IsInvisible;
	CCtrlEditBoolean	m_IsTranslucent;
	CCtrlEditBoolean	m_IsPassable;
	CCtrlEditBoolean	m_IsPortal;
	CComboBox	m_ComboFriction;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgPolygon)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgPgPolygon)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFrictionCombo();
	afx_msg void OnDropdownFrictionCombo();
	afx_msg void OnSelchangeMirrorCombo();
	afx_msg void OnDropdownMirrorCombo();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // DLGPGPOLYGON_H
