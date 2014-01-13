// EasyGenDoc.cpp : implementation of the CEasyGenDoc class
//

#include "EasyGen.h"

#include "CmdFormView.h"
#include "EasyGenDoc.h"
#include "EasyGenView.h"
////////
#include <FLOAT.H>
#include "Nemo.h"
#include "Nemo3D.h"
#include "NmVec2.h"
#include "NmStr.h"
#include "NmList.h"
#include "NmParser.h"
////////
#include "Preferences.h"
#include "MainFrm.h"
#include "Q3AMapSetup.h"
#include "FlipDlg.h"
#include "BMPImportSetupDlg.h"
#include "ProgDlg.h"
#include "TextureMan.h"
#include "Tris.h"
#include "Md3.h"
////////
#include "ImportMapOptionsDlg.h"
#include "Tab1Dlg.h"
#include "Tab2Dlg.h"
#include "Tab3Dlg.h"
////////
#include "RadiantMap.h"
#include "ShaderFile.h"

// extern
extern CPreferences g_Prefs;
extern BOOL         g_FirstDoc;

#include "AlphamapMan.h"
extern CAlphamapMan g_AlphamapMan;

#include "Grid.h"
extern CGrid      g_Grid;
extern CGridStack g_GridUndo;

#include "Nemo3D.h"
extern CNmPolyList g_World;

#include "Modifier.h"
extern CModifier g_Mod;

extern CMd3Man g_Md3Man;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString TextureLastFolder = "";

/////////////////////////////////////////////////////////////////////////////
// CEasyGenDoc

IMPLEMENT_DYNCREATE(CEasyGenDoc, CDocument)

BEGIN_MESSAGE_MAP(CEasyGenDoc, CDocument)
//	ON_COMMAND(ID_FILE_EXPORT_TXT, OnFileExportTXT)
//{{AFX_MSG_MAP(CEasyGenDoc)
ON_COMMAND(ID_FILE_EXPORT_Q3AMAP, OnFileExportQ3RMap)
ON_COMMAND(ID_MODIFIER_APPLY, OnModifierApply)
ON_COMMAND(ID_MODIFIER_HIDE, OnModifierHide)
ON_COMMAND(ID_MODIFIER_NONE, OnModifierNone)
ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
ON_COMMAND(ID_FILE_SAVE, OnFileSave)
ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
ON_COMMAND(ID_TEST, OnTest)
ON_COMMAND(ID_TEX_FACTOR_DN, OnTexFactorDn)
ON_COMMAND(ID_TEX_FACTOR_UP, OnTexFactorUp)
ON_COMMAND(ID_TEX_FACTOR_NATURAL, OnTexFactorNatural)
ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
ON_COMMAND(ID_FILE_EXPORT_BITMAP, OnFileExportBitmap)
ON_COMMAND(ID_FILE_IMPORT_BITMAP, OnFileImportBitmap)
ON_COMMAND(ID_TEXTURES_LOADDIR, OnTexturesLoadDir)
ON_COMMAND(ID_TEXTURES_CLEAR, OnTexturesClear)
ON_UPDATE_COMMAND_UI(ID_VIEW_FLAT, OnUpdateViewFlat)
ON_UPDATE_COMMAND_UI(ID_VIEW_TEXTURE, OnUpdateViewTexture)
ON_UPDATE_COMMAND_UI(ID_VIEW_WIRES, OnUpdateViewWires)
ON_COMMAND(ID_VIEW_WIRES, OnViewWires)
ON_COMMAND(ID_VIEW_FLAT, OnViewFlat)
ON_COMMAND(ID_VIEW_FLATSHADE, OnViewFlatShade)
ON_COMMAND(ID_VIEW_FLATSMOOTH, OnViewFlatSmooth)
ON_COMMAND(ID_VIEW_TEXTURE, OnViewTexture)
ON_COMMAND(ID_VIEW_CULLING, OnViewCulling)
ON_UPDATE_COMMAND_UI(ID_VIEW_CULLING, OnUpdateViewCulling)
ON_UPDATE_COMMAND_UI(ID_VIEW_FLATSHADE, OnUpdateViewFlatshade)
ON_UPDATE_COMMAND_UI(ID_VIEW_FLATSMOOTH, OnUpdateViewFlatsmooth)
ON_COMMAND(ID_PREFERENCES, OnPreferences)
ON_COMMAND(ID_TOGGLEAXIS, OnToggleAxis)
ON_UPDATE_COMMAND_UI(ID_TOGGLEAXIS, OnUpdateToggleAxis)
ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
ON_COMMAND(ID_EDIT_CUT, OnEditCut)
ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
ON_COMMAND(ID_VIEW_ALPHAMAP, OnViewAlphamap)
ON_UPDATE_COMMAND_UI(ID_VIEW_ALPHAMAP, OnUpdateViewAlphamap)
ON_COMMAND(ID_VIEW_ADDWIRES, OnViewAddWires)
ON_UPDATE_COMMAND_UI(ID_VIEW_ADDWIRES, OnUpdateViewAddWires)
ON_COMMAND(ID_FILE_EXPORT_ALPHAMAP, OnFileExportAlphamap)
ON_COMMAND(ID_FILE_IMPORT_ALPHAMAP, OnFileImportAlphamap)
ON_COMMAND(ID_FILE_EXPORT_METASHADER, OnFileExportMetashader)
ON_COMMAND(ID_VIEW_TRISFRAME, OnViewTrisframe)
ON_UPDATE_COMMAND_UI(ID_VIEW_TRISFRAME, OnUpdateViewTrisframe)
ON_COMMAND(ID_VIEW_NOFRAME, OnViewNoframe)
ON_UPDATE_COMMAND_UI(ID_VIEW_NOFRAME, OnUpdateViewNoframe)
ON_COMMAND(ID_FLIP, OnFlip)
ON_COMMAND(ID_TOGGLE_FLATTRIS, OnToggleFlatTris)
ON_UPDATE_COMMAND_UI(ID_TOGGLE_FLATTRIS, OnUpdateToggleFlatTris)
ON_COMMAND(ID_TOGGLE_SLOPE, OnToggleSlopeTris)
ON_UPDATE_COMMAND_UI(ID_TOGGLE_SLOPE, OnUpdateToggleSlopeTris)
ON_COMMAND(ID_FILE_IMPORT_FROMMAP, OnFileImportFromMap)
ON_COMMAND(ID_TOGGLE_EXCLUDEDTRIS, OnToggleExcludedtris)
ON_UPDATE_COMMAND_UI(ID_TOGGLE_EXCLUDEDTRIS, OnUpdateToggleExcludedtris)
ON_COMMAND(ID_TERRAIN_CLEAREXCLUDEDTRIS, OnTerrainClearExcludedTris)
ON_COMMAND(ID_WORLD_REMOVE, OnWorldRemove)
ON_COMMAND(ID_WORLD_HIDE, OnWorldHide)
ON_UPDATE_COMMAND_UI(ID_WORLD_HIDE, OnUpdateWorldHide)
ON_COMMAND(ID_WORLD_SHOW, OnWorldShow)
ON_UPDATE_COMMAND_UI(ID_WORLD_SHOW, OnUpdateWorldShow)
ON_COMMAND(ID_TOGGLE_WORLD, OnToggleWorld)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyGenDoc construction/destruction

CEasyGenDoc::CEasyGenDoc()
{
}

CEasyGenDoc::~CEasyGenDoc()
{
}


void CEasyGenDoc::GridModScale(CGrid& Grid, float x, float y, float strength, float radius)
{
	CNmVec2 pA, pB;

	int   i, j, k;
	float *z;

	float d;
	int   i_start, i_end, j_start, j_end;

	BOOL NoRadiusCheck = radius == -1;

	float maxh;

	if (NoRadiusCheck)
	{
		i_start = 0;
		j_start = 0;
		i_end   = Grid.GetSizeX() - 1;
		j_end   = Grid.GetSizeY() - 1;
	}
	else
	{
		maxh = Grid.GetMaxHeight();

		if (maxh == 0.0f)
		{
			return;
		}

		pB.Set(x + radius, y + radius);
		pA.Set(x - radius, y - radius);

		Grid.GetIndexBounds(pA, pB, j_start, j_end, i_start, i_end);
	}

	Grid.ScanRectSetup(i_start, j_start, i_end, j_end);

	while (Grid.ScanRect(i, j, k, z))
	{
		// strength 0..2
		if (NoRadiusCheck)
		{
			*z *= strength;
		}
		else
		{     // strength ~256
			d = distance(x, y, Grid.GetX(i), Grid.GetY(j));

			if (d < radius)
			{
				*z = *z * strength / maxh;
			}
		}
	}
}


//
// radius = -1 -> uniform noise
//
void CEasyGenDoc::GridModNoise(CGrid& Grid, float x, float y, float dzMax, float radius)
{
	int   i, j, k;
	float *z;

	float dz, d;
	float RAND_MAX_1 = 1.f / (float)RAND_MAX;
	float radius_1   = 1 / radius;

	BOOL NoRadiusCheck = radius == -1;

	srand(UINT((x - (float)floor(x) + y - (float)floor(y)) * 100));

	CNmVec2 pA, pB;
	int     i_start, i_end, j_start, j_end;

	if (NoRadiusCheck)
	{
		i_start = 0;
		j_start = 0;
		i_end   = Grid.GetSizeX() - 1;
		j_end   = Grid.GetSizeY() - 1;
	}
	else
	{
		pB.Set(x + radius, y + radius);
		pA.Set(x - radius, y - radius);
		Grid.GetIndexBounds(pA, pB, j_start, j_end, i_start, i_end);
	}

	Grid.ScanRectSetup(i_start, j_start, i_end, j_end);

	while (Grid.ScanRect(i, j, k, z))
	{
		d = distance(Grid.GetX(i), Grid.GetY(j), x, y);

		if (NoRadiusCheck)
		{
			dz = dzMax * ((float)rand() * RAND_MAX_1 - 0.5f);
			dz = *z += dz;
			if (dz < ASSUME_ZERO)
			{
				*z = 0.0f;
			}
		}
		else if (d < radius)
		{
			dz = dzMax * ((float)rand() * RAND_MAX_1 - 0.5f) * (radius - d) * radius_1;
			dz = *z += dz;
			if (dz < ASSUME_ZERO)
			{
				*z = 0.0f;
			}
		}
	}
}

//
// filter level =	3 5 7 9
//
//					0 none
//
// radius = -1 -> applies to all grid
void CEasyGenDoc::GridModSmooth(CGrid& Grid, float x, float y, int level, float radius)
{
	float maxh = 0.0f;
	float mz;
	float d;

	int   i, j, k;
	float *z;

	if (level > (int)cmp_min(Grid.GetSizeX(), Grid.GetSizeY()))
	{
		level = (int)cmp_min(Grid.GetSizeX(), Grid.GetSizeY()) - 1;
		if (!(level % 2))
		{
			level--;
		}
		m_modStrength = (float)level / 10.f;
	}

	if ((level == 0) || (level == 1))
	{
		return;
	}
	if (!(level % 2))
	{
		return;
	}

	BOOL NoRadiusCheck = radius == -1;

	CNmVec2 pA, pB;
	int     i_start, i_end, j_start, j_end;

	if (NoRadiusCheck)
	{
		i_start = 0;
		j_start = 0;
		i_end   = Grid.GetSizeX() - 1;
		j_end   = Grid.GetSizeY() - 1;
	}
	else
	{
		pB.Set(x + radius, y + radius);
		pA.Set(x - radius, y - radius);

		Grid.GetIndexBounds(pA, pB, j_start, j_end, i_start, i_end);
	}

	CGrid *tmpGrid = new CGrid;

	*tmpGrid = Grid;

	int jj_start, jj_end, ii_start, ii_end;

	int   ix, iy;
	int   level_2     = level / 2;
	int   level_rest  = level % 2; //always 1
	int   level_up    = level_2 + level_rest;
	float level_level = (float)level * level;

	//main inner smooth

	Grid.ScanRectSetup(i_start, j_start, i_end, j_end);

	while (Grid.ScanRect(i, j, k, z))
	{
		d = distance(Grid.GetX(i), Grid.GetY(j), x, y);

		if (NoRadiusCheck || (d < radius))
		{
			mz       = 0;
			jj_start = j - level_2;
			jj_end   = j + level_up;
			for (int jj = jj_start; jj < jj_end; jj++)
			{
				iy = jj;
				if (iy < 0)
				{
					iy = 0;
				}
				if (iy >= Grid.GetSizeY())
				{
					iy = Grid.GetSizeY() - 1;
				}

				ii_start = i - level_2;
				ii_end   = i + level_up;
				for (int ii = ii_start; ii < ii_end; ii++)
				{
					ix = ii;
					if (ix < 0)
					{
						ix = 0;
					}
					if (ix >= Grid.GetSizeX())
					{
						ix = 2 * Grid.GetCellsX() - ix;
					}
					mz += tmpGrid->GetZ(ix, iy);
				}
			}

			*z = mz /= level_level;
		}
	}

	delete tmpGrid;
}

BOOL CEasyGenDoc::OnNewDocument()
{
	if (!g_FirstDoc)
	{
		int ok = AfxMessageBox("Start new terrain and loose changes ?", MB_YESNOCANCEL);
		if (ok != IDYES)
		{
			return FALSE;
		}
	}

	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}

	g_FirstDoc = FALSE;

////////////////////////////

	ResetAll();

	UpdateFormView(TAB_GRID);
	UpdateFormView(TAB_MODIFIER);
	UpdateFormView(TAB_ALPHAMAP);
	UpdateFormView(TAB_MODELS);

	SetWindowTitle("");

//	SetStatusText(STATUS_PLAYER,"Player: x:0 y:0");
//	SetStatusText(STATUS_PLAYER,"Mod: x:0 y:0 z:0 r:0 h:0");

	return TRUE;
}


// Headers:
//
// 0.5 : i,f,f,0.f,0.f,0.f
//
// for others version such as float(A.B)
//
// A.B : E,A,S,Y,G,E,N,float(A.B)
//
void CEasyGenDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEasyGenDoc diagnostics

#ifdef _DEBUG
void CEasyGenDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEasyGenDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEasyGenDoc commands

