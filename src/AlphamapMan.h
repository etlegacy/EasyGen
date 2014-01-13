// AlphamapMan.h: interface for the CAlphamapMan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALPHAMAPMAN_H__64170960_E1D9_459A_BF25_6C6C81E19090__INCLUDED_)
#define AFX_ALPHAMAPMAN_H__64170960_E1D9_459A_BF25_6C6C81E19090__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NmList.h"

class CTexture;

struct S_ALPHAINDEXDATA
{
	int index;
	CTexture *tex;
	COLORREF col;
	CRect rect;
};

class CAlphamapMan
{
public:
	CAlphamapMan();
	virtual ~CAlphamapMan();

	S_ALPHAINDEXDATA m_Pal[256];
	int              m_Layers;
	int              m_Zoom;
	int              m_Selected;

	BOOL m_DrawColors;

	void        Reset();

	void        ArchiveLoad(CArchive& ar);
	void        ArchiveSave(CArchive& ar);
	void        FileSave_1_3(const char *filename);
	bool        FileLoad_1_3(const char *filename);
	void        FileSave_1_4(const char *filename);
	bool        FileLoad_1_4(const char *filename);
	void        FromFile(LPCTSTR filename);
	void        FromData(EasyImage& pcx);

	void        Delete(int index);

	void        Set(int index, CRect &rect);
	void        Set(int index, CTexture *tex);
	void        Set(int index, COLORREF col);
	void        Swap(int a, int b);

	int         Get(COLORREF col);
	int         Get(CPoint point);
	CTexture *GetTex(int index);
	COLORREF    GetCol(int index);
	S_ALPHAINDEXDATA *Get(int index);
	int         GetLayers();

	int         GetCurSel();
	BOOL        SetCurSel(int index);

	BOOL        IsTextureInUse(CTexture *tex);
	CView *GetTextureView();
	void        Draw(CDC *pDC);
	void        SetZoom(int zoom);

	bool        GenMetashaderList(CNmListStr& list, const char *szMsFolder, const char *szMs);
};

#endif // !defined(AFX_ALPHAMAPMAN_H__64170960_E1D9_459A_BF25_6C6C81E19090__INCLUDED_)
