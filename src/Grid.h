////////////
// Grid.h //
////////////

#ifndef GRID_H
#define GRID_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Nemo3D.h"
#include "NmVec2.h"
#include "NmVec3.h"
////////
#include "Tris.h"
#include "Texture.h"
#include "AlphamapMan.h"
////////
#include "EgDefines.h"

struct S_FACEDATA
{
	float sx;
	float sy;
	float sxx;
	float syy;
	float xstep;
	float ystep;
};

struct S_FACEDATAEX
{
	float sx;
	float sy;
	float sxx;
	float syy;
	float xstep;
	float ystep;
	UINT flags;
};

struct S_POSINFO
{
	int i, j;
	float z;
};

//(int r_mode,CAlphamapMan* amap, bool aas_wires)
struct S_GRIDDRAWINFO
{
	int dwRenderMode;
	CAlphamapMan *pAlphamapManager;
	bool bAASWires;
};

class CGrid
{
private:
	int   m_i, m_j, m_k, m_iStart, m_jStart, m_iEnd, m_jEnd;
	float m_fWidthX, m_fWidthY;
	float m_fCellWidthX, m_fCellWidthY;
	int   m_dwCellsX, m_dwCellsY;
	int   m_dwSizeX, m_dwSizeY, m_dwSizeZ;
	float *m_X;
	float *m_Y;
	float *m_Z;         // pointer to a buffer
	// buffers
	float *m_ZBuffer[GRID_Z_BUFFERS];

	int          *m_Alphamap;
	CTris        *m_Tris;
	S_COLORDATA  *m_Color;
	S_FACEDATAEX *m_Face;

	CNmVec3 m_vOrigin;

	bool m_bMarkFlatTris;
	bool m_bMarkSlopeTris;
	bool m_bMarkExcludedTris;
	int  m_dwSizeFace, m_dwSizeTris;

	void        TrisLink();
	void        TextureStretch(int factor);

public:
	void TextureScaleRefresh();
	int m_TextureStretchFactor;

	void        OptionFlatTrisSet(bool state);
	void        OptionFlatTrisToggle();
	bool        OptionFlatTrisGet();
	void        OptionSlopeTrisSet(bool state);
	void        OptionSlopeTrisToggle();
	bool        OptionSlopeTrisGet();
	void        OptionExcludedTrisSet(bool state);
	void        OptionExcludedTrisToggle();
	bool        OptionExcludedTrisGet();

	void        SelectZBuf(int buf);

	CGrid();
	CGrid(CGrid &src);
	CGrid(int CellsX, int CellsY, float WidthX, float WidthY);

	virtual ~CGrid();
	void    Delete();

	void    ResetDefault();
	void    CalcColors();

	void    ArchiveLoad_1_0(CArchive & ar);
	void    ArchiveLoad_1_1(CArchive &ar);
	void    ArchiveSave(CArchive& ar);
	void    FileSave_1_3(const char *filename);
	void    FileSave_1_4(const char *filename);
	bool    FileLoad_1_3(const char *filename);
	bool    FileLoad_1_4(const char *filename);
	bool    FromPolyList(CNmPolyList& polys);

	void    FaceOrientateTris(int ci, int cj, bool invert);
	bool    AlphamapFromData(EasyImage& pcx);
	bool    PtInGrid(float x, float y);
	void    FlipH(bool swap, bool full);
	void    FlipV(bool swap, bool full);
	void    SetMaxHeight(float h);
	unsigned int    Memory(unsigned int cellsx, unsigned int cellsy);
	bool    AlphamapFromFile(LPCTSTR filename);
	void    UpdateTrisZ(CTris *t);

	void    SetZ(int i, int j, float z);
	void    SetOrigin(CNmVec3& origin);
	void    GetOrigin(CNmVec3& origin);

	float   GetX(int i);
	float   GetY(int j);
	float   GetZ(int i, int j);
	float   GetZ(int k);
	int     GetK(int i, int j);

	int     GetLeft_j(float y);
	int     GetLeft_i(float x);
	int     GetRight_j(float y);
	int     GetRight_i(float x);

	int     GetCellsY();
	int     GetCellsX();
	int     GetSizeX();
	int     GetSizeY();
	int     GetSizeZ();
	float   GetWidthY();
	float   GetWidthX();
	float   GetCellWidthY();
	float   GetCellWidthX();
	int     GetTrisCount();

	int     GetAlphaIndex(int k);
	int     GetAlphaIndex(int i, int j);
	void    SetAlphaIndex(int k, int index);
	void    SetAlphaIndex(int i, int j, int index);
	void    PaintAlphaIndex(int i_center, int j_center, int radius, int index);

	float *GetPtr(int k);
	float *GetPtr(int i, int j);

	float   GetApproxZonX(float x, int j);
	float   GetApproxZonY(float y, int i);

	S_FACEDATAEX *GetFace(int fk);
	S_FACEDATAEX *GetFace(int fi, int fj);

	int     GetTris(float x, float y);
	CTris *GetTris(int m);
	void    GetTris(int fi, int fj, CTris *& t0, CTris *& t1);
/*	CTris*	GetTris0(int ci, int cj);
    CTris*	GetTris0(int ck);			//	get tris0 of cell at (ci,cj)||(ck)
    CTris*	GetTris1(int ci, int cj);
    CTris*	GetTris1(int ck);			//	get tris1 of cell at (ci,cj)||(ck)	*/

	void    GetSTGen(int& sGen, int& tGen);

	bool    GetInfo(float x, float y, S_POSINFO *inf);

	void    ScanFullSetup();
	bool    ScanFull(int &i, int &j, int &k, float * &z);

	void    ScanRectSetup(int iStart, int jStart, int iEnd, int jEnd);
	bool    ScanRect(int &i, int &j, int &k, float * & z);
	bool    ScanRectRow(int &j);                        // use togheter
	bool    ScanRectColumn(int &i, int &k, float * & z);   // use togheter

	bool    ScanTris(CTris *&t);
	void    ScanTrisSetup();

	void    ReCellsX(int cells, bool lock, int align);
	void    ReCellsY(int cells, bool lock, int align);

	void    ReWidthX(int width, bool lock, int align);
	void    ReWidthY(int width, bool lock, int align);

	void    DrawGL(S_GRIDDRAWINFO *drawinfo);
	void    SetColor(int k, float r, float g, float b);

	void operator=(const CGrid& src);
	void    Set(int CellsX, int CellsY, float WidthX, float WidthY);
	void    GetIndexBounds(CNmVec2 pA, CNmVec2 pB, int& j_start, int& j_end, int& i_start, int& i_end);
	float   GetMinHeight();
	float   GetMaxHeight();

	int     TextureScaleUP();
	int     TextureScaleDN();
	int     TextureScaleFit();

	void    ZBufCpy(int dest, int src);
	void    CalcTrisNormals();

	bool    GetDistanceTrisRay(CNmVec3& eye, CNmVec3& ray, CNmVec3& risu, float& distance);
private:
	int m_m;     // scan tris counter
};

class CGridStack
{
private:
	int   m_dwSize;
	int   m_dwPos;
	CGrid **m_Stack;
public:
	CGridStack();
	CGridStack(int dwSize);
	virtual ~CGridStack();

	void    Push(CGrid *grid);
	void    Pop(CGrid *grid);

	void    SetSize(int dwSize);
	bool    IsEmpty();

	void    Reset();
};

#endif
