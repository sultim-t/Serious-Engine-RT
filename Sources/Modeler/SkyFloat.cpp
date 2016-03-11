/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// SkyFloat.cpp : implementation of DDX_SkyFloat

/////////////////////////////////////////////////////////////////////////////
// Public functions

#include "StdAfx.h"

//--------------------------------------------------------------------------------------------
void AFXAPI DDX_SkyFloat(CDataExchange* pDX, int nIDC, float &fNumber)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
		if (!FloatFromString(hWndCtrl, fNumber))
		{
			//AfxMessageBox("Invalid character entered");
			pDX->Fail();
		}
	}
	else
	{
		StringFromFloat(hWndCtrl, fNumber);
	}
}

BOOL FloatFromString(CWnd* pWnd, float& fNumber)
{
	ASSERT(pWnd != NULL);
	return FloatFromString(pWnd->m_hWnd, fNumber);
}

BOOL FloatFromString(HWND hWnd, float &fNumber)
{
	char szWindowText[20];
	::GetWindowTextA(hWnd, szWindowText, 19);
  
  float fTmpNumber = fNumber;
  int iNumLen, iRetLen;
  iNumLen = strlen( szWindowText);
  iRetLen = sscanf( szWindowText, "%f", &fTmpNumber);
  if( (iRetLen == 1)  || ((iNumLen == 1) && (szWindowText[0] == '-') || (iNumLen == 0)) )
  {
    fNumber = fTmpNumber;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void StringFromFloat(CWnd* pWnd, float fNumber)
{
	ASSERT(pWnd != NULL);
	StringFromFloat(pWnd->m_hWnd, fNumber);
}

void StringFromFloat(HWND hWnd, float fNumber)
{
	CString str;
	str.Format(_T("%.3f"), fNumber);
	::SetWindowText(hWnd, str.GetBufferSetLength(20));
}
//--------------------------------------------------------------------------------------------