void CEasyGenDoc::RenderScene()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	CCmdFormView *pForm = (CCmdFormView *)GetFormView();

	CString   s;
	S_POSINFO inf;

///////////////////////////////////////////////////
////// GridPreview affected by cur modifier    ////

	if (m_bInvalidateGridPreview)
	{
		m_bInvalidateGridPreview = FALSE;

		g_Grid.SelectZBuf(GRID_Z_PREVIEW);

		g_Grid.ZBufCpy(GRID_Z_PREVIEW, GRID_Z_REAL);

		GridModApply(g_Grid);

		UpdateFormView(TAB_GRID);

//		g_Grid.CalcTrisNormals();

		g_Grid.SelectZBuf(GRID_Z_REAL);

		m_bInvalidateGridColor = TRUE;
	}

///////////////////////////////////////////////////

	if (m_bShowAxis)
	{
		GetModifier() == MOD_NONE ?
		g_Grid.SelectZBuf(GRID_Z_REAL) :
		g_Grid.SelectZBuf(GRID_Z_PREVIEW);

		RenderAxes();

		g_Grid.SelectZBuf(GRID_Z_REAL);
	}

///////////////////////////////////////////////////
	if (m_bInvalidateGridColor)
	{
		m_bInvalidateGridColor = FALSE;

		g_Grid.SelectZBuf(GRID_Z_PREVIEW);

		g_Grid.CalcColors();

		g_Grid.SelectZBuf(GRID_Z_REAL);
	}

//	RenderFloor();

//////////////////////////
//
//	Rendering Grid Preview
//
//////////////////////////

	pView->m_ViewCulling ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

	glShadeModel(pView->m_ViewShadeModel);

	S_GRIDDRAWINFO drawinfo;

	drawinfo.dwRenderMode     = pView->m_RenderMode;
	drawinfo.pAlphamapManager = &g_AlphamapMan;
	drawinfo.bAASWires        = g_Prefs.m_FrameAAS;

	g_Grid.SelectZBuf(GRID_Z_PREVIEW);
	g_Grid.DrawGL(&drawinfo);
	g_Grid.SelectZBuf(GRID_Z_REAL);

////////////////////////////////
//
//	Rendering Grid Preview - END
//
////////////////////////////////

	m_bShowWorld == TRUE ? RenderWorld() : 1;

	// Modifier

	GLboolean culling;

	glGetBooleanv(GL_CULL_FACE, &culling);

	glDisable(GL_CULL_FACE);

	// custom
	if ((m_Modifier >= MODIFIERS) && (m_modShow))
	{
		g_Mod.DrawGL();
	}
	else if ((m_Modifier != MOD_NONE) && (m_Modifier != MOD_BASE) && (m_modShow))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(1.f, 0.f, 0.f);

		glCube(
		    m_modX,
		    m_modY,
		    0.f,
		    m_modRadius * 2,
		    m_modRadius * 2,
		    m_modStrength);
	}

	if (culling)
	{
		glEnable(GL_CULL_FACE);
	}

	//
	// Models
	//

	if (GetActiveTab() == TAB_MODELS)
	{
		RenderModels();
	}

	//
	// Player
	//
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor3f(1.f, 0.f, 0.f);

	g_Grid.SelectZBuf(GRID_Z_PREVIEW);

	m_vPlayer.z = 0.f;

	if (g_Grid.GetInfo(m_vPlayer.x, m_vPlayer.y, &inf))
	{
		m_vPlayer.z = inf.z;
	}

	glCube(
	    m_vPlayer.x,
	    m_vPlayer.y,
	    m_vPlayer.z,
	    g_Prefs.m_PlayerWidthX,
	    g_Prefs.m_PlayerWidthY,
	    g_Prefs.m_PlayerHeight
	    );

	g_Grid.SelectZBuf(GRID_Z_REAL);
}

CView *CEasyGenDoc::GetRenderView()
{
	CEasyGenApp *pApp = (CEasyGenApp *)AfxGetApp();
	if (!pApp)
	{
		return NULL;
	}
	CMainFrame *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	if (!pMainFrame)
	{
		return NULL;
	}
	CView *pView = (CView *)pMainFrame->m_wndSplitter.GetPane(0, 1);
	return pView;
}

void CEasyGenDoc::GridModApply(CGrid& Grid)
{
	int   smooth_level;
	float min_cw = cmp_min(g_Grid.GetCellWidthX(), g_Grid.GetCellWidthY());

	// value check
	switch (m_Modifier)
	{
	case MOD_BASE:
		GridModNoise(Grid, m_modX, m_modY, m_modBaseNoise * 256.f, -1);
		GridModSmooth(Grid, 0, 0, (int)m_modBaseSmooth, -1);
		GridModScale(Grid, 0, 0, m_modBaseScale, -1);
		break;
	case MOD_SMOOTH:
		if (m_modRadius < min_cw)
		{
			m_modRadius = min_cw;
		}
		if (m_modStrength < 0.0f)
		{
			m_modStrength = 0.0f;
		}
		if (m_modStrength > 255.f)
		{
			m_modStrength = 255.f;
		}
		smooth_level = (int)floor(m_modStrength * 5.f / 255.f);
		if (smooth_level != 0)
		{
			smooth_level *= 2;
			smooth_level++;
		}
		GridModSmooth(Grid, m_modX, m_modY, smooth_level, m_modRadius);
		break;
	}

	if (m_Modifier >= MODIFIERS)
	{
		if (g_Mod.GetRadius() < 4)
		{
			g_Mod.RadiusSet(4);
		}

		UINT flags = g_Mod.GetFlags();

		if ((flags & MOD_FLAG_POSITIVEHEIGHT) && (g_Mod.GetHeight() < 0))
		{
			g_Mod.HeightSet(0);
		}
		g_Mod.Modify(&Grid, m_modMode);
	}
}

void CEasyGenDoc::ModSelect(int mod)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	S_POSINFO inf;
	m_modXYstep = 8.f;
	float z     = 0.f;
	float std_r = 256.f;
	float std_s = 256.f;

	m_Modifier = mod;

	g_Grid.SelectZBuf(GRID_Z_PREVIEW);
	if (g_Grid.GetInfo(m_modX, m_modY, &inf))
	{
		z = inf.z;
	}
	g_Grid.SelectZBuf(GRID_Z_REAL);

	switch (mod)
	{
	case MOD_BASE:
		break;
	case MOD_SMOOTH:
		m_modRadius   = std_r;
		m_modStrength = std_s / 4.f;
		break;
	}

	if (mod >= MODIFIERS)
	{
		g_Mod.HeightSet(std_s);
		g_Mod.RadiusSet(std_r);
		g_Mod.SelectModifier(mod - MODIFIERS);

		UINT flags = g_Mod.GetFlags();

		BOOL enAdd   = flags & MOD_FLAG_ADD;
		BOOL enMax   = flags & MOD_FLAG_MAX;
		BOOL enIdent = flags & MOD_FLAG_IDENT;

		if (enAdd)
		{
			m_modMode = MOD_MODE_ADD;
		}
		else if (enMax)
		{
			m_modMode = MOD_MODE_MAX;
		}
		else if (enIdent)
		{
			m_modMode = MOD_MODE_IDENT;
		}
	}

	if (pView)
	{
		pView->UpdateStatusBar();
	}

	//recalc preview
	m_bInvalidateGridPreview = TRUE;
}

void CEasyGenDoc::UpdateFormView(int what, BOOL dir)
{
	CCmdFormView *pForm = (CCmdFormView *)GetFormView();
	CString      s;
	CRect        rect;
	CNmVec3      t_origin;

	if (!pForm)
	{
		return;
	}

	// Doc -> pView
	if (dir)
	{
		switch (what)
		{
		case TAB_GRID:
			pForm->m_Tab1.UpdateCells(g_Grid.GetCellsX(), g_Grid.GetCellsY());
			pForm->m_Tab1.UpdateWidths(g_Grid.GetWidthX(), g_Grid.GetWidthY());
			pForm->m_Tab1.UpdateCellWidths(g_Grid.GetCellWidthX(), g_Grid.GetCellWidthY());
			pForm->m_Tab1.UpdateMaxHeight(g_Grid.GetMaxHeight());
			g_Grid.GetOrigin(t_origin); pForm->m_Tab1.UpdateOrigin(t_origin);
			break;
		case TAB_MODIFIER:
			pForm->m_Tab2.m_Modifiers.SetCurSel(m_Modifier);
			pForm->m_Tab2.UpdateModifier();
			break;
		case TAB_ALPHAMAP:
			pForm->m_Tab3.Invalidate();
			break;
		}
	}
	// Doc <- pView
	else
	{
		switch (what)
		{
		case TAB_GRID:
			break;
		case TAB_MODIFIER:
			break;
		case TAB_ALPHAMAP:
			m_dwPaintRadius = pForm->m_Tab3.GetPaintRadius();
			break;
		}
	}
}

CView *CEasyGenDoc::GetFormView()
{
	CEasyGenApp *pApp = (CEasyGenApp *)AfxGetApp();
	if (!pApp)
	{
		return NULL;
	}
	CMainFrame *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	if (!pMainFrame)
	{
		return NULL;
	}
	CView *pView = (CView *)pMainFrame->m_wndSplitterLeft.GetPane(0, 0);
	return pView;
}

