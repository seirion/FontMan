 // FontManView.cpp : implementation of the CFontManView class
//

#include "stdafx.h"
#include "FontMan.h"

#include "FontManDoc.h"
#include "FontManView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontManView

IMPLEMENT_DYNCREATE(CFontManView, CFormView)

BEGIN_MESSAGE_MAP(CFontManView, CFormView)
	//{{AFX_MSG_MAP(CFontManView)
	ON_BN_CLICKED(IDC_BUTTON_ADD_STROKE, OnButtonAddStroke)
	ON_BN_CLICKED(IDC_BUTTON_USER_STROKE, OnButtonUserStroke)
	ON_BN_CLICKED(IDC_BUTTON_CONTROL_POINT, OnButtonControlPoint)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_ANI, OnButtonShowAni)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_CBN_SELCHANGE(IDC_COMBO_STROKE, OnSelchangeComboStroke)
	ON_CBN_SELCHANGE(IDC_COMBO_SUBSTROKE, OnSelchangeComboSubstroke)
	ON_BN_CLICKED(IDC_CHECK_BUSU, OnCheckBusu)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_S1, OnButtonS1)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontManView construction/destruction

CFontManView::CFontManView()
	: CFormView(CFontManView::IDD)
{
	//{{AFX_DATA_INIT(CFontManView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_oldX = -5000;
	m_oldY = -5000;
	m_DragMode = DRAG_END;
}

CFontManView::~CFontManView()
{
}

void CFontManView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontManView)
	DDX_Control(pDX, IDC_BUTTON_ADD_STROKE, m_AddStrokeBtn);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_CtrlInput);
	DDX_Control(pDX, IDC_STATIC_TOTAL_STROKE, m_StrokeNum);
	DDX_Control(pDX, IDC_EDIT_CONTENT_HANJA, m_CtrlHanja);
	DDX_Control(pDX, IDC_STATIC_Y, m_MouseY);
	DDX_Control(pDX, IDC_STATIC_X, m_MouseX);
	DDX_Control(pDX, IDC_STATIC_UNICODE, m_Code);
	DDX_Control(pDX, IDC_STATIC_FONT_NAME, m_FontName);
	DDX_Control(pDX, IDC_EDIT_SOUND, m_Sound);
	DDX_Control(pDX, IDC_EDIT_MEAN, m_Meaning);
	DDX_Control(pDX, IDC_EDIT_CONTENT, m_Contents);
	DDX_Control(pDX, IDC_COMBO_SUBSTROKE, m_SubStrokeOrder);
	DDX_Control(pDX, IDC_COMBO_STROKE, m_StrokeOrder);
	DDX_Control(pDX, IDC_CHECK_BUSU, m_CtrlBusu);
	//}}AFX_DATA_MAP
}

BOOL CFontManView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CFontManView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}

/////////////////////////////////////////////////////////////////////////////
// CFontManView printing

BOOL CFontManView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFontManView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFontManView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CFontManView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CFontManView diagnostics

#ifdef _DEBUG
void CFontManView::AssertValid() const
{
	CFormView::AssertValid();
}

