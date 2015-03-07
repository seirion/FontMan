                              // FontManDoc.cpp : implementation of the CFontManDoc class
//

#include "stdafx.h"
#include "FontMan.h"

#include "FontManDoc.h"
#include "FontManView.h"
#include "MainFrm.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontManDoc

IMPLEMENT_DYNCREATE(CFontManDoc, CDocument)

BEGIN_MESSAGE_MAP(CFontManDoc, CDocument)
	//{{AFX_MSG_MAP(CFontManDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontManDoc construction/destruction

//
// 변수 초기화
CFontManDoc::CFontManDoc()
{
	int i,j;
	m_FontAniDlg = NULL;

	m_FontLoad = FALSE;
	m_pointNum = 0;
    m_point = NULL;
	m_tag = NULL;
	m_contourNum = 0;    
    m_contour = NULL;        
    m_flag = 0;           

	for( i = 0; i < HEIGHT; i++ )
		for( j = 0; j < WIDTH; j++ )
		{		
			m_image[i][j];		  						  
			m_thinned_image[i][j];
			m_boundary_image[i][j];
		}

	m_SavePoint = NULL; 
	m_Vector = NULL;

	m_StrokeOn = STROKE_END;
	m_CurStroke = -1;
	
	m_AniPoint = NULL;
	
	m_SkelSegmentNum = 0;
	m_SkeletonSegment = NULL;

	m_StrokeNum = 0;
	m_ControlPointSegment = NULL;

	m_Busu = 0;
	m_SelectedNum = 0;

	m_ShowStroke = TRUE;
	m_ShowPoint = TRUE;	
	m_DrawFont = TRUE;
	m_ShowBefore = FALSE;
	
	m_CurrentEditStroke = 0; 
	m_CurrentEditSubStroke = 0;
	m_SelectPoint = -1;

	m_ControlPointSegment = new CControlPoint[100];

}

CFontManDoc::~CFontManDoc()
{
	
	Init();
//	delete [] m_ControlPointSegment;
}

BOOL CFontManDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

/*	int i,j;

	if(m_FontAniDlg ) delete m_FontAniDlg;
	m_FontAniDlg = NULL;

	m_pointNum = 0;

	if( m_point ) delete []m_point;
    m_point = NULL;
	if( m_tag ) delete []m_tag;
	m_tag = NULL;
	m_contourNum = 0;    
    if(m_contour) delete []m_contour;
	m_contour = NULL;        
    m_flag = 0;           

	for( i = 0; i < HEIGHT; i++ )
		for( j = 0; j < WIDTH; j++ )
		{		
			m_image[i][j];		  						  
			m_thinned_image[i][j];
			m_boundary_image[i][j];
		}

	if( m_SavePoint ) delete m_SavePoint;
	m_SavePoint = NULL; 
	if( m_Vector ) delete m_Vector;
	m_Vector = NULL;
	m_StrokeOn = STROKE_END;
	m_CurStroke = -1;
	
	if(m_AniPoint ) delete []m_AniPoint;
	m_AniPoint = NULL;
	
	m_SkelSegmentNum = 0;
	if( m_SkeletonSegment) delete m_SkeletonSegment;
	m_SkeletonSegment = NULL;

	
	for( i = 0; i < m_StrokeNum; i++ ) 
		m_ControlPointSegment[i].Init();
	m_StrokeNum = 0;

	m_Busu = 0;
	m_SelectedNum = 0;

	m_ShowStroke = TRUE;
	m_ShowPoint = TRUE;	
	m_DrawFont = TRUE;
	
	m_CurrentEditStroke = 0; 
	m_CurrentEditSubStroke = 0;
	m_SelectPoint = -1;

	MemoryAlloc();
*/
	Init();	//Sang Ok Koo
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFontManDoc serialization

void CFontManDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFontManDoc diagnostics

#ifdef _DEBUG
void CFontManDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFontManDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFontManDoc commands

