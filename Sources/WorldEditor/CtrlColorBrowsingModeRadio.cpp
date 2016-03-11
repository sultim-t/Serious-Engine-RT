/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CtrlColorBrowsingModeRadio.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCtrlColorBrowsingModeRadio

CCtrlColorBrowsingModeRadio::CCtrlColorBrowsingModeRadio()
{
}

CCtrlColorBrowsingModeRadio::~CCtrlColorBrowsingModeRadio()
{
}

void CCtrlColorBrowsingModeRadio::SetDialogPtr( CPropertyComboBar *pDialog)
{
  m_pDialog = pDialog;
}


BEGIN_MESSAGE_MAP(CCtrlColorBrowsingModeRadio, CButton)
	//{{AFX_MSG_MAP(CCtrlColorBrowsingModeRadio)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCtrlColorBrowsingModeRadio message handlers

void CCtrlColorBrowsingModeRadio::OnClicked() 
{
  // don't do anything if document doesn't exist
  if( theApp.GetDocument() == NULL) return;
  // select clicked axis radio
  m_pDialog->SelectColorBrowsingModeRadio( this);
  // show that second axis have been selected
	m_pDialog->ArrangeControls();
}
