// PointVector.cpp: implementation of the PointVector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FontMan.h"
#include "PointVector.h"
#include "limits.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define POINT_SIZE	5

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPointVector::CPointVector()
{	
	m_StrokeArray = 32;
	m_Point = new short **[m_StrokeArray];
	m_Stroke = 0;

	m_Array = new short[m_StrokeArray];
	m_Size = new short[m_StrokeArray];

	m_brush = NULL;
	m_pen = NULL;
	m_DevSize = new int[m_StrokeArray];
	m_DevIndex = new int*[m_StrokeArray];
	int i;
	for( i = 0; i < m_StrokeArray; i++)
		m_DevIndex[i] = new int[4];
	
	for( i = 0; i < m_StrokeArray; i++)	m_DevSize[i] = 0;
		
}


CPointVector::~CPointVector()
{
	// memory deallocate
	Init();
}

void CPointVector::AddStroke()
{
	m_Array[m_Stroke] = 256;
	m_Point[m_Stroke] = new short*[2];

	m_Point[m_Stroke][X1] = new short[m_Array[m_Stroke]];
	m_Point[m_Stroke][Y1] = new short[m_Array[m_Stroke]];

	m_Size[m_Stroke] = 0;
	m_Stroke++;
}


void CPointVector::Add(short stroke, short x, short y )
{
	if( m_Stroke <= stroke ) return;

	if( m_Size[stroke] == m_Array[stroke] )
		SizeDouble(stroke);

	m_Point[stroke][X1][m_Size[stroke]] = x;
	m_Point[stroke][Y1][m_Size[stroke]] = y;

	m_Size[stroke]++;
}

void CPointVector::SizeDouble(short stroke)
{
	m_Array[stroke] = m_Array[stroke] * 2;
	short *arrayX = new short[m_Array[stroke]];
	short *arrayY = new short[m_Array[stroke]];

	for( int i = 0; i < m_Size[stroke]; i++)
	{
		arrayX[i] = m_Point[stroke][X1][i];
		arrayY[i] = m_Point[stroke][Y1][i];
	}

	delete [] m_Point[stroke][X1];
	delete [] m_Point[stroke][Y1];


	m_Point[stroke][X1] = arrayX;
	m_Point[stroke][Y1] = arrayY;

}

void CPointVector::Init()
{
	// memory deallocate
	for( int i = 0; i < m_Stroke; i++)
	{
		delete [] m_Point[i][X1];
		delete [] m_Point[i][Y1];

	}
	delete [] m_Point;

	delete m_Array;
	delete m_Size;
	delete [] m_DevSize;

	for( i = 0;i < m_StrokeArray; i++)
		delete [] m_DevIndex[i];
	delete m_DevIndex;

	// memory reallocate
	m_StrokeArray = 32;
	m_Point = new short **[m_StrokeArray];
	m_Stroke = 0;

	m_Array = new short[m_StrokeArray];
	m_Size = new short[m_StrokeArray];
	m_DevSize = new int[m_StrokeArray];

	m_DevIndex = new int*[m_StrokeArray];
	for( i = 0; i < m_StrokeArray; i++)
	{
		m_DevIndex[i] = new int[4];
		m_DevSize[i] = 0;
	}

	m_brush = NULL;
	m_pen = NULL;
}

int CPointVector::GetStrokeNum()
{ 
	return m_Stroke;
}

int CPointVector::GetStrokeSizeOf(int index)
{
	return m_Size[index];
}

int CPointVector::GetStrokeSizeOf(int index, int subIndex)
{
	if( m_DevSize[index] == 0 )return m_Size[index];
	if( m_DevSize[index] == subIndex )
	{
		return m_Size[index] - m_DevIndex[index][subIndex-1];
	}
	else
	{
		if( subIndex == 0 ) return m_DevIndex[index][0];
		else return m_DevIndex[index][subIndex] - m_DevIndex[index][subIndex-1];
	}
/*	if( subIndex == 0 ) return m_Size[index];
	else if( subIndex == 1 )
	//m_Size[index];

	return size;*/
}

//////////////////////////////////////////////////////////////////////////
// 그리기 

