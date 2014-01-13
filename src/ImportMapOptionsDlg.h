#if !defined(AFX_IMPORTMAPOPTIONSDLG_H__0BDED906_6087_48AB_AE53_47DF49A70317__INCLUDED_)
#define AFX_IMPORTMAPOPTIONSDLG_H__0BDED906_6087_48AB_AE53_47DF49A70317__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportMapOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportMapOptionsDlg dialog

class CImportMapOptionsDlg : public CDialog
{
// Construction
public:
	CString m_sFilename;
	int     m_dwTerrain;

	CImportMapOptionsDlg(CWnd *pParent = NULL);   // standard constructor
	void InsertItem(int i,
	                LPCTSTR szLayers,
	                LPCTSTR szAlphamap,
	                LPCTSTR szShader
	                );
// Dialog Data
	//{{AFX_DATA(CImportMapOptionsDlg)
	enum { IDD = IDD_IMPORT_FROMMAP };
	CListCtrl m_lcTerrains;
	BOOL      m_bModels;
	BOOL      m_bWorldspawn;
	CString   m_sMapInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportMapOptionsDlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportMapOptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnImportWorld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTMAPOPTIONSDLG_H__0BDED906_6087_48AB_AE53_47DF49A70317__INCLUDED_)
