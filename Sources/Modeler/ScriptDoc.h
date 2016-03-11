/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ScriptDoc.h : header file
//
#ifndef SCRIPTDOC_H
#define SCRIPTDOC_H 1

/////////////////////////////////////////////////////////////////////////////
// CScriptDoc document

class CScriptDoc : public CDocument
{
protected:
	CScriptDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScriptDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScriptDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CScriptDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // SCRIPTDOC_H