void CFontManView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CFontManDoc* CFontManView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFontManDoc)));
	return (CFontManDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFontManView message handlers

// 사용자 획 입력 버튼 
void CFontManView::OnButtonAddStroke() 
{
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	// by Sang Ok Koo
	if (pDoc->m_point == NULL)
	{
		AfxMessageBox("글자 입력란에 글자를 입력한 후 엔터키를 치세요.");
		return;
	}

	if( pDoc->m_StrokeOn == STROKE_END)
	{
		pDoc->m_StrokeOn = STROKE_READY;
		m_AddStrokeBtn.SetWindowText(_T("획입력끝내기"));
	}
	else
	{
		//획 별로 control point 모으기
		pDoc->SetStroke();

		//획 편집 창 업데이트 
		CString format;
		format.Format(_T("%d"), pDoc->m_Vector->GetStrokeNum());
		m_StrokeNum.SetWindowText(format);	//총획수
		
		//획 콤보박스 업데이트
		int i;
		for ( i = 0; i <= pDoc->m_Vector->GetStrokeNum(); i++)
			m_StrokeOrder.DeleteString(0);	// 삭제
		m_StrokeOrder.Clear();

		if (pDoc->m_Vector->GetStrokeNum() > 0 )
			m_StrokeOrder.InsertString(0, _T("전체"));
		for ( i = 1; i <= pDoc->m_Vector->GetStrokeNum(); i++)
		{
			format.Format( _T("%d"), i );
			m_StrokeOrder.InsertString( i, format );	// 삽입	
		}	
		m_StrokeOrder.SetCurSel(0);
		
		pDoc->m_ShowStroke = FALSE;
		
		// 버튼 모드 갱신
		pDoc->m_StrokeOn = STROKE_END;
		m_AddStrokeBtn.SetWindowText(_T("획 입력하기"));
	}

//	Invalidate();
	
}

void CFontManView::OnButtonUserStroke() 
{
	((CFontManDoc *)GetDocument())->SetShowStroke();
	Invalidate();
}

void CFontManView::OnButtonControlPoint() 
{
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->SetControlPoint();

	Invalidate();
}

//
// 컨트롤 포인트 추가
void CFontManView::OnButtonAdd() 
{
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	// 현재 선택된 획 가져오기 
	if( !m_StrokeOrder.GetCurSel() )	return;

	int cur_stroke = m_StrokeOrder.GetCurSel();
	int sub_stroke = m_SubStrokeOrder.GetCurSel();

	// 선택된 control point를 현재 획에 추가
	/////////////////////////////////////////////////
	if (pDoc->m_Vector->GetStrokeNum() <= 0 ) return;
	
	int i;
	MyPoint point;
	if( sub_stroke == 0 )
	{
		for ( i = 0; i < pDoc->m_SelectedNum; i++)
		{		 
			if ( pDoc->m_ControlPointSegment[cur_stroke-1].IndexOf(pDoc->m_Selected[i]) == -1)
			{
				int index = pDoc->m_SavePoint->IndexOf(pDoc->m_Selected[i]);
				point = pDoc->m_SavePoint->m_PointList[index];
				point.user_point = pDoc->m_Vector->FindUserPoint(cur_stroke-1, sub_stroke, point.x, point.y );
				pDoc->m_ControlPointSegment[cur_stroke-1].InsertPoint(-1, point );	
			}
		}	
	}
	else
	{
		for ( i = 0; i < pDoc->m_SelectedNum; i++)
		{		 
			if ( pDoc->m_ControlPointSegment[cur_stroke-1].m_Link[sub_stroke-1]->IndexOf(pDoc->m_Selected[i]) == -1)
			{
				int index = pDoc->m_SavePoint->IndexOf(pDoc->m_Selected[i]);
				point = pDoc->m_SavePoint->m_PointList[index];
				point.user_point = pDoc->m_Vector->FindUserPoint(cur_stroke-1, sub_stroke, point.x, point.y );
				pDoc->m_ControlPointSegment[cur_stroke-1].m_Link[sub_stroke-1]->InsertPoint(-1, point);	
			}
		}
	}
	
	if(sub_stroke == 0 )
	{
		pDoc->m_ControlPointSegment[cur_stroke-1].ImproveGrouping(pDoc->m_Vector, cur_stroke-1);
		//Sang Ok Koo
		pDoc->m_ControlPointSegment[cur_stroke-1].Sort();
	}
	else
	{
		pDoc->m_ControlPointSegment[cur_stroke-1].m_Link[sub_stroke-1]->ImproveGrouping(pDoc->m_Vector, cur_stroke-1);
		//Sang Ok Koo
		pDoc->m_ControlPointSegment[cur_stroke-1].m_Link[sub_stroke-1]->Sort();
	}	
	

	// control point 리스트 초기화
	pDoc->m_SelectedNum = 0;

	Invalidate();
	
}

//
// 컨트롤 포인트 삭제
void CFontManView::OnButtonDelete() 
{
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	// 현재 선택된 획 가져오기 
	if( !m_StrokeOrder.GetCurSel() )	return;

	int cur_stroke = m_StrokeOrder.GetCurSel();
	int sub_stroke = m_SubStrokeOrder.GetCurSel();

	// 선택된 control point를 현재 획에서 삭제
	/////////////////////////////////////////////////
	if (pDoc->m_Vector->GetStrokeNum() <= 0 ) return;
	
	int i, index;

	if( sub_stroke == 0 )
	{
		for ( i = 0; i < pDoc->m_SelectedNum; i++)
		{
			if ( (index = pDoc->m_ControlPointSegment[cur_stroke-1].IndexOf(pDoc->m_Selected[i])) != -1)
				pDoc->m_ControlPointSegment[cur_stroke-1].DeletePoint(index);	
		}	
	}
	else 
	{
		for ( i = 0; i < pDoc->m_SelectedNum; i++)
		{
			if ( (index = pDoc->m_ControlPointSegment[cur_stroke-1].m_Link[sub_stroke-1]->IndexOf(pDoc->m_Selected[i])) != -1)
				pDoc->m_ControlPointSegment[cur_stroke-1].m_Link[sub_stroke-1]->DeletePoint(index);	
		}
	}
	// control point 리스트 초기화
	pDoc->m_SelectedNum = 0;

	Invalidate();
	
}

void CFontManView::OnButtonShowAni() 
{
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);


	if ( pDoc->m_FontAniDlg == NULL )
	{
		// dialog 띄우기
		CMainFrame *pFrame = (CMainFrame *)GetParent();
		pDoc->m_FontAniDlg = new CFontAniDlg();
		pDoc->m_FontAniDlg->Create(IDD_DIALOG_FONT_ANI, pFrame);
		pDoc->m_FontAniDlg->ShowWindow(SW_SHOW);
	}
	else
	{		
		pDoc->m_FontAniDlg->SetAniData();
		pDoc->m_FontAniDlg->UpdateData(FALSE);		
		pDoc->m_FontAniDlg->ShowWindow(SW_SHOW);
	}
	
}