void CEasyGenDoc::OnFileExportQ3RMap()
{
	CTextureMan  *pTMan = (CTextureMan *)GetTextureView();
	CQ3AMapSetup map;
	CString      s, s1, s2, s3;
	CString      ShaderName;
	CNmListStr   vxremap_list;

	int     k, m, vx;
	CNmVec3 a, b, c, d, e, f, tmp, Pos, t_origin;
	CString fitX, fitY;
	CString scoord, tcoord;
	CString textop;
	float   maxh, minh;

	UINT flags;

	CTris *t;

	std::ofstream of;

	if (!CanMapBeExported())
	{
		AfxMessageBox("Failed to export");
		return;
	}

	map.m_TextureNotTop = "common/caulk";
	map.m_BoxIt         = FALSE;
	map.m_PlayerStart   = FALSE;
	map.m_bThinMesh     = FALSE;
	map.m_enableBreath  = FALSE;
	map.m_enableDust    = FALSE;
	map.m_gridsize.Format("%d %d %d", (int)g_Grid.GetCellWidthX(), (int)g_Grid.GetCellWidthY(), int(g_Grid.GetCellWidthX() * 2));

	if (m_sFilename != "")
	{
		map.m_AlphamapFilename   = GetFileNameNoExtNoPath(m_sFilename) + ".pcx";
		map.m_MetashaderFilename = GetFileNameNoExtNoPath(m_sFilename) + ".shader";
	}
	else
	{
		map.m_AlphamapFilename   = "";
		map.m_MetashaderFilename = "";
	}

	map.m_MetashaderFoldername = GetFileNameNoExtNoPath(m_sFilename);
	map.m_MetashaderName       = "terrain";

	map.m_bSaveMap    = TRUE;
	map.m_bSavePcx    = TRUE;
	map.m_bSaveShader = TRUE;

	map.m_bInsert        = FALSE;
	map.m_bModels        = TRUE;
	map.m_bExportAllTris = FALSE;

	if (map.DoModal() == IDCANCEL)
	{
		return;
	}

//	Pos.Set( map.m_fPosX, map.m_fPosY, map.m_fPosZ );
	g_Grid.GetOrigin(Pos);

	textop = map.m_ComboTexture;

	if (map.m_bSaveShader)
	{
		flags = 0;
		if (map.m_MS_Clouds)
		{
			flags |= MS_CLOUDS;
		}
		s.Format("%s\\scripts\\%s", g_Prefs.m_GameBasePath, map.m_MetashaderFilename);
		Nm_SaveFile((LPCTSTR)s, (LPCTSTR)map.m_EditTemplateDlg.m_Text);
	}

	if (map.m_bSavePcx)
	{
		s.Format("%s\\%s", g_Prefs.m_GameBasePath, map.m_AlphamapFilename);
		WriteAlphamap((LPCTSTR)s);
	}

	if (!map.m_bSaveMap)
	{
		return;
	}

	flags = OFN_HIDEREADONLY;

	if (map.m_bInsert)
	{
		flags |= OFN_FILEMUSTEXIST;
	}
	else
	{
		flags |= OFN_OVERWRITEPROMPT;
	}

	CFileDialog fd(
	    FALSE,
	    "map",
	    "",
	    flags,
	    "Radiant map files (*.map)|*.map||",
	    NULL
	    );

	fd.m_ofn.lpstrTitle = "Export - Map";

	s.Format("%s\\maps", (const char *)g_Prefs.m_GameBasePath);
	fd.m_ofn.lpstrInitialDir = s;

	if (fd.DoModal() == IDOK)
	{

		if (!map.m_bInsert)
		{
			of.open((LPCTSTR)fd.GetPathName(), std::ios::out | std::ios::binary);
		}
		else
		{
			of.open((LPCTSTR)fd.GetPathName(), std::ios::out | std::ios::binary | std::ios::app);

		}

		CProgressDlg progressDlg(IDS_PROGRESS_Q3AMAP);

		progressDlg.Create();

		progressDlg.SetRange(0, g_Grid.GetTrisCount());

		progressDlg.SetStep(1);


		int brushnum = 0;

		maxh = g_Grid.GetMaxHeight();

		minh = -64.f;

		//
		// BEGININNING WRITING THE MAP
		//

		if (!map.m_bInsert)
		{
			of << "// Generated by EasyGen\r\n";

			of << "{\r\n\"classname\" \"worldspawn\"\r\n";

			if (map.m_enableBreath)
			{
				of << "\"enableBreath\" \"1\"\r\n";
			}
			if (map.m_enableDust)
			{
				of << "\"enableDust\" \"1\"\r\n";
			}

			if (map.m_gridsize != "")
			{
				s.Format("\"gridsize\" \"%s\"\r\n", map.m_gridsize);
				of << (LPCTSTR)s;
			}
		}   //	!insert

		//
		// vertexremapshader here
		//
		CNmStr     vxshader, ms, vxms;
		CNmListStr ms_list;
		CNmStr     file, templ_vertex;
		int        pos_textures, pos_n;

		file.Format("templates\\%s_vertexremap.shader", (const char *)map.GetShaderTemplate());
		templ_vertex.FromFile((LPCTSTR)g_Prefs.GetFullPath((const char *)file));

		pos_textures = templ_vertex.Find("textures/");
		if (pos_textures != -1)
		{
			pos_n = templ_vertex.Find('\n', pos_textures);
			if (pos_n != -1)
			{
				vxshader = templ_vertex.Mid(pos_textures, pos_n - pos_textures + 1);
				vxshader.Replace("\n", "");
				vxshader.Replace("\r", "");
				CNmStr name;
				name.Format("%s/%s", (const char *)map.m_MetashaderFoldername, (const char *)map.m_MetashaderName);
				vxshader.Replace("%METASHADER_NAME%", (const char *)name);
				TRACE("OK");
			}
		}

		if (pos_textures == -1 || pos_n == -1)
		{
			vxshader.Format("textures/%s/%s.vertex", map.m_MetashaderFoldername, map.m_MetashaderName);
			TRACE("Failed.");
		}

		g_AlphamapMan.GenMetashaderList(
		    ms_list,
		    (LPCTSTR)map.m_MetashaderFoldername,
		    (LPCTSTR)map.m_MetashaderName
		    );

		// create remap pairs
		ms_list.ScanReset();
		while (ms_list.Scan(ms))
		{
			vxms.Format(
			    "%s;%s",
			    (const char *)ms,
			    (const char *)vxshader
			    );

			vxremap_list.Append(vxms);
		}

		// insert now if new map
		if (!map.m_bInsert)
		{
			vx = 0;
			vxremap_list.ScanReset();
			while (vxremap_list.Scan(ms))
			{
				vxms.Format(
				    "\"vertexremapshader%d\" \"%s\"\r\n",
				    vx++,
				    (const char *)ms
				    );

				of << (const char *)vxms;
			}
		}

		// vertexremapshader here - END

		if (!map.m_bInsert)
		{

			// box it
			if (map.m_BoxIt)
			{
				CString sky;

				maxh = g_Grid.GetMaxHeight();
				minh = -64.f;

				maxh  = (float)floor(maxh);
				maxh += 512;
				maxh -= int(maxh) % 64;

				int dd = 32;
				int x0, y0, x1, y1, z0, z1;

				x0 = (int)Pos.x + (int)g_Grid.GetX(0);
				x1 = (int)Pos.x + (int)g_Grid.GetX(g_Grid.GetSizeX() - 1);
				y0 = (int)Pos.y + (int)g_Grid.GetY(0);
				y1 = (int)Pos.y + (int)g_Grid.GetY(g_Grid.GetSizeY() - 1);
				z0 = (int)Pos.z + (int)minh;
				z1 = (int)Pos.z + (int)maxh;

				if (g_Prefs.m_IsQuake3)
				{
					sky = "skies/q3dm0";
				}
				else
				{
					sky = "common/caulk";
				}

				// 1
				q3r_cube(s, x0, y0 - dd, z0, x1, y0, z1, sky);
				of << (LPCTSTR)s;
				// 2
				q3r_cube(s, x1, y0, z0, x1 + dd, y1, z1, sky);
				of << (LPCTSTR)s;
				// 3
				q3r_cube(s, x0, y1, z0, x1, y1 + dd, z1, sky);
				of << (LPCTSTR)s;
				// 4
				q3r_cube(s, x0 - dd, y0, z0, x0, y1, z1, sky);
				of << (LPCTSTR)s;
				// 5
				q3r_cube(s, x0, y0, z1, x1, y1, z1 + dd, sky);
				of << (LPCTSTR)s;
				// 6
				q3r_cube(s, x0, y0, z0 - dd, x1, y1, z0, sky);
				of << (LPCTSTR)s;
			} // box close

			of << "}\r\n";

			// info_player_start
			if (map.m_PlayerStart)
			{
				int px = (int)Pos.x + (int)floor(m_vPlayer.x);
				px -= px % 16;
				int py = (int)Pos.y + (int)floor(m_vPlayer.y);
				py -= py % 16;
				int pz = (int)Pos.z + (int)floor(m_vPlayer.z) + 64;
				pz -= pz % 16;

				s.Format(
				    "{\r\n"
				    "\"classname\" \"info_player_start\"\r\n"
				    "\"origin\" \"%d %d %d\"\r\n"
				    "}\r\n",
				    px, py, pz
				    );

/*			s = "{\r\n";
            s+= "\"classname\" \"info_player_start\"\r\n";
            s+= "\"origin\" \"";
            s1.Format("%d %d %d",px,py,pz);
            s+= s1;
            s+= "\"\r\n";
            s+= "}\r\n"; */

				of << (LPCTSTR)s;
			}

		} // !insert end

		s  = "// entity 1\r\n";
		s += "{\r\n";
		s += "\"classname\" \"func_group\"\r\n";
		s += "\"terrain\" \"1\"\r\n";

		/////////// "Layers" "4"
		s1.Format("\"layers\" \"%d\"\r\n", g_AlphamapMan.GetLayers());
		//
		s += s1;

		/////////// "Shader" "terrain/mpterra2"
		s1 = map.m_MetashaderFilename;
		s1 = s1.Left(s1.ReverseFind('.'));
		//
		s += "\"shader\" \"";
		s += map.m_MetashaderFoldername;
		s += '/';
		s += map.m_MetashaderName;
		s += "\"\r\n";

		/////////// "Alphamap" "maps/alpha.bmp"
		s += "\"alphamap\" \"";
		s += map.m_AlphamapFilename;
		s += "\"\r\n";

		s.Replace('\\', '/');

		of << (LPCTSTR)s;

		k = 0;

//		g_Grid.ScanTrisSetup();
//		while (g_Grid.ScanTris(t))
		for (m = 0; m < g_Grid.GetTrisCount(); m++)
		{
			t = g_Grid.GetTris(m);
			g_Grid.UpdateTrisZ(t);

			if (!map.m_bExportAllTris && t->GetExcluded())
			{
				progressDlg.StepIt();
				continue;
			}

			a.Set(Pos.x + *t->m_vx[0], Pos.y + *t->m_vy[0], Pos.z + (float)floor(t->m_vz[0]));
			b.Set(Pos.x + *t->m_vx[1], Pos.y + *t->m_vy[1], Pos.z + (float)floor(t->m_vz[1]));
			c.Set(Pos.x + *t->m_vx[2], Pos.y + *t->m_vy[2], Pos.z + (float)floor(t->m_vz[2]));

			if (map.m_bThinMesh)
			{
				d.Set(Pos.x + *t->m_vx[0], Pos.y + *t->m_vy[0], Pos.z + (float)floor(t->m_vz[0]) - 16.f);
				e.Set(Pos.x + *t->m_vx[1], Pos.y + *t->m_vy[1], Pos.z + (float)floor(t->m_vz[1]) - 16.f);
				f.Set(Pos.x + *t->m_vx[2], Pos.y + *t->m_vy[2], Pos.z + (float)floor(t->m_vz[2]) - 16.f);
			}
			else
			{
				d.Set(Pos.x + *t->m_vx[0], Pos.y + *t->m_vy[0], Pos.z + minh);
				e.Set(Pos.x + *t->m_vx[1], Pos.y + *t->m_vy[1], Pos.z + minh);
				f.Set(Pos.x + *t->m_vx[2], Pos.y + *t->m_vy[2], Pos.z + minh);
			}

			s.Format("// brush %d\r\n{\r\n", brushnum++);

			of << (LPCTSTR)s;

			// (top)
			s.Format(
			    "( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) %s 0 0 0 0.5 0.5 134217728 0 0\r\n",
			    a.x, a.y, a.z,
			    c.x, c.y, c.z,
			    b.x, b.y, b.z,
			    textop
			    );

			of << (LPCTSTR)s;

			// (bottom)
			s.Format(
			    "( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) %s 0 0 0 0.5 0.5 134217728 0 0\r\n",
			    d.x, d.y, d.z,
			    e.x, e.y, e.z,
			    f.x, f.y, f.z,
			    map.m_TextureNotTop
			    );

			of << (LPCTSTR)s;

			// (a-c)
			s.Format(
			    "( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) %s 0 0 0 0.5 0.5 134217728 0 0\r\n",
			    a.x, a.y, a.z,
			    d.x, d.y, d.z,
			    f.x, f.y, f.z,
			    map.m_TextureNotTop
			    );

			of << (LPCTSTR)s;

			// (a-b)
			s.Format(
			    "( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) %s 0 0 0 0.5 0.5 134217728 0 0\r\n",
			    d.x, d.y, d.z,
			    a.x, a.y, a.z,
			    b.x, b.y, b.z,
			    map.m_TextureNotTop
			    );

			of << (LPCTSTR)s;

			// (b-c)
			s.Format(
			    "( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) ( %1.0f %1.0f %1.0f ) %s 0 0 0 0.5 0.5 134217728 0 0\r\n",
			    b.x, b.y, b.z,
			    c.x, c.y, c.z,
			    f.x, f.y, f.z,
			    map.m_TextureNotTop
			    );

			of << (LPCTSTR)s;

			of << "}\r\n";

			progressDlg.StepIt();
		}

		progressDlg.DestroyWindow();

		of << "}\r\n";

		// Writing models
		if (map.m_bModels)
		{
			RecalcModelsZ();

			S_MD3PTR *item;
			CNmStr   templ, md3use, md3tmp;
			char     sbasepath[_MAX_PATH];
			char     spath[_MAX_PATH];

			GetShortPathName(g_Prefs.m_GameBasePath, sbasepath, _MAX_PATH);

			// %POS  = "X Y Z"
			// %PATH = "models/mapobj~1/telepo~1/telepo~1.md3"
			templ  = "// entity x\r\n";
			templ += "{\r\n";
			templ += "\"origin\" \"%1.0f %1.0f %1.0f\"\r\n";
			templ += "\"model\" \"%s\"\r\n";
			templ += "\"classname\" \"misc_model\"\r\n";
			templ += "}\r\n";

			g_Md3Man.m_Placed.ScanReset();
			while (g_Md3Man.m_Placed.Scan(item))
			{
				GetShortPathName(item->pMd3->m_sName, spath, _MAX_PATH);
				md3tmp = spath;
				md3tmp.Delete(0, strlen(sbasepath) + 1); // and del '/' too
				md3tmp.MakeLower();
				md3tmp.Replace('\\', '/');

				md3use.Format((LPCTSTR)templ,
				              Pos.x + item->m_egvPos.x, Pos.y + item->m_egvPos.y, Pos.z + item->m_egvPos.z, (LPCTSTR)md3tmp);

				of << (const char *)md3use;
			}
		} // Models


/*
// entity 1
{
"origin" "96 72 24"
"model" "models/mapobj~1/telepo~1/telepo~1.md3"
"classname" "misc_model"
}
*/

		of.close();

		// vertexremapshader insert into wordlspawn
		if (map.m_bInsert)
		{
			CRadiantMap rmap;
			CNmStr      v_classname, vxremap, sline, sKey, dummy;
			int         vxx;

			rmap.Open((LPCTSTR)fd.GetPathName());

			while (rmap.ent_GetNext())
			{
				rmap.ent_GetValue("classname", v_classname);

				if (v_classname == "worldspawn")
				{
					TRACE("VXREMAP: entering worldspawn\n");

					// 'textures/test/terrain_0;textures/test/terrain.vertex'
					vxremap_list.ScanReset();
					while (vxremap_list.Scan(vxremap))
					{
						TRACE("VXREMAP: checking %s\n", (const char *)vxremap);

						// se non esiste questo remap
						if (!rmap.ent_GetKey((const char *)vxremap, sKey))
						{
							// lo faccio
							vxx = 0;
							sKey.Format("vertexremapshader%d", vxx++);
							while (rmap.ent_GetValue(sKey, dummy))
							{
								sKey.Format("vertexremapshader%d", vxx++);
							}

							TRACE("VXREMAP: inserting into %s\n", (const char *)sKey);

							rmap.ent_SetValue(sKey, vxremap);
						}
					}

					rmap.ent_Update();
					break;
				}
			}

			rmap.Close(true);
		}

	} //IDOK
}

