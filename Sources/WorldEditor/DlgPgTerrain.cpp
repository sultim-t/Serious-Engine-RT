/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPgTerrain.cpp : implementation file
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "DlgPgTerrain.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPgTerrain property page

IMPLEMENT_DYNCREATE(CDlgPgTerrain, CPropertyPage)

CDlgPgTerrain::CDlgPgTerrain() : CPropertyPage(CDlgPgTerrain::IDD)
{
	//{{AFX_DATA_INIT(CDlgPgTerrain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgPgTerrain::~CDlgPgTerrain()
{
}

void CDlgPgTerrain::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPgTerrain)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPgTerrain, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgPgTerrain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPgTerrain message handlers

BOOL CDlgPgTerrain::OnIdle(LONG lCount)
{
  CWorldEditorDoc* pDoc = theApp.GetDocument();
  if( (pDoc == NULL) || !IsWindow(m_hWnd)) return FALSE;
  m_wndTerrainInterface.OnIdle();
  return TRUE;
}


BOOL CDlgPgTerrain::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
  
  CRect rect;
  GetClientRect(rect);

  // create window for active default primitive texture
  m_wndTerrainInterface.Create( NULL, NULL, WS_BORDER|WS_VISIBLE,
    CRect( rect.left, rect.top, rect.right+8, rect.bottom+16),
    this, IDW_TERRAIN_INTERFACE);

  DragAcceptFiles();
	return TRUE;
}
