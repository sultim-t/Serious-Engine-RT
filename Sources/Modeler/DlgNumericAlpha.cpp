/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgNumericAlpha.cpp : implementation file
//

#include "stdafx.h"
#include "DlgNumericAlpha.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNumericAlpha dialog


CDlgNumericAlpha::CDlgNumericAlpha(int iAlpha, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNumericAlpha::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNumericAlpha)
	m_iAlpha = 0;
	//}}AFX_DATA_INIT
  m_iAlpha = iAlpha;
}


void CDlgNumericAlpha::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNumericAlpha)
	DDX_Text(pDX, IDC_ALPHA, m_iAlpha);
	DDV_MinMaxInt(pDX, m_iAlpha, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNumericAlpha, CDialog)
	//{{AFX_MSG_MAP(CDlgNumericAlpha)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNumericAlpha message handlers