//////////////////////////////////////////////////////////////////////////
// 파일에 쓰기 관련 
void CFontManDoc::SaveFAFile(CString path)
{
	CStdioFile file;
	if( !file.Open( path, CFile::modeCreate | CFile::modeWrite ) ) 
	{
	   #ifdef _DEBUG
		  afxDump << "Unable to open file" << "\n";
	   #endif
	   exit( 1 );
	}
	CString write, sub;

	CFontManView *pView = (CFontManView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();

	//
	// 한자내용정보
	// 음,훈,풀이

	//코드
	pView->m_Code.GetWindowText(sub);
	write.Format(_T("%x\n"), atoi(sub.GetBuffer(16)));
	file.WriteString(write);
	//한자	
	pView->m_CtrlInput.GetWindowText(write);
	file.WriteString(write + _T("\n"));
	// 음
	pView->m_Sound.GetWindowText(write);
	file.WriteString(write + _T("\n"));
	// 훈 
	pView->m_Meaning.GetWindowText(write);
	file.WriteString(write + _T("\n"));
	// 풀이
	pView->m_Contents.GetWindowText(write);
	file.WriteString(write + _T("\n"));
	// 부수
	write.Format( _T("%d\n"), m_Busu );
	file.WriteString(write);

	//
	// 1 총 획수
	// 2 점(point) 수 []
	// 3 총 int 수 
	// 4 획당 int 수 []
	// 5 point 4 개 []

	// 전체 획수 
	int stroke = m_Vector->GetStrokeNum();
	write.Format(_T("%d"), stroke); 
	file.WriteString(write + _T("\n"));

	int sx[32][1024]; // start X
	int sy[32][1024];
	int ex[32][1024]; 
	int ey[32][1024]; // end Y

	int total_point = 0;	// 총 point 개수
	int int_per_stroke[4][128]; // 획당 int 수 
	int point_per_stroke[64]; // 획당 point 개수 
	int i;
	MyPoint *pointList;
	for( i = 0; i < stroke; i++)
	{
		point_per_stroke[i] = m_AniPoint[i].m_Size/2;
		pointList = m_AniPoint[i].m_PointList;
		int_per_stroke[0][i] = GetSXData(sx[i], pointList, point_per_stroke[i], m_AniPoint[i].m_Direction );
		int_per_stroke[1][i] = GetSYData(sy[i], pointList, point_per_stroke[i], m_AniPoint[i].m_Direction );
		int_per_stroke[2][i] = GetEXData(ex[i], pointList, point_per_stroke[i], m_AniPoint[i].m_Direction );
		int_per_stroke[3][i] = GetEYData(ey[i], pointList, point_per_stroke[i], m_AniPoint[i].m_Direction );
	}

	// 2 획당 point 수 ,total_point(전체 포인트 수)
	write.Empty();
	sub.Empty();
	
	for( i = 0; i < stroke; i++)
	{
		if(!i) sub.Format(_T("%d"), point_per_stroke[i]);
		else sub.Format(_T(",%d"), point_per_stroke[i]);
		write += sub;
	}
	write += _T("\n");
	file.WriteString(write);

	// 3 획당 int 수 * 4가지 (SX, SY, EX, EY)
	write.Empty();
	sub.Empty();
	
	for( i = 0; i < stroke; i++)// SX
	{
		if(!i) sub.Format(_T("%d"), int_per_stroke[0][i]);
		else sub.Format(_T(",%d"), int_per_stroke[0][i]);
		write += sub;
	}
	write += _T("\n");
	file.WriteString(write);


	write.Empty();
	sub.Empty();
	
	for( i = 0; i < stroke; i++)// SY
	{
		if(!i) sub.Format(_T("%d"), int_per_stroke[1][i]);
		else sub.Format(_T(",%d"), int_per_stroke[1][i]);
		write += sub;
	}
	write += _T("\n");
	file.WriteString(write);


	write.Empty();
	sub.Empty();
	
	for( i = 0; i < stroke; i++)// EX
	{
		if(!i) sub.Format(_T("%d"), int_per_stroke[2][i]);
		else sub.Format(_T(",%d"), int_per_stroke[2][i]);
		write += sub;
	}
	write += _T("\n");
	file.WriteString(write);


	write.Empty();
	sub.Empty();
	
	for( i = 0; i < stroke; i++)// EY
	{
		if(!i) sub.Format(_T("%d"), int_per_stroke[3][i]);
		else sub.Format(_T(",%d"), int_per_stroke[3][i]);
		write += sub;
	}
	write += _T("\n");
	file.WriteString(write);
	

	// 4 획 내용 (SX SY EX EY)
	write.Empty();
	sub.Empty();
	for( i = 0; i < stroke; i++) // SX
	{
		for( int j = 0; j < int_per_stroke[0][i]; j++)
		{
			if( !j) sub.Format( _T("%d"), sx[i][j]);
			else sub.Format( _T(",%d"), sx[i][j]);
			write += sub;
		}
		write += _T("\n");
	}
	file.WriteString(write);

	write.Empty();
	sub.Empty();
	for( i = 0; i < stroke; i++) // SY
	{
		for( int j = 0; j < int_per_stroke[1][i]; j++)
		{
			if( !j) sub.Format( _T("%d"), sy[i][j]);
			else sub.Format( _T(",%d"), sy[i][j]);
			write += sub;
		}
		write += _T("\n");
	}
	file.WriteString(write);

	write.Empty();
	sub.Empty();
	for( i = 0; i < stroke; i++) // EX
	{
		for( int j = 0; j < int_per_stroke[2][i]; j++)
		{
			if( !j) sub.Format( _T("%d"), ex[i][j]);
			else sub.Format( _T(",%d"), ex[i][j]);
			write += sub;
		}
		write += _T("\n");
	}
	file.WriteString(write);

	write.Empty();
	sub.Empty();
	for( i = 0; i < stroke; i++) // EY
	{
		for( int j = 0; j < int_per_stroke[3][i]; j++)
		{
			if( !j) sub.Format( _T("%d"), ey[i][j]);
			else sub.Format( _T(",%d"), ey[i][j]);
			write += sub;
		}
		write += _T("\n");
	}
	file.WriteString(write);

	// 나머지 한자 (내용에 들어가는 한자)
	CString strHanja;
	pView->m_CtrlHanja.GetWindowText(strHanja);
	strHanja.TrimLeft();
	strHanja.TrimRight();
	int length = strHanja.GetLength();
	write.Format(_T("%d\n"), length/2); // 1. 개수 
	file.WriteString(write);

	unsigned short *wcsCode = new unsigned short[32];

	if( length > 0 )
	{
		MultiByteToWideChar(
				CP_ACP,         // code page
				MB_PRECOMPOSED,         // character-type options
				strHanja.GetBuffer(32),		// address of string to map
				length,      // number of bytes in string
				wcsCode,  // address of wide-character buffer
				32        // size of buffer
			);
	}

	length = length/2;
	for( i = 0; i < length; i++) // 2. 비트맵
	{
		write.Empty();
		MakeHanjaCode( wcsCode[i], write );
		file.WriteString(write);
	}
	
	delete [] wcsCode;
	file.Close();
}

void CFontManDoc::Draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y, CString &write)
{
	FT_Int  i, j, p, q;

	BYTE array[18][18];
	unsigned int chi[8];
	
	//초기화
	for ( i = 0; i < 18; i++)
		for ( j = 0; j < 18; j++)
			array[i][j] = 0;

	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;

	for ( i = x, p = 0; i < x_max; i++, p++ )
	{
		for ( j = y, q = 0; j < y_max; j++, q++ )
		{
		  if ( i >= 18 || j >= 18 )
			continue;

		  array[j][i] = bitmap->buffer[q * bitmap->width + p];
		}
	}
	
	// convert_to_binary
	for(i=0;i<18;i++)
	{
		for(j=0;j<18;j++)
		{
			if (array[i][j]>127) 
				array[i][j] = 1;//FOREGROUND;
			else
				array[i][j] = 0;//BACKGROUND;
		}
	}

	for( i = 0; i < 8; i++)chi[i] = 0;

	int indexing;
	for(i = 0; i < 16; i++)
	{
		for( j = 0; j < 16; j++)
		{
			if( array[i+1][j+1] )
			{
				if( i % 2 == 1) indexing = 0;
				else indexing = 16;
				chi[i/2] += 1 << (indexing+j);
			}
		}
	}

	CString sub;
	sub.Format( _T("%d %d %d %d %d %d %d %d\n"), 
		chi[0], chi[1],chi[2],chi[3],chi[4],chi[5],chi[6],chi[7]);
	write += sub;
}

