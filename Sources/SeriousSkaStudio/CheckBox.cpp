/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "seriousskastudio.h"
#include "CheckBox.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckBox

CCheckBox::CCheckBox()
{
}

CCheckBox::~CCheckBox()
{
}
void CCheckBox::SetIndex(INDEX iFlagIndex, ULONG ulFlags)
{
  if(ulFlags&((1UL<<iFlagIndex))) {
    SetCheck(1);
  } else {
    SetCheck(0);
  }
  m_iIndex = iFlagIndex;
}


BEGIN_MESSAGE_MAP(CCheckBox, CButton)
	//{{AFX_MSG_MAP(CCheckBox)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckBox message handlers

void CCheckBox::OnClicked() 
{
  BOOL bCheched = FALSE;
  
  switch(GetCheck()) {
    case 0:
      bCheched = FALSE;
    break;
    case 1:
      bCheched = TRUE;
    break;
    default:
      ASSERTALWAYS("Unknown state");
      return;
    break;
  }

  // control is in shader dialog
  theApp.m_dlgBarTreeView.ChangeFlagOnSelectedSurfaces(m_strID,bCheched,m_iIndex);
}
