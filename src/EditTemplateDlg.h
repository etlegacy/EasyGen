#if !defined(AFX_EDITTEMPLATEDLG_H__9107F5B0_52D6_4FAB_A625_1D63270D0817__INCLUDED_)
#define AFX_EDITTEMPLATEDLG_H__9107F5B0_52D6_4FAB_A625_1D63270D0817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditTemplateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditTemplateDlg dialog

class CEditTemplateDlg : public CDialog
{
// Construction
public:
	BOOL LoadShaderPreview(LPCTSTR szShaderTemplate, LPCTSTR szMsFoldername, LPCTSTR szMsName);
	CEditTemplateDlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditTemplateDlg)
	enum { IDD = IDD_SHADERTEMPLATE };
	CString m_Text;
	CString m_sFind;
	CString m_sRepwith;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditTemplateDlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditTemplateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnReplaceAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CFont m_Font;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTEMPLATEDLG_H__9107F5B0_52D6_4FAB_A625_1D63270D0817__INCLUDED_)
