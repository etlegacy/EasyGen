#if !defined(AFX_FLIPDLG_H__6A2CDB9C_0D28_4D73_8E78_30CA9AF02DD0__INCLUDED_)
#define AFX_FLIPDLG_H__6A2CDB9C_0D28_4D73_8E78_30CA9AF02DD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlipDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlipDlg dialog

class CFlipDlg : public CDialog
{
// Construction
public:
	CFlipDlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFlipDlg)
	enum { IDD = IDD_FLIPS };
	int  m_rMode;
	BOOL m_bEnlarge;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlipDlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFlipDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLIPDLG_H__6A2CDB9C_0D28_4D73_8E78_30CA9AF02DD0__INCLUDED_)
