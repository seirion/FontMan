  // ControlPoint.cpp: implementation of the CControlPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FontMan.h"
#include "ControlPoint.h"

#include "BitmapDC.h"
#include "AutoPen.h"
#include "PointVector.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CControlPoint::CControlPoint()
{
	m_Size = 0;
	m_ArraySize = 64;
	m_PointList = new MyPoint[m_ArraySize];
	m_Contour = new int[32];
	m_ContourNum = 0;

	m_pen = NULL;
	m_brush = NULL;

	m_Direction = 1;

	m_LinkNum = 0;

}

CControlPoint::CControlPoint( int size )
{
	m_Size = 0;
	m_ArraySize = size;
	m_PointList = new MyPoint[m_ArraySize];
	m_Contour = new int[32];
	m_ContourNum = 0;

	m_pen = NULL;
	m_brush = NULL;

	m_Direction = 1;

	m_LinkNum = 0;
}

// destructor
CControlPoint::~CControlPoint()
{
	Init();
}

void CControlPoint::Init()
{
	if( m_PointList )	delete[] m_PointList;
	m_PointList = NULL;
	if( m_Contour ) delete[] m_Contour;
	m_Contour = NULL;
	
	m_PointList = new MyPoint[m_ArraySize];
	m_Contour = new int[32];

	m_Size = 0;
	m_ArraySize = 64;
	m_ContourNum = 0;

	int i;
	for( i = 0; i < m_LinkNum; i++)
		delete m_Link[i];
}

void CControlPoint::ArrayDouble()
{
	MyPoint *newArray;
	newArray = new MyPoint[m_ArraySize*2];

	m_ArraySize = m_ArraySize * 2;
	
	for( int i = 0; i < m_Size; i++)
		newArray[i] = m_PointList[i];

	// �޸� ����		
	delete [] m_PointList;

	// ���ο� array ����
	m_PointList = newArray;	
}


// return last index
int CControlPoint::Add( int x, int y, int tag)
{
	if( m_ArraySize == m_Size ) ArrayDouble();

	m_PointList[m_Size].x = x;
	m_PointList[m_Size].y = y;
	m_PointList[m_Size].tag = tag;

	if (m_Size == 0)
	{
		m_PointList[m_Size].id = 0;	
		m_PointList[m_Size].cgroup = 0;
	}
	else 
	{
		m_PointList[m_Size].id = m_PointList[m_Size-1].id + 1;
		m_PointList[m_Size].cgroup = m_PointList[m_Size-1].cgroup;
	}

	m_Size++;

	return m_Size;
}


// return last index
int CControlPoint::Add( CFontManDoc *pDoc )
{
	while(TRUE)
	{
		if( pDoc->m_pointNum < m_ArraySize )break;
		else ArrayDouble();
	}

	int i, j;

	m_ContourNum = pDoc->m_contourNum;
	for( i = 0; i < m_ContourNum; i++)
	{
		m_Contour[i] = pDoc->m_contour[i];	
	}

	j = 0;
	m_Size = pDoc->m_pointNum;	
	for( i = 0; i < m_Size; i++)
	{
		m_PointList[i].x = pDoc->m_point[i].x;
		m_PointList[i].y = pDoc->m_point[i].y;
		m_PointList[i].tag = pDoc->m_tag[i];
		m_PointList[i].id = i;

		// �� point���� contour��ȣ �����ϱ�
		m_PointList[i].cgroup = j;

		if( i >= m_Contour[j])
			j++;
	}	

	// �̿��ϴ� ��Ʈ�� ����Ʈ ���� ���̺� ����� - �� ó������ �������
//	MakeNBTable();

	return m_Size;
}

/////////////////////////////////////////////////////////////////////////////////
// �׸��� ���� �Լ���

//
// 100 * 100 ���ΰ� ä���� bitmap �����
void CControlPoint::MakeStrokeBitmap(CDC* dc)
{
	// bitmap DC�� �׸���
	CBitmapDC bitmapDC(A_HEIGHT, A_WIDTH, dc);

	// �����
	bitmapDC.Rectangle(-10, -10, A_HEIGHT+20, A_WIDTH+20);
	
	// contour �׸��� 
	SetPen(&bitmapDC, 1, RGB(255,0,0));
	Draw(&bitmapDC);

	m_BoundaryPoints = new CControlPoint();

	int x, y;
	for ( y=0; y<A_HEIGHT; y++)
	{
		for ( x=0; x<A_WIDTH; x++)
		{
			m_StrokeBitmap[y][x] = BACKGROUND;

			COLORREF color = bitmapDC.GetPixel(x, y);
			if( color == RGB(255,0,0) ) 
			{
				// contour ����
				m_BoundaryPoints->Add( x, y, 1);
				m_StrokeBitmap[y][x] = FOREGROUND;
				if( y < A_HEIGHT-1 && bitmapDC.GetPixel(x, y+1) != RGB(255,0,0) )
				{
					m_BoundaryPoints->Add( x, y+1, 1);
					m_StrokeBitmap[y+1][x] = FOREGROUND;
				}
				if( x < A_WIDTH-1 && y<A_HEIGHT-1 && bitmapDC.GetPixel(x+1, y+1) != RGB(255,0,0) )
				{
					m_BoundaryPoints->Add( x+1, y+1, 1);
					m_StrokeBitmap[y+1][x+1] = FOREGROUND;
				}

			}
		}
	}

	// contour �̹����� ������ �� ��(1,1)�� ������ ä��� 
	regionfilling(m_StrokeBitmap, 1, 1);
	
	ReleasePen(&bitmapDC);
	bitmapDC.Close();
}

void CControlPoint::DrawStrokeBitmap(CDC *pDC)
{
	// Filling�� ȹ �׸���
	int x, y;
	for ( x=1; x<A_HEIGHT-1; x++)
		for ( y=1; y<A_WIDTH-1; y++)
			if (m_StrokeBitmap[x][y]==FOREGROUND)
				pDC->SetPixel(y,x, RGB(0, 0, 255));

}