void CEasyGenDoc::OnModifierApply()
{
	UndoGet(&g_Grid);

	GridModApply(g_Grid);

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnModifierHide()
{
	m_modShow = !m_modShow;
	RefreshView();
}

void CEasyGenDoc::OnModifierNone()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	switch (GetActiveTab())
	{
	case (TAB_GRID):
		break;
	case (TAB_MODIFIER):
		ModifierNone();
		break;
	case (TAB_ALPHAMAP):
		break;
	case (TAB_MODELS):
		pView->m_Md3Highlight = 0;
		RefreshView();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEasyGenDoc commands

void CEasyGenDoc::SetWindowTitle(LPCTSTR filename)
{
	CEasyGenApp *pApp = (CEasyGenApp *)AfxGetApp();

	CString apptitle, s;

	apptitle.LoadString(AFX_IDS_APP_TITLE);
	s.Format(WTITLE_FORMAT, filename, g_Prefs.m_GameBasePath, apptitle);

	if ((pApp) && (pApp->m_pMainWnd))
	{
		pApp->m_pMainWnd->SetWindowText((LPCTSTR)s);
	}
}

void CEasyGenDoc::OnFileOpen()
{
	CFileDialog fd(TRUE,
	               "egn",
	               "",
	               OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	               "EasyGen (*.egn)|*.egn||",
	               NULL);

	CString         ext;
	CString         idir;
	S_EASYGENHEADER hdr;
	std::ifstream   f;

	idir = g_Prefs.GetFullPath("Save");

	fd.m_ofn.lpstrInitialDir = idir;

	if (fd.DoModal() != IDOK)
	{
		return;
	}

	m_sFilename = fd.GetPathName();

	ext = m_sFilename.Right(3);
	ext.MakeLower();

	DisposeBrushes();
	g_World.RemoveAll();

	g_Md3Man.ResetNewMap();

	// START - detect version
	f.open((LPCTSTR)m_sFilename, std::ios::in | std::ios::binary);
	f.read((char *)&hdr, sizeof(S_EASYGENHEADER));
	f.close();

	if (strcmp("EASYGEN", hdr.appname) != 0)
	{
		AfxMessageBox("Error: this file does not appear to be a valid EasyGen file.");
		return;
	}

	if (ext == "egn")   // this is a g_saveVersion, not g_appVersion...
	{
		if (hdr.version == 1.0f || hdr.version == 1.1f)
		{
			CFile file;
			file.Open((LPCTSTR)m_sFilename, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary);
			file.Seek(sizeof(S_EASYGENHEADER), CFile::begin);
			CArchive ar(&file, CArchive::load, 8192);

			if (hdr.version == 1.0f)
			{
				g_Grid.ArchiveLoad_1_0(ar);
				g_AlphamapMan.ArchiveLoad(ar);
			}
			else if (hdr.version == 1.1f)
			{
				g_Grid.ArchiveLoad_1_1(ar);
				g_AlphamapMan.ArchiveLoad(ar);
			}

			ar.Close();
			file.Close();

			// Models loading
			CString md3file;
			md3file = m_sFilename;
			md3file.Delete(md3file.ReverseFind('.'), 4);
			md3file += ".models";
			g_Md3Man.LoadPlaced(md3file);
		}
		else if (hdr.version == 1.3f)
		{
			g_Grid.ResetDefault();
			g_Grid.FileLoad_1_3((LPCTSTR)m_sFilename);
			g_AlphamapMan.FileLoad_1_3((LPCTSTR)m_sFilename);
			g_Md3Man.FileLoad_1_3((LPCTSTR)m_sFilename);
		}
		else if (hdr.version == 1.4f)
		{
			g_Grid.ResetDefault();
			g_Grid.FileLoad_1_4((LPCTSTR)m_sFilename);
			g_AlphamapMan.FileLoad_1_4((LPCTSTR)m_sFilename);
			g_Md3Man.FileLoad_1_4((LPCTSTR)m_sFilename);
		}
		else // version not supported by EasyGen
		{
			AfxMessageBox("Invalid version.\r\n\r\n"
			              "If you want load from a previous version of EasyGen do it through an heightmap.");
			return;
		}
	}
	else
	{
		TRACE("Error opening %s\n", ext);
	}

	if (g_AlphamapMan.GetTex(0) != NULL)
	{
		SetCurrentTexture(g_AlphamapMan.GetTex(0));
	}

	CTextureMan *pTMan = (CTextureMan *)GetTextureView();

	SetWindowTitle(m_sFilename);

	UpdateFormView(TAB_GRID);
	UpdateFormView(TAB_ALPHAMAP);

	pTMan->SelectTexture(g_AlphamapMan.GetTex(0));
	pTMan->RefreshLocking();
	pTMan->Invalidate();

	m_bInvalidateGridPreview = TRUE;
	m_bInvalidateGridColor   = TRUE;

	ResetCamera();

	RefreshView();

	Sleep(SLEEP_TIME);
}

void CEasyGenDoc::FileSave(LPCTSTR filename)
{
	std::ofstream   f;
	S_EASYGENHEADER hdr;

	strcpy(hdr.appname, "EASYGEN");
	hdr.version = g_SaveVersion;
	f.open((LPCTSTR)m_sFilename, std::ios::out | std::ios::binary);
	f.write((char *)&hdr, sizeof(S_EASYGENHEADER));
	f.close();

	g_AlphamapMan.FileSave_1_4((LPCTSTR)m_sFilename);
	g_Grid.FileSave_1_4((LPCTSTR)m_sFilename);
	g_Md3Man.FileSave_1_4((LPCTSTR)m_sFilename);
}

void CEasyGenDoc::OnFileSave()
{
	CString idir;

	if (m_sFilename == "")
	{
		CFileDialog fd(
		    FALSE,
		    "egn",
		    "",
		    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		    "EasyGen (*.egn)|*.egn||",
		    NULL
		    );

		idir = g_Prefs.GetFullPath("Save");

		fd.m_ofn.lpstrInitialDir = idir;

		if (fd.DoModal() != IDOK)
		{
			return;
		}

		m_sFilename = fd.GetPathName();
	}

	// saving...
	FileSave((LPCTSTR)m_sFilename);
	// saved

	SetWindowTitle(m_sFilename);
}

void CEasyGenDoc::OnFileSaveAs()
{
	CString     idir;
	CFileDialog fd(
	    FALSE,
	    "egn",
	    "",
	    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	    "EasyGen (*.egn)|*.egn||",
	    NULL
	    );

	idir = g_Prefs.GetFullPath("Save");

	fd.m_ofn.lpstrInitialDir = idir;

	if (fd.DoModal() != IDOK)
	{
		return;
	}

	m_sFilename = fd.GetPathName();

	// saving...
	FileSave((LPCTSTR)m_sFilename);
	// saved

	SetWindowTitle(m_sFilename);
}

void CEasyGenDoc::OnTest()
{
/*
    CRadiantMap		map;
    CShaderFile		shader;
    CNmStr			dbg,k_message;

//	PostMessage(m_pMainWnd->m_hWnd,ID_FILE_NEW,0,0);	return;

    shader.Open("d:\\giochi\\quake3\\baseq3\\scripts\\kh_terrain.shader");

    if (shader.IsOpen())
    {
        TRACE("Ok\n");
    }

    while (shader.sh_GetNext())
    {
//		TRACE("...\n");
//		dbg = "[" + map.m_curBlock + "]";
//		Nm_SaveFile("c:\\logegn.txt",dbg.GetBuffer());
//		AfxMessageBox("wait");

        if (shader.sh_GetName(dbg))
        {
            TRACE("[%s]\n",(const char*)dbg);
        }

        while (shader.stg_GetNext())
        {
            shader.stg_GetValue("map",dbg);

            TRACE("MAP = [%s]\n",(const char*)dbg);
        }

    }

    shader.Close();
*/
}

void CEasyGenDoc::OnTexFactorDn()
{
	g_Grid.TextureScaleDN();

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnTexFactorUp()
{
	g_Grid.TextureScaleUP();

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnTexFactorNatural()
{
	g_Grid.TextureScaleFit();

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnEditCopy()
{
}

void CEasyGenDoc::OnEditPaste()
{
}

void CEasyGenDoc::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(0);
}

void CEasyGenDoc::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(0);
}

void CEasyGenDoc::OnFileExportBitmap()
{
	CString       s;
	EasyImage     bmp;
	int           x, y;
	float         maxh;
	unsigned char rgb;

	CFileDialog fd(
	    FALSE,
	    "bmp",
	    (LPCTSTR)GetFileNameNoExtNoPath(m_sFilename),
	    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	    "BMP image (*.bmp)|*.bmp||",
	    NULL
	    );

	fd.m_ofn.lpstrTitle = "Export - Bitmap";

	g_Prefs.m_BmpExportBasePath == "" ?
	fd.m_ofn.lpstrInitialDir                                                       = g_Prefs.m_sCurrentPath :
	                                                      fd.m_ofn.lpstrInitialDir = g_Prefs.m_BmpExportBasePath;

	if (fd.DoModal() != IDOK)
	{
		return;
	}

	s = fd.GetPathName();

	bmp.Create(g_Grid.GetSizeX(), g_Grid.GetSizeY(), 1);

	maxh = g_Grid.GetMaxHeight();

	for (y = 0; y < bmp.GetHeight(); y++)
	{
		for (x = 0; x < bmp.GetWidth(); x++)
		{
			// c1 = (unsigned char)g_Grid.m_Z[y][x];
			// c2 = (unsigned char)maxh;
			rgb = unsigned char(255 * (g_Grid.GetZ(x, y) / maxh));
			bmp.SetColorForPixel(x, y, BTRGB(rgb, rgb, rgb));
		}
	}

	bmp.Flip();

	bmp.Save((LPCTSTR)s, BTFILETYPE_BMP_NO_CRYP);   //no bmp crypt/pack

	bmp.ClearData();
}

BOOL LoadBMPImage(LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal)
{
	CFile file;
	if (!file.Open(sBMPFile, CFile::modeRead))
	{
		return FALSE;
	}

	BITMAPFILEHEADER bmfHeader;

	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		return FALSE;
	}

	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
	{
		return FALSE;
	}

	// Get length of the remainder of the file and allocate memory
	DWORD   nPackedDIBLen = file.GetLength() - sizeof(BITMAPFILEHEADER);
	HGLOBAL hDIB          = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if (hDIB == 0)
	{
		return FALSE;
	}

	// Read the remainder of the bitmap file.
	//Jacker :: changed this line
	if (file.Read((LPSTR)hDIB, nPackedDIBLen) != nPackedDIBLen)
	{
		::GlobalFree(hDIB);
		return FALSE;
	}


	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO       &bmInfo    = *(LPBITMAPINFO)hDIB ;

	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed :
	              1 << bmiHeader.biBitCount;

	LPVOID lpDIBBits;
	if (bmInfo.bmiHeader.biBitCount > 8)
	{
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) +
		                     ((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	}
	else
	{
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	}

	// Create the logical palette
	if (pPal != NULL)
	{
		// Create the palette
		if (nColors <= 256)
		{
			UINT       nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP  = (LOGPALETTE *) new BYTE[nSize];

			pLP->palVersion    = 0x300;
			pLP->palNumEntries = nColors;

			for (int i = 0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed   = bmInfo.bmiColors[i].rgbRed;
				pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
				pLP->palPalEntry[i].peBlue  = bmInfo.bmiColors[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}

			pPal->CreatePalette(pLP);

			delete[] pLP;
		}
	}

	CClientDC dc(NULL);
	CPalette  *pOldPalette = NULL;
	if (pPal)
	{
		pOldPalette = dc.SelectPalette(pPal, FALSE);
		dc.RealizePalette();
	}

	HBITMAP hBmp = CreateDIBitmap(dc.m_hDC,         // handle to device context
	                              &bmiHeader, // pointer to bitmap size and format data
	                              CBM_INIT, // initialization flag
	                              lpDIBBits, // pointer to initialization data
	                              &bmInfo, // pointer to bitmap color-format data
	                              DIB_RGB_COLORS); // color-data usage
	bitmap.Attach(hBmp);

	if (pOldPalette)
	{
		dc.SelectPalette(pOldPalette, FALSE);
	}

	::GlobalFree(hDIB);
	return TRUE;
}

void CEasyGenDoc::OnFileImportBitmap()
{
	CBMPImportSetupDlg dlg;

	dlg.m_Height = 256.f;

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CString       s;
	BTCOLORREF    col;
	int           i, j, k;
	float         *z;
	unsigned char rgb;

	if ((dlg.m_BTCData->GetWidth() > GRID_MAX_SIZE))
	{
		AfxMessageBox("Too large bitmap. Bitmap can't be greater than 129x129.");
		return;
	}

	dlg.m_BTCData->Flip();

	UndoGet(&g_Grid);

	g_Grid.Set(
	    dlg.m_BTCData->GetWidth() - 1,
	    dlg.m_BTCData->GetHeight() - 1,
	    float(128 * (dlg.m_BTCData->GetWidth() - 1)),
	    float(128 * (dlg.m_BTCData->GetHeight() - 1))
	    );

	dlg.m_Height /= 255.f;

	g_Grid.ScanFullSetup();

	while (g_Grid.ScanFull(i, j, k, z))
	{
		col = dlg.m_BTCData->GetColorFromPixel(i, j);
		rgb = BTGetRValue(col);
		g_Grid.SetZ(i, j, dlg.m_Height * float(rgb));
	}

	UpdateFormView(TAB_GRID);

	m_bInvalidateGridPreview = TRUE;
	RefreshView();

	Sleep(SLEEP_TIME);
}


CView *CEasyGenDoc::GetTextureView()
{
	CEasyGenApp *pApp = (CEasyGenApp *)AfxGetApp();
	if (!pApp)
	{
		return NULL;
	}
	CMainFrame *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	if (!pMainFrame)
	{
		return NULL;
	}
	CView *pView = (CView *)pMainFrame->m_wndSplitterLeft.GetPane(1, 0);
	return pView;
}

static int __stdcall TexturesLoadDirCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	TCHAR   szDir[MAX_PATH] = { NULL };
	CString s;
	if (uMsg == BFFM_INITIALIZED)
	{
		s  = g_Prefs.m_GameBasePath;
		s += "\\textures";
		//  Browse dialog initialized - set it to our default folder
		wsprintf(szDir, "%s", (LPCTSTR)s);
		// set to what you want
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
	}
	return 0;
}

void CEasyGenDoc::OnTexturesLoadDir()
{
	CTextureMan  *pTMan = (CTextureMan *)GetTextureView();
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	CString    s;
	int        i;
	char       chName[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.hwndOwner = pView->m_hWnd;
	bi.lpfn      = TexturesLoadDirCallbackProc;
	bi.lpszTitle = "Select folder to load textures from";
	LPITEMIDLIST lpIDList = ::SHBrowseForFolder(&bi);
	::SHGetPathFromIDList(lpIDList, chName);

	CString folder = chName;
	CString mask, filename, ext;

	HGLOBAL hgMem;
	hgMem = GlobalHandle(lpIDList);
	GlobalFree(hgMem);

	if (folder == "")
	{
		return;
	}

	TextureLastFolder = folder;

	folder += "\\";

	mask = folder + "*.*";

	WIN32_FIND_DATA data;
	HANDLE          hFind = FindFirstFile(_T((LPCTSTR)mask), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (lstrcmp(data.cFileName, _T(".")) == 0 ||
			    lstrcmp(data.cFileName, _T("..")) == 0 ||
			    (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}

			filename = folder + data.cFileName;
			filename.MakeLower();

			i = filename.ReverseFind('.');

			if (i == -1)
			{
				continue;
			}

			ext = filename.Right(filename.GetLength() - i - 1);

			ext.MakeLower();

			if (g_Prefs.m_TextureFilter.Find((LPCTSTR)ext) != -1)
			{
				pTMan->AddTexture((LPCTSTR)filename);
			}

		}
		while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	pTMan->InvalidateRect(NULL, FALSE);
}

void CEasyGenDoc::SetCurrentTexture(CTexture *tex)
{
	g_Grid.TextureScaleRefresh();

	RefreshView();
}

void CEasyGenDoc::OnTexturesClear()
{
	CTextureMan *pTMan = (CTextureMan *)GetTextureView();
	pTMan->Clear();
}


void CEasyGenDoc::OnViewWires()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->SetRenderMode(R_WIREFRAME);
	RefreshView();
}

void CEasyGenDoc::OnViewFlat()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->SetRenderMode(R_FLAT);
	RefreshView();
}

void CEasyGenDoc::OnViewTexture()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->SetRenderMode(R_TEXTURE);
	RefreshView();
}

void CEasyGenDoc::OnViewAlphamap()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->SetRenderMode(R_ALPHAMAP);
	RefreshView();
}

void CEasyGenDoc::OnViewFlatShade()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->m_ViewShadeModel = GL_FLAT;
	RefreshView();
}

