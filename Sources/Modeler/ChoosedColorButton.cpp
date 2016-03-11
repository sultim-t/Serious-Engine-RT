/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ChoosedColorButton.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChoosedColorButton

CChoosedColorButton::CChoosedColorButton()
{
}

CChoosedColorButton::~CChoosedColorButton()
{
}


BEGIN_MESSAGE_MAP(CChoosedColorButton, CButton)
	//{{AFX_MSG_MAP(CChoosedColorButton)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChoosedColorButton message handlers

void CChoosedColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  CDC *pDC = GetDC();
  
  CModelerView *pModelerView = CModelerView::GetActiveView();
  if( pModelerView == NULL)
  {
    CBrush *MyBrush = &CBrush();
    ASSERT( MyBrush->CreateHatchBrush(HS_DIAGCROSS, 0x00777777) != FALSE);

    pDC->FillSolidRect( &lpDrawItemStruct->rcItem, 0x00bbbbbb);
    pDC->FillRect( &lpDrawItemStruct->rcItem, MyBrush);
    pDC->DrawText( CString("none"), &lpDrawItemStruct->rcItem, 
                   DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  }
  else
  {
    COLORREF clrfColor = CLRF_CLR( PaletteColorValues[ pModelerView->m_iChoosedColor]);
    pDC->FillSolidRect( &lpDrawItemStruct->rcItem, clrfColor);
  }
  pDC->DrawEdge( &lpDrawItemStruct->rcItem, EDGE_RAISED, BF_RECT);
  ReleaseDC( pDC);
}

void CChoosedColorButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
  CModelerView *pModelerView = CModelerView::GetActiveView();
  if( pModelerView != NULL)
  {
    pModelerView->m_bOnColorMode = !pModelerView->m_bOnColorMode;
    CMainFrame* pMainFrame = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    pMainFrame->m_dlgPaletteDialog->UpdateData(FALSE);
  }
	
	CButton::OnLButtonDown(nFlags, point);
}
