/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CtrlEnumCombo.cpp : implementation file
//

#include "stdafx.h"
#include "CtrlEnumCombo.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCtrlEnumCombo

CCtrlEnumCombo::CCtrlEnumCombo()
{
}

CCtrlEnumCombo::~CCtrlEnumCombo()
{
}

void CCtrlEnumCombo::SetDialogPtr( CPropertyComboBar *pDialog)
{
  m_pDialog = pDialog;
}


BEGIN_MESSAGE_MAP(CCtrlEnumCombo, CComboBox)
	//{{AFX_MSG_MAP(CCtrlEnumCombo)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCtrlEnumCombo message handlers

void CCtrlEnumCombo::OnSelchange() 
{
  theApp.GetDocument()->SetModifiedFlag( TRUE);
  // update dialog data (to reflect data change)
	m_pDialog->UpdateData( TRUE);
	m_pDialog->SetIntersectingEntityClassName();
  m_pDialog->ArrangeControls();
	m_pDialog->UpdateData( FALSE);
}

void CCtrlEnumCombo::OnDropdown() 
{
  INDEX ctItems = GetCount();
  if( ctItems == CB_ERR) return;
  
  CRect rectCombo;
  GetWindowRect( &rectCombo);
  
  PIX pixScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
  PIX pixMaxHeight = pixScreenHeight - rectCombo.top;

  m_pDialog->ScreenToClient( &rectCombo);
  PIX pixNewHeight = GetItemHeight(0)*(ctItems+2);
  rectCombo.bottom = rectCombo.top + ClampUp( pixNewHeight, pixMaxHeight);
  MoveWindow( rectCombo);
}
