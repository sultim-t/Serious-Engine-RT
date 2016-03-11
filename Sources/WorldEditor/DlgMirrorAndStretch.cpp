/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgMirrorAndStretch.cpp : implementation file
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "DlgMirrorAndStretch.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMirrorAndStretch dialog


CDlgMirrorAndStretch::CDlgMirrorAndStretch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMirrorAndStretch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMirrorAndStretch)
	m_fStretch = 0.0f;
	m_iMirror = -1;
	//}}AFX_DATA_INIT
	
  m_fStretch = 1.0f;
	m_iMirror = 0;
}


void CDlgMirrorAndStretch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMirrorAndStretch)
	DDX_Text(pDX, IDC_STRETCH, m_fStretch);
	DDX_Radio(pDX, IDC_MIRROR_NONE, m_iMirror);
	//}}AFX_DATA_MAP

  ASSERT(IsWindow(m_hWnd));
  SetWindowText(CString(m_strName));
}


BEGIN_MESSAGE_MAP(CDlgMirrorAndStretch, CDialog)
	//{{AFX_MSG_MAP(CDlgMirrorAndStretch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMirrorAndStretch message handlers
