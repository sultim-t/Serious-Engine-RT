/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ScriptDoc.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptDoc

IMPLEMENT_DYNCREATE(CScriptDoc, CDocument)

CScriptDoc::CScriptDoc()
{
}

BOOL CScriptDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CScriptDoc::~CScriptDoc()
{
}


BEGIN_MESSAGE_MAP(CScriptDoc, CDocument)
	//{{AFX_MSG_MAP(CScriptDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptDoc diagnostics

#ifdef _DEBUG
void CScriptDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CScriptDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScriptDoc serialization

void CScriptDoc::Serialize(CArchive& ar)
{
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CScriptDoc commands
