/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgComment.cpp : implementation file
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "DlgComment.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgComment dialog


CDlgComment::CDlgComment(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgComment::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgComment)
	m_strComment = _T("");
	//}}AFX_DATA_INIT
}


void CDlgComment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgComment)
	DDX_Text(pDX, IDC_COMMENT, m_strComment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgComment, CDialog)
	//{{AFX_MSG_MAP(CDlgComment)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgComment message handlers
