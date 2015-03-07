// FontMan.h : main header file for the FONTMAN application
//

#if !defined(AFX_FONTMAN_H__5BC48B4B_82FD_41FD_B8E7_DE93512F79F3__INCLUDED_)
#define AFX_FONTMAN_H__5BC48B4B_82FD_41FD_B8E7_DE93512F79F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CFontManApp:
// See FontMan.cpp for the implementation of this class
//

class CFontManApp : public CWinApp
{
public:
	CFontManApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontManApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CFontManApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTMAN_H__5BC48B4B_82FD_41FD_B8E7_DE93512F79F3__INCLUDED_)