//�� �� ȹ �׸���
void CControlPoint::Draw(CDC *pDC )
{
	if (m_Size == 0) return;
	
	double INCREMENT = 0.01;

	int start, ends;
	int bezierStart, bezierEnd;	
	int pointNum;				// bezier curve : controll point #

	pointNum = 1;
	
	start = 0;
	ends = m_Size-1;

	// contour�� ���� //Sang Ok Koo
//	if (m_PointList[start].tag == 0)	//������ ��輱 ���� ���� �ǵ��� ��
//	{	start++; ends=0;	}

	bezierStart = start;

	pDC->MoveTo( m_PointList[bezierStart].x ,m_PointList[bezierStart].y);

	double u;	// for bezier curve's indexing
	//////////////////////////////////////////////////////////////////
	
	while(true)
	{	
		if( bezierStart != start && bezierStart == ends ) bezierEnd = start;
		else bezierEnd = bezierStart+1;

		while( true )
		{
			if( m_PointList[bezierEnd].tag == 1 ) break;	// �ٷ� ������ ������ ��
			if( bezierEnd == start ) break;		// ������ point���� ��������
			if( bezierEnd == ends )  // contour ������ point ���� ��������
			{
				bezierEnd = start;
				pointNum++;
				break;
			}
			else bezierEnd++;

			pointNum++;
		}

		if( bezierEnd == start )
		{
			if( pointNum == 1 ) pDC->LineTo(m_PointList[bezierEnd].x, m_PointList[bezierEnd].y);
			else
			{
				for( u = 0.0; u <= 1.1; u += INCREMENT )
					MakeBezierCurve2(pDC, bezierStart, ends, start, u );
				pointNum = 1;
			}
			break;
		}
		else if( pointNum == 1 )
		{
			pDC->LineTo(m_PointList[bezierEnd].x,m_PointList[bezierEnd].y);
			bezierStart = bezierEnd;
		}
		else
		{
			for( u = 0.0; u <= 1.1; u += INCREMENT )
				MakeBezierCurve(pDC, bezierStart, bezierEnd, u );
			bezierStart = bezierEnd;
			pointNum = 1;
		}
	}// end of while ; for some bezier' controll points

	ReleasePen(pDC);
	ReleaseBrush(pDC);
	
}

//��ü ȹ �׸���
void CControlPoint::DrawAll(CDC *pDC, CFontManDoc *pDoc)
{
	int contour = 0;
	int start, end;
	int j;

	//////////////////////////////////////////////////////////////////////////////
	// �ܰ��� �׸���
	double INCREMENT = 0.01;

	int bezierStart, bezierEnd;
	SetPen(pDC, 1, RGB(0,0,0));
	int pointNum;				// bezier curve : controll point #
	for( j = 0; j < m_ContourNum; j++)
	{
		pointNum = 1;

		// ���ο� contour�� ����
		if( j == 0 ) start = 0;
		else start = m_Contour[j-1]+1;
		end = m_Contour[j];
		pDC->MoveTo( m_PointList[start].x ,m_PointList[start].y);


		double u;	// for bezier curve's indexing
		//////////////////////////////////////////////////////////////////

		bezierStart = start;
		while(true)
		{
			//if( bezierStart == 22 ) return;
			if( bezierStart != start && bezierStart == end ) bezierEnd = start;
			else bezierEnd = bezierStart+1;
			
			while( true )
			{
				if( m_PointList[bezierEnd].tag == 1 ) break;	// �ٷ� ������ ������ ��
				if( bezierEnd == start ) break;		// ������ point���� ��������
				if( bezierEnd == end )  // contour ������ point ���� ��������
				{
					bezierEnd = start;
					pointNum++;
					break;
				}
				else bezierEnd++;
				pointNum++;
			}

			if( bezierEnd == start )
			{
				if( pointNum == 1 ) pDC->LineTo(m_PointList[bezierEnd].x, m_PointList[bezierEnd].y);
				else
				{
					for( u = 0.0; u <= 1.1; u += INCREMENT )
						MakeBezierCurve2(pDC, bezierStart, end, start, u );
					pointNum = 1;
				}
				break;
			}
			else if( pointNum == 1 )
			{
				pDC->LineTo(m_PointList[bezierEnd].x,m_PointList[bezierEnd].y);
				bezierStart = bezierEnd;
			}
			else
			{
				for( u = 0.0; u <= 1.1; u += INCREMENT )
					MakeBezierCurve(pDC, bezierStart, bezierEnd, u );
				bezierStart = bezierEnd;
				pointNum = 1;
			}

		}// end of while ; for some bezier' controll points
	} // end of for(j) ; for all contour

	ReleasePen(pDC);
	ReleaseBrush(pDC);
	
}

void CControlPoint::ReleasePen(CDC *pDC)
{
	if( m_pen == NULL)	return;
	pDC->SelectObject(m_oldPen);
	delete m_pen;
	m_pen = NULL;
}

void CControlPoint::ReleaseBrush(CDC *pDC)
{
	if( m_brush == NULL)return;
	pDC->SelectObject(m_oldBrush);
	delete m_brush;
	m_brush = NULL;
}

void CControlPoint::SetBrush(CDC *pDC, COLORREF color)
{
	if( m_brush != NULL) delete m_brush;
	m_brush = new CBrush;
	m_brush->CreateSolidBrush(color);
	m_oldBrush = pDC->SelectObject(m_brush);
}

void CControlPoint::SetPen(CDC *pDC, int width, COLORREF color)
{
	if( m_pen != NULL ) m_pen->DeleteObject();
	m_pen = new CPen;
	m_pen->CreatePen(PS_SOLID, width, color);
	m_oldPen = pDC->SelectObject(m_pen);
}

void CControlPoint::MakeBezierCurve(CDC *pDC, int start, int end, double u)
{
	if( u > 1.0) u = 1.0;
	if( u == 0.0 )
	{
		//pDC->MoveTo(pDoc->m_point[start].x, pDoc->m_point[start].y );
		return;
	}
	if( u == 1.0 )
	{
		pDC->LineTo(m_PointList[end].x,m_PointList[end].y);
		return;
	}
	int pointNum = end - start;
	
	int nFact = Factorial( pointNum );

	double x,y;			// estimated point coordinates
	x = 0.0;
	y = 0.0;

	for( int i = 0; i <= pointNum; i++)
	{
		x += m_PointList[start+i].x * nFact / Factorial(i) / Factorial(pointNum-i)
			* Power( u, i ) * Power( 1.0-u, pointNum-i);
		y += m_PointList[start+i].y * nFact / Factorial(i) / Factorial(pointNum-i)
			* Power( u, i ) * Power( 1.0-u, pointNum-i);
	}

	pDC->LineTo((int)(x+.5),(int)(y+.5));
}