//
// tiker 에 들어갈 한자 코드 쓰기 
void CFontManDoc::MakeHanjaCode(int code, CString &write)
{

	FT_Library    library;
	FT_Face       face;
	FT_Int error;
	
	FT_GlyphSlot  slot;
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Matrix     matrix;                 /* transformation matrix */
	double angle  = 0.0;

	// initiallize freetype library
	error = FT_Init_FreeType( &library );
	if (error)
	{
		AfxMessageBox(_T("an error occurred during library initiallization...\n"));	return;
	}	
	// load a font face
	char fontPath[256];

	GetWindowsDirectory( fontPath, 256 );

	CString path;
	path.Format( "%s", fontPath );
	path += "\\Fonts\\gulim.ttc";
	
	error = FT_New_Face( library, path, 0, &face );
	
	if (error == FT_Err_Unknown_File_Format)
	{
		AfxMessageBox(_T("the font format is unsupported...\n"));	return;
	}
	else if (error)
	{
		AfxMessageBox(_T("the font file could not opened or read...\n"));	return;
	}
	
	// 픽셀의 사이즈 
	error = FT_Set_Pixel_Sizes(
			 face,			 /* handle to face object */
			 18,			/* pixel_width           */
			 18);			/* pixel_height          */ 

	if (error) AfxMessageBox(_T("pixel size setting 실패..."));

	slot = face->glyph;

	/* set up matrix */
	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	/* the pen position in 26.6 cartesian space coordinates; */
	pen.x = 0 * 64;		//왼쪽 여백
	pen.y = (int)(18.0/10*1.5) * 64;		//아래 여백 여유

	FT_Set_Transform( face, &matrix, &pen );

	/* load glyph image into the slot (erase previous one) */
	error = FT_Load_Char( face, code, FT_LOAD_RENDER );
	if ( error ) AfxMessageBox(_T("character 로딩 실패..."));

	/* now, draw to our target surface (convert position) */
	Draw_bitmap( &slot->bitmap,
				 slot->bitmap_left,
				 18 - slot->bitmap_top, write );

	FT_Done_FreeType( library ); 
}


