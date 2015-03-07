//----------------------------------------------------------------------------
// N O L D U S   I N F O R M A T I O N   T E C H N O L O G Y   B . V .
//----------------------------------------------------------------------------
// Filename:      BitmapDC.cpp
// Project:       EthoVision
// Module:        Visualization
// Programmer:    Anneke Sicherer-Roetman
// Version:       1.00
// Revision Date: 08-10-1999
//----------------------------------------------------------------------------
// Description:   Definition of class BitmapDC
//                See BitmapDC.h
//----------------------------------------------------------------------------
// Revision history:
// 08-10-1999 - First implementation
//----------------------------------------------------------------------------
// Bugs: ........
//----------------------------------------------------------------------------
// @doc
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "BitmapDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------------------
// Function DrawBitmap
// @func    draws bitmap at specified point in specified device context
// @rdesc   nothing
// @parm    const CBitmap | *bitmap | bitmap to draw
// @parm    const CDC     | *pDC    | device context to draw in
// @parm    const CPoint  | &point  | top left point of bitmap
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void DrawBitmap(const CBitmap *bitmap, const CDC *pDC, const CPoint &point)
{
  // determine bitmap size
  BITMAP bm; ((CBitmap*)bitmap)->GetBitmap(&bm);
  int w = bm.bmWidth; 
  int h = bm.bmHeight;

  // create memory device context
  CDC memDC; VERIFY(memDC.CreateCompatibleDC((CDC*)pDC));
  CBitmap *pBmp = memDC.SelectObject((CBitmap*)bitmap);
  memDC.SetMapMode(pDC->GetMapMode());

  // blit bitmap to specified device context
  ((CDC*)pDC)->BitBlt(point.x, point.y, w, h, &memDC, 0, 0, SRCCOPY);

  // clean up
  memDC.SelectObject(pBmp);
}

//----------------------------------------------------------------------------
// Function DrawBitmap
// @func    draws bitmap centered in specified rectangle in specified device context
// @rdesc   nothing
// @parm    const CBitmap | *bitmap | bitmap to draw
// @parm    const CDC     | *pDC    | device context to draw in
// @parm    const CRect   | &rect   | rectangle to center in
// @parm    bool          | stretch | stretch? (default false)
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void DrawBitmap(const CBitmap *bitmap, const CDC *pDC, const CRect &rect, 
                bool stretch /* = false */)
{
  // determine bitmap size
  BITMAP bm; ((CBitmap*)bitmap)->GetBitmap(&bm);
  int w = bm.bmWidth; 
  int h = bm.bmHeight;

  if (stretch) {
    CPoint org(0, 0);
    CSize orgSize(w, h);
    // create memory device context
    CDC memDC; VERIFY(memDC.CreateCompatibleDC((CDC*)pDC));
    CBitmap *pBmp = memDC.SelectObject((CBitmap*)bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    // blit bitmap to specified device context with stretching
    ((CDC*)pDC)->SetStretchBltMode(HALFTONE);
    ((CDC*)pDC)->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 
                            0, 0, w, h, SRCCOPY);
    // clean up
    memDC.SelectObject(pBmp);
  } else { // !stretch
    CPoint point;
    point.x = rect.left + ((rect.right - rect.left) / 2) - (w / 2);
    point.y = rect.top + ((rect.bottom - rect.top) / 2) - (h / 2);
    DrawBitmap(bitmap, pDC, point);
  }
}

//----------------------------------------------------------------------------
// Function CBitmapDC
// @func    constructor
// @parm    int   | width      | width of memory bitmap
// @parm    int   | height     | height of memory bitmap
// @parm    CDC   | *pOrigDC   | DC to make bitmap compatible with (default NULL = screen)
// @parm COLORREF | background | background color (default white)
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 08-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
//lint -save -e1732 -e1733 new in constructor and no assignment operator / copy constructor
CBitmapDC::	CBitmapDC(int width, int height, CDC *pOrigDC /* = NULL */, 
                      COLORREF background /* = RGB(255,255,255) */):
  CDC(),
    m_pBitmap(NULL),
    m_pOldBmp(NULL)
{
  // create new bitmap
  CWindowDC dc(NULL); // screen
  if (!pOrigDC)
    pOrigDC = &dc;
  m_pBitmap = new CBitmap();
  VERIFY(m_pBitmap->CreateCompatibleBitmap(pOrigDC, width, height)); 

  // make compatible device context and select bitmap into it
  VERIFY(CreateCompatibleDC(pOrigDC));
  m_pOldBmp = SelectObject(m_pBitmap);
  CDC::SetMapMode(pOrigDC->GetMapMode());

  // paint background in bitmap
  FillSolidRect(0, 0, width, height, background);
}
//lint -restore

//----------------------------------------------------------------------------
// Function CBitmapDC::~CBitmapDC
// @mfunc   destructor, destroys bitmap if it has not been released with <mf CBitmapDC::GetFinalBitmap>
// @xref    <c CBitmapDC>
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 08-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
//lint -save -e1740 pointer member not directly freed or zero'ed by destructor
CBitmapDC::~CBitmapDC()
{
  if (m_pBitmap) {
    CBitmap *pBitmap;
    pBitmap = Close();
    delete pBitmap;
    pBitmap = NULL;
  }
}
//lint -restore

//----------------------------------------------------------------------------
// Function CBitmapDC::Close
// @mfunc   returns pointer to memory bitmap and prevents further use <nl>
//          NOTE: The programmer is now responsible for deleting this bitmap
// @rdesc   pointer to memory bitmap (CBitmap*) (to be deleted by caller)
// @xref    <c CBitmapDC>
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 08-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
CBitmap *CBitmapDC::Close()
{
  ASSERT(m_pBitmap);
  CBitmap *pBitmap = m_pBitmap;
  SelectObject(m_pOldBmp);
  m_pBitmap = NULL;
  return pBitmap;
}

//----------------------------------------------------------------------------
