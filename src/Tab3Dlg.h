#if !defined(AFX_TAB3DLG_H__36869510_0F67_4D51_BAE8_E0CAA6E4DD36__INCLUDED_)
#define AFX_TAB3DLG_H__36869510_0F67_4D51_BAE8_E0CAA6E4DD36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tab3Dlg.h : header file
//

class CEasyGenDoc;

/////////////////////////////////////////////////////////////////////////////
// CTab3Dlg dialog

class CTab3Dlg : public CDialog
{
// Construction
public:
	int GetPaintRadius();
	int m_IndexGot;
	void RefreshPal();
	CRect m_rTex;
	CRect m_rCol;
	CRect m_rPal;
	CEasyGenDoc *GetDocument();
	void Init();
	CTab3Dlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTab3Dlg)
	enum { IDD = IDD_TAB3 };
	CSliderCtrl m_slPaintRadius;
	CComboBox   m_lbZoom;
	BOOL        m_bDrawTexture;
	int         m_dwPaintRadius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTab3Dlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateSliders();
	void    OnCancel();
	void    OnOK();

	// Generated message map functions
	//{{AFX_MSG(CTab3Dlg)
	afx_msg void OnSelchangeZoom();
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteIndex();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDrawTexture();
	afx_msg void OnPosterize();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAB3DLG_H__36869510_0F67_4D51_BAE8_E0CAA6E4DD36__INCLUDED_)
