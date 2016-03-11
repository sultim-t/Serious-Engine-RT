/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// CtrlAxisRadio.cpp : implementation file
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "CtrlAxisRadio.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCtrlAxisRadio

CCtrlAxisRadio::CCtrlAxisRadio()
{
}

CCtrlAxisRadio::~CCtrlAxisRadio()
{
}

void CCtrlAxisRadio::SetDialogPtr( CPropertyComboBar *pDialog)
{
  m_pDialog = pDialog;
}

BEGIN_MESSAGE_MAP(CCtrlAxisRadio, CButton)
	//{{AFX_MSG_MAP(CCtrlAxisRadio)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCtrlAxisRadio message handlers

void CCtrlAxisRadio::OnClicked() 
{
  // don't do anything if document doesn't exist
  if( theApp.GetDocument() == NULL) return;
  // select clicked axis radio
  m_pDialog->SelectAxisRadio( this);
  // show that second axis have been selected
	m_pDialog->ArrangeControls();
}
