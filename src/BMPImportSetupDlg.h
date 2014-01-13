#if !defined(AFX_BMPIMPORTSETUPDLG_H__BD4DC7E7_184E_442E_B46D_52544832EB97__INCLUDED_)
#define AFX_BMPIMPORTSETUPDLG_H__BD4DC7E7_184E_442E_B46D_52544832EB97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BMPImportSetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBMPImportSetupDlg dialog

class CBMPImportSetupDlg : public CDialog
{
// Construction
public:
	EasyImage m_BTCObj;
	EasyImage *m_BTCData;
	RECT      m_rectImage;
	CString   m_sFilename;

	CBMPImportSetupDlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBMPImportSetupDlg)
	enum { IDD = IDD_BITMAP_IMPORT };
	float   m_Height;
	CString m_sInformation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMPImportSetupDlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBMPImportSetupDlg)
	virtual void OnOK();
	afx_msg void OnBrowse();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPIMPORTSETUPDLG_H__BD4DC7E7_184E_442E_B46D_52544832EB97__INCLUDED_)
