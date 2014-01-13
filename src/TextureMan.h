#if !defined(AFX_TEXTUREMAN_H__6CDFFDB1_0EB2_40BE_89F5_7E54CA8D0080__INCLUDED_)
#define AFX_TEXTUREMAN_H__6CDFFDB1_0EB2_40BE_89F5_7E54CA8D0080__INCLUDED_

#include "TextureList.h"    // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextureMan.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextureMan view

#include "TextureList.h"

class CEasyGenDoc;
class CEasyGenView;
class CTexture;

class CTextureMan : public CScrollView
{
protected:
	CTextureMan();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTextureMan)

// Attributes
public:

// Operations
public:
	void SelectTexture(CTexture *tex);
	void RefreshLocking();
	BOOL IsTextureValid(CTexture *tex);
	void Lock(CTexture *tex);
	CFont m_Font;
	void Clear();
	CTexture *m_TextureSelected;
	CEasyGenDoc *GetDocument();
	CTexture *AddTexture(LPCTSTR filename);
	CTextureList m_Textures;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureMan)
protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTextureMan();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTextureMan)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTUREMAN_H__6CDFFDB1_0EB2_40BE_89F5_7E54CA8D0080__INCLUDED_)
