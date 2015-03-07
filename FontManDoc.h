// FontManDoc.h : interface of the CFontManDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FONTMANDOC_H__083023FB_5FD4_4D2D_AA2A_CFDCD5F84842__INCLUDED_)
#define AFX_FONTMANDOC_H__083023FB_5FD4_4D2D_AA2A_CFDCD5F84842__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
//
#include <ft2build.h>
#include FT_FREETYPE_H

#include "ControlPoint.h"
#include "PointVector.h"
#include "FontImageProcessing.h"
#include "MyEdit.h"
#include "FontAniDlg.h"
//////////////////////////////////////////////////////////////////////////
#define STROKE_READY 0
#define STROKE_START 1
#define STROKE_END 2

//////////////////////////////////////////////////////////////////////////

class CFontAniDlg;
class CControlPoint;
class CPointVector;
struct MyPoint;

class CFontManDoc : public CDocument
{
protected: // create from serialization only
	CFontManDoc();
	DECLARE_DYNCREATE(CFontManDoc)

// Attributes
public:
	BOOL m_FontLoad;
	//////////////////////////////////////////////////////////////////////////
	// TTF 관련 변수 
	short       m_pointNum;       /* number of points in the glyph      */
    FT_Vector*  m_point;          /* the outline's points               */
	char*       m_tag;            /* the points flags                   */
	short       m_contourNum;     /* number of contours in glyph        */
    short*      m_contour;        /* the contour end points             */
    int         m_flag;           /* outline masks                      */

	unsigned char m_image[HEIGHT][WIDTH];	/* bitmap image rendered from FreeType*/	  						  
	unsigned char m_thinned_image[HEIGHT][WIDTH];
	unsigned char m_boundary_image[HEIGHT][WIDTH];

	CControlPoint *m_SavePoint; // control point save
	CPointVector *m_Vector;		// 사용자가 입력한 획 저장 
	int m_StrokeOn;
	int m_CurStroke;
	BOOL m_ShowBefore;
	//////////////////////////////////////////////////////////////////////////
	// animation 
	CControlPoint *m_AniPoint;
	
	// skeleton을 branch point를 중심으로 분할한 결과
	short m_SkelSegmentNum;	// Skeleton Segment의 개수
	CControlPoint *m_SkeletonSegment;

	// 획 분할 결과: bezier curve의 control point 그룹핑 결과
	short m_StrokeNum;
	CControlPoint *m_ControlPointSegment;

	// boundary point들의 그룹핑 결과
//	CControlPoint *m_BoundarySegment;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontManDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	void StrokePointSorting();
	void MemoryAlloc();
	void Init();
	BOOL LoadFont(int code);
	
	// 저장 관련
	void SaveFAFile(CString path);
	void Draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y, CString &write);
	void MakeHanjaCode(int code, CString& write);

	//////////////////////////////////////////////////////////////////////////
	void GetLineDirection(int stroke, int sub);
	void StrokeImprovedGrouping();
	void PolygonApproximation(int start, int end);

	int GetEYData( int ey[], MyPoint* point, int num, BOOL direction);
	int GetEXData( int ex[], MyPoint* point, int num, BOOL direction);
	int GetSYData( int sy[], MyPoint* point, int num, BOOL direction);
	int GetSXData( int sx[], MyPoint* point, int num, BOOL direction);
	
	void SetCurrentEditStroke(int order);
	void SetStroke();
	void DrawStroke(CDC* pDC);
	void SetShowStroke();
	void SetControlPoint();
/*	
	
	
	BOOL LoadFreeTypeLibrary(char* );
	void LetterImageProcessing();*/

	int m_Busu;
	int m_SelectedNum;
	int m_Selected[256];

	BOOL m_ShowStroke;
	BOOL m_ShowPoint;	
	BOOL m_FileOpen;
	BOOL m_DrawFont;
	int  m_CurrentEditStroke; // 현재 에디트 중인 획 부분 
	int  m_CurrentEditSubStroke;
	int  m_SelectPoint;

//	CString m_FilePath;
	CFontAniDlg * m_FontAniDlg;

	virtual ~CFontManDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFontManDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

	void StrokeApproximateGrouping();
/*	void GetBranchEndPoints();
	void SkeletonSegmentation();	
	void ControlPointSegmentation();	
	void GetConnectedPath(CControlPoint* segStart, unsigned char image[][WIDTH], CControlPoint* path, int sid);
	short GetBranchNumberOf(int x, int y, unsigned char info_image[][WIDTH]);	*/
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTMANDOC_H__083023FB_5FD4_4D2D_AA2A_CFDCD5F84842__INCLUDED_)
