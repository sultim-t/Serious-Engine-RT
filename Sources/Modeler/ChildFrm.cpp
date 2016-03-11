/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ChildFrm.cpp : implementation of the CChildFrame class  
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_COMMAND(ID_WINDOW_CLOSE, OnWindowClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
  m_bAllreadyUnmaximized = FALSE;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  BOOL bMaximized = FALSE;
  CMDIFrameWnd *pFrame = (CMDIFrameWnd *)AfxGetApp()->m_pMainWnd;
  if( ((pFrame->MDIGetActive( &bMaximized) == NULL) || (bMaximized)) && 
      (theApp.m_Preferences.ap_AutoMaximizeWindow) )
  {
    cs.style |= WS_VISIBLE | WS_MAXIMIZE;
  }

	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::OnWindowClose() 
{
	OnClose();	
}

