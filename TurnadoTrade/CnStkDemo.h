// CnStkDemo.h : main header file for the CNSTKDEMO application
//

#if !defined(AFX_CNSTKDEMO_H__2A334C15_C989_4014_BB2F_7EBC83D1DBD1__INCLUDED_)
#define AFX_CNSTKDEMO_H__2A334C15_C989_4014_BB2F_7EBC83D1DBD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCnStkDemoApp:
// See CnStkDemo.cpp for the implementation of this class
//

class CCnStkDemoApp : public CWinApp
{
public:
	CCnStkDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCnStkDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCnStkDemoApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNSTKDEMO_H__2A334C15_C989_4014_BB2F_7EBC83D1DBD1__INCLUDED_)
