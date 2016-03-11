/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ConsoleSymbolsCombo.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConsoleSymbolsCombo

CConsoleSymbolsCombo::CConsoleSymbolsCombo()
{
}

CConsoleSymbolsCombo::~CConsoleSymbolsCombo()
{
}


BEGIN_MESSAGE_MAP(CConsoleSymbolsCombo, CComboBox)
	//{{AFX_MSG_MAP(CConsoleSymbolsCombo)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConsoleSymbolsCombo message handlers

BOOL CConsoleSymbolsCombo::PreTranslateMessage(MSG* pMsg) 
{
  if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
  {
    INDEX iSelectedSymbol = GetCurSel();
    // if there is a valid player selected
    if( iSelectedSymbol != LB_ERR)
    {
      CString strSelectedSymbolW;
      GetLBText( iSelectedSymbol, strSelectedSymbolW);
      CTString strSelectedSymbol = CStringA(strSelectedSymbolW);

      INDEX ctLetters = strlen(strSelectedSymbol);
      char achrSelectedSymbol[ 256];
      sprintf( achrSelectedSymbol, strSelectedSymbol);
      if( achrSelectedSymbol[ ctLetters-1] == ']')
      {
        for( INDEX iLetter=ctLetters-2; iLetter>0; iLetter--)
        {
          ASSERT( iLetter>0);
          if( ((char *)achrSelectedSymbol)[ iLetter] == '[')
          {
            achrSelectedSymbol[ iLetter+1] = ']';
            achrSelectedSymbol[ iLetter+2] = 0;
            strSelectedSymbol = achrSelectedSymbol;
            break;
          }
        }
      }

      ((CDlgConsole *)GetParent())->m_ctrlEditConsole.ReplaceSel( CString(strSelectedSymbol), TRUE);
      ((CDlgConsole *)GetParent())->m_ctrlEditConsole.SetFocus();
      return TRUE;
    }
  }
	return CComboBox::PreTranslateMessage(pMsg);
}

void CConsoleSymbolsCombo::OnSelchange() 
{
  ShowDropDown( TRUE);
}
