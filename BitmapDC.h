//----------------------------------------------------------------------------
// N O L D U S   I N F O R M A T I O N   T E C H N O L O G Y   B . V .
//----------------------------------------------------------------------------
// Filename:      BitmapDC.h
// Project:       EthoVision
// Module:        Visualization
// Programmer:    Anneke Sicherer-Roetman
// Version:       1.00
// Revision Date: 08-10-1999
//----------------------------------------------------------------------------
// Description:   Declaration of class BitmapDC
//----------------------------------------------------------------------------
// Revision history:
// 08-10-1999 - First implementation
//----------------------------------------------------------------------------
// Bugs: ........
//----------------------------------------------------------------------------
// @doc
//----------------------------------------------------------------------------
#if !defined(AFX_BITMAPDC_H__CD3E3864_7D7C_11D3_A615_0060085FE616__INCLUDED_)
#define AFX_BITMAPDC_H__CD3E3864_7D7C_11D3_A615_0060085FE616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// draws bitmap at specified point in specified device context
void DrawBitmap(const CBitmap *bitmap, const CDC *pDC, const CPoint &point);

// draws bitmap centered in specified rectangle in specified device context
void DrawBitmap(const CBitmap *bitmap, const CDC *pDC, const CRect &rect, 
                bool stretch = false);

//----------------------------------------------------------------------------
// @class         CBitmapDC |
//                memory bitmap device context
// @base          public | CDC
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 08-10-1999 - First implementation
//----------------------------------------------------------------------------
// @ex The memory bitmap device context can be used in two ways: |
//  // usage 1 : make complete bitmap and use afterwards.
//  // make memory bitmap DC, draw in it, close DC, 
//  // draw resulting bitmap, delete bitmap
//  CBitmapDC bitmapDC_1(50, 50, pDC);
//  CAutoPen pen1(&bitmapDC_1, PS_SOLID, 1, RGB(0,0,255));
//  bitmapDC_1.Rectangle(0, 0, 50, 50);
//  bitmapDC_1.MoveTo(0,50);
//  bitmapDC_1.LineTo(50,0);
//  CBitmap *pbmp = bitmapDC_1.Close();
//  DrawBitmap(pbmp, pDC, CPoint(10, 10));
//  delete pbmp;
//  pDC->TextOut(10, 70, "draw CBitmap");
//
//  // usage 2 : use DC as scratch pad and blit whenever needed.
//  // make memory bitmap DC, draw in it, blit DC to screen,
//  // use automatic cleanup in CBitmapDC's destructor
//  CBitmapDC bitmapDC_2(50, 50, pDC);
//  CAutoPen pen2(&bitmapDC_2, PS_SOLID, 1, RGB(255,0,0));
//  bitmapDC_2.Rectangle(0, 0, 50, 50);
//  bitmapDC_2.MoveTo(0, 0);
//  bitmapDC_2.LineTo(50, 50);
//  pDC->BitBlt(200, 10, 50, 50, &bitmapDC_2, 0, 0, SRCCOPY);
//  pDC->TextOut(200, 70, "blit CBitmapDC");
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
//lint -save -e1712 Default constructor not defined
class CBitmapDC : public CDC
{

  // @access Public Member Functions and Variables
public:

  // @cmember
  // constructor
  CBitmapDC(int width, int height, CDC *pOrigDC = NULL, COLORREF background = RGB(255,255,255));

  // @cmember
  // destructor, destroys bitmap if it has not been released with <mf CBitmapDC::GetFinalBitmap>
  virtual ~CBitmapDC();

  // @cmember
  // returns pointer to memory bitmap and prevents further use
  CBitmap *Close();

  // @access Private Member Functions and Variables
private:

  CBitmap *m_pBitmap; // @cmember pointer to memory bitmap
  CBitmap *m_pOldBmp; // @cmember pointer to old DC bitmap
};
//lint -restore

//----------------------------------------------------------------------------

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPDC_H__CD3E3864_7D7C_11D3_A615_0060085FE616__INCLUDED_)
