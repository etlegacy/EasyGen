#if !defined(AFX_TAB1DLG_H__4713CB13_DA51_466A_8A9E_658A81BE0D3D__INCLUDED_)
#define AFX_TAB1DLG_H__4713CB13_DA51_466A_8A9E_658A81BE0D3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tab1Dlg.h : header file
//

#include "BtnST.h"

class CEasyGenDoc;

/////////////////////////////////////////////////////////////////////////////
// CTab1Dlg dialog

class CTab1Dlg : public CDialog
{
// Construction
public:
	void UpdateMaxHeight(float maxh);
	void UpdateCellWidths(float cwX, float cwY);
	void UpdateWidths(float WidthX, float WidthY);
	void UpdateCells(int CellsX, int CellsY);
	void UpdateOrigin(CNmVec3& origin);
	CEasyGenDoc *GetDocument();
	void Init();
	CTab1Dlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTab1Dlg)
	enum { IDD = IDD_TAB1 };
	CButtonST m_butStepUp;
	CButtonST m_butStepDown;
	CButtonST m_butStepLeft;
	CButtonST m_butStepRight;
	CButtonST m_butStepBackward;
	CButtonST m_butStepForward;
	CComboBox m_cboxAlignY;
	CComboBox m_cboxAlignX;
	int       m_CellsX;
	int       m_CellsY;
	CString   m_CurCellsX;
	CString   m_CurCellsY;
	CString   m_CurWidthX;
	CString   m_CurWidthY;
	float     m_WidthX;
	float     m_WidthY;
	BOOL      m_Lock;
	float     m_CellWidthX;
	float     m_CellWidthY;
	CString   m_CurCellWidthX;
	CString   m_CurCellWidthY;
	CString   m_CurMaxHeight;
	float     m_MaxHeight;
	float     m_fOriginX;
	float     m_fOriginY;
	float     m_fOriginZ;
	float     m_fStepBy;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTab1Dlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void    OnCancel();
	void    OnOK();

	// Generated message map functions
	//{{AFX_MSG(CTab1Dlg)
	afx_msg void OnPaint();
	afx_msg void OnReCells();
	afx_msg void OnReWidths();
	afx_msg void OnLock();
	afx_msg void OnReCellWidths();
	virtual BOOL OnInitDialog();
	afx_msg void OnReMaxHeight();
	afx_msg void OnOriginSet();
	afx_msg void OnStepForward();
	afx_msg void OnStepBackward();
	afx_msg void OnStepLeft();
	afx_msg void OnStepRight();
	afx_msg void OnStepUp();
	afx_msg void OnStepDown();
	afx_msg void OnOriginGet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAB1DLG_H__4713CB13_DA51_466A_8A9E_658A81BE0D3D__INCLUDED_)
