/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#if !defined(AFX_DLG_TEMPLATE_INCLUDED_)
#define AFX_DLG_TEMPLATE_INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif

#include "SplitterFrame.h"

class CDlgTemplate : public CDialogBar
{
public:
  CDlgTemplate();
  virtual ~CDlgTemplate();
  virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
  virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
  virtual INDEX GetDockingSide();
  void EnableDockingSides(ULONG ulDockingSides);
  void DockCtrlBar();

  void SetSplitterControlID(INDEX iSplitterID);
  void AdjustSplitter();

  CSize m_Size;

  ULONG dlg_ulEnabledDockingSides;
protected:
  CSplitterFrame dlg_spSlitter;
  INDEX dlg_iSplitterID;
  BOOL dlg_bDockingEnabled;
	//{{AFX_MSG(CDlgTemplate)
  	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
