/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// PaletteButton.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteButton

CPaletteButton::CPaletteButton()
{
}

CPaletteButton::~CPaletteButton()
{
}


BEGIN_MESSAGE_MAP(CPaletteButton, CButton)
	//{{AFX_MSG_MAP(CPaletteButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteButton message handlers

void CPaletteButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  CDC *pDC = GetDC();

  CModelerView *pModelerView = CModelerView::GetActiveView();
  if( pModelerView == NULL)
  {
    pDC->FillSolidRect( &lpDrawItemStruct->rcItem, 0x00aaaaaa);
    pDC->DrawEdge( &lpDrawItemStruct->rcItem, EDGE_RAISED, BF_RECT);
  }
  else
  {
    INDEX iButtonNo = lpDrawItemStruct->CtlID - IDC_COLOR_PALETTE_BUTTON_BASE;
  
    COLORREF clrfColor = CLRF_CLR( PaletteColorValues[ iButtonNo]);

    pDC->FillSolidRect( &lpDrawItemStruct->rcItem, clrfColor);
    ULONG ulMask = pModelerView->m_ModelObject.mo_ColorMask;
    if( ulMask & ((1L) << iButtonNo))
      pDC->DrawEdge( &lpDrawItemStruct->rcItem, EDGE_SUNKEN, BF_RECT);
    else
      pDC->DrawEdge( &lpDrawItemStruct->rcItem, EDGE_RAISED, BF_RECT);
  }
  ReleaseDC( pDC);
}

void CPaletteButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	INDEX iClickedColor = GetDlgCtrlID()-IDC_COLOR_PALETTE_BUTTON_BASE;
  CModelerView *pModelerView = CModelerView::GetActiveView();
  if( pModelerView != NULL)
  {
    // Doubble clicked, change mode
    if( pModelerView->m_iChoosedColor == iClickedColor)
    {
      pModelerView->m_bOnColorMode = !pModelerView->m_bOnColorMode;
    }
    else
    {
      pModelerView->m_iChoosedColor = iClickedColor;
    }
    CMainFrame* pMainFrame = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    pMainFrame->m_dlgPaletteDialog->m_ChoosedColorButton.Invalidate(FALSE);
    pMainFrame->m_dlgPaletteDialog->UpdateData(FALSE);
  }

  CButton::OnLButtonDown(nFlags, point);
}

void CPaletteButton::OnRButtonDown(UINT nFlags, CPoint point) 
{
	INDEX iClickedColor = GetDlgCtrlID()-IDC_COLOR_PALETTE_BUTTON_BASE;
	
  CModelerView *pModelerView = CModelerView::GetActiveView();
  if( (pModelerView != NULL) && (iClickedColor < 30))
  {
    ULONG ulMask = pModelerView->m_ModelObject.mo_ColorMask;
    ulMask = ulMask ^ ((1L) << iClickedColor);
    pModelerView->m_ModelObject.mo_ColorMask = ulMask;
    Invalidate( FALSE);
    pModelerView->Invalidate( FALSE);
  }
  
	CButton::OnRButtonDown(nFlags, point);
}
