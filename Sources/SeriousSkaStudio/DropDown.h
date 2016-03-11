/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DROPDOWN_H__8736EA9E_5675_44BB_AD38_E72FB7FF7C76__INCLUDED_)
#define AFX_DROPDOWN_H__8736EA9E_5675_44BB_AD38_E72FB7FF7C76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DropDown.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDropDown window

class CDropDown : public CComboBox
{
// Construction
public:
	CDropDown();
	virtual ~CDropDown();

  void SetDataPtr(INDEX *pID);
  CTString m_strID; // ID of control (base texture)
  void RememberIDs();
  INDEX *m_pInt;  // pointing to index to change
  BOOL m_bSetID;  // is pointer pointion to ID or index in list


  
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDropDown)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CDropDown)
	afx_msg void OnSelendok();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DROPDOWN_H__8736EA9E_5675_44BB_AD38_E72FB7FF7C76__INCLUDED_)
