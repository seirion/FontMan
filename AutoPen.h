//****************************************************************************
// N O L D U S   I N F O R M A T I O N   T E C H N O L O G Y   B . V .
//****************************************************************************
// Filename:      AutoPen.h
// Programmer:    Anneke Sicherer-Roetman
// Version:       1.00
// Revision Date: 17-02-1999
//****************************************************************************
// Description:   Declaration of classes CAutoPen & CAutoBrush
//                (header-only classes)
//****************************************************************************
// Revision history:
//   17-02-1999 - First implementation
//****************************************************************************
// Bugs: ........
//****************************************************************************
// @doc
//****************************************************************************

#if !defined(AFX_AUTOPEN_H__FE7A35F5_C649_11D2_A614_0060085FE616__INCLUDED_)
#define AFX_AUTOPEN_H__FE7A35F5_C649_11D2_A614_0060085FE616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//****************************************************************************
// @class         CAutoPen |
//                CPen that IS correctly destroyed<nl>
//                If you create a temporary CPen object on the stack with
//                one of the one-step CPen constructors, you can easily forget 
//                to deselect the pen from the device context before it goes out
//                of scope. In that case, although stepping through the code and 
//                BoundsChecker will make you believe otherwise, this causes a 
//                serious GDI resource leak. The CAutoPen class overcomes this 
//                flaw and also selects the pen into the device context in the 
//                constructors. MFC is a much too thin wrapper around Win32!!!
// @base          public | CPen
//****************************************************************************
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 17-02-1999 - First implementation
// @ex            Example of the usage of a CAutoPen and a <c CAutoBrush> |
//
// void CMyView::OnDraw(CDC* pDC)
// {
//   // make red pen
//   CAutoPen NewPen(pDC, PS_SOLID, 1, RGB(255,0,0));
//   // make green brush
//   CAutoBrush NewBrush(pDC, RGB(0, 255, 0));
//   // draw rectangle
//   CRect rect(10, 10, 110, 110);
//   pDC->Rectangle(&rect);
// }
//
//****************************************************************************
// @todo 
//****************************************************************************
class CAutoPen : public CPen
{

  // @access Public Member Functions and Variables
public:

  // @cmember,mfunc
  // 1st constructor, see CPen docs
  CAutoPen(CDC *pDC, int nPenStyle, int nWidth, COLORREF crColor) :
  CPen(nPenStyle, nWidth, crColor), m_pDC(pDC), m_pOldGdi(NULL) 
  { 
    Initialize();  
  }

  // @cmember,mfunc
  // 2nd constructor, see CPen docs
  CAutoPen(CDC *pDC, int nPenStyle, int nWidth, const LOGBRUSH *pLogBrush,
           int nStyleCount = 0, const DWORD *lpStyle = NULL) :
  CPen(nPenStyle, nWidth, pLogBrush, nStyleCount, lpStyle), m_pDC(pDC), m_pOldGdi(NULL)
  { 
    Initialize();  
  }

  // @cmember,mfunc
  // destructor, deselects pen from device context
  virtual ~CAutoPen()
  { 
    ASSERT_VALID(m_pOldGdi);
    ASSERT_VALID(m_pDC);
    m_pDC->SelectObject(m_pOldGdi);
  }

  // @access Private Member Functions and Variables
private:

  CDC        *m_pDC;      // @cmember remembers device context
  CGdiObject *m_pOldGdi;  // @cmember remembers previous pen

  CAutoPen() { }          // default constructor cannot be called

  // @cmember,mfunc
  // selects pen into device context
  void Initialize() 
  {
    m_pOldGdi = m_pDC->SelectObject(this); 
    ASSERT_VALID(m_pOldGdi);
  }

};

//****************************************************************************
// @class         CAutoBrush |
//                CBrush that IS correctly destroyed<nl>
//                See documentation of class <c CAutoPen>
// @base          public | CBrush
//****************************************************************************
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 17-02-1999 - First implementation
//****************************************************************************
// @todo 
//****************************************************************************
class CAutoBrush : public CBrush
{

  // @access Public Member Functions and Variables
public:

  // @cmember,mfunc
  // 1st constructor, see CBrush docs
  CAutoBrush(CDC *pDC, COLORREF crColor) :
  CBrush(crColor), m_pDC(pDC), m_pOldGdi(NULL)
  { 
    Initialize();  
  }

  // @cmember,mfunc
  // 2nd constructor, see CBrush docs
  CAutoBrush(CDC *pDC, int nIndex, COLORREF crColor) :
  CBrush(nIndex, crColor), m_pDC(pDC), m_pOldGdi(NULL)
  { 
    Initialize();  
  }

  // @cmember,mfunc
  // 3rd constructor, see CBrush docs
  CAutoBrush(CDC *pDC, CBitmap* pBitmap) :
  CBrush(pBitmap), m_pDC(pDC), m_pOldGdi(NULL)
  { 
    Initialize();  
  }

  // @cmember,mfunc
  // destructor, deselects brush from device context
  virtual ~CAutoBrush()
  { 
    ASSERT_VALID(m_pOldGdi);
    ASSERT_VALID(m_pDC);
    m_pDC->SelectObject(m_pOldGdi);
  }

  // @access Private Member Functions and Variables
private:

  CDC        *m_pDC;      // @cmember remembers device context
  CGdiObject *m_pOldGdi;  // @cmember remembers previous pen

  CAutoBrush() { }        // default constructor cannot be called

  // @cmember,mfunc
  // selects brush into device context
  void Initialize() 
  {
    m_pOldGdi = m_pDC->SelectObject(this); 
    ASSERT_VALID(m_pOldGdi);
  }

};

#endif // !defined(AFX_AUTOPEN_H__FE7A35F5_C649_11D2_A614_0060085FE616__INCLUDED_)
