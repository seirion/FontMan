// ControlPoint.h: interface for the CControlPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLPOINT_H__1DABD542_0B2A_4992_8698_DEFFF4E2B706__INCLUDED_)
#define AFX_CONTROLPOINT_H__1DABD542_0B2A_4992_8698_DEFFF4E2B706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FontManDoc.h"

class CPointVector;
class CFontManDoc;
class CPointVector;
////////////////////////////////////////////////////////////////////////
// point type
#define LINE 1
#define CURVE 0
#define POINT_SIZE 2

struct MyPoint
{
	int id;			// control point id
	int x;	
	int y;
	short tag;		// contour 상에 있으면 1
	short cgroup;	// contour 그룹 번호
	short user_point; // user point index
};

/*struct StrokeAxis
{
	double centerX;
	double centerY;

	double slope;		// 기울기
	double y_intercept;	// y 절편
	double x_intercept;	// x 절편 
};

struct StrokeBox
{
	// 변환된 좌표에서의 min, max값
	double min[2];
	double max[2];

	// 원래 좌표에서의 bounding box
	int minmin[2];
	int minmax[2];
	int maxmin[2];
	int maxmax[2];
};*/

#define FOREGROUND 1
#define BACKGROUND 0

class CControlPoint
{
public:	
	void PointReverse(int start, int end);
	BOOL Partition(int index, int x, int y);
	void FindUserPoint(CPointVector *vector, int stroke, int sub);
	int m_LineDirection;
	void ImproveGrouping( CPointVector *vector, int stroke);
	void SortXminusY(int **array, int size);
	void SortXplusY(int **array, int size);
	int GetLineDirection( CPoint &startPoint, CPoint &endlPoint);
	void SortX(int** array, int size);
	void SortY(int** array, int size);
	void SortX(double** array, int size);
	void SortY(double** array, int size);

	int GetContourGroupNum(int TYPE);
	void GetStrokePrincipalAxes( CControlPoint &point, CPointVector *vector, int strokeIndex, int subIndex);
	void Sort(CFontManDoc *pDoc); // Connectivity에 근거해서 정렬 
	void Sort();	// id 순으로 정렬
	int FindRegion( CPoint start, CPoint end, int selected[] );
	void InsertPoint( int index, MyPoint point);
	BOOL DeletePoint(int index);
	void SetPointAt(int index, int x, int y, int tag);
	int IndexOf(int x, int y);
	int IndexOf( int id);
	MyPoint* GetPointPtr(int x, int y);
	BOOL ContainsPoint(int x, int y);
	int Add(int x, int y, int tag);
	int Add( CFontManDoc *pDoc );
	
	void Init();
	CControlPoint();
	CControlPoint( int size );
	virtual ~CControlPoint();

	void MakeStrokeBitmap(CDC* dc);
	void DrawStrokeBitmap(CDC *pDC);
	int Find(CPoint point);
	void DrawControlPoints(CDC *pDC);
	void Draw( CDC *pDC );
	void DrawAll(CDC *pDC, CFontManDoc *pDoc);
	void SetPen( CDC *pDC, int width, COLORREF color);
	void SetBrush(CDC *pDC, COLORREF color);

	///////////////////////////////////////////////////////////////////////
	// data
	MyPoint *m_PointList;	// 데이터의 list
	int m_Size;				// 실제 data의 수
	int m_ArraySize;		// 할당된 array의 크기
	int *m_Contour;
	int m_ContourNum;
	BOOL m_Direction;		// 획 방향 

	// 1획 분할 사용시 사용 
	CControlPoint *m_Link[4];
	int m_LinkNum;

	// 각 획별 boundary point 리스트
	CControlPoint* m_BoundaryPoints;
	// 각 획별 Filling된 bitmap 정보
	unsigned char m_StrokeBitmap[A_HEIGHT][A_WIDTH];
	
	// 획의 Axis정보
//	StrokeAxis m_Axis[2];
//	StrokeBox	m_Box;

private:
	BOOL NearPoint( CPoint &point, int x, int y);
	void Shift( MyPoint* list, int i, int j);
	void Swap( MyPoint* list, int i, int j);
	void MakeNBTable();
	int** m_NBTable;	// Neighbor Map, 이웃하는 포인트 맵
	void ArrayDouble();

	////////////////////////////////////////////////////////////////////////
	// 그리기 관련
	CPen *m_pen, *m_oldPen;
	CBrush *m_brush, *m_oldBrush;

	void ReleaseBrush(CDC *pDC);
	void ReleasePen(CDC *pDC);

	double Power( double n, int k);
	int Factorial(int n);
	void MakeBezierCurve(CDC *pDC, int start, int end, double u);
	void MakeBezierCurve2(CDC *pDC, int start, int end,int last, double u);

};

#endif // !defined(AFX_CONTROLPOINT_H__1DABD542_0B2A_4992_8698_DEFFF4E2B706__INCLUDED_)
