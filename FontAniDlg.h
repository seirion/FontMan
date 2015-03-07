#if !defined(AFX_FONTANIDLG_H__21D0EF4F_3955_47D9_899F_9CB616C91818__INCLUDED_)
#define AFX_FONTANIDLG_H__21D0EF4F_3955_47D9_899F_9CB616C91818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FontAniDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFontAniDlg dialog
#include "ControlPoint.h"

class CControlPoint;
class CFontAniDlg : public CDialog
{
// Construction
public:
	void SetAniData();
	CFontAniDlg(CWnd* pParent = NULL);   // standard constructor

	CControlPoint* m_PointList;
	int m_Size;

// Dialog Data
	//{{AFX_DATA(CFontAniDlg)
	enum { IDD = IDD_DIALOG_FONT_ANI };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontAniDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFontAniDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_ViewY;
	int m_ViewX;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTANIDLG_H__21D0EF4F_3955_47D9_899F_9CB616C91818__INCLUDED_)
