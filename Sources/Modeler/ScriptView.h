/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ScriptView.h : header file
//
#ifndef SCRIPTVIEW_H
#define SCRIPTVIEW_H 1

/////////////////////////////////////////////////////////////////////////////
// CScriptView view

class CScriptView : public CEditView
{
protected:
	CScriptView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScriptView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScriptView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CScriptView)
	afx_msg void OnScriptMakeModel();
	afx_msg void OnScriptUpdateMipmodels();
	afx_msg void OnScriptUpdateAnimations();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // SCRIPTVIEW_H
