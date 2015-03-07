// FontAniDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontMan.h"
#include "FontAniDlg.h"
#include "FontManDoc.h"
#include "MainFrm.h"
#include "BitmapDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontAniDlg dialog


CFontAniDlg::CFontAniDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontAniDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFontAniDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFontAniDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontAniDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFontAniDlg, CDialog)
	//{{AFX_MSG_MAP(CFontAniDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontAniDlg message handlers


BOOL CFontAniDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetAniData();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFontAniDlg::SetAniData()
{
	CMainFrame *pFrame = (CMainFrame *)GetParent();
	CFontManDoc *pDoc = (CFontManDoc *)pFrame->GetActiveDocument();

	m_Size = pDoc->m_Vector->GetStrokeNum(); // 획수 저장  
	m_PointList = new CControlPoint[m_Size];

	//////////////////////////////////////////////////////////////////////////
	// animation (ani 정보가 들어가는 메모리 공간)
	pDoc->m_AniPoint = new CControlPoint[m_Size];
	
	int i,j,k, size;
	for(i = 0; i < m_Size; i++)
	{
		size = pDoc->m_ControlPointSegment[i].m_Size;
		for( j = 0; j < size; j++)
		{
			m_PointList[i].Add( (int)(pDoc->m_ControlPointSegment[i].m_PointList[j].x / ((double)HEIGHT/A_HEIGHT +.5)),
								(int)(pDoc->m_ControlPointSegment[i].m_PointList[j].y / ((double)WIDTH/A_WIDTH) +.5),
								pDoc->m_ControlPointSegment[i].m_PointList[j].tag);
		}
		m_PointList[i].m_LineDirection = pDoc->m_ControlPointSegment[i].m_LineDirection;

		if (i==1)
			TRACE1("size:\\t %d\\n",m_PointList[i].m_Size);

		// 나눠진 획을 모두 처리하기 위함
		m_PointList[i].m_LinkNum = pDoc->m_ControlPointSegment[i].m_LinkNum;
		for( k = 0; k < pDoc->m_ControlPointSegment[i].m_LinkNum; k++)
		{
			m_PointList[i].m_Link[k] = new CControlPoint;
			size = pDoc->m_ControlPointSegment[i].m_Link[k]->m_Size;
			for( j = 0; j < size; j++)
			{
				m_PointList[i].m_Link[k]->Add( 
					(int)(pDoc->m_ControlPointSegment[i].m_Link[k]->m_PointList[j].x / ((double)HEIGHT/A_HEIGHT +.5)),				
					(int)(pDoc->m_ControlPointSegment[i].m_Link[k]->m_PointList[j].y / ((double)WIDTH/A_WIDTH) +.5),
					pDoc->m_ControlPointSegment[i].m_Link[k]->m_PointList[j].tag);
			}

			m_PointList[i].m_Link[k]->m_LineDirection = pDoc->m_ControlPointSegment[i].m_Link[k]->m_LineDirection;
			m_PointList[i].m_LinkNum = pDoc->m_ControlPointSegment[i].m_LinkNum;
		}
		
	}

	// 획 그리기 정보 만들기
	CPaintDC dc(this); // device context for painting	
	CBitmapDC bitmapDC(A_HEIGHT, A_WIDTH, &dc);	// bitmap DC에 그리기	

	for( i = 0; i < m_Size; i++)
	{	
		// 획의 경계선 및 내부 칠하기 
		m_PointList[i].MakeStrokeBitmap( &bitmapDC );		
		// 획별 Principal Axes (Major, Minor) 구하기
		m_PointList[i].GetStrokePrincipalAxes(pDoc->m_AniPoint[i], pDoc->m_Vector, i, 0 );
		
		//pDoc->m_AniPoint[i].m_Size -= 2; //Sang Ok Koo
		//-_-
		for( j = 0; j < m_PointList[i].m_LinkNum; j++)
		{
			m_PointList[i].m_Link[j]->MakeStrokeBitmap( &bitmapDC );
			m_PointList[i].m_Link[j]->GetStrokePrincipalAxes(pDoc->m_AniPoint[i], pDoc->m_Vector, i, j+1 );
			//m_PointList[i].m_Link[j]->m_Size -=2;	//Sang Ok Koo
		}
			
	}

	bitmapDC.Close();
}

void CFontAniDlg::OnOK() 
{
//	int i;
//	for( i = 0; i < m_Size; i++)
//		m_PointList[i].Init();

	delete [] m_PointList;

	ShowWindow(SW_HIDE);
}

void CFontAniDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting	
	dc.Rectangle(0,0,m_ViewX,m_ViewY);

	CMainFrame *pFrame = (CMainFrame *)GetParent();
	CFontManDoc *pDoc = (CFontManDoc *)pFrame->GetActiveDocument();
	
	/////////////////////////////////////////////////////////////////////////
	// Double Buffering
	CDC bufferDC;
	bufferDC.CreateCompatibleDC( &dc );
 
	CBitmap bufferBMP, *pOldBMP;
	bufferBMP.CreateCompatibleBitmap(&dc, A_HEIGHT, A_WIDTH);
	///////////////////////////////////////////////////////////////////////////
	bufferDC.SetMapMode(MM_TEXT);
	bufferDC.SetBkMode(TRANSPARENT);
	pOldBMP = (CBitmap *)bufferDC.SelectObject(&bufferBMP);

	bufferDC.Rectangle(0,0,A_HEIGHT,A_WIDTH);	

	// pen
	CPen pen, *pOldPen;
	pen.CreatePen( PS_SOLID, 2, RGB(0,0,255));
	pOldPen = bufferDC.SelectObject(&pen);

	int i,j,line;
	for( i = 0; i < m_Size; i++)
	{
		Sleep(200);			

		// 획의 쓰는 방향으로 그리기  
		//m_PointList[i].DrawStrokeBitmap( &bufferDC );		

		line = pDoc->m_AniPoint[i].m_Size;
		// 애니메이션 그리기

		for( j = 0; j < line; j += 2) //Sang Ok Koo line-1 => line으로 고침
			{
				Sleep(10);
				bufferDC.MoveTo(pDoc->m_AniPoint[i].m_PointList[j].x,
								pDoc->m_AniPoint[i].m_PointList[j].y);
				bufferDC.LineTo(pDoc->m_AniPoint[i].m_PointList[j+1].x,
								pDoc->m_AniPoint[i].m_PointList[j+1].y);

				dc.BitBlt(m_ViewX/2-50,m_ViewY/2-70,A_WIDTH,A_HEIGHT,&bufferDC,0,0,SRCCOPY);
			}
	}
	
	bufferDC.SelectObject(pOldPen);
	pen.DeleteObject();
	bufferDC.SelectObject( pOldBMP);
}

void CFontAniDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	m_ViewX = cx;
	m_ViewY = cy;
}
