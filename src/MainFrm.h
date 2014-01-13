// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__79A1D62D_591C_4CA4_BA13_887517C0FCCB__INCLUDED_)
#define AFX_MAINFRM_H__79A1D62D_591C_4CA4_BA13_887517C0FCCB__INCLUDED_

#include "EasyGenSplitter.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	void AdjustFormWidth();
	void    SendStatusText(UINT id, LPCTSTR text);
	void    SetFormPaneHeight(int dwHeight);
	virtual ~CMainFrame();

	CEasyGenSplitter m_wndSplitter;
	CEasyGenSplitter m_wndSplitterLeft;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar m_wndStatusBar;
	CToolBar   m_wndToolBar;
	int        m_dwFormWidth;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__79A1D62D_591C_4CA4_BA13_887517C0FCCB__INCLUDED_)
