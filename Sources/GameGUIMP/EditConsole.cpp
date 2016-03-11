/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// EditConsole.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditConsole

CEditConsole::CEditConsole()
{
}

CEditConsole::~CEditConsole()
{
}


BEGIN_MESSAGE_MAP(CEditConsole, CEdit)
	//{{AFX_MSG_MAP(CEditConsole)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditConsole message handlers

void CEditConsole::SetTextFromConsole(void)
{
  INDEX ctLines = _pConsole->con_ctLines;
  INDEX ctChars = _pConsole->con_ctLines;
  // allocate new string with double line ends
  char *strNew = (char*)AllocMemory(ctLines*(ctChars+2)+1);
  const char *strString = _pConsole->GetBuffer();
  char *pch = strNew;
  // convert '\n' to '\r''\n'
  while(*strString!=0) {
    if (*strString=='\n') {
      *pch++='\r';
      *pch++='\n';
      strString++;
    }
    *pch++ = *strString++;
  }
  *pch = 0;

  CDlgConsole *pdlgParent = (CDlgConsole *) GetParent();
  CEdit *pwndOutput = (CEdit *) pdlgParent->GetDlgItem( IDC_CONSOLE_OUTPUT);
  pwndOutput->SetWindowText( CString(strNew));
  pwndOutput->LineScroll(ctLines-1);

  FreeMemory(strNew);
}

BOOL CEditConsole::PreTranslateMessage(MSG* pMsg) 
{
  BOOL bCtrl = (GetKeyState( VK_CONTROL) & 128) != 0;
  if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
  {
    // obtain current line index
    INDEX iCurrentLine = LineFromChar(-1);
    INDEX ctLinesEdited = GetLineCount();
    // obtain char offset of current line in whole edit string
    INDEX iCharOffset = LineFromChar(iCurrentLine);
    if( !bCtrl && (iCharOffset != -1) )
    {
      // extract string to execute
      wchar_t achrToExecute[ 1024];
      INDEX ctLetters = GetLine( iCurrentLine, achrToExecute, 1023);
      // set EOF delimiter
      achrToExecute[ ctLetters] = 0;
      CTString strToExecute = CStringA(achrToExecute);
      CPrintF( ">%s\n", strToExecute);
      if( ((const char*)strToExecute)[strlen(strToExecute)-1] != ';')
      {
        strToExecute += ";";
      }
      _pShell->Execute(strToExecute);
      // set new text for output window
      SetTextFromConsole();
      // remember input text into console input buffer
      CString sHistory;
      GetWindowText(sHistory);
      _pGame->gam_strConsoleInputBuffer = CStringA(sHistory);
    }
    // if Ctrl is not pressed and current line is not last line, "swallow return"
    if( !bCtrl && (ctLinesEdited-1 != iCurrentLine) )
    {
      return TRUE;
    }
  }

  return CEdit::PreTranslateMessage(pMsg);
}