// 사용자의 획 그리기 
void CPointVector::Draw(CDC *pDC)
{
	int i, j;
	RECT rt;

	COLORREF color = RGB(0,0,0);

	SetPen(pDC, 2, color);
	SetBrush(pDC, color);
	
	for( i = 0; i < m_Stroke; i++)
	{
		for( j = 0; j < m_Size[i]; j++)
		{
			SetRect(&rt,m_Point[i][X1][j] - POINT_SIZE, m_Point[i][Y1][j] - POINT_SIZE,
				m_Point[i][X1][j] + POINT_SIZE, m_Point[i][Y1][j] + POINT_SIZE);
			pDC->Ellipse( &rt ); 

		}
	}

	// 
	SetPen(pDC, 2, RGB(255,255,0));
	SetBrush(pDC, RGB(255,255,0));
	
	int in;
	for( i = 0; i < m_Stroke; i++)
	{
		for( j = 0; j < m_DevSize[i]; j++)
		{
			in = m_DevIndex[i][j];
			SetRect(&rt,m_Point[i][X1][in] - POINT_SIZE, m_Point[i][Y1][in] - POINT_SIZE,
				m_Point[i][X1][in] + POINT_SIZE, m_Point[i][Y1][in] + POINT_SIZE);
			pDC->Ellipse( &rt ); 
		}
	}

	ReleasePen(pDC);
	ReleaseBrush(pDC);
}

void CPointVector::SetBrush(CDC *pDC, COLORREF color)
{
	if( m_brush != NULL) m_brush->DeleteObject();
	m_brush = new CBrush;
	m_brush->CreateSolidBrush(color);
	m_oldBrush = pDC->SelectObject(m_brush);
}

void CPointVector::SetPen(CDC *pDC, int width, COLORREF color)
{
	if( m_pen != NULL ) m_pen->DeleteObject();
	m_pen = new CPen;
	m_pen->CreatePen(PS_SOLID, width, color);
	m_oldPen = pDC->SelectObject(m_pen);
}

void CPointVector::ReleasePen(CDC *pDC)
{
	if( m_pen == NULL)	return;
	pDC->SelectObject(m_oldPen);
	delete m_pen;
	m_pen = NULL;
}

void CPointVector::ReleaseBrush(CDC *pDC)
{
	if( m_brush == NULL)return;
	pDC->SelectObject(m_oldBrush);
	delete m_brush;
	m_brush = NULL;
}

////////////////////////////////////////////////////////////////////////////



int CPointVector::GetSize(int index)
{
	return m_Size[index];
}

//
// dev sort
void CPointVector::DevSort( int stroke )
{
	int i, j, temp;
	
	for( i = 1; i < m_DevSize[stroke]; i++)
	{
		for( j = i; j > 0; j--)
		{
			if(m_DevIndex[stroke][j] < m_DevIndex[stroke][j-1])
			{// swap
				temp = m_DevIndex[stroke][j];
				m_DevIndex[stroke][j] = m_DevIndex[stroke][j-1];
				m_DevIndex[stroke][j-1] = temp;
			}else break;
		}
	}
}

int CPointVector::FindUserPoint(int stroke, int sub, int x, int y)
{
	int result = 0;
	int distance;
	int max = INT_MAX;
	int i;
	if( sub == 0 && m_DevSize[stroke] == 0)
	{
		for( i = 0; i < m_Size[stroke]; i++ )
		{
			distance = (x-m_Point[stroke][X1][i])*(x-m_Point[stroke][X1][i])
				+ (y-m_Point[stroke][Y1][i])*(y-m_Point[stroke][Y1][i]);
			if( max > distance )
			{
				max = distance;
				result = i;
			}
			
		}
	}
	else
	{
		int sIndex = 0; // 시작 index
		if( sub != 0 )sIndex = m_DevIndex[stroke][sub-1];
		int limit;
		
		if( m_DevSize[stroke] == sub) limit = m_Size[stroke]-1;
		else limit = m_DevIndex[stroke][sub];

		for( i = sIndex; i < limit; i++ )
		{
			distance = (x-m_Point[stroke][X1][i])*(x-m_Point[stroke][X1][i])
				+ (y-m_Point[stroke][Y1][i])*(y-m_Point[stroke][Y1][i]);
			if( max > distance )
			{
				max = distance;
				result = i;
			}
		}

	}

	return result;
}
