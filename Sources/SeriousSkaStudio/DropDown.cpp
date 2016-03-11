/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DropDown.cpp : implementation file
//

#include "stdafx.h"
#include "seriousskastudio.h"
#include "DropDown.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDropDown

CDropDown::CDropDown()
{
  m_pInt = NULL;
  m_bSetID = FALSE;
}

CDropDown::~CDropDown()
{
}

void CDropDown::SetDataPtr(INDEX *pInt)
{
  m_pInt = pInt;
}

void CDropDown::RememberIDs()
{
  m_bSetID = TRUE;
}

BEGIN_MESSAGE_MAP(CDropDown, CComboBox)
	//{{AFX_MSG_MAP(CDropDown)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDropDown message handlers

void CDropDown::OnSelendok() 
{
  theApp.NotificationMessage(m_strID);
  wchar_t str[MAX_PATH];
  GetWindowText(str,MAX_PATH);
  CSeriousSkaStudioDoc *pDoc = theApp.GetDocument();
  // set ID of selected item
  if(m_bSetID)
  {
    theApp.m_dlgBarTreeView.ChangeTextureOnSelectedSurfaces(m_strID,CTString(CStringA(str)));
    pDoc->MarkAsChanged();
    return;
  }
  else
  {
    INDEX iIndex = GetCurSel();
    pDoc->MarkAsChanged();
    theApp.m_dlgBarTreeView.ChangeTextureCoordsOnSelectedSurfaces(m_strID,iIndex);
  }
}