void CEasyGenDoc::OnViewFlatSmooth()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->m_ViewShadeModel = GL_SMOOTH;
	RefreshView();
}

void CEasyGenDoc::OnViewCulling()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->m_ViewCulling = !pView->m_ViewCulling;
	RefreshView();
}

void CEasyGenDoc::OnViewAddWires()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->RenderFlagReset(R_FL_TRISFRAME);
	pView->RenderFlagSet(R_FL_QUADFRAME);

	RefreshView();
}

void CEasyGenDoc::OnUpdateViewFlat(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pCmdUI->SetCheck(pView->GetRenderMode() == R_FLAT);
}

void CEasyGenDoc::OnUpdateViewTexture(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pCmdUI->SetCheck(pView->GetRenderMode() == R_TEXTURE);
}

void CEasyGenDoc::OnUpdateViewWires(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pCmdUI->SetCheck(pView->GetRenderMode() == R_WIREFRAME);
}

void CEasyGenDoc::OnUpdateViewCulling(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pCmdUI->SetCheck(pView->m_ViewCulling);
}

void CEasyGenDoc::OnUpdateViewFlatshade(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pCmdUI->SetCheck(pView->m_ViewShadeModel == GL_FLAT);
}

void CEasyGenDoc::OnUpdateViewFlatsmooth(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pCmdUI->SetCheck(pView->m_ViewShadeModel == GL_SMOOTH);
}

void CEasyGenDoc::OnUpdateViewAlphamap(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pCmdUI->SetCheck(pView->GetRenderMode() == R_ALPHAMAP);
}

void CEasyGenDoc::OnUpdateViewAddWires(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	pCmdUI->SetCheck(pView->RenderFlagTest(R_FL_QUADFRAME));
	pCmdUI->Enable(pView->GetRenderMode() != R_WIREFRAME);
}

void CEasyGenDoc::OnPreferences()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	CString      oldmoddir;

	oldmoddir = g_Prefs.m_GameBasePath;

	g_Prefs.InputPreferences();

	if (g_Prefs.m_GameBasePath != oldmoddir)
	{
		SetWindowTitle(m_sFilename);
		AfxMessageBox("Since you've changed the work mod folder, you'd better to restart EasyGen.");
	}

	pView->SetBgColor(g_Prefs.m_BgColor);

	m_bInvalidateGridColor = true;

	RefreshView();
}

bool PolyAllTexturedBy(CNmPoly *poly, const char *texname)
{
	CNmFace *face;

	for (face = poly->m_Face; face; face = face->Next)
	{
		if (strcmp(face->GetTextureName(), texname) != 0)
		{
			return false;
		}
	}

	return true;
}

void CEasyGenDoc::InitBrushes(CNmPolyList& plist)
{
	CNmPoly        *poly;
	CNmPolySubList plsb;
	CNmFace        *face;
	CNmVec3        dir(1, 2, 3);
	CNmVec3        colors[3];
	CNmStr         tex;
	CNmListStr     ignoresh;
	int            curcol;
	float          col;
	CNmVec3        v;
	S_EGN_POLYFACE *data;
	bool           ignorepoly;

	colors[0].Set(64.f / 255.f, 64.f / 255.f, 196.f / 255.f);
	colors[1].Set(64.f / 255.f, 196.f / 255.f, 64.f / 255.f);
	colors[2].Set(196.f / 255.f, 64.f / 255.f, 64.f / 255.f);

	dir.Normalize();

	ignoresh.FromString((const char *)g_Prefs.m_sWorldShadersIgnoreList, ';');
	curcol = 0;

	SCAN_NM_POLY(plist, poly)
	{
		if (poly->m_Face != NULL)
		{
//			tex = poly->m_Face->GetTextureName();

			ignorepoly = false;

			ignoresh.ScanReset();
			while (ignoresh.Scan(tex))
			{
				if (PolyAllTexturedBy(poly, (const char *)tex))
				{
					ignorepoly = true;
					break;
				}
			}

//			if (!ignoresh.Find(tex))
			if (!ignorepoly)
			{
				for (face = poly->m_Face; face; face = face->Next)
				{
					col = (face->Plane->n * dir + 1) / 2;

					data          = new S_EGN_POLYFACE;
					data->f_color = colors[curcol] * col;

					face->SetDataPtr((void *)data);
				}

				curcol = (curcol + 1) % 3;
			}
			else // never draw brush
			{
				plsb.AddHead(poly);
			}
		}
	}

	u_RemovePolysInPListFromPSList_Safe(&plist, &plsb);
}

BOOL CEasyGenDoc::IsTextureInUse(CTexture *tex)
{
	return g_AlphamapMan.IsTextureInUse(tex);
}

bool CEasyGenDoc::LoadFromMap(LPCTSTR mapfilename)
{
	CImportMapOptionsDlg ImportOptionsDlg;
	CRadiantMap          map;

	CNmParser mapf;

	CNmStr      radbuf;
	CString     tests;
	CNmPolyList plylist1;

	// terrain stuff
	CNmStr terrain_alphamap, terrain_shader, terrain_layers;
	CNmStr shadername;

	CNmStr classname;
	CNmStr terrain_v;

	// md3 stuff
	CMd3     *md3;
	S_MD3PTR *item;
	char     wrkpath[_MAX_PATH];
	CString  fullshortpath;
	CNmStr   md3_file;
	CNmStr   md3_sorigin, s;
	CNmVec3  md3_vorigin;

	CNmVec3 gr_origin;

	int terrain_counter, dbg;

	ImportOptionsDlg.m_bModels     = FALSE;
	ImportOptionsDlg.m_bWorldspawn = FALSE;
	ImportOptionsDlg.m_sFilename   = mapfilename;

	if (ImportOptionsDlg.DoModal() != IDOK)
	{
		return false;
	}

	if (!map.Open(mapfilename))
	{
		return false;
	}

	// get back World to original position
//	g_Grid.GetOrigin(gr_origin);
//	g_World.Move(gr_origin);

	terrain_counter = 0;

	dbg = 0;

	while (map.ent_GetNext())
	{
		map.ent_GetValue("classname", classname);
		map.ent_GetValue("terrain", terrain_v);

		if (ImportOptionsDlg.m_dwTerrain != -1 &&
		    ImportOptionsDlg.m_dwTerrain == terrain_counter &&
		    classname == "func_group" && terrain_v == "1")
		{
			TRACE("Entity TERRAIN found\n");

			map.ent_GetValue("alphamap", terrain_alphamap);
			map.ent_GetValue("shader", terrain_shader);
			map.ent_GetValue("layers", terrain_layers);

			// process radbuf here
			plylist1.RemoveAll();
			plylist1.FromRadiantEntity(map.ent_GetPtr());

			UndoGet(&g_Grid);

			g_Grid.FromPolyList(plylist1);

			tests            = (const char *)(g_Prefs.m_GameBasePath);
			tests           += '\\';
			terrain_alphamap = (const char *)tests + terrain_alphamap;
			terrain_alphamap.Replace('/', '\\');

			g_AlphamapMan.Reset();

			if (g_Grid.AlphamapFromFile((const char *)terrain_alphamap))
			{
				g_AlphamapMan.FromFile((const char *)terrain_alphamap);

				shadername = "textures/" + terrain_shader;
				ImportTerrainShader((const char *)shadername);

				if (g_AlphamapMan.GetTex(0) != NULL)
				{
					SetCurrentTexture(g_AlphamapMan.GetTex(0));
				}
			}

		}
		else if (ImportOptionsDlg.m_bModels &&
		         classname == "misc_model")
		{
			TRACE("Entity MODEL found\n");

			map.ent_GetValue("model", md3_file);
			map.ent_GetValue("origin", md3_sorigin);

			sscanf((const char *)md3_sorigin, "%f %f %f", &md3_vorigin.x, &md3_vorigin.y, &md3_vorigin.z);

			GetShortPathName(g_Prefs.m_GameBasePath, wrkpath, _MAX_PATH);
			md3_file.Replace('/', '\\');

			fullshortpath  = wrkpath;
			fullshortpath += '\\';
			fullshortpath += md3_file;

			GetLongPathName((LPCTSTR)fullshortpath, wrkpath, _MAX_PATH);

			md3_file = wrkpath;
			md3_file.MakeLower();

			md3 = g_Md3Man.m_List.Get(md3_file);

			if (md3)
			{
				item       = g_Md3Man.m_Placed.Append();
				item->pMd3 = md3;

				item->m_egvPos = md3_vorigin;
			}

		}
		else if (ImportOptionsDlg.m_bWorldspawn &&
		         (classname == "worldspawn" || (classname == "func_group" && terrain_v != "1")))
		{
			TRACE("Entity WORLDSPAWN/func_group found\n");

			plylist1.RemoveAll();

//			s.Format("c:\\egn_%i.txt", dbg++);
//			Nm_SaveFile((const char*)s, map.ent_GetPtr());
			plylist1.FromRadiantEntity(map.ent_GetPtr());

			if (plylist1.GetNum() > 0)
			{
				TRACE("- polys: %d\n", plylist1.GetNum());

				InitBrushes(plylist1);
				plylist1.MoveToPolyList(&g_World);
				plylist1.RemoveAll();
			}

		}

		if (classname == "func_group" && terrain_v == "1")
		{
			terrain_counter++;
		}

	} //while (map.ent_GetNext())

	map.Close();

	// Set world to 'origin' position
//	g_Grid.GetOrigin(gr_origin);
//	gr_origin = -gr_origin;
//	g_World.Move(gr_origin);

	return true;
}
void CEasyGenDoc::OnToggleAxis()
{
	m_bShowAxis = !m_bShowAxis;
	RefreshView();
}

void CEasyGenDoc::OnUpdateToggleAxis(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowAxis);
}

void CEasyGenDoc::OnToggleFlatTris()
{
	g_Grid.OptionFlatTrisToggle();
	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnUpdateToggleFlatTris(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_Grid.OptionFlatTrisGet());
}

void CEasyGenDoc::RefreshView()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	ASSERT(pView);
	pView->InvalidateRect(NULL, FALSE);
}

void CEasyGenDoc::OnEditUndo()
{
	g_GridUndo.Pop(&g_Grid);

	UpdateFormView(TAB_GRID);

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!g_GridUndo.IsEmpty());
}

void CEasyGenDoc::UndoGet(CGrid *grid)
{
	g_GridUndo.Push(grid);
}

void CEasyGenDoc::OnEditCut()
{
}

void CEasyGenDoc::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(0);
}

void CEasyGenDoc::SetStatusText(UINT index, LPCTSTR text)
{
	CEasyGenApp *pApp       = (CEasyGenApp *)AfxGetApp();
	CMainFrame  *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	pMainFrame->SendStatusText(index, text);
}

