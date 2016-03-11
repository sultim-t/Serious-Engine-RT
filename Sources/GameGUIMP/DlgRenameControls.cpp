/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgRenameControls.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRenameControls dialog


CDlgRenameControls::CDlgRenameControls(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRenameControls::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRenameControls)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgRenameControls::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRenameControls)
	DDX_Text(pDX, IDC_EDIT_CONTROLS_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRenameControls, CDialog)
	//{{AFX_MSG_MAP(CDlgRenameControls)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRenameControls message handlers
