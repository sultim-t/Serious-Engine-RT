/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgAutTexturize.cpp : implementation file
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "DlgAutTexturize.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAutTexturize dialog


CDlgAutTexturize::CDlgAutTexturize(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAutTexturize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAutTexturize)
	m_bExpandEdges = FALSE;
	//}}AFX_DATA_INIT

  m_pixWidth=1;
  m_pixHeight=1;
}


void CDlgAutTexturize::DoDataExchange(CDataExchange* pDX)
{
  // if dialog is receiving data
  if( pDX->m_bSaveAndValidate == FALSE)
  {
    CWorldEditorDoc* pDoc = theApp.GetActiveDocument();
  	COLOR colOld=pDoc->m_woWorld.wo_colBackground;
    colOld=AfxGetApp()->GetProfileInt( L"World editor", L"Pretender bcg color", colOld);
    m_bExpandEdges=TRUE;
    m_bExpandEdges=AfxGetApp()->GetProfileInt( L"World editor", L"Auto expand edges", m_bExpandEdges);
    m_colBcg.SetColor(colOld);
    m_colBcg.SetPickerType( CColoredButton::PT_MFC);
  }

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAutTexturize)
	DDX_Control(pDX, IDC_PRETENDER_TEXTURE_STYLE, m_ctrlPretenderTextureStyle);
	DDX_Control(pDX, ID_SECTOR_COLOR, m_colBcg);
	DDX_Control(pDX, IDC_PRETENDER_TEXTURE_SIZE, m_ctrPretenderTextureSize);
	DDX_Check(pDX, IDC_EXPAND_EDGES, m_bExpandEdges);
	//}}AFX_DATA_MAP

  // if dialog gives data
  if( pDX->m_bSaveAndValidate != FALSE)
  {
    INDEX iSelected=m_ctrPretenderTextureSize.GetCurSel();
	  if( iSelected==CB_ERR) return;
    m_pixWidth=1<<iSelected;
    m_pixHeight=1<<iSelected;
    COLOR colResult=m_colBcg.GetColor();
    AfxGetApp()->WriteProfileInt( L"World editor", L"Pretender bcg color", m_colBcg.GetColor());
    AfxGetApp()->WriteProfileInt( L"World editor", L"Pretender resolution", iSelected);
    AfxGetApp()->WriteProfileInt( L"World editor", L"Auto expand edges", m_bExpandEdges);
    
    m_iPretenderStyle=m_ctrlPretenderTextureStyle.GetCurSel();
    AfxGetApp()->WriteProfileInt( L"World editor", L"Pretender style", m_iPretenderStyle);
  }
}


BEGIN_MESSAGE_MAP(CDlgAutTexturize, CDialog)
	//{{AFX_MSG_MAP(CDlgAutTexturize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAutTexturize message handlers

BOOL CDlgAutTexturize::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctrPretenderTextureSize.ResetContent();
  // add all available frictions
  for(INDEX iW=0; iW<10; iW++)
  {
    CTString strSize;
    strSize.PrintF("%dx%d pixels", 1<<iW, 1<<iW);
    m_ctrPretenderTextureSize.AddString( CString(strSize));
  }
  INDEX iSelected=6;
  iSelected=AfxGetApp()->GetProfileInt( L"World editor", L"Pretender resolution", iSelected);
  m_ctrPretenderTextureSize.SetCurSel(iSelected);

	m_ctrlPretenderTextureStyle.ResetContent();
  m_ctrlPretenderTextureStyle.AddString( L"Front view only");
  m_ctrlPretenderTextureStyle.AddString( L"Cylindrical view (FRBL)");
  m_ctrlPretenderTextureStyle.AddString( L"Boxed view (FRBLUD)");
  iSelected=1;
  iSelected=AfxGetApp()->GetProfileInt( L"World editor", L"Pretender style", iSelected);
  m_ctrlPretenderTextureStyle.SetCurSel(iSelected);
	return TRUE;
}
