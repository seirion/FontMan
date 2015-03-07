// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__67D5001D_E61F_47F3_8984_DC0723FED870__INCLUDED_)
#define AFX_STDAFX_H__67D5001D_E61F_47F3_8984_DC0723FED870__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// 그림 크기 
#define HEIGHT	640 
#define WIDTH	640

// 결과 크기 
#define A_HEIGHT	100
#define A_WIDTH		100

#define THRESHOLD 35   // 꺾임 - 값이 작을 수록 잘 꺾인다. 
#define STROKE_POINT_THRESHOLD	30	//사용자 획 주변의 컨트롤 포인트 모으기

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__67D5001D_E61F_47F3_8984_DC0723FED870__INCLUDED_)
