// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "FontMan.h"
#include "MyEdit.h"
#include "FontManView.h"
#include "FontManDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyEdit

CMyEdit::CMyEdit()
{
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	//{{AFX_MSG_MAP(CMyEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers

BOOL CMyEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
  
		CFontManView *pView;
		pView = (CFontManView *)GetParent();
		CFontManDoc *pDoc = (CFontManDoc *)pView->GetDocument();

		CString str;
		unsigned short *wcsCode = new unsigned short[32];

		GetWindowText(str);
		str.TrimLeft();
		str.TrimRight();
		if( str.GetLength() <= 0 ) return CEdit::PreTranslateMessage(pMsg);

		int size = str.GetLength();
		if( size > 2) size = 2;

		MultiByteToWideChar(
			CP_ACP,         // code page
			MB_PRECOMPOSED,         // character-type options
			str.GetBuffer(32),		// address of string to map
			size,      // number of bytes in string
			wcsCode,  // address of wide-character buffer
			32        // size of buffer
		);
		
		pView->LoadFont((int)wcsCode[0]);
		pDoc->LoadFont((int)wcsCode[0]);
		delete [] wcsCode;
		
		//Sang Ok Koo
		int i;
		for ( i = 0; i <= pDoc->m_Vector->GetStrokeNum(); i++)
			pView->m_StrokeOrder.DeleteString(0);	// »èÁ¦
		pView->m_StrokeOrder.Clear();
       
    }       
	return CEdit::PreTranslateMessage(pMsg);
}