//////////////////////////////////////////////////////////////////////////

void CFontManDoc::GetLineDirection(int stroke, int sub)
{
	int deltaX, deltaY;

	/*deltaX = endlPoint.x - startPoint.x;
	deltaY = endlPoint.y - startPoint.y;*/
	if( sub == 0)
	{
		if( m_Vector->m_DevSize[stroke] == 0 )
		{
			deltaX = m_Vector->m_Point[stroke][X1][m_Vector->m_Size[stroke]-1]
			- m_Vector->m_Point[stroke][X1][0];

			deltaY = m_Vector->m_Point[stroke][Y1][m_Vector->m_Size[stroke]-1]
			- m_Vector->m_Point[stroke][Y1][0];
		}else
		{
			deltaX = m_Vector->m_Point[stroke][X1][m_Vector->m_DevIndex[stroke][sub]]
				- m_Vector->m_Point[stroke][X1][0];
			deltaY = m_Vector->m_Point[stroke][Y1][m_Vector->m_DevIndex[stroke][sub]]
				- m_Vector->m_Point[stroke][Y1][0];
		}
	}else
	{
		if( m_Vector->m_DevSize[stroke] == sub )
		{
			deltaX = m_Vector->m_Point[stroke][X1][m_Vector->m_Size[stroke]-1]
				- m_Vector->m_Point[stroke][X1][m_Vector->m_DevIndex[stroke][sub-1]];
			deltaY = m_Vector->m_Point[stroke][Y1][m_Vector->m_Size[stroke]-1]
				- m_Vector->m_Point[stroke][Y1][m_Vector->m_DevIndex[stroke][sub-1]];
		}
		else
		{
			deltaX = m_Vector->m_Point[stroke][X1][m_Vector->m_DevIndex[stroke][sub]]
				- m_Vector->m_Point[stroke][X1][m_Vector->m_DevIndex[stroke][sub-1]];
			deltaY = m_Vector->m_Point[stroke][Y1][m_Vector->m_DevIndex[stroke][sub]]
				- m_Vector->m_Point[stroke][Y1][m_Vector->m_DevIndex[stroke][sub-1]];
		}
	}

	int result;

	if( deltaX == 0 ) result = 1;
	else if( deltaY == 0 ) result = 0;
	else 
	{
	
		double slope = deltaY/(double)deltaX;

		if( slope >= -0.5 && slope <= 0.5 ) result = 0;
		else if( slope >= 0.5 && slope < 2 ) result = 2;
		else if( slope >= 2 || slope <= -2) result = 1;
		else if( slope > -2 && slope < -0.5 ) result = 3;
	}
	

	if( sub == 0) m_ControlPointSegment[stroke].m_LineDirection = result;
	else m_ControlPointSegment[stroke].m_Link[sub-1]->m_LineDirection = result;
	
}


// by hyun gyu
// improved grouping
void CFontManDoc::StrokeImprovedGrouping()
{
	int i, j;
	// 가까운 user stroke 찾기
	// direction && 획 별 , sub 획 별 
	for( i = 0; i < m_Vector->m_Stroke; i++)
	{
		GetLineDirection(i,0);
		m_ControlPointSegment[i].FindUserPoint(m_Vector, i, 0);
		m_ControlPointSegment[i].ImproveGrouping( m_Vector, i);
		if( m_ControlPointSegment[i].m_LinkNum > 0 )
		{
			for( j = 0 ;j < m_ControlPointSegment[i].m_LinkNum; j++)
			{
				GetLineDirection(i,j+1);
				m_ControlPointSegment[i].m_Link[j]->FindUserPoint(m_Vector, i, j+1);
				m_ControlPointSegment[i].m_Link[j]->ImproveGrouping(m_Vector, i);
			}
		}
	}

}

