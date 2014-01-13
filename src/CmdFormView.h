#if !defined(AFX_CMDFORMVIEW_H__B33B6D75_4E66_4BE1_8007_80ABAAD7849F__INCLUDED_)
#define AFX_CMDFORMVIEW_H__B33B6D75_4E66_4BE1_8007_80ABAAD7849F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdFormView.h : header file
//

#include "BtnST.h"


/////////////////////////////////////////////////////////////////////////////
// CCmdFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CEasyGenDoc;

#include "Tab1Dlg.h"
#include "Tab2Dlg.h"
#include "Tab3Dlg.h"
#include "Tab4Dlg.h"

class CCmdFormView : public CFormView
{
protected:
	CCmdFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdFormView)

// Form Data
public:
	//{{AFX_DATA(CCmdFormView)
	enum { IDD = IDD_CMD };
	int m_TabButtons;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	int m_TabActive;
	void TabShow(int tabindex);
	CTab1Dlg m_Tab1;
	CTab2Dlg m_Tab2;
	CTab3Dlg m_Tab3;
	CTab4Dlg m_Tab4;
	CView *GetRenderView();
	CEasyGenDoc *GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdFormView)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdFormView)
	afx_msg void OnTabButton();
	afx_msg void OnTabButton2();
	afx_msg void OnTabButton3();
	afx_msg void OnTabButton4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDFORMVIEW_H__B33B6D75_4E66_4BE1_8007_80ABAAD7849F__INCLUDED_)
