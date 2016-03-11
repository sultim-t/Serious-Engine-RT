/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// SkyFloat.h : header file
//
#ifndef SKYFLOAT_H
#define SKYFLOAT_H 1

#include <afxwin.h>

void AFXAPI DDX_SkyFloat(CDataExchange* pDX, int nIDC, float &fNumber);
BOOL FloatFromString(CWnd* pWnd, float &fNumber);
BOOL FloatFromString(HWND  pWnd, float &fNumber);
void StringFromFloat(HWND hWnd, float fNumber);
CString StringFromFloat(float fNumber);

#endif // SKYFLOAT_H
