 // FontManView.h : interface of the CFontManView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FONTMANVIEW_H__C72532FA_D19D_4748_B986_3B0016AB442E__INCLUDED_)
#define AFX_FONTMANVIEW_H__C72532FA_D19D_4748_B986_3B0016AB442E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FontManDoc.h"


/////////////////////////////////////////////////////////////////////
// type definition
#define TYPE_VERTICAL 0
#define TYPE_HORIZONTAL 1
#define TYPE_PLUS 2 
#define TYPE_MINUS 3


/////////////////////////////////////////////////////////////////////
// type definition
#define FIRST 0
#define SECOND 1
#define ELSE 2

/////////////////////////////////////////////////////////////////////
// drag ( select region)
#define DRAG_START	0
#define DRAG_NOW	1
#define DRAG_END	2

class CFontManDoc;
class CFontManView : public CFormView
{
protected: // create from serialization only
	CFontManView();
	DECLARE_DYNCREATE(CFontManView)

public:
	//{{AFX_DATA(CFontManView)
	enum { IDD = IDD_FONTMAN_FORM };
	CButton	m_AddStrokeBtn;
	CMyEdit	m_CtrlInput;
	CStatic	m_StrokeNum;
	CEdit	m_CtrlHanja;
	CStatic	m_MouseY;
	CStatic	m_MouseX;
	CStatic	m_Code;
	CStatic	m_FontName;
	CEdit	m_Sound;
	CEdit	m_Meaning;
	CEdit	m_Contents;
	CComboBox	m_SubStrokeOrder;
	CComboBox	m_StrokeOrder;
	CButton	m_CtrlBusu;
	//}}AFX_DATA

// Attributes
public:
	CFontManDoc* GetDocument();

	int m_ViewX;
	int m_ViewY;
	int m_DragMode;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontManView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
	void LoadFont(int code);
	virtual ~CFontManView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFontManView)
	afx_msg void OnButtonAddStroke();
	afx_msg void OnButtonUserStroke();
	afx_msg void OnButtonControlPoint();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonShowAni();
	afx_msg void OnButtonSave();
	afx_msg void OnSelchangeComboStroke();
	afx_msg void OnSelchangeComboSubstroke();
	afx_msg void OnCheckBusu();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnButtonS1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void DrawBoundary(CDC * pDC, CFontManDoc *pDoc);
	void DrawControlPoints(CDC *pDC, CFontManDoc * pDoc);
	
/*	void DrawSkeletonSegments(CDC * pDC, CFontManDoc *pDoc);
	
	void DrawSkeleton(CDC *pDC, CFontManDoc *pDoc);
	
	void FindBoundary(CPoint point, int type, CPoint& p1, CPoint& p2);
*/
	//////////////////////////////////////////////////////////////////
	// 획순 그리기 관련
	short m_oldX;
	short m_oldY;

	CPoint m_StartPoint;	// 영역 박스를 그릴 때 시작부분

	//////////////////////////////////////////////////////////////////
	// 그리기 관련 변수, 함수
//	CPoint ConvertCoordinates(CPoint point);
};

#ifndef _DEBUG  // debug version in FontManView.cpp
inline CFontManDoc* CFontManView::GetDocument()
   { return (CFontManDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTMANVIEW_H__C72532FA_D19D_4748_B986_3B0016AB442E__INCLUDED_)