//
// polygon approximation 
// 자동 획 분할 '' 
void CFontManDoc::PolygonApproximation(int start, int end )
{
	int a, b, c; // coefficient (line equation)
	int x1, x2, y1, y2;

	x1 = m_Vector->m_Point[m_CurStroke][X1][start];
	y1 = m_Vector->m_Point[m_CurStroke][Y1][start];
	x2 = m_Vector->m_Point[m_CurStroke][X1][end];
	y2 = m_Vector->m_Point[m_CurStroke][Y1][end];

	a = (y2-y1);
	b = (x1-x2);
	c = -(a*x1 + b*y1);

	int maxIndex = start; // index of max value of distance
	const double RT = sqrt(a*a + b*b);
	int maxValue = (int)(abs(a*x1+b*y1+c)/RT+.5);
	int value;
	for( int i = start+1; i < end; i++)
	{
		x1 = m_Vector->m_Point[m_CurStroke][X1][i];
		y1 = m_Vector->m_Point[m_CurStroke][Y1][i];
		value = (int)(abs(a*x1+b*y1+c)/RT+.5);

		if( value > maxValue)
		{
			maxValue = value;
			maxIndex = i;
		}
	}

	if( maxValue > THRESHOLD )
	{
		if(m_Vector->m_DevSize[m_Vector->m_Stroke] == 4 )
		{
			AfxMessageBox(_T("something error... PolygonApproximation()"));
		}
		m_Vector->m_DevIndex[m_CurStroke][m_Vector->m_DevSize[m_CurStroke]] = maxIndex;
		m_Vector->m_DevSize[m_CurStroke]++;

		PolygonApproximation( start, maxIndex-1 );
		PolygonApproximation( maxIndex, end );
	}
}