void CFontManView::OnButtonSave() 
{
	//Sang Ok Koo
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->m_AniPoint == NULL)
	{	AfxMessageBox("애니메이션 확인을 한 뒤 저장하세요.");	return;	}
	//Sang Ok Koo

	CFileDialog dlg( FALSE, _T("fa"), _T("*.fa") );
		
	// default diretory 경로 지정
	CString path;
	if( dlg.DoModal() == IDOK )
	{
		CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->SaveFAFile( dlg.GetFileName() );
	}
	
}

void CFontManView::LoadFont(int code)
{
	CString str;
	str.Format( "%x", code);
	m_Code.SetWindowText(str);
}

void CFontManView::OnSelchangeComboStroke() 
{
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	if (!m_StrokeOrder.GetCurSel())
	{
		pDoc->SetCurrentEditStroke(-1);
		
		while ( m_SubStrokeOrder.GetCount() ) // clear combo box
		{
			m_SubStrokeOrder.DeleteString(0);
		}
		pDoc->m_CurrentEditSubStroke = 0;
	}
	else
	{
		int index = m_StrokeOrder.GetCurSel();
		pDoc->SetCurrentEditStroke(index);

		while ( m_SubStrokeOrder.GetCount() ) // clear combo box
		{
			m_SubStrokeOrder.DeleteString(0);
		}

		int count = pDoc->m_Vector->m_DevSize[index-1]+1;
		CString str;
		for( int i = 0; i < count; i++ )
		{
			str.Format(_T("%d/%d"), i+1, count);
			m_SubStrokeOrder.AddString(str);
		}
		m_SubStrokeOrder.SetCurSel(0);
		pDoc->m_CurrentEditSubStroke = 0;

		int sel = m_StrokeOrder.GetCurSel();
		int k = 1 << (sel-1);

		if( (pDoc->m_Busu & k ) ) m_CtrlBusu.SetCheck(TRUE);
		else m_CtrlBusu.SetCheck(FALSE);

	}

	SetFocus();
	
	Invalidate();
	
}

void CFontManView::OnSelchangeComboSubstroke() 
{
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->m_CurrentEditSubStroke = m_SubStrokeOrder.GetCurSel();
	SetFocus();
	Invalidate();
	
}

