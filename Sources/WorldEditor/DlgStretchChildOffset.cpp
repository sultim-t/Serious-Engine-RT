/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgStretchChildOffset.cpp : implementation file
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "DlgStretchChildOffset.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgStretchChildOffset dialog


CDlgStretchChildOffset::CDlgStretchChildOffset(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStretchChildOffset::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgStretchChildOffset)
	m_fStretchValue = 0.0f;
	//}}AFX_DATA_INIT
	m_fStretchValue = 1.0f;
}


void CDlgStretchChildOffset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStretchChildOffset)
	DDX_Text(pDX, IDC_CHILD_STRETCH, m_fStretchValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStretchChildOffset, CDialog)
	//{{AFX_MSG_MAP(CDlgStretchChildOffset)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStretchChildOffset message handlers
