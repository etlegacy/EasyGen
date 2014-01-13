// EasyGenDoc.h : interface of the CEasyGenDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EASYGENDOC_H__C7FCE65B_5866_48CE_A5CF_703EAABD2B13__INCLUDED_)
#define AFX_EASYGENDOC_H__C7FCE65B_5866_48CE_A5CF_703EAABD2B13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MainFrm.h"    // Added by ClassView
////////
#include "Grid.h"
#include "Texture.h"
#include "Md3.h"
////////
#include "NmVec3.h"
#include "NmStr.h"

struct S_SHADERTEMPLATEREPLACEINFO
{
	CNmStr sMETASHADER_NAME;
	CNmStr sI;
	CNmStr sJ;

	CNmStr sTEXTURE_I_MAP;
	CNmStr sTEXTURE_I_SIZEX;
	CNmStr sTEXTURE_I_SIZEY;

	CNmStr sTEXTURE_J_MAP;
	CNmStr sTEXTURE_J_SIZEX;
	CNmStr sTEXTURE_J_SIZEY;

	CNmStr sTEXTURE_0_MAP;
	CNmStr sTEXTURE_0_SIZEX;
	CNmStr sTEXTURE_0_SIZEY;
	CNmStr sTEXTURE_0_SGEN;
	CNmStr sTEXTURE_0_TGEN;
	CNmStr sTEXTURE_0_TCMOD;

	CNmStr sMESH_WIDTHX;
	CNmStr sMESH_WIDTHY;
	CNmStr sMESH_DIVWIDTHX;
	CNmStr sMESH_DIVWIDTHY;
};

void GenMetashaderEx(CString& buffer, LPCTSTR szMsFolder, LPCTSTR szMs, UINT nFlags, LPCTSTR szNoBlendT, LPCTSTR szBlendT, LPCTSTR szVertexT);

class CEasyGenDoc : public CDocument
{
protected: // create from serialization only
	CEasyGenDoc();
	DECLARE_DYNCREATE(CEasyGenDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasyGenDoc)
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
private:
	bool LoadFromMap(LPCTSTR mapfilename);

public:
	void    Posterize(int levels);
	void    SetOrigin(CNmVec3& origin);
	int     GetModifier();
	void    RecalcModelsZ();
	CMd3 *GetMd3SelectedTabModels();
	void ModifierNone();
	void GridReMaxHeight(float h);
	int GetActiveTab();
	BOOL CanMapBeExported();
	void SwapAlphaIndexes(int i1, int i2);
	void DeleteAlphaIndex(int index);
	void WriteAlphamap(LPCTSTR filename);
	void WriteMetashader(LPCTSTR filename, LPCTSTR szMsFolder, LPCTSTR szMs, UINT nFlags);
	BOOL SetAlphaTexture(CTexture *tex);
	void GridReCellWidths(float cwX, float cwY, BOOL lock, int alignX, int alignY);
	void SetStatusText(UINT index, LPCTSTR text);
	void UndoGet(CGrid *grid);
	void RefreshView();
	void SetCurrentTexture(CTexture *tex);
	CView *GetTextureView();
	void SetWindowTitle(LPCTSTR filename);

	virtual ~CEasyGenDoc();

	void    ResetAll();
	void    ResetCamera();

	CString m_sFilename;

	CNmVec3 m_vPlayer;

	int m_dwPaintRadius;

	BOOL m_bInvalidateGridColor;
	BOOL m_bInvalidateGridPreview;

	BOOL m_bShowAxis;
	BOOL m_bShowWorld;

	CView *GetFormView();
	CView *GetRenderView();

	void    UpdateFormView(int what, BOOL dir = TRUE);

	int   m_Modifier;
	float m_modBaseSmooth;
	float m_modBaseScale;
	float m_modBaseNoise;
	float m_modRadius;
	float m_modStrength;
	float m_modY;
	float m_modX;
	float m_modXYstep;
	BOOL  m_modShow;
	int   m_modMode;

