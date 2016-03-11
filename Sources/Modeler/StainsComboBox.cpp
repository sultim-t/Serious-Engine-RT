/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// StainsComboBox.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStainsComboBox

CStainsComboBox::CStainsComboBox()
{
}

CStainsComboBox::~CStainsComboBox()
{
}


BEGIN_MESSAGE_MAP(CStainsComboBox, CComboBox)
	//{{AFX_MSG_MAP(CStainsComboBox)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStainsComboBox message handlers
void CStainsComboBox::Refresh()
{
  CModelerApp *pApp = ((CModelerApp *)AfxGetApp());
  int i=GetCurSel();

  ResetContent();
  
  if( !pApp->m_WorkingPatches.IsEmpty())
  {
    FOREACHINLIST( CWorkingPatch, wp_ListNode, pApp->m_WorkingPatches, it)
    {
      AddString( CString(it->wp_FileName.FileName()));
    }
    SetCurSel( 0);
  }
  else
  {
    AddString( L"None available");
    SetCurSel( 0);
  }
}

void CStainsComboBox::OnDropdown() 
{
  INDEX ctItems = GetCount();
  if( ctItems == CB_ERR) return;
  
  CRect rectCombo;
  GetWindowRect( &rectCombo);
  
  PIX pixScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
  PIX pixMaxHeight = pixScreenHeight - rectCombo.top;

  CWnd *pwndParent = GetParent();
  if( pwndParent == NULL) return;
  pwndParent->ScreenToClient( &rectCombo);
  PIX pixNewHeight = GetItemHeight(0)*(ctItems+2);
  rectCombo.bottom = rectCombo.top + ClampUp( pixNewHeight, pixMaxHeight);
  MoveWindow( rectCombo);
}
