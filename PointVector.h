// PointVector.h: interface for the PointVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTVECTOR_H__EF5DEA7A_4D5A_4DD4_8ACE_77B94A594272__INCLUDED_)
#define AFX_POINTVECTOR_H__EF5DEA7A_4D5A_4DD4_8ACE_77B94A594272__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define X1 0
#define Y1 1
#define X2 2
#define Y2 3
#define X0 4
#define Y0 5


class CPointVector  
{
public:
	int FindUserPoint(int stroke, int sub, int x, int y);
	int GetStrokeSizeOf(int index, int subIndex);
	void DevSort( int stroke );
	int GetSize(int index);
	int GetStrokeSizeOf(int index);
	void SetPen(CDC *pDC, int width, COLORREF color);
	void SetBrush(CDC *pDC, COLORREF color);
	int GetStrokeNum();
	void Draw(CDC* pDC);
	void Init();
	void Add( short Stroke, short x, short y );
	void AddStroke();
	CPointVector();
	virtual ~CPointVector();

	short ***m_Point;		// ������ �κ�

	int *m_DevSize;
	int **m_DevIndex;


	void SizeDouble(short stroke );
	short m_StrokeArray;	// ȹ���� ��� ũ��
	short m_Stroke;			// ���� ȹ�� ����
	short *m_Array;			// �������� ��� ũ��
	short *m_Size;			// ���� ������ ũ��
	
private :
	////////////////////////////////////////////////////////////////////////
	// �׸��� ����
	CPen *m_pen, *m_oldPen;
	CBrush *m_brush, *m_oldBrush;
	void ReleaseBrush(CDC *pDC);
	void ReleasePen(CDC *pDC);
};

#endif // !defined(AFX_POINTVECTOR_H__EF5DEA7A_4D5A_4DD4_8ACE_77B94A594272__INCLUDED_)
