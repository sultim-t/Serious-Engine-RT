/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgInfoPgNone.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoPgNone property page


CDlgInfoPgNone::CDlgInfoPgNone() : CPropertyPage(CDlgInfoPgNone::IDD)
{
	//{{AFX_DATA_INIT(CDlgInfoPgNone)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  theApp.m_pPgInfoNone = this;
}


void CDlgInfoPgNone::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInfoPgNone)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInfoPgNone, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgInfoPgNone)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoPgNone message handlers

BOOL CDlgInfoPgNone::OnIdle(LONG lCount)
{
  // refresh info frame size
  ((CMainFrame *)( theApp.m_pMainWnd))->m_pInfoFrame->SetSizes();
  return TRUE;
}

