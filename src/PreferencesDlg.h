#if !defined(AFX_PREFERENCESDLG_H__581E23D7_3EB4_4B12_879F_2728CED7222C__INCLUDED_)
#define AFX_PREFERENCESDLG_H__581E23D7_3EB4_4B12_879F_2728CED7222C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

class CPreferencesDlg : public CDialog
{
// Construction
public:
	COLORREF m_BgColor;
	COLORREF m_FrameColor;
	CPreferencesDlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPreferencesDlg)
	enum { IDD = IDD_PREFERENCES };
	CStatic m_FrameColorCtrl;
	CStatic m_BgColorCtrl;
	CString m_BmpExportBasePath;
	CString m_BmpImportBasePath;
	int     m_pgWidthX;
	int     m_pgWidthY;
	int     m_pgHeight;
	CString m_TextureFilter;
	CString m_GameBasePath;
	BOOL    m_FrameAAS;
	float   m_fSlopeAngle;
	int     m_dwUndoLevels;
	CString m_sWorldShadersIgnoreList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferencesDlg)
	afx_msg void OnBrowseBitmapExpPath();
	afx_msg void OnBrowseBitmapImpPath();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnBrowseGamePath();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESDLG_H__581E23D7_3EB4_4B12_879F_2728CED7222C__INCLUDED_)
