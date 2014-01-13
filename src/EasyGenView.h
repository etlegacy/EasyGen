// EasyGenView.h : interface of the CEasyGenView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EASYGENVIEW_H__C46C884D_AD21_44AD_8EBF_9887F8932B1A__INCLUDED_)
#define AFX_EASYGENVIEW_H__C46C884D_AD21_44AD_8EBF_9887F8932B1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NmVec3.h"

#include "Md3.h"
extern CMd3Man g_Md3Man;

class CEasyGenView : public CView
{
protected: // create from serialization only
	CEasyGenView();
	DECLARE_DYNCREATE(CEasyGenView)

// Attributes
public:
	CEasyGenDoc *GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasyGenView)
public:
	virtual void OnDraw(CDC *pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void UpdateStatusBar();
	void RenderFlagReset(int flag);
	void RenderFlagSet(int flag);
	int GetRenderMode();
	BOOL RenderFlagTest(int flag);
	void RenderFlagToggle(int flag);
	void SetRenderMode(int mode);
	int m_VertexGot;
	BOOL GetXYZ(CPoint point, CNmVec3& p);
	BOOL GetRay(CPoint point, CNmVec3& ray);
	void SetStatusText(UINT index, LPCTSTR text);
	void SetBgColor(COLORREF col);
	BOOL   m_ViewCulling;
	GLuint m_ViewShadeModel;
	int    m_RenderMode;
	float  m_Phase;
	CPoint m_LDP;
	CPoint m_RDP;
	HGLRC  m_hGLContext;
	int    m_GLPixelIndex;

	// Colors
	float m_ClearColorRed;
	float m_ClearColorGreen;
	float m_ClearColorBlue;

	virtual ~CEasyGenView();
	BOOL CEasyGenView::CreateViewGLContext(HDC hDC);
	BOOL CEasyGenView::SetWindowPixelFormat(HDC hDC);

	// Position, rotation ,scaling
	void InitGeometry(void);

	float   m_xRot;
	float   m_yRot;
	CNmVec3 m_Eye;
	float   m_xScaling;
	float   m_yScaling;
	float   m_zScaling;

	S_MD3PTR *m_Md3Highlight;

	GLfloat m_LightPos[4];
	GLfloat m_LightDir[4];

	CNmVec3 m_vGenericPos;  // for status bar

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEasyGenView)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCameraAlign();
	afx_msg void OnCameraLeft();
	afx_msg void OnCameraRight();
	afx_msg void OnCameraForward();
	afx_msg void OnCameraBackward();
	afx_msg void OnCameraStrafeLeft();
	afx_msg void OnCameraStrafeRight();
	afx_msg void OnCameraDown();
	afx_msg void OnCameraUp();
	afx_msg void OnCameraLookup();
	afx_msg void OnCameraLookdown();
	afx_msg void OnCameraFloor();
	afx_msg void OnCameraZeroh();
	afx_msg void OnCameraAlignZ();
	afx_msg void OnBackspace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RenderView.cpp
inline CEasyGenDoc *CEasyGenView::GetDocument()
{
	return (CEasyGenDoc *)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_EASYGENVIEW_H__C46C884D_AD21_44AD_8EBF_9887F8932B1A__INCLUDED_)
