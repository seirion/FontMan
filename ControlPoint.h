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
	short tag;		// contour �� ������ 1
	short cgroup;	// contour �׷� ��ȣ
	short user_point; // user point index
};

/*struct StrokeAxis
{
	double centerX;
	double centerY;

	double slope;		// ����
	double y_intercept;	// y ����
	double x_intercept;	// x ���� 
};

struct StrokeBox
{
	// ��ȯ�� ��ǥ������ min, max��
	double min[2];
	double max[2];

	// ���� ��ǥ������ bounding box
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
	void Sort(CFontManDoc *pDoc); // Connectivity�� �ٰ��ؼ� ���� 
	void Sort();	// id ������ ����
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
	MyPoint *m_PointList;	// �������� list
	int m_Size;				// ���� data�� ��
	int m_ArraySize;		// �Ҵ�� array�� ũ��
	int *m_Contour;
	int m_ContourNum;
	BOOL m_Direction;		// ȹ ���� 

	// 1ȹ ���� ���� ��� 
	CControlPoint *m_Link[4];
	int m_LinkNum;

	// �� ȹ�� boundary point ����Ʈ
	CControlPoint* m_BoundaryPoints;
	// �� ȹ�� Filling�� bitmap ����
	unsigned char m_StrokeBitmap[A_HEIGHT][A_WIDTH];
	
	// ȹ�� Axis����
//	StrokeAxis m_Axis[2];
//	StrokeBox	m_Box;

private:
	BOOL NearPoint( CPoint &point, int x, int y);
	void Shift( MyPoint* list, int i, int j);
	void Swap( MyPoint* list, int i, int j);
	void MakeNBTable();
	int** m_NBTable;	// Neighbor Map, �̿��ϴ� ����Ʈ ��
	void ArrayDouble();

	////////////////////////////////////////////////////////////////////////
	// �׸��� ����
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
