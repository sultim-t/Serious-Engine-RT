/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// ModelTreeCtrl.h: interface for the CModelTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELTREECTRL_H__C5D259C2_AD00_11D5_8AB4_00C0262D9BFE__INCLUDED_)
#define AFX_MODELTREECTRL_H__C5D259C2_AD00_11D5_8AB4_00C0262D9BFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct NodeInfo
{
  INDEX ni_iType;
  BOOL  ni_bSelected;
  void *ni_pPtr;
  CModelInstance *pmi;
};


class CModelTreeCtrl : public CTreeCtrl
{
public:
	CModelTreeCtrl();
// Operations
public:
// Overrides
  HTREEITEM hLastSelected;

// Implementation
public:
	virtual ~CModelTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CModelTreeCtrl)
  afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_MODELTREECTRL_H__C5D259C2_AD00_11D5_8AB4_00C0262D9BFE__INCLUDED_)
