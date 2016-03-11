/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgEditButtonAction.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditButtonAction dialog


CDlgEditButtonAction::CDlgEditButtonAction(CButtonAction *pbaButtonAction, 
                                           CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditButtonAction::IDD, pParent)
{
  // remember button action that is edited
  m_pbaButtonAction = pbaButtonAction;

  //{{AFX_DATA_INIT(CDlgEditButtonAction)
	m_strButtonActionName = _T("");
	m_strButtonDownCommand = _T("");
	m_strButtonUpCommand = _T("");
	//}}AFX_DATA_INIT
}


void CDlgEditButtonAction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  // if dialog is recieving data
  if( pDX->m_bSaveAndValidate == FALSE)
  {
    m_strButtonActionName = m_pbaButtonAction->ba_strName;
    m_strButtonDownCommand = m_pbaButtonAction->ba_strCommandLineWhenPressed;
    m_strButtonUpCommand = m_pbaButtonAction->ba_strCommandLineWhenReleased;
  }

	//{{AFX_DATA_MAP(CDlgEditButtonAction)
	DDX_Text(pDX, IDC_BUTTON_ACTION_NAME, m_strButtonActionName);
	DDX_Text(pDX, IDC_BUTTON_DOWN_COMMAND, m_strButtonDownCommand);
	DDX_Text(pDX, IDC_BUTTON_UP_COMMAND, m_strButtonUpCommand);
	//}}AFX_DATA_MAP

  // if dialog is giving data
  if( pDX->m_bSaveAndValidate != FALSE)
  {
    m_pbaButtonAction->ba_strName = m_strButtonActionName;
    m_pbaButtonAction->ba_strCommandLineWhenPressed = m_strButtonDownCommand;
    m_pbaButtonAction->ba_strCommandLineWhenReleased = m_strButtonUpCommand;
  }
}


BEGIN_MESSAGE_MAP(CDlgEditButtonAction, CDialog)
	//{{AFX_MSG_MAP(CDlgEditButtonAction)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditButtonAction message handlers