void CControlPoint::MakeBezierCurve2(CDC *pDC, int start, int end,int last, double u)
{
	if( u > 1.0) u = 1.0;
	if( u == 0.0 )
	{
		//pDC->MoveTo(pDoc->m_point[last].x, pDoc->m_point[last].y );
		return;
	}
	if( u == 1.0 )
	{
		pDC->LineTo(m_PointList[last].x,m_PointList[last].y);
		return;
	}
	int pointNum = end - start+1;
	
	int nFact = Factorial( pointNum );

	double x,y;			// estimated point coordinates
	x = 0.0;
	y = 0.0;

	for( int i = 0; i < pointNum; i++)
	{
		x += m_PointList[start+i].x * nFact / Factorial(i) / Factorial(pointNum-i)
			* Power( u, i ) * Power( 1.0-u, pointNum-i);
		y += m_PointList[start+i].y * nFact / Factorial(i) / Factorial(pointNum-i)
			* Power( u, i ) * Power( 1.0-u, pointNum-i);
	}

	x += m_PointList[last].x * Power( u, pointNum );
	y += m_PointList[last].y * Power( u, pointNum );

	pDC->LineTo((int)(x+.5),(int)(y+.5));
}
//
// return factorial number
int CControlPoint::Factorial(int n)
{
	if ( n == 0 || n == 1 ) return 1;
	
	int result = 1;
	for( int i = 2; i <= n; i++)
		result = result * i;
	return result;
}

double CControlPoint::Power(double n, int k)
{
	if( n == 0 ) return 1;
	if( k == 0 ) return 1;
	if( k == 1 ) return n;

	double result = n;

	for( int i = 1; i < k; i++)
		result = result * n;
	return result;
}

