#if !defined(AFX_Q3AMAPSETUP_H__01168290_BB14_4711_939C_86FDBE04FAE3__INCLUDED_)
#define AFX_Q3AMAPSETUP_H__01168290_BB14_4711_939C_86FDBE04FAE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EditTemplateDlg.h"

// Q3AMapSetup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQ3AMapSetup dialog

class CQ3AMapSetup : public CDialog
{
// Construction
public:
	CString m_sShaderTemplate;
	CString GetShaderTemplate();
//	int m_TextureStretchFactor;
	CButton          *tex128, *tex256;
	CEdit            *m_edX, *m_edY, *m_edH;
	CEditTemplateDlg m_EditTemplateDlg;

	CQ3AMapSetup(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQ3AMapSetup)
	enum { IDD = IDD_EXPORT_Q3A_SETUP };
	CComboBox m_lbTemplatelist;
	CButton   m_butEditSelected;
	CButton   m_butQ3MAP2;
	CString   m_TextureNotTop;
	CString   m_MetashaderName;
	CString   m_MetashaderFilename;
	CString   m_AlphamapFilename;
	BOOL      m_MS_Clouds;
	CString   m_ComboTexture;
	BOOL      m_BoxIt;
	BOOL      m_PlayerStart;
	BOOL      m_enableBreath;
	BOOL      m_enableDust;
	CString   m_gridsize;
	BOOL      m_bSaveMap;
	BOOL      m_bSavePcx;
	BOOL      m_bSaveShader;
	BOOL      m_bThinMesh;
	BOOL      m_bInsert;
	CString   m_MetashaderFoldername;
	BOOL      m_bModels;
	BOOL      m_bExportAllTris;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQ3AMapSetup)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQ3AMapSetup)
	virtual BOOL OnInitDialog();
	afx_msg void OnButMetashader();
	afx_msg void OnButAlphamap();
	virtual void OnOK();
	afx_msg void OnEditShader();
	afx_msg void OnSelchangeShaderTemplate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Q3AMAPSETUP_H__01168290_BB14_4711_939C_86FDBE04FAE3__INCLUDED_)