void CEasyGenDoc::GridReCells(int CellsX, int CellsY, BOOL lock, int alignX, int alignY)
{
	UndoGet(&g_Grid);

	g_Grid.ReCellsX(CellsX, lock, alignX);
	g_Grid.ReCellsY(CellsY, lock, alignY);
	g_Grid.TextureScaleRefresh();

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::GridReWidths(float WidthX, float WidthY, BOOL lock, int alignX, int alignY)
{
	UndoGet(&g_Grid);

	g_Grid.ReWidthX((int)WidthX, lock, alignX);
	g_Grid.ReWidthY((int)WidthY, lock, alignY);
	g_Grid.TextureScaleRefresh();

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::GridReCellWidths(float cwX, float cwY, BOOL lock, int alignX, int alignY)
{
	UndoGet(&g_Grid);

	float wx, wy;

	wx = g_Grid.GetCellsX() * cwX;
	wy = g_Grid.GetCellsY() * cwY;

	g_Grid.ReWidthX((int)wx, lock, alignX);
	g_Grid.ReWidthY((int)wy, lock, alignY);
	g_Grid.TextureScaleRefresh();

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

BOOL CEasyGenDoc::SetAlphaTexture(CTexture *tex)
{
	int cursel = g_AlphamapMan.GetCurSel();

	if (cursel == -1)
	{
		return FALSE;
	}

	g_AlphamapMan.Set(cursel, tex);

	UpdateFormView(TAB_ALPHAMAP);

	return TRUE;
}

// Reads:	g_Grid
//			g_AlphamapMan
//
void CEasyGenDoc::WriteMetashader(LPCTSTR filename, LPCTSTR szMsFolder, LPCTSTR szMs, UINT nFlags)
{
	CString          ext;
	CString          s;
	S_ALPHAINDEXDATA *a, *a1, *a2;
	float            scalefx, scalefy;
	int              l, m;

	std::ofstream of;

	S_FACEDATAEX *f = g_Grid.GetFace(0, 0);

	of.open(filename, std::ios::out | std::ios::binary);

	CNmStr templ, templ_blend, templ_vertexremap, templ_work;

	templ.FromFile((LPCTSTR)g_Prefs.GetFullPath("templates\\standard_noblend.shader"));
	templ_blend.FromFile((LPCTSTR)g_Prefs.GetFullPath("templates\\standard_blend.shader"));
	templ_vertexremap.FromFile((LPCTSTR)g_Prefs.GetFullPath("templates\\standard_vertexremap.shader"));

	RemoveTemplateComments(templ);
	RemoveTemplateComments(templ_blend);
	RemoveTemplateComments(templ_vertexremap);

	for (l = 0; l < g_AlphamapMan.GetLayers(); l++)
	{
		templ_work = templ;

		s.Format("%s/%s", szMsFolder, szMs);
		templ_work.Replace("%NAME%", (LPCTSTR)s);

		s.Format("%d", l);
		templ_work.Replace("%I%", (LPCTSTR)s);

		a = g_AlphamapMan.Get(l);
		if (a->tex != NULL)
		{
			ext = GetExt(a->tex->m_sPath);
			s   = "textures/" + Q3A_TexturePathFilter(a->tex->m_sPath) + "." + ext;
		}
		else
		{
			AfxMessageBox("Writemetashader: texture not defined.");
			return;
//			w = "textures/NONE\r\n";
		}
		templ_work.Replace("%TEXTURE_I%", (LPCTSTR)s);

		scalefx = 0.125f * f->xstep * a->tex->m_glWidth / g_Grid.GetCellWidthX();
		scalefy = 0.125f * f->ystep * a->tex->m_glHeight / g_Grid.GetCellWidthY();
		s.Format("%0.9f %0.9f", scalefx, scalefy);
		templ_work.Replace("%TCMOD%", (LPCTSTR)s);

		of << (const char *)templ_work << "\r\n\r\n";
	} //for

	for (l = 0; l < g_AlphamapMan.GetLayers() - 1; l++)
	{
		for (m = l + 1; m < g_AlphamapMan.GetLayers(); m++)
		{
			templ_work = templ_blend;

			s.Format("%s/%s", szMsFolder, szMs);
			templ_work.Replace("%NAME%", (LPCTSTR)s);

			s.Format("%d", l);
			templ_work.Replace("%I%", (LPCTSTR)s);
			s.Format("%d", m);
			templ_work.Replace("%J%", (LPCTSTR)s);

			a1  = g_AlphamapMan.Get(l);
			ext = GetExt(a1->tex->m_sPath);
			s   = "textures/" + Q3A_TexturePathFilter(a1->tex->m_sPath) + "." + ext;
			templ_work.Replace("%TEXTURE_I%", (LPCTSTR)s);

			a2  = g_AlphamapMan.Get(m);
			ext = GetExt(a2->tex->m_sPath);
			s   = "textures/" + Q3A_TexturePathFilter(a2->tex->m_sPath) + "." + ext;
			templ_work.Replace("%TEXTURE_J%", (LPCTSTR)s);

			scalefx = 0.125f * f->xstep * a1->tex->m_glWidth / g_Grid.GetCellWidthX();
			scalefy = 0.125f * f->ystep * a1->tex->m_glHeight / g_Grid.GetCellWidthY();
			s.Format("%0.9f %0.9f", scalefx, scalefy);
			templ_work.Replace("%TCMOD%", (LPCTSTR)s); // two replaces should occur

			of << (const char *)templ_work << "\r\n\r\n";

		} //for
	} //for

	// vertexmapshader
	templ_work = templ_vertexremap;

	s.Format("%s/%s", szMsFolder, szMs);
	templ_work.Replace("%NAME%", (LPCTSTR)s);

	a   = g_AlphamapMan.Get(0);
	ext = GetExt(a->tex->m_sPath);
	s   = "textures/" + Q3A_TexturePathFilter(a->tex->m_sPath) + "." + ext;
	templ_work.Replace("%TEXTURE_0%", (LPCTSTR)s);

	scalefx = 0.125f * f->xstep * a->tex->m_glWidth / g_Grid.GetCellWidthX();
	scalefy = 0.125f * f->ystep * a->tex->m_glHeight / g_Grid.GetCellWidthY();
	s.Format("%0.9f %0.9f", scalefx, scalefy);
	templ_work.Replace("%TCMOD%", (LPCTSTR)s);

	of << (const char *)templ_work;

	of.close();
}

void ResetSTRI(S_SHADERTEMPLATEREPLACEINFO& stri)
{
	stri.sMETASHADER_NAME = "";

	stri.sI = "";
	stri.sJ = "";

	stri.sTEXTURE_I_MAP   = "";
	stri.sTEXTURE_I_SIZEX = "";
	stri.sTEXTURE_I_SIZEY = "";

	stri.sTEXTURE_J_MAP   = "";
	stri.sTEXTURE_J_SIZEX = "";
	stri.sTEXTURE_J_SIZEY = "";

	stri.sTEXTURE_0_MAP   = "";
	stri.sTEXTURE_0_SIZEX = "";
	stri.sTEXTURE_0_SIZEY = "";
	stri.sTEXTURE_0_SGEN  = "";
	stri.sTEXTURE_0_TGEN  = "";
	stri.sTEXTURE_0_TCMOD = "";

	stri.sMESH_WIDTHX    = "";
	stri.sMESH_WIDTHY    = "";
	stri.sMESH_DIVWIDTHX = "";
	stri.sMESH_DIVWIDTHY = "";
}

void LoadSTRI(
    S_SHADERTEMPLATEREPLACEINFO& stri,
    int i,
    int j,
    LPCTSTR szMsFolder, LPCTSTR szMs,
    S_ALPHAINDEXDATA *a0,
    S_ALPHAINDEXDATA *ai,
    S_ALPHAINDEXDATA *aj
    )
{
	CString      ext;
	int          sgen, tgen;
	float        scalefx, scalefy;
	S_FACEDATAEX *f = g_Grid.GetFace(0, 0);

	stri.sMETASHADER_NAME.Format("%s/%s", szMsFolder, szMs);
	stri.sI.Format("%d", i);
	stri.sJ.Format("%d", j);

	if (a0 != NULL)
	{
		ext                 = GetExt(a0->tex->m_sPath);
		stri.sTEXTURE_0_MAP = "textures/" + Q3A_TexturePathFilter(a0->tex->m_sPath) + "." + ext;
		stri.sTEXTURE_0_SIZEX.Format("%d", a0->tex->m_glWidth);
		stri.sTEXTURE_0_SIZEY.Format("%d", a0->tex->m_glHeight);
		scalefx = 0.125f * f->xstep * a0->tex->m_glWidth / g_Grid.GetCellWidthX();
		scalefy = 0.125f * f->ystep * a0->tex->m_glHeight / g_Grid.GetCellWidthY();
		stri.sTEXTURE_0_TCMOD.Format("%0.3f %0.3f", scalefx, scalefy);
		g_Grid.GetSTGen(sgen, tgen);
		stri.sTEXTURE_0_SGEN.Format("%d 0 0", sgen);
		stri.sTEXTURE_0_TGEN.Format("0 %d 0", tgen);
	}

	if (ai != NULL)
	{
		ext                 = GetExt(ai->tex->m_sPath);
		stri.sTEXTURE_I_MAP = "textures/" + Q3A_TexturePathFilter(ai->tex->m_sPath) + "." + ext;
		stri.sTEXTURE_I_SIZEX.Format("%d", ai->tex->m_glWidth);
		stri.sTEXTURE_I_SIZEY.Format("%d", ai->tex->m_glHeight);
	}

	if (aj != NULL)
	{
		ext                 = GetExt(aj->tex->m_sPath);
		stri.sTEXTURE_J_MAP = "textures/" + Q3A_TexturePathFilter(aj->tex->m_sPath) + "." + ext;
		stri.sTEXTURE_J_SIZEX.Format("%d", aj->tex->m_glWidth);
		stri.sTEXTURE_J_SIZEY.Format("%d", aj->tex->m_glHeight);
	}

	stri.sMESH_DIVWIDTHX.Format("%1.0f", g_Grid.GetCellWidthX());
	stri.sMESH_DIVWIDTHX.Format("%1.0f", g_Grid.GetCellWidthY());
	stri.sMESH_WIDTHX.Format("%1.0f", g_Grid.GetWidthX());
	stri.sMESH_WIDTHY.Format("%1.0f", g_Grid.GetWidthY());
}

void TemplateFormat(CNmStr& t, S_SHADERTEMPLATEREPLACEINFO *stri)
{
	CNmStr sI, sJ;

	t.Replace("%METASHADER_NAME%", (const char *)stri->sMETASHADER_NAME);
	t.Replace("%I%", (const char *)stri->sI);
	t.Replace("%J%", (const char *)stri->sJ);

	t.Replace("%TEXTURE_0_MAP%", (const char *)stri->sTEXTURE_0_MAP);
	t.Replace("%TEXTURE_0_SIZEX%", (const char *)stri->sTEXTURE_0_SIZEX);
	t.Replace("%TEXTURE_0_SIZEY%", (const char *)stri->sTEXTURE_0_SIZEY);
	t.Replace("%TEXTURE_0_SGEN%", (const char *)stri->sTEXTURE_0_SGEN);
	t.Replace("%TEXTURE_0_TGEN%", (const char *)stri->sTEXTURE_0_TGEN);
	t.Replace("%TEXTURE_0_TCMOD%", (const char *)stri->sTEXTURE_0_TCMOD);

	t.Replace("%TEXTURE_I_MAP%", (const char *)stri->sTEXTURE_I_MAP);
	t.Replace("%TEXTURE_I_SIZEX%", (const char *)stri->sTEXTURE_I_SIZEX);
	t.Replace("%TEXTURE_I_SIZEY%", (const char *)stri->sTEXTURE_I_SIZEY);

	t.Replace("%TEXTURE_J_MAP%", (const char *)stri->sTEXTURE_J_MAP);
	t.Replace("%TEXTURE_J_SIZEX%", (const char *)stri->sTEXTURE_J_SIZEX);
	t.Replace("%TEXTURE_J_SIZEY%", (const char *)stri->sTEXTURE_J_SIZEY);

	t.Replace("%MESH_DIVWIDTHX%", (const char *)stri->sMESH_DIVWIDTHX);
	t.Replace("%MESH_DIVWIDTHY%", (const char *)stri->sMESH_DIVWIDTHY);
	t.Replace("%MESH_WIDTHX%", (const char *)stri->sMESH_WIDTHX);
	t.Replace("%MESH_WIDTHY%", (const char *)stri->sMESH_WIDTHY);
}

void GenMetashaderEx(CString& buffer, LPCTSTR szMsFolder, LPCTSTR szMs, UINT nFlags, LPCTSTR szNoBlendT, LPCTSTR szBlendT, LPCTSTR szVertexT)
{
	CNmStr                      templ_work;
	CString                     ext;
	CString                     s;
	S_SHADERTEMPLATEREPLACEINFO stri;
	int                         i, j;

	S_FACEDATAEX *f = g_Grid.GetFace(0, 0);

	buffer = "";

	for (i = 0; i < g_AlphamapMan.GetLayers(); i++)
	{
		templ_work = szNoBlendT;
		ResetSTRI(stri);

		LoadSTRI(
		    stri,
		    i,
		    -1,
		    szMsFolder, szMs,
		    g_AlphamapMan.Get(0),
		    g_AlphamapMan.Get(i),
		    NULL
		    );

		TemplateFormat(templ_work, &stri);

		buffer += (const char *)templ_work;
		buffer += "\r\n\r\n";
	} //for

	for (i = 0; i < g_AlphamapMan.GetLayers() - 1; i++)
	{
		for (j = i + 1; j < g_AlphamapMan.GetLayers(); j++)
		{
			templ_work = szBlendT;
			ResetSTRI(stri);

			LoadSTRI(
			    stri,
			    i,
			    j,
			    szMsFolder, szMs,
			    g_AlphamapMan.Get(0),
			    g_AlphamapMan.Get(i),
			    g_AlphamapMan.Get(j)
			    );

			TemplateFormat(templ_work, &stri);

			buffer += (const char *)templ_work;
			buffer += "\r\n\r\n";
		} //for
	} //for

	// vertexmapshader
	templ_work = szVertexT;
	ResetSTRI(stri);

	LoadSTRI(
	    stri,
	    -1,
	    -1,
	    szMsFolder, szMs,
	    g_AlphamapMan.Get(0),
	    NULL,
	    NULL
	    );

	TemplateFormat(templ_work, &stri);

	buffer += (const char *)templ_work;
}

void CEasyGenDoc::WriteAlphamap(LPCTSTR filename)
{
	EasyImage  bmp;
	BTCOLORREF btcol;
	COLORREF   col;
	int        x, y, l;
	CString    ext;

	ext = filename;
	ext = ext.Right(3);
	ext.MakeLower();

	bmp.Create(g_Grid.GetSizeX(), g_Grid.GetSizeY(), 8);

	for (l = 0; l < g_AlphamapMan.GetLayers(); l++)
	{
		col   = g_AlphamapMan.GetCol(l);
		btcol = BTRGB(GetRValue(col), GetGValue(col), GetBValue(col));
		bmp.SetColorForIndex(l, btcol);
	}

	for (l = g_AlphamapMan.GetLayers(); l < 256; l++)
	{
		btcol = BTRGB(0, 0, 0);
		bmp.SetColorForIndex(l, btcol);
	}

	for (y = 0; y < bmp.GetHeight(); y++)
	{
		for (x = 0; x < bmp.GetWidth(); x++)
		{
			bmp.SetColorForPixel(x, y, g_Grid.GetAlphaIndex(x, y));
		}
	}

	bmp.Flip();

	if (ext == "pcx")
	{
		bmp.Save(filename, BTFILETYPE_PCX);
	}
	else
	{
		bmp.Save(filename);
	}

	bmp.ClearData();
}


void CEasyGenDoc::DeleteAlphaIndex(int index)
{
	int   i, j, k;
	int   ii;
	float *z;

	g_Grid.ScanFullSetup();

	while (g_Grid.ScanFull(i, j, k, z))
	{
		ii = g_Grid.GetAlphaIndex(k);

		if (ii > index)
		{
			g_Grid.SetAlphaIndex(k, ii - 1);
		}
		else
		if (ii == index)
		{
			g_Grid.SetAlphaIndex(k, 0);
		}
	}

	RefreshView();
}

void CEasyGenDoc::SwapAlphaIndexes(int i1, int i2)
{
	int   i, j, k;
	int   ii;
	float *z;

	g_Grid.ScanFullSetup();

	while (g_Grid.ScanFull(i, j, k, z))
	{
		ii = g_Grid.GetAlphaIndex(k);

		if (ii == i1)
		{
			g_Grid.SetAlphaIndex(k, i2);
		}
		else
		if (ii == i2)
		{
			g_Grid.SetAlphaIndex(k, i1);
		}
	}
}

BOOL CEasyGenDoc::CanMapBeExported()
{
	CString s;
	int     i, j, k, l;
	float   *z;

	if (g_Prefs.m_GameBasePath == "")
	{
		AfxMessageBox("You haven't still located your Quake3 work mod folder.\r\n\r\n"
		              "Open Preferences (Edit/Preferences) and browse for it now\r\n"
		              "or EasyGen will not be able to export.");
		return FALSE;
	}

	for (l = 0; l < g_AlphamapMan.GetLayers(); l++)
	{
		if (g_AlphamapMan.GetTex(l) == NULL)
		{
			s.Format("Texture not defined for palette index %d", l);
			AfxMessageBox((LPCTSTR)s);
			return FALSE;
		}

		if (g_AlphamapMan.GetCol(l) == BLACK)
		{
			s.Format("Color for index %d can't be BLACK", l);
			AfxMessageBox((LPCTSTR)s);
			return FALSE;
		}
	} //layers

	g_Grid.ScanFullSetup();
	while (g_Grid.ScanFull(i, j, k, z))
	{
		l = g_Grid.GetAlphaIndex(k);

		if ((l < 0) || (l >= g_AlphamapMan.GetLayers()))
		{
			s.Format("Vertex at %d %d has an invalid alphamap palette index\r\n\r\n"
			         "You must define the alphamap palette first.", i, j);
			AfxMessageBox((LPCTSTR)s);
			return FALSE;
		}
	}

	return TRUE;
}

int CEasyGenDoc::GetActiveTab()
{
	CCmdFormView *pForm = (CCmdFormView *)GetFormView();

	return pForm->m_TabActive;
}

void CEasyGenDoc::OnFileExportAlphamap()
{
	CFileDialog fd(FALSE,
	               "",
	               GetFileNameNoExtNoPath(m_sFilename),
	               OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	               "PCX image (*.pcx)|*.pcx|PNG image (*.png)|*.png|BMP image (*.bmp)|*.bmp||",
	               NULL);

	CString s, alpha;

	alpha = g_Prefs.m_GameBasePath;

	fd.m_ofn.lpstrTitle      = "Export - Alphamap";
	fd.m_ofn.lpstrInitialDir = alpha;

	if (fd.DoModal() == IDOK)
	{

		WriteAlphamap((LPCTSTR)fd.GetPathName());
	}
}

void CEasyGenDoc::OnFileImportAlphamap()
{
	CFileDialog fd(
	    TRUE,
	    NULL,
	    NULL,
	    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	    "PCX image (*.pcx)|*.pcx|PNG image (*.png)|*.png|BMP image (*.bmp)|*.bmp||",
	    NULL
	    );

	CString s, alpha;

	alpha  = g_Prefs.m_GameBasePath;
	alpha += "\\";

	fd.m_ofn.lpstrTitle      = "Import - Alphamap";
	fd.m_ofn.lpstrInitialDir = alpha;

	if (fd.DoModal() == IDOK)
	{
		s = fd.GetPathName();

		if (g_Grid.AlphamapFromFile((LPCTSTR)s))
		{
			g_AlphamapMan.FromFile((LPCTSTR)s);
		}
		else
		{
			AfxMessageBox("Error importing the alphamap.\n\n"
			              "Possible cause could be wrong width and/or height of PCX image");
			return;
		}

		UpdateFormView(TAB_ALPHAMAP);

		RefreshView();
		Sleep(SLEEP_TIME);
	}
}

void CEasyGenDoc::OnFileExportMetashader()
{
	AfxMessageBox("To export the metashader use the 'Quake 3 map' export function\r\n"
	              "and disable 'save map' & 'save alphamap'");
}

void CEasyGenDoc::OnViewTrisframe()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->RenderFlagReset(R_FL_QUADFRAME);
	pView->RenderFlagSet(R_FL_TRISFRAME);
	RefreshView();
}

void CEasyGenDoc::OnUpdateViewTrisframe(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	pCmdUI->SetCheck(pView->RenderFlagTest(R_FL_TRISFRAME));
	pCmdUI->Enable(pView->GetRenderMode() != R_WIREFRAME);
}

void CEasyGenDoc::OnViewNoframe()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	pView->RenderFlagReset(R_FL_QUADFRAME);
	pView->RenderFlagReset(R_FL_TRISFRAME);
	RefreshView();
}

void CEasyGenDoc::OnUpdateViewNoframe(CCmdUI *pCmdUI)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	pCmdUI->SetCheck(!pView->RenderFlagTest(R_FL_QUADFRAME) &&
	                 !pView->RenderFlagTest(R_FL_TRISFRAME));
	pCmdUI->Enable(pView->GetRenderMode() != R_WIREFRAME);
}

