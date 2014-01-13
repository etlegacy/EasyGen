#if !defined(AFX_TAB2DLG_H__A5A20DCC_01E9_4396_8A73_AE1ECE9D3A54__INCLUDED_)
#define AFX_TAB2DLG_H__A5A20DCC_01E9_4396_8A73_AE1ECE9D3A54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tab2Dlg.h : header file
//

struct S_MOD_BUTTON;

class CEasyGenView;
class CEasyGenDoc;

/////////////////////////////////////////////////////////////////////////////
// CTab2Dlg dialog

class CTab2Dlg : public CDialog
{
// Construction
public:
	S_MOD_BUTTON m_ModButtons[32];
	void UpdateModifier();
	void UpdateSliders();
	CEasyGenDoc *GetDocument();
	void EnableSliders(BOOL enable);
	void Init();
	void EnableModMode(BOOL add, BOOL cmp, BOOL force);
	CTab2Dlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTab2Dlg)
	enum { IDD = IDD_TAB2 };
	CSpinButtonCtrl m_butVertexSpin;
	CSliderCtrl     m_slScale;
	CSliderCtrl     m_slSmooth;
	CSliderCtrl     m_slNoise;
	CComboBox       m_Modifiers;
	float           m_modBaseNoise;
	float           m_modBaseScale;
	float           m_modBaseSmooth;
	int             m_modMode;
	CString         m_sDescription;
	float           m_fVertexDragStep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTab2Dlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void    OnCancel();
	void    OnOK();

	// Generated message map functions
	//{{AFX_MSG(CTab2Dlg)
	afx_msg void OnModeAdd();
	afx_msg void OnModeMax();
	afx_msg void OnModeIdent();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void OnModApply();
	afx_msg void OnSelchangeModifiers();
	afx_msg void OnPaint();
	afx_msg void OnChangeVertexDragStep();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAB2DLG_H__A5A20DCC_01E9_4396_8A73_AE1ECE9D3A54__INCLUDED_)