void CFontManView::OnCheckBusu() 
{
	CFontManDoc* pDoc = (CFontManDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	int sel = m_StrokeOrder.GetCurSel();

	if( sel != 0)
	{
		if( m_CtrlBusu.GetCheck() )	pDoc->m_Busu = pDoc->m_Busu | ( 1 << (sel-1));
		else pDoc->m_Busu = pDoc->m_Busu ^ ( 1 << (sel-1));
	}	
}

void CFontManView::OnDraw(CDC* pDC) 
{
	CFontManDoc *pDoc = (CFontManDoc *)GetDocument();
	
	if( !pDoc->m_FontLoad )return;
	if( !pDoc->m_DrawFont )return;

	/////////////////////////////////////////////////////////////////////////
	// Double Buffering
	CDC bufferDC;
	bufferDC.CreateCompatibleDC( pDC );	

	CBitmap bufferBMP, *pOldBMP;

	int width = (int)(WIDTH*1.1);
	int height = (int)(HEIGHT*1.1);
	//bufferBMP.CreateCompatibleBitmap(pDC, m_ViewX, m_ViewY);
	bufferBMP.CreateCompatibleBitmap(pDC, width, height);
	///////////////////////////////////////////////////////////////////////////
	bufferDC.SetMapMode(MM_TEXT);

	// Clear background
	bufferDC.SetBkMode(TRANSPARENT);

	pOldBMP = (CBitmap *)bufferDC.SelectObject(&bufferBMP);
	bufferDC.Rectangle(0,0,width,height);
	pDC->Rectangle(0,0,width,height);

	///////////////////////////////////////////////////////////////////////////
	// Draw bitmap  - skeleton and boundary		
	// DrawBoundary(&bufferDC, pDoc);	
	pDoc->m_SavePoint->DrawAll(&bufferDC, pDoc);

	// 각 Stroke Contour 그리기
	if (pDoc->m_CurrentEditStroke > 0)	 
	{
		// contour 그리기
		if( pDoc->m_CurrentEditSubStroke == 0 )
		{
			pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].SetPen(&bufferDC, 2, RGB(255,0,0));
			pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].Draw(&bufferDC);
		}
		else 
		{
			pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].m_Link[pDoc->m_CurrentEditSubStroke-1]->SetPen(&bufferDC, 2, RGB(255,0,0));
			pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].m_Link[pDoc->m_CurrentEditSubStroke-1]->Draw(&bufferDC);

			if( pDoc->m_ShowBefore ) // 이전 획 그리기
			{
				if(pDoc->m_CurrentEditSubStroke == 1)
				{
					pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].SetPen(&bufferDC, 2, RGB(0,127,0));
					pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].Draw(&bufferDC);
				}else
				{
					pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].m_Link[pDoc->m_CurrentEditSubStroke-2]->SetPen(&bufferDC, 2, RGB(0,127,0));
					pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].m_Link[pDoc->m_CurrentEditSubStroke-2]->Draw(&bufferDC);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////
	// Draw Control Points and Branch Points
	if (pDoc->m_ShowPoint)
	{
		DrawControlPoints(&bufferDC, pDoc);
	}
	
	////////////////////////////////////////////////////////////////////////
	// 사용자가 그린 획 그리기
	if( pDoc->m_Vector != NULL && pDoc->m_ShowStroke )
	{
		pDoc->m_Vector->Draw(&bufferDC);
	}
	
	/////////////////////////////////////////////////////////////////////////	
	pDC->BitBlt(0,0,width,height,&bufferDC,0,0,SRCCOPY);
	
	bufferDC.SelectObject( pOldBMP);
	ReleaseDC(pDC);
}

void CFontManView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	m_ViewX = cx;
	m_ViewY = cy;
	
}

void CFontManView::DrawBoundary(CDC *pDC, CFontManDoc *pDoc)
{
	if( pDC == NULL ) return; // assertion error
	int x, y;
	for (x=0; x<HEIGHT; x++)
	{
		for (y=0; y<WIDTH; y++)		
		{	
			if (pDoc->m_boundary_image[x][y] == 1)			
				pDC->SetPixel(y,x,RGB(0,255,0));	//green-boundary	
		}
	}
}