void CEasyGenDoc::GridReMaxHeight(float h)
{
	UndoGet(&g_Grid);

	g_Grid.SetMaxHeight(h);

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::ModifierNone()
{
	CCmdFormView *pForm = (CCmdFormView *)GetFormView();

	ModSelect(MOD_NONE);
	UpdateFormView(TAB_MODIFIER);

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnFlip()
{
	CFlipDlg flipdlg;

	flipdlg.m_bEnlarge = FALSE;
	flipdlg.m_rMode    = 0;

	if (flipdlg.DoModal() != IDOK)
	{
		return;
	}

	UndoGet(&g_Grid);

	if (flipdlg.m_bEnlarge)
	{
		CGrid *tmpgrid = new CGrid;
		int   i, j, k;
		int   r;
		float *z;

		if ((flipdlg.m_rMode == FLIP_A) || (flipdlg.m_rMode == FLIP_E))
		{
			tmpgrid->Set(g_Grid.GetCellsX(),
			             g_Grid.GetCellsY() * 2,
			             g_Grid.GetWidthX(),
			             g_Grid.GetWidthY() * 2
			             );

			g_Grid.ScanFullSetup();
			while (g_Grid.ScanFull(i, j, k, z))
			{
				r = g_Grid.GetCellsY();

				tmpgrid->SetZ(i, j + r, *z);
				tmpgrid->SetAlphaIndex(i, j + r, g_Grid.GetAlphaIndex(k));
			}

			g_Grid = *tmpgrid;
		}
		else if ((flipdlg.m_rMode == FLIP_B) || (flipdlg.m_rMode == FLIP_F))
		{
			tmpgrid->Set(g_Grid.GetCellsX(),
			             g_Grid.GetCellsY() * 2,
			             g_Grid.GetWidthX(),
			             g_Grid.GetWidthY() * 2
			             );

			g_Grid.ScanFullSetup();
			while (g_Grid.ScanFull(i, j, k, z))
			{
				tmpgrid->SetZ(i, j, *z);
				tmpgrid->SetAlphaIndex(i, j, g_Grid.GetAlphaIndex(k));
			}

			g_Grid = *tmpgrid;
		}
		else if ((flipdlg.m_rMode == FLIP_C) || (flipdlg.m_rMode == FLIP_G))
		{
			tmpgrid->Set(g_Grid.GetCellsX() * 2,
			             g_Grid.GetCellsY(),
			             g_Grid.GetWidthX() * 2,
			             g_Grid.GetWidthY()
			             );

			g_Grid.ScanFullSetup();
			while (g_Grid.ScanFull(i, j, k, z))
			{
				r = g_Grid.GetCellsX();

				tmpgrid->SetZ(i + r, j, *z);
				tmpgrid->SetAlphaIndex(i + r, j, g_Grid.GetAlphaIndex(k));
			}

			g_Grid = *tmpgrid;
		}
		else if ((flipdlg.m_rMode == FLIP_D) || (flipdlg.m_rMode == FLIP_H))
		{
			tmpgrid->Set(g_Grid.GetCellsX() * 2,
			             g_Grid.GetCellsY(),
			             g_Grid.GetWidthX() * 2,
			             g_Grid.GetWidthY()
			             );

			g_Grid.ScanFullSetup();
			while (g_Grid.ScanFull(i, j, k, z))
			{
				tmpgrid->SetZ(i, j, *z);
				tmpgrid->SetAlphaIndex(i, j, g_Grid.GetAlphaIndex(k));
			}

			g_Grid = *tmpgrid;
		}

		delete tmpgrid;
	} // enlarge ?

	switch (flipdlg.m_rMode)
	{
	case FLIP_A:
		g_Grid.FlipV(FALSE, TRUE);
		g_Grid.FlipH(TRUE, FALSE);
		break;
	case FLIP_B:
		g_Grid.FlipV(TRUE, TRUE);
		g_Grid.FlipV(FALSE, TRUE);
		g_Grid.FlipH(TRUE, FALSE);
		g_Grid.FlipV(TRUE, TRUE);
		break;
	case FLIP_C:
		g_Grid.FlipH(FALSE, TRUE);
		g_Grid.FlipV(TRUE, FALSE);
		break;
	case FLIP_D:
		g_Grid.FlipH(TRUE, TRUE);
		g_Grid.FlipH(FALSE, TRUE);
		g_Grid.FlipV(TRUE, FALSE);
		g_Grid.FlipH(TRUE, TRUE);
		break;
	case FLIP_E:
		g_Grid.FlipV(FALSE, TRUE);
		break;
	case FLIP_F:
		g_Grid.FlipV(TRUE, TRUE);
		g_Grid.FlipV(FALSE, TRUE);
		g_Grid.FlipV(TRUE, TRUE);
		break;
	case FLIP_G:
		g_Grid.FlipH(FALSE, TRUE);
		break;
	case FLIP_H:
		g_Grid.FlipH(TRUE, TRUE);
		g_Grid.FlipH(FALSE, TRUE);
		g_Grid.FlipH(TRUE, TRUE);
		break;
	}

	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

// Reset EasyGen to a new terrain
void CEasyGenDoc::ResetAll()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();
	CTextureMan  *pTMan = (CTextureMan *)GetTextureView();

	// member variables
	///////////////////
	m_dwPaintRadius = 0;

	m_bShowAxis  = TRUE;
	m_bShowWorld = TRUE;

	m_Modifier = MOD_NONE;

	m_sFilename = "";

	m_modX          = 0;
	m_modY          = 0;
	m_modXYstep     = 8.f;
	m_modStrength   = 0;
	m_modRadius     = 0;
	m_modBaseNoise  = 0;
	m_modBaseSmooth = 0;
	m_modBaseScale  = 1.f;
	m_modMode       = MOD_MODE_MAX;
	m_modShow       = TRUE;

	m_vPlayer.Set(0.f, 0.f, 0.f);

	// global variables
	///////////////////
	g_Grid.ResetDefault();
	g_GridUndo.Reset();
	DisposeBrushes();       //toghether
	g_World.RemoveAll();    //toghether
	g_Mod.SelectModifier(0);
	g_AlphamapMan.Reset();
	g_Md3Man.ResetNewMap();

	// other objects
	////////////////
	if (pView)
	{
		pView->InitGeometry();
	}
	if (pTMan)
	{
		pTMan->RefreshLocking();
		pTMan->Clear();
	}
}

void CEasyGenDoc::ResetCamera()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	pView->m_Eye.Set(g_Grid.GetX(0),
	                 g_Grid.GetY(0),
	                 g_Grid.GetMaxHeight() + g_Grid.GetMaxHeight() * 2 / 3 + 64);

	pView->m_yRot = -45;
	pView->m_xRot = 45;
}

void CEasyGenDoc::RenderFloor()
{
	int i, j;

	glBegin(GL_LINES);

	glColor3f(0.5f, 0.5f, 0.5f);

	for (i = 0; i < g_Grid.GetSizeX(); i++)
	{
		glVertex3f(g_Grid.GetX(i), g_Grid.GetY(0), 0.f);
		glVertex3f(g_Grid.GetX(i), g_Grid.GetY(g_Grid.GetSizeY() - 1), 0.f);
	}

	for (j = 0; j < g_Grid.GetSizeY(); j++)
	{
		glVertex3f(g_Grid.GetX(0), g_Grid.GetY(j), 0.f);
		glVertex3f(g_Grid.GetX(g_Grid.GetSizeX() - 1), g_Grid.GetY(j), 0.f);
	}

	glEnd();
}

void CEasyGenDoc::RenderAxes()
{
	int     j, i;
	float   x, y, z;
	GLfloat linewidth;
	// axis X,Y,Z
	float l  = 0.f;
	float lc = 1.f;     // light color
	float dc = 0.1f;    // dark color

	glPushMatrix();
	glTranslated(0.f, 0.f, 8.f);

	glGetFloatv(GL_LINE_WIDTH, &linewidth);
	glLineWidth(4.f);

	glDisable(GL_POLYGON_OFFSET_LINE);

	// X axis
	glBegin(GL_LINE_STRIP);
	glColor3f(lc, dc, dc);
	y = g_Grid.GetY(g_Grid.GetSizeY() / 2);
	for (i = 0; i < g_Grid.GetSizeX(); i++)
	{
		glVertex3f(g_Grid.GetX(i), y, g_Grid.GetZ(i, g_Grid.GetSizeY() / 2));
	}
	glEnd();

	// X axis arrow
	glBegin(GL_LINES);
	x = g_Grid.GetX(g_Grid.GetSizeX() - 1);
	y = g_Grid.GetY(g_Grid.GetSizeY() / 2);
	z = g_Grid.GetZ(g_Grid.GetSizeX() - 1, g_Grid.GetSizeY() / 2);
	glVertex3f(x, y, z);
	x += 320.f;
	glVertex3f(x, y, z);
	glEnd();
	glBegin(GL_LINE_STRIP);
	x -= 256.f;
	y += 128;
	glVertex3f(x, y, z);
	x += 256.f;
	y -= 128.f;
	glVertex3f(x, y, z);
	x -= 256.f;
	y -= 128.f;
	glVertex3f(x, y, z);
	glEnd();

	// Y axis
	glBegin(GL_LINE_STRIP);
	glColor3f(dc, lc, dc);
	x = g_Grid.GetX(g_Grid.GetSizeX() / 2);
	for (j = 0; j < g_Grid.GetSizeY(); j++)
	{
		glVertex3f(x, g_Grid.GetY(j), g_Grid.GetZ(g_Grid.GetSizeX() / 2, j));
	}
	glEnd();

	// Y axis arrow
	glBegin(GL_LINES);
	x = g_Grid.GetX(g_Grid.GetSizeX() / 2);
	y = g_Grid.GetY(g_Grid.GetSizeY() - 1);
	z = g_Grid.GetZ(g_Grid.GetSizeX() / 2, g_Grid.GetSizeY() - 1);
	glVertex3f(x, y, z);
	y += 320.f;
	glVertex3f(x, y, z);
	glEnd();
	glBegin(GL_LINE_STRIP);
	y -= 256.f;
	x += 128;
	glVertex3f(x, y, z);
	y += 256.f;
	x -= 128.f;
	glVertex3f(x, y, z);
	y -= 256.f;
	x -= 128.f;
	glVertex3f(x, y, z);
	glEnd();

	// extern axes
/*	glBegin(GL_LINES);
        glColor3f(lc,dc,dc);
        glVertex3f(g_Grid.GetX(0)-l,g_Grid.GetY(0)-l,0.f);
        glVertex3f(g_Grid.GetX(g_Grid.GetCellsX()),g_Grid.GetY(0)-l,0.f);
        glColor3f(dc,lc,dc);
        glVertex3f(g_Grid.GetX(0)-l,g_Grid.GetY(0)-l,0.f);
        glVertex3f(g_Grid.GetX(0)-l,g_Grid.GetY(g_Grid.GetCellsY()),0.f);
    glEnd();*/
	//

	glLineWidth(linewidth);
	glPopMatrix();
}

void CEasyGenDoc::RenderModels()
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	S_MD3PTR *item;
	S_MD3PTR *m;
	g_Md3Man.m_Placed.ScanReset();

	while (g_Md3Man.m_Placed.Scan(item))
	{
		item->pMd3->glDraw(item->m_egvPos.x, item->m_egvPos.y, item->m_egvPos.z);

/*		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glDisable(GL_CULL_FACE);
        glColor3f(1.f,0.f,0.f);

        m = item;

        glCube(
            m->m_egvPos.x,
            m->m_egvPos.y,
            m->m_egvPos.z,
            m->pMd3->m_vRay.x,
            m->pMd3->m_vRay.y,
            m->pMd3->m_vRay.z
            );

        glEnable(GL_CULL_FACE);
*/
	}

	if (pView->m_Md3Highlight)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		glColor3f(0.f, 1.f, 0.f);

		m = pView->m_Md3Highlight;

		glCube(
		    m->m_egvPos.x,
		    m->m_egvPos.y,
		    m->m_egvPos.z,
		    m->pMd3->m_vRay.x,
		    m->pMd3->m_vRay.y,
		    m->pMd3->m_vRay.z
		    );

		glEnable(GL_CULL_FACE);
	}
}

