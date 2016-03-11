/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPleaseWait.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPleaseWait dialog


CDlgPleaseWait::CDlgPleaseWait( const CTString &strMessage, const CTString &strArgument,
                                CWnd* pParent /*= NULL*/)
	            : CDialog(CDlgPleaseWait::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPleaseWait)
	//m_Message = _T("");
	//m_MessageArgument = _T("");
	//}}AFX_DATA_INIT
  m_Message = strMessage;
  m_MessageArgument = strArgument;
}


void CDlgPleaseWait::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPleaseWait)
	DDX_Text(pDX, IDC_MESSAGE_ARGUMENT_T, m_MessageArgument);
	DDX_Text(pDX, IDC_MESSAGE_T, m_Message);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPleaseWait, CDialog)
	//{{AFX_MSG_MAP(CDlgPleaseWait)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPleaseWait message handlers
