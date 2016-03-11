/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgNewProgress.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewProgress dialog


CDlgNewProgress::CDlgNewProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewProgress::IDD, pParent)
{
  
	//{{AFX_DATA_INIT(CDlgNewProgress)
	m_ProgressMessage = _T("");
	//}}AFX_DATA_INIT
}


void CDlgNewProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

  if( !pDX->m_bSaveAndValidate)
  {
    m_ProgressMessage = m_strNewMessage;
  }

  //{{AFX_DATA_MAP(CDlgNewProgress)
	DDX_Control(pDX, IDC_PROGRESS1, m_NewProgressLine);
	DDX_Text(pDX, IDC_PROGRESS_MESSAGE, m_ProgressMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewProgress, CDialog)
	//{{AFX_MSG_MAP(CDlgNewProgress)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewProgress message handlers