void CFontManView::DrawControlPoints(CDC *pDC, CFontManDoc *pDoc)
{

	int i;
	CControlPoint *p;

	// Draw Control Points		
	p = pDoc->m_SavePoint;
	for( i = 0; i < p->m_Size; i++)
	{
		if( p->m_PointList[i].tag )	// 1 : fixed - points on boundary
		{
			p->SetPen(pDC, 1, RGB(0,0,255));
			pDC->Rectangle( p->m_PointList[i].x - POINT_SIZE, p->m_PointList[i].y - POINT_SIZE,
							p->m_PointList[i].x + POINT_SIZE, p->m_PointList[i].y + POINT_SIZE );				
		}
		else  
		{
			p->SetPen(pDC, 1, RGB(255,0,0));
			pDC->Ellipse( p->m_PointList[i].x - POINT_SIZE, p->m_PointList[i].y - POINT_SIZE,
						  p->m_PointList[i].x + POINT_SIZE, p->m_PointList[i].y + POINT_SIZE );
		}
	}

	// 각 획별 그리기 - 굵게
	if (pDoc->m_CurrentEditStroke > 0)	 
	{
		if( pDoc->m_CurrentEditSubStroke == 0 )
		{
			pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].SetPen(pDC, 2, RGB(255,0,0));
			pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].DrawControlPoints(pDC);
		}
		else
		{
			pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].m_Link[pDoc->m_CurrentEditSubStroke-1]->SetPen(pDC, 2, RGB(255,0,0));
			pDoc->m_ControlPointSegment[pDoc->m_CurrentEditStroke-1].m_Link[pDoc->m_CurrentEditSubStroke-1]->DrawControlPoints(pDC);
		}
	}
}


void CFontManView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CFontManDoc *pDoc;
	pDoc = (CFontManDoc *)GetDocument();

	if( pDoc->m_FontLoad == FALSE ) return;
	SetCapture();
	if( pDoc->m_StrokeOn == STROKE_READY ) 
	{
		
		pDoc->m_StrokeOn = STROKE_START;
		pDoc->m_CurStroke++;
		pDoc->m_Vector->AddStroke();
	
		m_oldX = (int)point.x;
		m_oldY = (int)point.y;
	}
	else if(m_DragMode == DRAG_END)	// 그냥 단순 드래그의 시작
	{
		if( pDoc->m_ShowPoint == FALSE ) return;
		m_DragMode = DRAG_START;
		m_StartPoint = point;	// 시작 포인트 저장
		m_oldX = (int)point.x;	// 직전 포인트 저장
		m_oldY = (int)point.y;
	}
	
	CFormView::OnLButtonDown(nFlags, point);
}

void CFontManView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( GetCapture() != this ) return;
	ReleaseCapture();

	CFontManDoc *pDoc;
	pDoc = (CFontManDoc *)GetDocument();

	if( pDoc->m_StrokeOn == STROKE_START || pDoc->m_StrokeOn == STROKE_READY )
	{
		pDoc->m_StrokeOn = STROKE_READY;

		// polygone approximation
		pDoc->PolygonApproximation(0, pDoc->m_Vector->GetSize(pDoc->m_CurStroke)-1);
		pDoc->m_Vector->DevSort(pDoc->m_Vector->m_Stroke-1);
		Invalidate();
	}
	if( m_DragMode == DRAG_NOW || m_DragMode == DRAG_START )
	{
		CDC *pDC = NULL;
		pDC = GetDC();
		if( pDC == NULL )
		{
			CFormView::OnLButtonUp(nFlags, point);
		}
		pDC->SetROP2(R2_XORPEN);
		// pen setting
		CPen pen, *oldPen;
		pen.CreatePen(PS_SOLID, 1, RGB(127,127,127));
		oldPen = pDC->SelectObject(&pen);
		ReleaseDC(pDC);
		// 지우기 
		/*
		pDC->MoveTo(m_StartPoint);
		pDC->LineTo(m_oldX , m_StartPoint.y);
		pDC->LineTo(m_oldX, m_oldY);
		pDC->LineTo(m_StartPoint.x, m_oldY);
		pDC->LineTo(m_StartPoint);
		
		pDC->SelectObject(oldPen);
		pDC->SetROP2(R2_COPYPEN);
		*/

		m_DragMode = DRAG_END;

		// 영역 내에 있는 컨트롤 포인트 찾기
		pDoc->m_SelectedNum = pDoc->m_SavePoint->FindRegion( m_StartPoint, point, pDoc->m_Selected );
	}

	if( pDoc->m_SelectPoint != -1 )
	{
		pDoc->m_SelectPoint = -1;
		InvalidateRect(NULL);
	}

	
	CFormView::OnLButtonUp(nFlags, point);
}

