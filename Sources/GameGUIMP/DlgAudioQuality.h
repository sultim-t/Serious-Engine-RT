/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// DlgAudioQuality.h : header file
//
#ifndef DLGAUDIOQUALITY_H
#define DLGAUDIOQUALITY_H 1

/////////////////////////////////////////////////////////////////////////////
// CDlgAudioQuality dialog

class CDlgAudioQuality : public CDialog
{
// Construction
public:
	CDlgAudioQuality(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAudioQuality)
	enum { IDD = IDD_AUDIO_QUALITY };
	int		m_iAudioQualityRadio;
	BOOL	m_bUseDirectSound3D;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAudioQuality)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAudioQuality)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // DLGAUDIOQUALITY_H