int CControlPoint::Find(CPoint point)
{
	for( int i = 0; i < m_Size; i++)
	{
		if( abs(m_PointList[i].x - point.x ) < POINT_SIZE && 
			abs(m_PointList[i].y - point.y ) < POINT_SIZE )
			return i;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////

BOOL CControlPoint::ContainsPoint(int x, int y)
{
	for (int i=0; i<m_Size; i++)
		if (m_PointList[i].x == x && m_PointList[i].y == y)
			return TRUE;

	return FALSE;
}

MyPoint* CControlPoint::GetPointPtr(int x, int y)
{
	for (int i=0; i<m_Size; i++)
		if (m_PointList[i].x == x && m_PointList[i].y == y)
			return &m_PointList[i];

	return &m_PointList[i];
}

int CControlPoint::IndexOf(int x, int y)
{
	for (int i=0; i<m_Size; i++)
		if (m_PointList[i].x == x && m_PointList[i].y == y)
			return i;

	return -1;
}

int CControlPoint::IndexOf(int id)
{
	for (int i=0; i<m_Size; i++)
		if (m_PointList[i].id == id)
			return i;

	return -1;
}

void CControlPoint::SetPointAt(int index, int x, int y, int tag)
{
	m_PointList[index].x = x;
	m_PointList[index].y = y;
	m_PointList[index].tag = tag;
}

BOOL CControlPoint::DeletePoint(int index)
{
	if (m_Size <= 3)
	{
		AfxMessageBox("�������� 3�� �����̹Ƿ� ��� �����մϴ�.");
		m_Size = 0;
	}

	if (index < m_Size)
	{
		for( int i = index+1; i < m_Size; i++)
		{
			m_PointList[i-1] = m_PointList[i];
		}

		m_Size--;

		return TRUE;
	}

	return FALSE;
}

void CControlPoint::InsertPoint(int index, MyPoint point)
{
	if( m_ArraySize == m_Size ) ArrayDouble();
	
	if( index == -1 )	index = m_Size;

	for( int i = m_Size-1; i >= index; i--)
	{
		m_PointList[i+1] = m_PointList[i];
	}
	m_PointList[index] = point;
	
	m_Size++;

}

// ��Ʈ�� ����Ʈ ����ϱ�
void CControlPoint::DrawControlPoints(CDC *pDC)
{
	int i;

	// Draw Control Points
	for( i = 0; i < m_Size; i++)
	{
		if( m_PointList[i].tag )	// 1 : fixed - points on boundary
		{
			SetPen(pDC, 2, RGB(0,0,255));
			pDC->Rectangle( m_PointList[i].x - POINT_SIZE-1, m_PointList[i].y - POINT_SIZE-1,
				m_PointList[i].x + POINT_SIZE+1, m_PointList[i].y + POINT_SIZE+1 );

			
			// control point id ����ϱ�
			CString idStr; 
			idStr.Format( _T("%d"), m_PointList[i].id);
			RECT rect = {	m_PointList[i].x - POINT_SIZE-10, m_PointList[i].y - POINT_SIZE-12,
							m_PointList[i].x + POINT_SIZE+10, m_PointList[i].y + POINT_SIZE };
			SetPen(pDC, 1, RGB(0,100,100));
			pDC->DrawText( idStr, &rect, DT_LEFT | DT_TOP);			
		}
		else  
		{
			SetPen(pDC, 2, RGB(255,0,0));
			pDC->Ellipse(m_PointList[i].x - POINT_SIZE-1, m_PointList[i].y - POINT_SIZE-1,
				m_PointList[i].x + POINT_SIZE+1, m_PointList[i].y + POINT_SIZE+1 );

		}

	}

}


int CControlPoint::FindRegion( CPoint start, CPoint end, int selected[])
{
	int number = 0;
	int minX, minY, maxX, maxY;

	if( start.x < end.x)
	{
		minX = start.x;
		maxX = end.x;
	}else
	{
		maxX = start.x;
		minX = end.x;
	}

	if( start.y < end.y)
	{
		minY = start.y;
		maxY = end.y;
	}else
	{
		maxY = start.y;
		minY = end.y;
	}

	for( int i = 0; i < m_Size; i++)
	{
		if( m_PointList[i].x >= minX && m_PointList[i].x <= maxX &&
			m_PointList[i].y >= minY && m_PointList[i].y <= maxY )
		{
			selected[number] = m_PointList[i].id;	// point�� index ����
			number++;
		}
	}
	return number;
}

// id ������ sorting�ϱ�
void CControlPoint::Sort()
{
	int i, j;
	
	for( i = 0; i < m_Size-1; i++)
	{
		if (m_PointList[i].cgroup != m_PointList[i+1].cgroup)
			return;	
	}


	for( i = 0; i < m_Size-1; i++)
	{
		for( j = i+1; j < m_Size; j++)
		{
			if (m_PointList[i].id > m_PointList[j].id)
			{
				Swap (m_PointList, i, j);
				//Shift (m_PointList, i, j);
			}
		}
	}
}

// control point ���� ����
void CControlPoint::Sort(CFontManDoc *pDoc)
{		
	int i, j;
	BOOL FoundNextPoint = FALSE;
	
	for( i = 0; i < m_Size; i++)
	{
		FoundNextPoint = FALSE;

		for( j = i+1; j < m_Size; j++)
		{
			// � ����Ʈ�� ���� ����Ʈ�� �̿��ϸ� �� ����Ʈ�� ���� ����Ʈ ���� ��ġ�� �ű�� break 
			if ( pDoc->m_SavePoint->m_NBTable[m_PointList[i].id][m_PointList[j].id] == 1 )
			{
				if ( j != i+1 )	
					Shift (m_PointList, i+1, j);
			
				FoundNextPoint = TRUE;
				break;
			}
		}
		
		// ���� �̿��ϴ� ����Ʈ�� �� ã������ ���� ����Ʈ���� �Ÿ������� ���� ����� ����Ʈ�� ã�´�. 
		if ( FoundNextPoint == FALSE )
		{
			int dist;
			int MinDist = 999;
			int NearestPoint = i+1;
			
			for( j = i+1; j < m_Size; j++)
			{				
				dist =  (int) sqrt(  pow( m_PointList[i].x - m_PointList[j].x, 2) 
					               + pow( m_PointList[i].y - m_PointList[j].y, 2) );

				//dist += abs(m_PointList[j].id - m_PointList[j].id); // id�� ���̵� ����

				if ( dist < MinDist )
				{
					MinDist = dist;
					NearestPoint = j;
				}
			}

			if ( NearestPoint != i+1 )
				Shift (m_PointList, i+1, NearestPoint);
		}
	}

}

void CControlPoint::MakeNBTable()
{
	if (m_Size <= 0)	return;

	int i, j;
	m_NBTable = new int*[m_Size];
	for ( i=0; i<m_Size; i++)
		m_NBTable[i] = new int[m_Size];
	
	// �ʱ�ȭ 
	for ( i=0; i<m_Size; i++)
		for ( j=0; j<m_Size; j++)
			m_NBTable[i][j] = 0;

	// Neighbor point table�� �����.
	int cgroup = 0; 
	int start = 0;
	m_NBTable[0][0] = 0;
	for ( i=1; i<m_Size; i++ )
	{
		m_NBTable[i][i] = 2;	// �ڱ� �ڽŰ��� 2
		
		if ( cgroup == m_PointList[i].cgroup )
		{
			m_NBTable[i-1][i] = 1;
			m_NBTable[i][i-1] = 1;
		}
		else	
		{
			m_NBTable[start][i-1] = 1;	//���� contour�� ���۰� �� ����
			m_NBTable[i-1][start] = 1;
			
			// ���ο� �׷��� ����
			cgroup = m_PointList[i].cgroup;
			start = i;
		}
	}

}

void CControlPoint::Swap(MyPoint *list, int i, int j)
{
	MyPoint temp;

	// swap
	temp.id = list[j].id;
	temp.x = list[j].x;
	temp.y = list[j].y;
	temp.tag = list[j].tag;
	temp.cgroup = list[j].cgroup;
	
	list[j].id = list[i].id;
	list[j].x = list[i].x;
	list[j].y = list[i].y;
	list[j].tag = list[i].tag;
	list[j].cgroup = list[i].cgroup;

	list[i].id = temp.id;
	list[i].x = temp.x;
	list[i].y = temp.y;
	list[i].tag = temp.tag;
	list[i].cgroup = temp.cgroup;
}

// i ���� j-1 ������ ���������� ��ĭ�� �̵��ϰ�, 
// j ��° point�� i ��ġ�� ������.
void CControlPoint::Shift(MyPoint *list, int i, int j)
{
	MyPoint temp;

	temp.id = list[j].id;
	temp.x = list[j].x;
	temp.y = list[j].y;
	temp.tag = list[j].tag;
	temp.cgroup = list[j].cgroup;

	if (i < j)
	{
		for ( int k=j-1; k>=i; k-- )
		{		
			list[k+1].id = list[k].id;
			list[k+1].x = list[k].x;
			list[k+1].y = list[k].y;
			list[k+1].tag = list[k].tag;
			list[k+1].cgroup = list[k].cgroup;
		}
	}

	list[i].id = temp.id;
	list[i].x = temp.x;
	list[i].y = temp.y;
	list[i].tag = temp.tag;
	list[i].cgroup = temp.cgroup;
}


// �Ķ���� : �ִϸ��̼� �� ȹ���� ���� ����� ����� 
void CControlPoint::GetStrokePrincipalAxes(CControlPoint &point, CPointVector *vector, int strokeIndex, int subIndex)
{
	int i;

	int dStart; // ���� �� ȹ�� �� ������ ó���ϱ� ���� 
	dStart = point.m_Size;

	// �Է����� boundary point���� 
	int dataNum;
	
	dataNum = m_BoundaryPoints->m_Size;
	

	int **X = new int*[dataNum];	//���� ��ǥ
	double **XC = new double*[dataNum];	//��ȯ�� ��ǥ
	int **XS = new int*[dataNum]; // ��Ʈ��(�ִϸ��̼ǿ� ����� ����)
	for ( i=0; i<dataNum; i++)
	{
		X[i] = new int[2];
		XC[i] = new double[2];
		XS[i] = new int[3];
	}
	for ( i=0; i<dataNum; i++)
	{
		X[i][0] = XS[i][0] = m_BoundaryPoints->m_PointList[i].x;
		X[i][1] = XS[i][1] = m_BoundaryPoints->m_PointList[i].y;
		XS[i][2] = i;
	}

	CPoint startPoint, endPoint; // ������ ������ ����

	int lineDirection = m_LineDirection;
	//////////////////////////////////////////////////////////////////////////
	// ���⼭���� �ִϸ��̼� ��ǥ ���ϱ�-_-

	int t1,t2,t3, ot1,ot2,ot3, num,start;
	if( lineDirection == 0 ) // ����ȹ 
	{
		SortY(XS,dataNum);
		SortX(XS,dataNum);
		num = 1;
		for( i = 0; i < dataNum; i++)
		{
			t1 = XS[i][0];
			t2 = XS[i][1];
			t3 = XS[i][2];
		
			///////////////////////////////////////////////////////////////
			// processing
			if( !i )
			{
				start = t3;
				num = 1;
				ot1 = XS[i][0];
				ot2 = XS[i][1];
				ot3 = XS[i][2];
				continue;
			}
			
			if(t1 != ot1)// x ��ǥ�� ��ȭ
			{
				// point add �ϱ� 
				if( num == 1) // ù��° ���� ��� 
				{
					point.Add(X[start][0],X[start][1],0);
					point.Add(X[start][0],X[start][1],0);

				}else // 2�� �̻��� ���� ��� 
				{
					point.Add(X[start][0],X[start][1],0);
					point.Add(X[ot3][0],X[ot3][1],0);
				}
				start = t3;
				num = 1;
			}else num++;

			// save old values
			ot1 = XS[i][0];
			ot2 = XS[i][1];
			ot3 = XS[i][2];
		}
	}
	else if( lineDirection == 1 ) // ����ȹ 
	{ // ���� �ȼ� ��ǥ�� ���� �ؼ�
		SortX(XS,dataNum);
		SortY(XS,dataNum);
		num = 1;
		for( i = 0; i < dataNum; i++)
		{
			t1 = XS[i][0];
			t2 = XS[i][1];
			t3 = XS[i][2];
		
			///////////////////////////////////////////////////////////////
			// processing
			if( !i )
			{
				start = t3;
				num = 1;
				ot1 = XS[i][0];
				ot2 = XS[i][1];
				ot3 = XS[i][2];
				continue;
			}
			
			if(t2 != ot2)// �� ��ǥ�� ��ȭ
			{
				// point add �ϱ� 
				if( num == 1) // ù��° ���� ��� 
				{
					point.Add(X[start][0],X[start][1],0);
					point.Add(X[start][0],X[start][1],0);
				}else // 2�� �̻��� ���� ��� 
				{
					point.Add(X[start][0],X[start][1],0);
					point.Add(X[ot3][0],X[ot3][1],0);
				}
				start = t3;
				num = 1;
			}else num++;

			// save old values
			ot1 = XS[i][0];
			ot2 = XS[i][1];
			ot3 = XS[i][2];
		}
	}
	else if( lineDirection == 2 ) // ��
	{
		SortX(XS,dataNum);
		SortXplusY(XS,dataNum);
		num = 1;
		ot1 = ot2 = -1;
		for( i = 0; i < dataNum; i++)
		{
			t1 = XS[i][0];
			t2 = XS[i][1];
			t3 = XS[i][2];
		
			///////////////////////////////////////////////////////////////
			// processing
			if( !i )
			{
				start = t3;
				num = 1;
				ot1 = XS[i][0];
				ot2 = XS[i][1];
				ot3 = XS[i][2];
				continue;
			}
			
			if( t1+t2 != ot1+ot2 )// �� ��ǥ�� ��ȭ
			{
				// point add �ϱ� 
				if( num == 1) // ù��° ���� ��� 
				{
					
					// 0. ù��° ���� ��� 
					if( i == 1 )
					{
						point.Add(X[start][0],X[start][1],0);
						point.Add(X[start][0],X[start][1],0);
						// ���� ���� ������ ������
						startPoint.x = X[start][0];
						startPoint.y = X[start][1];
						endPoint.x = X[start][0];
						endPoint.y = X[start][1];
					}
					else
					{
						if( NearPoint( startPoint, X[start][0], X[start][1] )) // �������� ����
						{
							point.Add(X[start][0],X[start][1],0);
							if( startPoint.x == X[start][0])
							{
								point.Add(endPoint.x, endPoint.y+1,0);
								endPoint.y++;
							}
							else
							{
								point.Add(endPoint.x+1, endPoint.y,0);
								endPoint.x++;
							}
							
							startPoint.x = X[start][0];
							startPoint.y = X[start][1];
							
						}
						else if( NearPoint( endPoint, X[start][0], X[start][1] ) )// ������ ���� 
						{
							point.Add(X[start][0],X[start][1],0);
							if( endPoint.x == X[start][0])
							{
								point.Add(startPoint.x, startPoint.y+1,0);
								startPoint.y++;
							}
							else
							{
								point.Add(startPoint.x+1, startPoint.y,0);
								startPoint.x++;
							}
							
							endPoint.x = X[start][0];
							endPoint.y = X[start][1];

							//SwapPoint( startPoint, endPoint);
						}else 
						{
							//AfxMessageBox(_T("Error! CControlPoint; 1092 line"));
						}
					}
				}else // 2�� �̻��� ���� ��� 
				{
					point.Add(X[start][0],X[start][1],0);
					point.Add(X[ot3][0],X[ot3][1],0);
					startPoint.x = X[start][0];
					startPoint.y = X[start][1];
					endPoint.x = X[ot3][0];
					endPoint.y = X[ot3][1];

				}
				start = t3;
				num = 1;
			}else num++;

			// save old values
			ot1 = XS[i][0];
			ot2 = XS[i][1];
			ot3 = XS[i][2];
		}
	}else // ��
	{
		SortX(XS,dataNum);
		SortXminusY(XS,dataNum);
		num = 1;
		ot1 = ot2 = -1;
		for( i = 0; i < dataNum; i++)
		{
			if( i == 334 )
			{
				int k1;
				k1 = 1;
				
			}
			t1 = XS[i][0];
			t2 = XS[i][1];
			t3 = XS[i][2];
		
			///////////////////////////////////////////////////////////////
			// processing
			if( !i )
			{
				start = t3;
				num = 1;
				ot1 = XS[i][0];
				ot2 = XS[i][1];
				ot3 = XS[i][2];
				continue;
			}
			
			if( t1-t2 != ot1-ot2 )// �� ��ǥ�� ��ȭ
			{
				// point add �ϱ� 
				if( num == 1) // ù��° ���� ��� 
				{
					
					// 0. ù��° ���� ��� 
					if( i == 1 )
					{
						point.Add(X[start][0],X[start][1],0);
						point.Add(X[start][0],X[start][1],0);
						// ���� ���� ������ ������
						startPoint.x = X[start][0];
						startPoint.y = X[start][1];
						endPoint.x = X[start][0];
						endPoint.y = X[start][1];
					}
					else
					{
						if( NearPoint( startPoint, X[start][0], X[start][1] )) // �������� ����
						{
							point.Add(X[start][0],X[start][1],0);
							if( startPoint.x == X[start][0])
							{
								point.Add(endPoint.x, endPoint.y-1,0);
								endPoint.y--;
							}
							else
							{
								point.Add(endPoint.x+1, endPoint.y,0);
								endPoint.x++;
							}
							
							startPoint.x = X[start][0];
							startPoint.y = X[start][1];
							
						}
						else //if( NearPoint( endPoint, X[start][0], X[start][1] ) )// ������ ���� 
						{
							point.Add(X[start][0],X[start][1],0);
							if( endPoint.x == X[start][0])
							{
								point.Add(startPoint.x, startPoint.y-1,0);
								startPoint.y--;
							}
							else
							{
								point.Add(startPoint.x+1, startPoint.y,0);
								startPoint.x++;
							}
							
							endPoint.x = X[start][0];
							endPoint.y = X[start][1];

							//SwapPoint( startPoint, endPoint);
						}
					}
				}else // 2�� �̻��� ���� ��� 
				{
					point.Add(X[start][0],X[start][1],0);
					point.Add(X[ot3][0],X[ot3][1],0);
					startPoint.x = X[start][0];
					startPoint.y = X[start][1];
					endPoint.x = X[ot3][0];
					endPoint.y = X[ot3][1];

				}
				start = t3;
				num = 1;
			}else num++;

			// save old values
			ot1 = XS[i][0];
			ot2 = XS[i][1];
			ot3 = XS[i][2];
		}
	}
	
	
	// ȹ�� ���� ���� �ϱ� 
/*	int v1, v2, s, l;
	if( subIndex == 0 )
	{ 
		startPoint.x = (int)(vector->m_Point[strokeIndex][X1][0]*(double)A_HEIGHT/HEIGHT+.5);
		startPoint.y = (int)(vector->m_Point[strokeIndex][Y1][0]*(double)A_HEIGHT/HEIGHT+.5);
		
	}
	else
	{
		s = vector->m_DevIndex[strokeIndex][subIndex-1];
		startPoint.x = (int)(vector->m_Point[strokeIndex][X1][s]*(double)A_HEIGHT/HEIGHT+.5);
		startPoint.y = (int)(vector->m_Point[strokeIndex][Y1][s]*(double)A_HEIGHT/HEIGHT+.5);
	}
	
	v1 = (startPoint.x-point.m_PointList[dStart].x)*(startPoint.x-point.m_PointList[dStart].x)
		+(startPoint.y-point.m_PointList[dStart].y)*(startPoint.y-point.m_PointList[dStart].y);

	l = point.m_Size - 1; // last Index -1
	v2 = (startPoint.x-point.m_PointList[l].x)*(startPoint.x-point.m_PointList[l].x)
		+(startPoint.y-point.m_PointList[l].y)*(startPoint.y-point.m_PointList[l].y);

	// -_-
	if( v1 > v2 )
	{
		int dEnd = point.m_Size;
		point.PointReverse(dStart, dEnd);
	}*/

	BOOL reverse = FALSE;
	CPoint start1, end1;
	int s;
	if( subIndex == 0 )
	{ 
		start1.x = (int)(vector->m_Point[strokeIndex][X1][0]*(double)A_HEIGHT/HEIGHT+.5);
		start1.y = (int)(vector->m_Point[strokeIndex][Y1][0]*(double)A_HEIGHT/HEIGHT+.5);
		
		if( vector->m_DevSize[strokeIndex] == 0 ) s = vector->m_Size[strokeIndex]-1;
		else s = vector->m_DevIndex[strokeIndex][0];

		end1.x = (int)(vector->m_Point[strokeIndex][X1][s]*(double)A_HEIGHT/HEIGHT+.5);
		end1.y = (int)(vector->m_Point[strokeIndex][Y1][s]*(double)A_HEIGHT/HEIGHT+.5);

	}
	else // �и� ȹ 
	{
		s = vector->m_DevIndex[strokeIndex][subIndex-1];
		start1.x = (int)(vector->m_Point[strokeIndex][X1][s]*(double)A_HEIGHT/HEIGHT+.5);
		start1.y = (int)(vector->m_Point[strokeIndex][Y1][s]*(double)A_HEIGHT/HEIGHT+.5);

		if( vector->m_DevSize[strokeIndex] == subIndex ) s = vector->m_Size[strokeIndex]-1;
		else s = vector->m_DevIndex[strokeIndex][subIndex];

		end1.x = (int)(vector->m_Point[strokeIndex][X1][s]*(double)A_HEIGHT/HEIGHT+.5);
		end1.y = (int)(vector->m_Point[strokeIndex][Y1][s]*(double)A_HEIGHT/HEIGHT+.5);
	}

	
	if( lineDirection == 0 || lineDirection == 2) // ����ȹ 
	{
		if( start1.x > end1.x ) reverse = TRUE;
	}else if( lineDirection == 1 ) // ���� 
	{
		if( start1.y > end1.y ) reverse = TRUE;
	}else if( lineDirection == 2 ) // 
	{
		if( start1.x > end1.x ) reverse = TRUE;
	}else // / 
	{
		if( start1.y < end1.y ) reverse = TRUE;
	}

	if( reverse )
	{
		int dEnd = point.m_Size;
		point.PointReverse(dStart, dEnd);
	}
}

int CControlPoint::GetContourGroupNum(int TYPE)
{
	int i, k;

	// ��ȹ���� ��Ʈ�� ����Ʈ�� contour group �� ����
	int groupNum;
	short* groups = new short[10];	// ȹ�� ���� contour groups
	int* counts = new int[10];		// ȹ�� �� contour group�� ����

	// �ʱ�ȭ
	groupNum = 1;
	for ( i = 0; i < 10; i++)
	{
		groups[i] = -1;
		counts[i] = 0;
	}
		
	groups[0] = m_PointList[0].cgroup;	// initial group 	

	BOOL isMatched;
	for( k = 0; k < m_Size; k++)
	{
		isMatched = FALSE;
		for ( i = 0; i < groupNum; i++)
		{
			if (groups[i] == m_PointList[k].cgroup)	
			{
				counts[i]++;
				isMatched = TRUE;
			}			
		}
		if (!isMatched)
		{
			groups[groupNum] = m_PointList[k].cgroup;
			counts[groupNum] = 1;
			groupNum++;
		}
	}

	/////////////////////////////////////////////////////////////////
	if (TYPE == 0 ) // ��ó�� �ڵ����� ȹ ã�Ƴ� ��
	{
		// main contour group ã��
		int mainGroup = -1;
		for ( i = 0; i < groupNum; i++)
		{
			if (counts[mainGroup] <= counts[i])
				mainGroup = i;
		}

		// outlier ����
		int pointNum = m_Size;
		int removedGroupNum = 0;
		for ( i = 0; i < groupNum; i++)
		{
			if ( (i == mainGroup) || (counts[mainGroup] < 3) ) continue;

			if ( (counts[i] == 1) || (((double)counts[i] / pointNum) < 0.2)  )
			{
				for( k = 0; k < pointNum; k++)
				{
					if (groups[i] == m_PointList[k].cgroup)
					{
						DeletePoint(k);
						k--;
						pointNum--;
					}
				}
				removedGroupNum++;
			}		
		}

		groupNum = groupNum - removedGroupNum;
	}
	///////////////////////////////////////////////////////////////

	delete[] groups;
	delete[] counts;

	return groupNum;
}

void CControlPoint::SortX(double **array, int size)
{
	int i,j;
	double x,y;
	double index;
	for( i = 1; i < size; i++)
	{
		for( j = i; j > 0; j--)
		{
			if( array[j-1][0] > array[j][0])
			{
				// swapping
				x = array[j][0];
				y = array[j][1];
				index = array[j][2];

				array[j][0] = array[j-1][0];
				array[j][1] = array[j-1][1];
				array[j][2] = array[j-1][2];

				array[j-1][0] = x;
				array[j-1][1] = y;
				array[j-1][2] = index;
			}
			else break;
		}
	}
}
void CControlPoint::SortY(double **array, int size)
{
	int i,j;
	double x,y;	
	double index;
	for( i = 1; i < size; i++)
	{
		for( j = i; j > 0; j--)
		{
			if( array[j-1][1] > array[j][1])
			{
				// swapping
				x = array[j][0];
				y = array[j][1];
				index = array[j][2];

				array[j][0] = array[j-1][0];
				array[j][1] = array[j-1][1];
				array[j][2] = array[j-1][2];

				array[j-1][0] = x;
				array[j-1][1] = y;
				array[j-1][2] = index;
			}
			else break;
		}
	}
}

void CControlPoint::SortX(int **array, int size)
{
	int i,j;
	int x,y;
	int index;
	for( i = 1; i < size; i++)
	{
		for( j = i; j > 0; j--)
		{
			if( array[j-1][0] > array[j][0])
			{
				// swapping
				x = array[j][0];
				y = array[j][1];
				index = array[j][2];

				array[j][0] = array[j-1][0];
				array[j][1] = array[j-1][1];
				array[j][2] = array[j-1][2];

				array[j-1][0] = x;
				array[j-1][1] = y;
				array[j-1][2] = index;

			}
			else break;
		}
	}
}
void CControlPoint::SortY(int **array, int size)
{
	int i,j;
	int x,y;	
	int index;
	for( i = 1; i < size; i++)
	{
		for( j = i; j > 0; j--)
		{
			if( array[j-1][1] > array[j][1])
			{
				// swapping
				x = array[j][0];
				y = array[j][1];
				index = array[j][2];

				array[j][0] = array[j-1][0];
				array[j][1] = array[j-1][1];
				array[j][2] = array[j-1][2];

				array[j-1][0] = x;
				array[j-1][1] = y;
				array[j-1][2] = index;
			}
			else break;
		}
	}
}

void CControlPoint::SortXplusY(int **array, int size)
{
	int i,j;
	int x,y;	
	int index;
	for( i = 1; i < size; i++)
	{
		for( j = i; j > 0; j--)
		{
			if( array[j-1][0]+array[j-1][1] > array[j][0]+array[j][1])
			{
				// swapping
				x = array[j][0];
				y = array[j][1];
				index = array[j][2];

				array[j][0] = array[j-1][0];
				array[j][1] = array[j-1][1];
				array[j][2] = array[j-1][2];

				array[j-1][0] = x;
				array[j-1][1] = y;
				array[j-1][2] = index;
			}
			else break;
		}
	}
}


//
// �� ���� ���� ���⸦ ������ 
int CControlPoint::GetLineDirection(CPoint &startPoint, CPoint &endlPoint)
{
	int deltaX, deltaY;

	deltaX = endlPoint.x - startPoint.x;
	deltaY = endlPoint.y - startPoint.y;

	if( deltaX == 0 ) return 1;
	if( deltaY == 0 ) return 0;

	double slope = deltaY/(double)deltaX;

	if( slope >= -0.5 && slope <= 0.5 ) return 0;
	else if( slope >= 0.5 && slope < 2 ) return 2;
	else if( slope >= 2 || slope <= -2) return 1;
	else if( slope > -2 && slope < -0.5 ) return 3;

	return 4; // it's error
}


BOOL CControlPoint::NearPoint(CPoint &point, int x, int y)
{
	if( point.x == x && point.y+1 == y ) return TRUE;
	if( point.x+1 == x && point.y == y ) return TRUE;
	if( point.x+1 == x && point.y+1 == y ) return TRUE;
	return FALSE;
}

/*
void CControlPoint::SwapPoint(CPoint &p1, CPoint &p2)
{

}
*/
void CControlPoint::SortXminusY(int **array, int size)
{
	int i,j;
	int x,y;	
	int index;
	for( i = 1; i < size; i++)
	{
		for( j = i; j > 0; j--)
		{
			if( array[j-1][0]+(100-array[j-1][1]) > array[j][0]+(100-array[j][1]))
			{
				// swapping
				x = array[j][0];
				y = array[j][1];
				index = array[j][2];

				array[j][0] = array[j-1][0];
				array[j][1] = array[j-1][1];
				array[j][2] = array[j-1][2];

				array[j-1][0] = x;
				array[j-1][1] = y;
				array[j-1][2] = index;
			}
			else break;
		}
	}
}

// ���� ����� ����� point ã�� 
void CControlPoint::FindUserPoint(CPointVector *vector, int stroke, int sub)
{
	int i;
	for( i = 0; i < m_Size; i++)
	{
		m_PointList[i].user_point = vector->FindUserPoint(stroke, sub,m_PointList[i].x, m_PointList[i].y);
	}
}

// �� grouping �ϱ� 
void CControlPoint::ImproveGrouping( CPointVector *vector, int stroke )
{
	// Line direction �� ���� 
	// if( m_LineDirection )
	MyPoint part1[128], part2[128], part3[128];
	int size1 ,size2, size3;
	size1 = size2 = size3 = 0; // ũ�� 

	int i, j;
	// 1. partition , 2 �κ����� �и� 
	for( i = 0; i < m_Size; i++)
	{
		j = m_PointList[i].user_point;
		if ( m_PointList[i].tag == 0 )
		{
			part3[size3] = m_PointList[i];
			size3++;
		}
		else if (Partition( i, vector->m_Point[stroke][X1][j], vector->m_Point[stroke][Y1][j]))
		{
			part1[size1] = m_PointList[i];
			size1++;
		}
		else
		{
			part2[size2] = m_PointList[i];
			size2++;
		}
	}

	MyPoint temp;

	// 2.1 ��ǥ�� sorting
	switch(m_LineDirection) {
	case 0 : // --
		for( i = 1; i < size1; i++ ) // x
		{
			for( j = i; j > 0; j--)
			{
				if( part1[j].x < part1[j-1].x ) // swap
				{
					temp = part1[j];
					part1[j] = part1[j-1];
					part1[j-1] = temp;
				}
				else break;
			}
		} // part1
		for( i = 1; i < size2; i++ ) // x
		{
			for( j = i; j > 0; j--)
			{
				if( part2[j].x < part2[j-1].x ) // swap
				{
					temp = part2[j];
					part2[j] = part2[j-1];
					part2[j-1] = temp;
				}
				else break;
			}
		} // part2
		break;
	case 1 : // |
		for( i = 1; i < size1; i++ ) // y
		{
			for( j = i; j > 0; j--)
			{
				if( part1[j].y < part1[j-1].y ) // swap
				{
					temp = part1[j];
					part1[j] = part1[j-1];
					part1[j-1] = temp;
				}
				else break;
			}
		} // part1
		for( i = 1; i < size2; i++ ) // y
		{
			for( j = i; j > 0; j--)
			{
				if( part2[j].y < part2[j-1].y ) // swap
				{
					temp = part2[j];
					part2[j] = part2[j-1];
					part2[j-1] = temp;
				}
				else break;
			}
		} // part2
		break;
	case 2 : //
		for( i = 1; i < size1; i++ ) // x+y
		{
			for( j = i; j > 0; j--)
			{
				if( part1[j].x+part1[j].y < part1[j-1].x+part1[j-1].y ) // swap
				{
					temp = part1[j];
					part1[j] = part1[j-1];
					part1[j-1] = temp;
				}
				else break;
			}
		} // part1
		for( i = 1; i < size2; i++ ) // x+y
		{
			for( j = i; j > 0; j--)
			{
				if( part2[j].x+part2[j].y < part2[j-1].x+part2[j-1].y ) // swap
				{
					temp = part2[j];
					part2[j] = part2[j-1];
					part2[j-1] = temp;
				}
				else break;
			}
		} // part2
		break;
	case 3 : // /
		for( i = 1; i < size1; i++ ) // x-y
		{
			for( j = i; j > 0; j--)
			{
				if( part1[j].x-part1[j].y < part1[j-1].x-part1[j-1].y ) // swap
				{
					temp = part1[j];
					part1[j] = part1[j-1];
					part1[j-1] = temp;
				}
				else break;
			}
		} // part1
		for( i = 1; i < size2; i++ ) // x-y
		{
			for( j = i; j > 0; j--)
			{
				if( part2[j].x-part2[j].y < part2[j-1].x-part2[j-1].y ) // swap
				{
					temp = part2[j];
					part2[j] = part2[j-1];
					part2[j-1] = temp;
				}
				else break;
			}
		} // part2
	
		break;
	default: break;
	}


	// 3. merge
	int limit = size1+size2;
	for( i = 0; i < size1; i++)
	{
		m_PointList[i] = part1[i];
	}
	for( i = 0; i < size2; i++)
	{
		m_PointList[i+size1] = part2[size2-i-1];
	}
	for( i = 0; i < size3; i++ )
	{
		m_PointList[i+limit] = part3[i];
	}

	// 4. tag 0 ó��
	int total = limit + size3;
	int close, value, position;
	int k; // indexing (for)
	for( i = limit; i < total; i++)
	{
		value = 10000;
		for( j = 0; j < i; j++)
		{
			if( abs(m_PointList[i].id-m_PointList[j].id) < value )
			{
				value = abs(m_PointList[i].id-m_PointList[j].id);
				close = j;
			}
		}
		
		if( close == 0 ) // ó�� 
		{
			if( m_PointList[i].id > m_PointList[close].id)
			{
				if( m_PointList[i].id > m_PointList[close+1].id) position = 0;
				else position = 1;
			}
			else
			{
				if( m_PointList[i].id < m_PointList[close+1].id) position = 0;
				else position = 1;
			}
		}
		else if(close == i-1) // �� 
		{
			if( m_PointList[i].id > m_PointList[close].id)
			{
				if( m_PointList[i].id > m_PointList[close-1].id) position = -1;
				else position = close;
			}
			else 
			{
				if( m_PointList[i].id < m_PointList[close-1].id) position = -1;
				else position = close;
			}
		}
		else // �߰��� ���� 
		{
			if( m_PointList[i].id > m_PointList[close].id) // ���� 1_1
			{
				if( m_PointList[i].id > m_PointList[close-1].id) // ���� 2_1
				{
					if( m_PointList[i].id > m_PointList[close+1].id ) // ���� ��� ���ɼ� �ִ°� ?
					{
						if( m_PointList[i].id-m_PointList[close+1].id < m_PointList[i].id - m_PointList[close-1].id)
							position = close;
						else position = close+1;
					}else position = close+1;
					//position = close+1;
				}
				else // ���� 2_2       m_PointList[i].id < m_PointList[close-1].id
				{
					if( m_PointList[i].id < m_PointList[close+1].id ) // ���� 3_1 ���� ��� ���ɼ� �ִ°� ?
					{
						if( m_PointList[close+1].id-m_PointList[i].id
							< m_PointList[close-1].id -m_PointList[i].id)
							position = close+1;
						else position = close;
					}
					else position = close;
				}
			}
			else // ���� 1_2         m_PointList[i].id < m_PointList[close-1].id
			{
				if( m_PointList[i].id > m_PointList[close-1].id) // 
				{
					if( m_PointList[i].id > m_PointList[close+1].id ) // ���� ��� ���ɼ� �ִ°� ?
					{
						if( m_PointList[i].id - m_PointList[close+1].id 
							 < m_PointList[i].id - m_PointList[close-1].id )
							position = close+1;
						else position = close;
					}else position = close;
				}
				else position = close+1;
			}
		}

		if( position != -1 )
		{
			temp = m_PointList[i];
			for( k = i; k > position; k-- )
			{
				m_PointList[k] = m_PointList[k-1];
			}
			m_PointList[position] = temp;
		}
	} // part1
}



BOOL CControlPoint::Partition(int index, int x, int y)
{
	int sx, sy; 
	sx = m_PointList[index].x;
	sy = m_PointList[index].y;
	
	switch(m_LineDirection) {
	case 0 : // ����
		if( sy > y ) return TRUE;
		else return FALSE;
		break;
	case 1 : //  ����  
		if( sx > x ) return TRUE;
		else return FALSE;
		break;
	case 2 : //  
		if( sx - sy > x - y ) return TRUE;
		else return FALSE;
		break;
	case 3 : //  /  
		if( sx + sy > x + y ) return TRUE;
		else return FALSE;
		break;
	default: break;
	}
	return TRUE;
}

// ��ǥ ������ ??
void CControlPoint::PointReverse(int start, int end)
{
	end = end - 2;
	int temp;
	while( start < end )
	{
		// swapping
		temp = m_PointList[start].x;
		m_PointList[start].x = m_PointList[end].x;
		m_PointList[end].x = temp;
		temp = m_PointList[start].y;
		m_PointList[start].y = m_PointList[end].y;
		m_PointList[end].y = temp;

		temp = m_PointList[start+1].x;
		m_PointList[start+1].x = m_PointList[end+1].x;
		m_PointList[end+1].x = temp;
		temp = m_PointList[start+1].y;
		m_PointList[start+1].y = m_PointList[end+1].y;
		m_PointList[end+1].y = temp;

		start += 2;
		end -=2;
	}
}
