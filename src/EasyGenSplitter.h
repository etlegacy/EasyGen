#if !defined(AFX_EASYGENSPLITTER_H__836076D4_F070_47DA_A0AA_AF623C9EA98D__INCLUDED_)
#define AFX_EASYGENSPLITTER_H__836076D4_F070_47DA_A0AA_AF623C9EA98D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EasyGenSplitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEasyGenSplitter window

class CEasyGenSplitter : public CSplitterWnd
{
// Construction
public:
	CEasyGenSplitter();

// Attributes
public:
	void OnDrawSplitter(CDC *pDC, ESplitType nType, const CRect& rect);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasyGenSplitter)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEasyGenSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEasyGenSplitter)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYGENSPLITTER_H__836076D4_F070_47DA_A0AA_AF623C9EA98D__INCLUDED_)
