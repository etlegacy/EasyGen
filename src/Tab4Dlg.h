#if !defined(AFX_TAB4DLG_H__82A6A607_E37D_4E3D_8A5F_A86E608316FB__INCLUDED_)
#define AFX_TAB4DLG_H__82A6A607_E37D_4E3D_8A5F_A86E608316FB__INCLUDED_

#include "Md3.h"    // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tab4Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTab4Dlg dialog

class CTab4Dlg : public CDialog
{
// Construction
public:
	CMd3 *GetSelected();
	void Init();

	CTab4Dlg(CWnd *pParent = NULL);   // standard constructor

	CMd3 *m_Md3Selected;

// Dialog Data
	//{{AFX_DATA(CTab4Dlg)
	enum { IDD = IDD_TAB4 };
	CListBox m_lbMd3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTab4Dlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CRect m_ViewXZ;
	CRect m_ViewYZ;
	void    OnCancel();
	void    OnOK();

	// Generated message map functions
	//{{AFX_MSG(CTab4Dlg)
	afx_msg void OnSelchangeMd3List();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void DrawXZ(CDC *pDC, CMd3 *md3);
	void DrawYZ(CDC *pDC, CMd3 *md3);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAB4DLG_H__82A6A607_E37D_4E3D_8A5F_A86E608316FB__INCLUDED_)