int CFontManDoc::GetSXData(int data[], MyPoint *point, int num, BOOL direction)
{
	int total = 0; // return value(총 int 수)
	int i, currentBit, temp;
	int currentData;

	if(direction){//*************************************************************
	for( i = 0; i < num; i++)
	{
		if( !i) // 첫번째
		{
			data[0] = point[i*2].x;
			total = 1;
			currentBit = 0;
			currentData = 0;
		}
		else
		{
			if(point[i*2-2].x == point[i*2].x) // 0
			{
				currentBit++;
			}else if(point[i*2-2].x == point[i*2].x-1) // +1
			{
				temp = 1 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else if(point[i*2-2].x == point[i*2].x+1) // -1
			{
				temp = 2 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else // discontinue
			{
				temp = 3 << (currentBit*2);
				currentData |= temp;
				currentBit = 0;
				data[total] = currentData;
				currentData = 0;
				total++;
				data[total] = point[i*2].x;
				total++;
			}
		}

		if( currentBit == 15 ) // 비트가 full 인지 검사
		{
			data[total] = currentData;
			total++;
			currentBit = 0;
			currentData = 0;
		}
	}}//*************************************************************


	else{//*************************************************************
	for( i = num-1; i >= 0; i--)
	{
		if( i == num-1) // 첫번째
		{
			data[0] = point[i*2].x;
			total = 1;
			currentBit = 0;
			currentData = 0;
		}
		else
		{
			if(point[i*2].x == point[i*2+2].x)
			{
				currentBit++;
			}else if(point[i*2+2].x == point[i*2].x-1) // +1
			{
				temp = 1 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else if(point[i*2+2].x == point[i*2].x+1) // -1
			{
				temp = 2 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else
			{
				temp = 3 << (currentBit*2);
				currentData |= temp;
				currentBit = 0;
				data[total] = currentData;
				currentData = 0;
				total++;
				data[total] = point[i*2].x;
				total++;
			}
		}

		if( currentBit == 15 ) // 비트가 full 인지 검사
		{
			data[total] = currentData;
			total++;
			currentBit = 0;
			currentData = 0;
		}
	}}//*************************************************************
	// 자투리 
	if( currentBit != 15 && currentBit != 0)
	{
		data[total] = currentData;
		total++;
	}
	return total;
}

int CFontManDoc::GetSYData( int data[], MyPoint *point, int num, BOOL direction)
{
	int total = 0; // return value(총 int 수)
	int i, currentBit, temp;
	int currentData;

	if(direction){//*************************************************************
	for( i = 0; i < num; i++)
	{
		if( !i) // 첫번째
		{
			data[0] = point[i*2].y;
			total = 1;
			currentBit = 0;
			currentData = 0;
		}
		else
		{
			if(point[i*2-2].y == point[i*2].y)
			{
				currentBit++;
			}else if(point[i*2-2].y == point[i*2].y-1) // +1
			{
				temp = 1 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else if(point[i*2-2].y == point[i*2].y+1) // -1
			{
				temp = 2 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else
			{
				temp = 3 << (currentBit*2);
				currentData |= temp;
				currentBit = 0;
				data[total] = currentData;
				currentData = 0;
				total++;
				data[total] = point[i*2].y;
				total++;
			}
		}

		if( currentBit == 15 ) // 비트가 full 인지 검사
		{
			data[total] = currentData;
			total++;
			currentBit = 0;
			currentData = 0;
		}
	}}//*************************************************************


	else{//*************************************************************
	for( i = num-1; i >= 0; i--)
	{
		if( i == num-1) // 첫번째
		{
			data[0] = point[i*2].y;
			total = 1;
			currentBit = 0;
			currentData = 0;
		}
		else
		{
			if(point[i*2].y == point[i*2+2].y)
			{
				currentBit++;
			}else if(point[i*2+2].y == point[i*2].y-1) // +1
			{
				temp = 1 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else if(point[i*2+2].y == point[i*2].y+1) // -1
			{
				temp = 2 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else
			{
				temp = 3 << (currentBit*2);
				currentData |= temp;
				currentBit = 0;
				data[total] = currentData;
				currentData = 0;
				total++;
				data[total] = point[i*2].y;
				total++;
			}
		}

		if( currentBit == 15 ) // 비트가 full 인지 검사
		{
			data[total] = currentData;
			total++;
			currentBit = 0;
			currentData = 0;
		}
	}}//*************************************************************
	// 자투리 
	if( currentBit != 15 && currentBit != 0)
	{
		data[total] = currentData;
		total++;
	}
	return total;
}

int CFontManDoc::GetEXData( int data[], MyPoint *point, int num, BOOL direction)
{
	int total = 0; // return value(총 int 수)
	int i, currentBit, temp;
	int currentData;

	if(direction){//*************************************************************
	for( i = 0; i < num; i++)
	{
		if( !i) // 첫번째
		{
			data[0] = point[i*2+1].x;
			total = 1;
			currentBit = 0;
			currentData = 0;
		}
		else
		{
			if(point[i*2-1].x == point[i*2+1].x)
			{
				currentBit++;
			}else if(point[i*2-1].x == point[i*2+1].x-1) // +1
			{
				temp = 1 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else if(point[i*2-1].x == point[i*2+1].x+1) // -1
			{
				temp = 2 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else
			{
				temp = 3 << (currentBit*2);
				currentData |= temp;
				currentBit = 0;
				data[total] = currentData;
				currentData = 0;
				total++;
				data[total] = point[i*2+1].x;
				total++;
			}
		}

		if( currentBit == 15 ) // 비트가 full 인지 검사
		{
			data[total] = currentData;
			total++;
			currentBit = 0;
			currentData = 0;
		}
	}}//*************************************************************


	else{//*************************************************************
	for( i = num-1; i >= 0; i--)
	{
		if( i == num-1) // 첫번째
		{
			data[0] = point[i*2+1].x;
			total = 1;
			currentBit = 0;
			currentData = 0;
		}
		else
		{
			if(point[i*2+3].x == point[i*2+1].x)
			{
				currentBit++;
			}else if(point[i*2+3].x == point[i*2+1].x-1) // +1
			{
				temp = 1 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else if(point[i*2+3].x == point[i*2+1].x+1) // -1
			{
				temp = 2 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else
			{
				temp = 3 << (currentBit*2);
				currentData |= temp;
				currentBit = 0;
				data[total] = currentData;
				currentData = 0;
				total++;
				data[total] = point[i*2+1].x;
				total++;
			}
		}

		if( currentBit == 15 ) // 비트가 full 인지 검사
		{
			data[total] = currentData;
			total++;
			currentBit = 0;
			currentData = 0;
		}
	}}//*************************************************************

	// 자투리 
	if( currentBit != 15 && currentBit != 0)
	{
		data[total] = currentData;
		total++;
	}
	return total;
}

int CFontManDoc::GetEYData( int data[], MyPoint *point, int num, BOOL direction)
{
	int total = 0; // return value(총 int 수)
	int i, currentBit, temp;
	 int currentData;

	if(direction){//*************************************************************
	for( i = 0; i < num; i++)
	{
		if( !i) // 첫번째
		{
			data[0] = point[i*2+1].y;
			total = 1;
			currentBit = 0;
			currentData = 0;
		}
		else
		{
			if(point[i*2-1].y == point[i*2+1].y)
			{
				currentBit++;
			}else if(point[i*2-1].y == point[i*2+1].y-1) // +1
			{
				temp = 1 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else if(point[i*2-1].y == point[i*2+1].y+1) // -1
			{
				temp = 2 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else
			{
				temp = 3 << (currentBit*2);
				currentData |= temp;
				currentBit = 0;
				data[total] = currentData;
				currentData = 0;
				total++;
				data[total] = point[i*2+1].y;
				total++;
			}
		}

		if( currentBit == 15 ) // 비트가 full 인지 검사
		{
			data[total] = currentData;
			total++;
			currentBit = 0;
			currentData = 0;
		}
	}}//*************************************************************


	else{//*************************************************************
	for( i = num-1; i >= 0; i--)
	{
		if( i == num-1) // 첫번째
		{
			data[0] = point[i*2+1].y;
			total = 1;
			currentBit = 0;
			currentData = 0;
		}
		else
		{
			if(point[i*2+3].y == point[i*2+1].y)
			{
				currentBit++;
			}else if(point[i*2+3].y == point[i*2+1].y-1) // +1
			{
				temp = 1 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else if(point[i*2+3].y == point[i*2+1].y+1) // -1
			{
				temp = 2 << (currentBit*2);
				currentData |= temp;
				currentBit++;
			}else
			{
				temp = 3 << (currentBit*2);
				currentData |= temp;
				currentBit = 0;
				data[total] = currentData;
				currentData = 0;
				total++;
				data[total] = point[i*2+1].y;
				total++;
			}
		}

		if( currentBit == 15 ) // 비트가 full 인지 검사
		{
			data[total] = currentData;
			total++;
			currentBit = 0;
			currentData = 0;
		}
	}}//*************************************************************
	// 자투리 
	if( currentBit != 15 && currentBit != 0)
	{
		data[total] = currentData;
		total++;
	}
	return total;
}


void CFontManDoc::SetCurrentEditStroke(int order)
{
	m_CurrentEditStroke = order;
}

// user가 그린 획을 가지고, control point를 grouping한다. 
void CFontManDoc::SetStroke()
{
	if (m_Vector->GetStrokeNum() == 0)	return;
	//////////////////////////////////////////////////////////////
	
	// 대략적인 그룹핑
	StrokeApproximateGrouping();

	// 보다 정확한 그룹핑 작업
	StrokeImprovedGrouping();
	
	// Stroke Point Sorting - 한개의 contour 그룹일때만
	StrokePointSorting();
}


// 획의 대략적인 그룹핑 
// 방법 2. 단순 거리 계산으로 대략적인 그룹을 나눈다.
void CFontManDoc::StrokeApproximateGrouping()
{	
	// threshold도 입력으로 받도록 해야 되겠다.!!!!!!!
//	const int STROKE_POINT_THRESHOLD = 30;

	int i, j, k;
	int x, y, xc, yc, id, tag, cgroup;
	double dist;

	int sIndex; // start index
	CControlPoint *tmp;
	for( i = m_StrokeNum; i < m_Vector->GetStrokeNum(); i++)
	{
		//////////////////////////////////////////////////////////////
		for( int z = 0; z <= m_Vector->m_DevSize[i]; z++)
		{ /* for 추가 */
	
			tmp = new CControlPoint();

			if( z == 0 )sIndex = 0;
			else sIndex = m_Vector->m_DevIndex[i][z-1];

			tmp->m_ContourNum = 1;
			int tsize = m_Vector->GetStrokeSizeOf(i,z);

			for( j = 0; j < tsize; j++)
			{
				// 사용자가 그린 점 
				x = m_Vector->m_Point[i][X1][j+sIndex];
				y = m_Vector->m_Point[i][Y1][j+sIndex];
				
				// 모든 control point에 관해 검사 및 그룹핑
				for( k = 0; k < m_SavePoint->m_Size; k++)
				{
					xc = m_SavePoint->m_PointList[k].x;
					yc = m_SavePoint->m_PointList[k].y;
					
					// 단순 거리 계산
					dist = sqrt( pow( xc - x, 2) + pow( yc - y, 2) );
					
					if ( dist < STROKE_POINT_THRESHOLD )
					{
						if ( !tmp->ContainsPoint(xc, yc) )
						{
							tag = m_SavePoint->m_PointList[k].tag;
							id  = m_SavePoint->m_PointList[k].id;
							cgroup = m_SavePoint->m_PointList[k].cgroup;

							tmp->Add(xc, yc, tag);						
							tmp->GetPointPtr(xc, yc)->id = id;
							tmp->GetPointPtr(xc, yc)->cgroup = cgroup;
						}
					}
				}
			}
			
			if( z == 0)
			{
				m_ControlPointSegment[i] = *tmp;
				m_StrokeNum++;
//				delete tmp;
			}
			else
			{
				m_ControlPointSegment[i].m_Link[z-1] = tmp; // sub stroke //
				m_ControlPointSegment[i].m_LinkNum++;
			}
			//else m_ControlPointSegment[i]. = *tmp;
			
			
		}/* for 추가 */ // z 변수에 의한 (sub stroke)
	} // for (i)	
}


void CFontManDoc::DrawStroke(CDC *pDC)
{
	if( m_ShowStroke == FALSE ) return;
	m_Vector->Draw(pDC);
}

void CFontManDoc::SetShowStroke()
{
	if( m_ShowStroke == TRUE )	m_ShowStroke = FALSE;
	else m_ShowStroke = TRUE;
}

// 
// 컨트롤 포인트 설정 
void CFontManDoc::SetControlPoint()
{
	if( m_ShowPoint == TRUE ) m_ShowPoint = FALSE;
	else m_ShowPoint = TRUE;
}

//
// 폰트 로딩 
BOOL CFontManDoc::LoadFont(int code)
{
	if( m_FontLoad ) Init();
	
	BOOL result = TRUE;
	char fontPath[256];
	FT_Library library;
	FT_Face face;

	if ( FT_Init_FreeType( &library ) ) result = FALSE;
	GetWindowsDirectory( fontPath, 256 );
	CString path;
	path.Format( "%s", fontPath );
	 path += "\\Fonts\\gulim.ttc";
	// path += "\\Fonts\\UNI_HSR.TTF";
	//path += "\\Fonts\\arial.ttf";
	//path.Format("C:\\arial.ttf");

	if( FT_New_Face( library, path, 0, &face) ) return FALSE;
	
	if( FT_Set_Pixel_Sizes( face, WIDTH, HEIGHT) ) return FALSE;


	// transform
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Matrix     matrix;                 /* transformation matrix */
	double angle  = 0.0;	
	
	/* set up matrix */
	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	/* the pen position in 26.6 cartesian space coordinates; */
	pen.x = 20 * 64;		//왼쪽 여백
	pen.y = (int)( HEIGHT/10*1.2) * 64;		//아래 여백 여유
	FT_Set_Transform( face, &matrix, &pen );

	// load glyph image into the slot (erase previous one)
	if( FT_Load_Char( face, code, FT_LOAD_RENDER ) ) return FALSE;
	
	// 1. outline
	int i;
	m_pointNum = face->glyph->outline.n_points;
	m_point = new FT_Vector[m_pointNum];
	m_tag = new char[m_pointNum];
	for( i = 0; i < m_pointNum; i++)
	{
		m_point[i].x = face->glyph->outline.points[i].x/64;
		m_point[i].y = HEIGHT - face->glyph->outline.points[i].y/64;
		m_tag[i] = (face->glyph->outline.tags[i]%2 ? 1 : 0 );
	}

	m_contourNum = face->glyph->outline.n_contours;
	m_contour = new short[m_contourNum];
	for( i = 0; i < m_contourNum; i++)
		m_contour[i] = face->glyph->outline.contours[i];

	m_flag = face->glyph->outline.flags;

	/////////////////////////////////////////////////////////////////////////
	
	MemoryAlloc();	//Sang Ok Koo
	m_SavePoint->Add( this );	//Control Point 정보를 읽어서 CControlPoint객체 만들기

	
	//////////////////////////////////////////////////////////////////////////
	// release memory
	FT_Done_FreeType( library ); 
	m_FontLoad = TRUE;
	
	UpdateAllViews(NULL);
	return result;
}

void CFontManDoc::Init()
{
	int i,j;

	if( m_FontAniDlg ) delete m_FontAniDlg;
	m_FontAniDlg = NULL;

	m_pointNum = 0;
	if( m_point ) delete []m_point;
    m_point = NULL;
	if( m_tag ) delete []m_tag;
	m_tag = NULL;
	m_contourNum = 0;    
    if( m_contour ) delete []m_contour;
	m_contour = NULL;        
    m_flag = 0;           

	for( i = 0; i < HEIGHT; i++ )
		for( j = 0; j < WIDTH; j++ )
		{		
			m_image[i][j];		  						  
			m_thinned_image[i][j];
			m_boundary_image[i][j];
		}

	if( m_SavePoint ) delete m_SavePoint;
	m_SavePoint = NULL; 
	if( m_Vector ) delete m_Vector;
	m_Vector = NULL;

	m_StrokeOn = STROKE_END;
	m_CurStroke = -1;
	
	if( m_AniPoint ) delete [] m_AniPoint;
	m_AniPoint = NULL;
	
	m_SkelSegmentNum = 0;
	if( m_SkeletonSegment) delete m_SkeletonSegment;
	m_SkeletonSegment = NULL;

	for( i = 0; i < m_StrokeNum; i++ ) 
		m_ControlPointSegment[i].Init();
	m_StrokeNum = 0;

	m_Busu = 0;
	m_SelectedNum = 0;

	m_ShowStroke = TRUE;
	m_ShowPoint = TRUE;	
	m_DrawFont = TRUE;
	m_ShowBefore = FALSE;
	
	m_CurrentEditStroke = 0; 
	m_CurrentEditSubStroke = 0;
	m_SelectPoint = -1;
}


void CFontManDoc::MemoryAlloc()
{
	m_Vector = new CPointVector();
	m_SavePoint = new CControlPoint();	
}

void CFontManDoc::StrokePointSorting()
{
	// 각 획 ID로 Sorting //Sang Ok Koo
	int i, j;
	for( i = 0; i < m_Vector->m_Stroke; i++)
	{
		m_ControlPointSegment[i].Sort(); 
		
		 if( m_ControlPointSegment[i].m_LinkNum > 0 )
		{
			for( j = 0 ;j < m_ControlPointSegment[i].m_LinkNum; j++)
			{
				m_ControlPointSegment[i].m_Link[j]->Sort();
			}
		}
	}
}
