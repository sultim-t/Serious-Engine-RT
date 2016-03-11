/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgPgRenderingStatistics.h : header file
//
#ifndef DLGPGRENDERINGSTATISTICS_H
#define DLGPGRENDERINGSTATISTICS_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgPgRenderingStatistics dialog

class CDlgPgRenderingStatistics : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgPgRenderingStatistics)

// Construction
public:
  CUpdateableRT m_udStatsUpdated;
	CDlgPgRenderingStatistics();
	~CDlgPgRenderingStatistics();
  BOOL OnIdle(LONG lCount);

// Dialog Data
	//{{AFX_DATA(CDlgPgRenderingStatistics)
	enum { IDD = IDD_PG_RENDERING_STATISTICS };
	CString	m_strRenderingStatistics;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgRenderingStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgPgRenderingStatistics)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // DLGPGRENDERINGSTATISTICS_H