	void ModSelect(int mod);

	BOOL IsTextureInUse(CTexture *tex);

	void RenderScene();

	void GridReCells(int CellsX, int CellsY, BOOL lock, int alignX, int alignY);
	void GridReWidths(float WidthX, float WidthY, BOOL lock, int alignX, int alignY);
	void GridModApply(CGrid& Grid);
	void GridModNoise(CGrid& Grid, float x, float y, float dzMax, float radius);
	void GridModScale(CGrid& Grid, float x, float y, float strength, float radius);
	void GridModSmooth(CGrid& Grid, float x, float y, int level, float radius);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	BOOL    ImportTerrainShader(LPCTSTR shadername);
	void    DisposeBrushes();
	void    InitBrushes(CNmPolyList& plist);

	void    RenderAxes();
	void    RenderFloor();
	void    RenderWorld();
	void    RenderModels();

	void    FileSave(LPCTSTR filename);
//	afx_msg void OnFileExportTXT();
	//{{AFX_MSG(CEasyGenDoc)
	afx_msg void OnFileExportQ3RMap();
	afx_msg void OnModifierApply();
	afx_msg void OnModifierHide();
	afx_msg void OnModifierNone();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnTest();
	afx_msg void OnTexFactorDn();
	afx_msg void OnTexFactorUp();
	afx_msg void OnTexFactorNatural();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnFileExportBitmap();
	afx_msg void OnFileImportBitmap();
	afx_msg void OnTexturesLoadDir();
	afx_msg void OnTexturesClear();
	afx_msg void OnUpdateViewFlat(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewTexture(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewWires(CCmdUI *pCmdUI);
	afx_msg void OnViewWires();
	afx_msg void OnViewFlat();
	afx_msg void OnViewFlatShade();
	afx_msg void OnViewFlatSmooth();
	afx_msg void OnViewTexture();
	afx_msg void OnViewCulling();
	afx_msg void OnUpdateViewCulling(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFlatshade(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFlatsmooth(CCmdUI *pCmdUI);
	afx_msg void OnPreferences();
	afx_msg void OnToggleAxis();
	afx_msg void OnUpdateToggleAxis(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnViewAlphamap();
	afx_msg void OnUpdateViewAlphamap(CCmdUI *pCmdUI);
	afx_msg void OnViewAddWires();
	afx_msg void OnUpdateViewAddWires(CCmdUI *pCmdUI);
	afx_msg void OnFileExportAlphamap();
	afx_msg void OnFileImportAlphamap();
	afx_msg void OnFileExportMetashader();
	afx_msg void OnViewTrisframe();
	afx_msg void OnUpdateViewTrisframe(CCmdUI *pCmdUI);
	afx_msg void OnViewNoframe();
	afx_msg void OnUpdateViewNoframe(CCmdUI *pCmdUI);
	afx_msg void OnFlip();
	afx_msg void OnToggleFlatTris();
	afx_msg void OnUpdateToggleFlatTris(CCmdUI *pCmdUI);
	afx_msg void OnToggleSlopeTris();
	afx_msg void OnUpdateToggleSlopeTris(CCmdUI *pCmdUI);
	afx_msg void OnFileImportFromMap();
	afx_msg void OnToggleExcludedtris();
	afx_msg void OnUpdateToggleExcludedtris(CCmdUI *pCmdUI);
	afx_msg void OnTerrainClearExcludedTris();
	afx_msg void OnWorldRemove();
	afx_msg void OnWorldHide();
	afx_msg void OnUpdateWorldHide(CCmdUI *pCmdUI);
	afx_msg void OnWorldShow();
	afx_msg void OnUpdateWorldShow(CCmdUI *pCmdUI);
	afx_msg void OnToggleWorld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYGENDOC_H__C7FCE65B_5866_48CE_A5CF_703EAABD2B13__INCLUDED_)