CMd3 *CEasyGenDoc::GetMd3SelectedTabModels()
{
	CCmdFormView *pForm = (CCmdFormView *)GetFormView();

	if (pForm)
	{
		return pForm->m_Tab4.GetSelected();
	}

	return 0;
}

void CEasyGenDoc::RecalcModelsZ()
{
	S_MD3PTR  *item;
	S_POSINFO info;

	g_Md3Man.m_Placed.ScanReset();
	while (g_Md3Man.m_Placed.Scan(item))
	{
		if (g_Grid.GetInfo(item->m_egvPos.x, item->m_egvPos.y, &info))
		{
			item->m_egvPos.z = info.z;
		}
		else
		{
			item->m_egvPos.z = 0;
		}
	}
}

int CEasyGenDoc::GetModifier()
{
	return m_Modifier;
}

void CEasyGenDoc::OnToggleSlopeTris()
{
	g_Grid.OptionSlopeTrisToggle();
	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnUpdateToggleSlopeTris(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_Grid.OptionSlopeTrisGet());
}

void CEasyGenDoc::OnToggleExcludedtris()
{
	g_Grid.OptionExcludedTrisToggle();
	m_bInvalidateGridPreview = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnUpdateToggleExcludedtris(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_Grid.OptionExcludedTrisGet());
}

void CEasyGenDoc::OnFileImportFromMap()
{
	CTextureMan *pTMan = (CTextureMan *)GetTextureView();
	CString     idir;
	CFileDialog fd(
	    TRUE,
	    "map",
	    "",
	    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	    "Radiant map files (*.map)|*.map||",
	    NULL
	    );

	idir.Format("%s\\maps", g_Prefs.m_GameBasePath);

	fd.m_ofn.lpstrTitle      = "Choose a map file...";
	fd.m_ofn.lpstrInitialDir = idir;

	if (fd.DoModal() != IDOK)
	{
		return;
	}

	////////////////
//	ResetAll();
	////////////////

	if (!LoadFromMap((LPCTSTR)fd.GetPathName()))
	{
		return;
	}

//	SetWindowTitle("");

	UpdateFormView(TAB_GRID);
	UpdateFormView(TAB_MODIFIER);
	UpdateFormView(TAB_ALPHAMAP);
	UpdateFormView(TAB_MODELS);

	// NO TEXTURE SUPPORT LOADING FROM MAP
// g_Grid & g_AlphamapMan are set in LoadFromMap
//	if (g_AlphamapMan.GetTex(0) != NULL)
//	{
//		SetCurrentTexture( g_AlphamapMan.GetTex(0) );
//	}
//	ASSERT(pTMan);
//	pTMan->SelectTexture( g_AlphamapMan.GetTex(0) );
//	pTMan->RefreshLocking();
//	pTMan->Invalidate();

	m_bInvalidateGridPreview = TRUE;
	m_bInvalidateGridColor   = TRUE;

	if (pTMan)
	{
		pTMan->Invalidate();
	}

//	ResetCamera();

	RefreshView();

	Sleep(SLEEP_TIME);
}

void CEasyGenDoc::OnTerrainClearExcludedTris()
{
	CTris *t;

	UndoGet(&g_Grid);

	g_Grid.ScanTrisSetup();
	while (g_Grid.ScanTris(t))
	{
		t->SetExcluded(false);
	}

	m_bInvalidateGridColor = TRUE;
	RefreshView();
}

/*
void CEasyGenDoc::RenderWorld()
{
    CNmPoly*	poly;

    GLfloat	ambientProperties[] = {0.7f,0.7f,0.7f,1.0f};
    GLfloat	diffuseProperties[] = {1.f,1.f,1.f,1.0f};
    GLfloat m_LightPos[]		= {0.0f,1024.f,128.0f,1.0f};

    glLightfv( GL_LIGHT1, GL_AMBIENT, ambientProperties);
    glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuseProperties);
    glLightfv( GL_LIGHT1, GL_POSITION, m_LightPos);			// Position The Light

    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);

    SCAN_NM_POLY(g_World,poly)
    {
//		glColor3f(1.f,0.5f,1.f);
        poly->glDrawFaces(false);
    }

    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
}
*/

/*
glPolygonOffset(1,1);
glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
glEnable(GL_POLYGON_OFFSET_FILL);
SetMaterialsForFilled();
RenderObjects();
glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
glDisable(GL_POLYGON_OFFSET_FILL);
SetMaterialsForLine();
RenderObjects();
*/
void CEasyGenDoc::RenderWorld()
{
	CNmPoly        *poly;
	CNmFace        *face;
	CNmVec3        n, t_origin;
	CNmVec3        *v;
	int            i;
	S_EGN_POLYFACE *data;

	g_Grid.GetOrigin(t_origin);

	glPushMatrix();

	glTranslatef(-t_origin.x, -t_origin.y, -t_origin.z);

//	glEnable(GL_POLYGON_OFFSET_LINE);
//	glPolygonOffset(1,1);

	SCAN_NM_POLY(g_World, poly)
	{
		for (face = poly->m_Face; face; face = face->Next)
		{
			face->GetDataPtr((void *&)data);

			if (data == NULL)
			{
				continue;
			}

			glColor3f(data->f_color.x, data->f_color.y, data->f_color.z);

			n = face->Plane->n;
//			n.Invert();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBegin(GL_POLYGON);

			glNormal3f(n.x, n.y, n.z);

			for (i = 0; i < face->m_NumFcPts; i++)
			{
				v = face->_FcPtGetAt(i)->vecptr;
				glVertex3f(v->x, v->y, v->z);
			}

			glEnd();
		}
	}

	glPopMatrix();
}

void CEasyGenDoc::OnWorldRemove()
{
	DisposeBrushes();
	g_World.RemoveAll();
	RefreshView();
}

void CEasyGenDoc::OnWorldHide()
{
	m_bShowWorld = FALSE;
	RefreshView();
}

void CEasyGenDoc::OnUpdateWorldHide(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bShowWorld);
}

void CEasyGenDoc::OnWorldShow()
{
	m_bShowWorld = TRUE;
	RefreshView();
}

void CEasyGenDoc::OnUpdateWorldShow(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bShowWorld);
}

void CEasyGenDoc::Posterize(int levels)
{
	CTextureMan   *pTMan = (CTextureMan *)GetTextureView();
	EasyImage     bmp;
	int           i, x, y;
	float         maxh, minh;
	unsigned char rgb;
	BTCOLORREF    col;

	if ((levels < 2) || (levels > 255))
	{
		return;
	}

	bmp.Create(g_Grid.GetSizeX(), g_Grid.GetSizeY(), 32);

	// build heightmap (blue)
	maxh = g_Grid.GetMaxHeight();
	minh = g_Grid.GetMinHeight();
	for (y = 0; y < bmp.GetHeight(); y++)
	{
		for (x = 0; x < bmp.GetWidth(); x++)
		{
			rgb = unsigned char(255 * ((g_Grid.GetZ(x, y) - minh) / (maxh - minh)));
			bmp.SetColorForPixel(x, y, BTRGB(rgb, rgb, 255));
		}
	}

	bmp.Quantize(8);

	bmp.Posterize(levels);

	// count & save used colors
//	BTCOLORREF *ntable = new BTCOLORREF[levels];
	BTCOLORREF ntable[256];
	BTCOLORREF prec = BTRGB(0, 0, 0);
	int        c;

	c = 0;
	for (int i = 0; i < 256; i++)
	{
		col = bmp.GetColorFromIndex(i);

		if (prec != col)
		{
			ntable[c] = col;
			prec      = col;
			c++;
		}
	}

	// link grid colors with saved colors
	for (y = 0; y < bmp.GetHeight(); y++)
	{
		for (x = 0; x < bmp.GetWidth(); x++)
		{
			col = bmp.GetColorFromPixel(x, y);

			c = 0;
			while (ntable[c] != col)
				c++;

			bmp.SetColorForPixel(x, y, c); // index of color
		}
	}

	// build alphamap palette
	for (i = 0; i < levels; i++)
	{
		bmp.SetColorForIndex(i, ntable[i]);
	}

	for (i = levels; i < 256; i++)
	{
		bmp.SetColorForIndex(i, BTRGB(0, 0, 0));
	}

	g_AlphamapMan.FromData(bmp);

	g_Grid.AlphamapFromData(bmp);

	RefreshView();

	UpdateFormView(TAB_ALPHAMAP);

	pTMan->RefreshLocking();
}

void CEasyGenDoc::DisposeBrushes()
{
	CNmPoly        *poly;
	CNmFace        *face;
	S_EGN_POLYFACE *data;

	SCAN_NM_POLY(g_World, poly)
	{
		for (face = poly->m_Face; face; face = face->Next)
		{
			face->GetDataPtr((void *&)data);

			if (data != NULL)
			{
				delete data;
				face->SetDataPtr(NULL);
			}
		}
	}
}

void CEasyGenDoc::OnCloseDocument()
{
	DisposeBrushes();

	CDocument::OnCloseDocument();
}

void CEasyGenDoc::OnToggleWorld()
{
	m_bShowWorld = !m_bShowWorld;
	RefreshView();
}

void CEasyGenDoc::SetOrigin(CNmVec3 &origin)
{
	CEasyGenView *pView = (CEasyGenView *)GetRenderView();

	CNmVec3 delta_origin, cur_origin;

	g_Grid.GetOrigin(cur_origin);
	g_Grid.SetOrigin(origin);

	delta_origin = -(origin - cur_origin);

//	g_World.Move( delta_origin );

	pView->m_Eye += delta_origin;

	RefreshView();
}

BOOL CEasyGenDoc::ImportTerrainShader(LPCTSTR shadername)
{
	CTextureMan *pTMan = (CTextureMan *)GetTextureView();
	CString     path, basepath, filename, texfilename;
	int         i_under, i_index;
	CNmStr      sh_name, sh_basicname, sh_index, sh_maptexture;
	CShaderFile shader;
	bool        found;

	basepath = g_Prefs.m_GameBasePath + "\\scripts\\";
	path     = g_Prefs.m_GameBasePath + "\\scripts\\*.shader";

	found = false;

	WIN32_FIND_DATA data;
	HANDLE          hFind = FindFirstFile(_T((LPCTSTR)path), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (lstrcmp(data.cFileName, _T(".")) == 0 ||
			    lstrcmp(data.cFileName, _T("..")) == 0 ||
			    (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}

			filename = basepath + data.cFileName;
			filename.MakeLower();

			shader.Open((const char *)filename);

			while (shader.sh_GetNext())
			{
				shader.sh_GetName(sh_name);

				i_under = sh_name.ReverseFind('_');

				if (i_under != -1 && sh_name.Find('t', i_under) == -1)
				{
					sh_basicname = sh_name.Left(i_under);

					if (sh_basicname == shadername) // found shaderfile!
					{
						found = true;

						sh_index = sh_name.Right(sh_name.GetLength() - i_under - 1);
						i_index  = atoi((const char *)sh_index);

						if (shader.stg_GetNext())
						{
							shader.stg_GetValue("map", sh_maptexture);

							texfilename  = g_Prefs.m_GameBasePath + "\\";
							texfilename += (const char *)sh_maptexture;

							CTexture *added;

							added = pTMan->AddTexture((LPCTSTR)texfilename);

							if (i_index < g_AlphamapMan.GetLayers())
							{
								g_AlphamapMan.Set(i_index, added);
							}
						}
					}
				}
			}

			shader.Close();

//			pTMan->AddTexture((LPCTSTR)filename);

		}
		while (FindNextFile(hFind, &data) && !found);
		FindClose(hFind);
	}

	if (found)
	{
		pTMan->RefreshLocking();
	}

	return true;
}
