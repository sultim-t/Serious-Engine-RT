/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CSGDesitnationCombo.h : header file
//
#ifndef CSGDESTINATIONCOMBO_H
#define CSGDESTINATIONCOMBO_H 1

/////////////////////////////////////////////////////////////////////////////
// CCSGDesitnationCombo window
class CWorldEditorDoc;

class CCSGDesitnationCombo : public CComboBox
{
// Construction
public:
  CCSGDesitnationCombo();
  BOOL OnIdle(LONG lCount);
  void SelectBrushEntity( CEntity *penBrush);

  CTString m_strLastSelectedName;
  CEntity *GetSelectedBrushEntity(void);
  
  CUpdateableRT m_udComboEntries;
  CWorldEditorDoc *m_pLastDoc;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSGDesitnationCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCSGDesitnationCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCSGDesitnationCombo)
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // CSGDESTINATIONCOMBO_H
