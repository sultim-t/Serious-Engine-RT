/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DChooseAnim.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDChooseAnim dialog


CDChooseAnim::CDChooseAnim(CAnimObject *pAO, CWnd* pParent /*=NULL*/)
	: CDialog(CDChooseAnim::IDD, pParent)
{
  m_pAnimObject = pAO;
}


void CDChooseAnim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

  //{{AFX_DATA_MAP(CDChooseAnim)
  DDX_Control(pDX, IDC_LIST1, m_ListBox);
  //}}AFX_DATA_MAP
    
  // if initializing data
  if (!pDX->m_bSaveAndValidate)
  {
    CAnimInfo aiInfo;
    for( INDEX i=0; i<m_pAnimObject->GetAnimsCt(); i++)
    {
      m_pAnimObject->GetAnimInfo( i, aiInfo);
      m_ListBox.AddString(CString(aiInfo.ai_AnimName));
    }
  }
  else
  {
    int iSelection=m_ListBox.GetCurSel();
    // if valid selection      
    if (iSelection!=LB_ERR)
    {
      m_pAnimObject->SetAnim( iSelection);
    }
  }
}


BEGIN_MESSAGE_MAP(CDChooseAnim, CDialog)
	//{{AFX_MSG_MAP(CDChooseAnim)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDChooseAnim message handlers

void CDChooseAnim::OnDblclkList1() 
{
	CDialog::OnOK();
}