void CFontManView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CFontManDoc *pDoc;
	pDoc = (CFontManDoc *)GetDocument();

	/////////////////////////////////////////////////////////////////////////////
	// mouse pointer coordinates
	
	// 그리기 위한 준비 
	CDC *pDC = GetDC();

	if( pDoc->m_StrokeOn == STROKE_START )
	{
		// 마우스 좌표의 변화가 있는 경우에만 실행된다
		if( m_oldX != (int)point.x || m_oldY != (int)point.y ) 
		{
			pDoc->m_Vector->Add(pDoc->m_CurStroke,(short)point.x, (short)point.y); 

			// save old coordinates
			m_oldX = (short)point.x;
			m_oldY = (short)point.y;

			pDC->SetPixel(point, RGB(255,0,0));
		}		
	}
	else if( m_DragMode != DRAG_END)
	{
		/////////////////////////////////////////////////////////////////////////
		// control point drag

		if( !pDC) return; // assertion error
		pDC->SetROP2(R2_XORPEN);
		// pen setting
		CPen pen, *oldPen;
		pen.CreatePen(PS_SOLID, 1, RGB(127,127,127));
		oldPen = pDC->SelectObject(&pen);
		
		// 지우기
		if( m_DragMode == DRAG_NOW )
		{
			pDC->MoveTo(m_StartPoint);
			pDC->LineTo(m_oldX , m_StartPoint.y);
			pDC->LineTo(m_oldX, m_oldY);
			pDC->LineTo(m_StartPoint.x, m_oldY);
			pDC->LineTo(m_StartPoint);
			
		}else m_DragMode = DRAG_NOW;

		// 새로 그리기
		pDC->MoveTo(m_StartPoint);
		pDC->LineTo(point.x , m_StartPoint.y);
		pDC->LineTo(point);
		pDC->LineTo(m_StartPoint.x, point.y);
		pDC->LineTo(m_StartPoint);

		pDC->SelectObject(oldPen);
		pDC->SetROP2(R2_COPYPEN);
		
		m_oldX = (int)point.x;	// 직전 포인트 저장
		m_oldY = (int)point.y;

	}
	ReleaseDC(pDC);
	CFormView::OnMouseMove(nFlags, point);
}

BOOL CFontManView::PreTranslateMessage(MSG* pMsg) 
{
	
	if(pMsg->message == WM_KEYDOWN )
	{
		if(  pMsg->wParam == VK_DELETE )
		{
			CFontManDoc *pDoc;
			pDoc = (CFontManDoc *)GetDocument();

			if(pDoc->m_SelectedNum <= 0 ) return CFormView::PreTranslateMessage(pMsg);
			OnButtonDelete();
		
		}
		else if(  pMsg->wParam == VK_INSERT )
		{
			CFontManDoc *pDoc;
			pDoc = (CFontManDoc *)GetDocument();

			if(pDoc->m_SelectedNum <= 0 ) return CFormView::PreTranslateMessage(pMsg);
			OnButtonAdd();
		}
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}

// 이전획 보여주기
void CFontManView::OnButtonS1() 
{
	// TODO: Add your control notification handler code here
	CFontManDoc *pDoc;
	pDoc = (CFontManDoc *)GetDocument();
	
	if( pDoc->m_ShowBefore ) pDoc->m_ShowBefore = FALSE;
	else pDoc->m_ShowBefore = TRUE;

	Invalidate();
}
