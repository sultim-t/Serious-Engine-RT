/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// TextureComboBox.h : header file
//
#ifndef TEXTURECOMBO_H
#define TEXTURECOMBO_H 1

/////////////////////////////////////////////////////////////////////////////
// CTextureComboBox window

class CTextureComboBox : public CComboBox
{
// Construction
public:
	BOOL OnIdle(LONG lCount);
  CTextureData *m_ptdSelectedTexture;
	CTextureComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTextureComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTextureComboBox)
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // TEXTURECOMBO_H
