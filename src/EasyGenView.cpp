// EasyGenView.cpp : implementation of the CEasyGenView class
//

#include "EasyGen.h"

#include "EasyGenDoc.h"
#include "EasyGenView.h"
#include "CmdFormView.h"
#include "MainFrm.h"
////////
#include "Nemo3D.h"
#include "NmVec3.h"
////////
#include "Tris.h"

// extern
#include "Preferences.h"
extern CPreferences g_Prefs;

#include "AlphamapMan.h"
extern CAlphamapMan g_AlphamapMan;

#include "Grid.h"
extern CGrid g_Grid;

#include "Modifier.h"
extern CModifier g_Mod;

#include "Md3.h"
extern CMd3Man g_Md3Man;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEasyGenView

IMPLEMENT_DYNCREATE(CEasyGenView, CView)

BEGIN_MESSAGE_MAP(CEasyGenView, CView)
//{{AFX_MSG_MAP(CEasyGenView)
ON_WM_DESTROY()
ON_WM_CREATE()
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_SIZE()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_KEYDOWN()
ON_WM_LBUTTONDBLCLK()
ON_WM_TIMER()
ON_WM_RBUTTONDBLCLK()
ON_COMMAND(ID_CAMERA_ALIGN, OnCameraAlign)
ON_COMMAND(ID_CAMERA_LEFT, OnCameraLeft)
ON_COMMAND(ID_CAMERA_RIGHT, OnCameraRight)
ON_COMMAND(ID_CAMERA_FORWARD, OnCameraForward)
ON_COMMAND(ID_CAMERA_BACKWARD, OnCameraBackward)
ON_COMMAND(ID_CAMERA_STRAFELEFT, OnCameraStrafeLeft)
ON_COMMAND(ID_CAMERA_STRAFERIGHT, OnCameraStrafeRight)
ON_COMMAND(ID_CAMERA_DOWN, OnCameraDown)
ON_COMMAND(ID_CAMERA_UP, OnCameraUp)
ON_COMMAND(ID_CAMERA_LOOKUP, OnCameraLookup)
ON_COMMAND(ID_CAMERA_LOOKDOWN, OnCameraLookdown)
ON_COMMAND(ID_CAMERA_FLOOR, OnCameraFloor)
ON_COMMAND(ID_CAMERA_ZEROH, OnCameraZeroh)
ON_COMMAND(ID_CAMERA_ALIGNZ, OnCameraAlignZ)
ON_COMMAND(ID_BACKSPACE, OnBackspace)
//}}AFX_MSG_MAP
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyGenView construction/destruction

CEasyGenView::CEasyGenView()
{
	// OpenGL
	m_hGLContext   = NULL;
	m_GLPixelIndex = 0;

	// Mouse
//	m_CursorRotation = AfxGetApp()->LoadCursor(IDC_CURSOR_ROTATION);

	m_ClearColorRed   = (float)GetRValue(g_Prefs.m_BgColor) / 255.f;
	m_ClearColorGreen = (float)GetGValue(g_Prefs.m_BgColor) / 255.f;
	m_ClearColorBlue  = (float)GetBValue(g_Prefs.m_BgColor) / 255.f;

	m_RenderMode = R_FLAT | R_FL_QUADFRAME;

	m_ViewCulling    = TRUE;
	m_ViewShadeModel = GL_SMOOTH;

	m_VertexGot = -1;

	m_Md3Highlight = 0;

	m_vGenericPos.Set(0, 0, 0);

	InitGeometry();
}

CEasyGenView::~CEasyGenView()
{
}

//********************************************
// InitGeometry
//********************************************
void CEasyGenView::InitGeometry(void)
{
	m_xRot = 0.0f;
	m_yRot = -90.0f;

	m_Eye.Set(0.f, -START_WIDTHY * 3 / 4, 512.f);

	m_xScaling = 1.0f;
	m_yScaling = 1.0f;
	m_zScaling = 1.0f;
	m_Phase    = 0.0f;
}

//********************************************
// CreateViewGLContext
// Create an OpenGL rendering context
//********************************************
BOOL CEasyGenView::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);

	if (m_hGLContext == NULL)
	{
		return FALSE;
	}

	if (wglMakeCurrent(hDC, m_hGLContext) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CEasyGenView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEasyGenView drawing

void CEasyGenView::OnDraw(CDC *pDC)
{
	CEasyGenDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CEasyGenView printing

BOOL CEasyGenView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEasyGenView::OnBeginPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEasyGenView::OnEndPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CEasyGenView diagnostics

#ifdef _DEBUG
void CEasyGenView::AssertValid() const
{
	CView::AssertValid();
}

void CEasyGenView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEasyGenDoc *CEasyGenView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEasyGenDoc)));
	return (CEasyGenDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEasyGenView message handlers

void CEasyGenView::OnDestroy()
{
	if (wglGetCurrentContext() != NULL)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (m_hGLContext != NULL)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
	}

	CView::OnDestroy();
}

//********************************************
// SetWindowPixelFormat
//********************************************
BOOL CEasyGenView::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize    = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;

	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW |
	                    PFD_SUPPORT_OPENGL |
	                    PFD_DOUBLEBUFFER;
	//|PFD_STEREO_DONTCARE;
	pixelDesc.iPixelType      = PFD_TYPE_RGBA;
	pixelDesc.cColorBits      = 16;
	pixelDesc.cRedBits        = 8;
	pixelDesc.cRedShift       = 16;
	pixelDesc.cGreenBits      = 8;
	pixelDesc.cGreenShift     = 8;
	pixelDesc.cBlueBits       = 8;
	pixelDesc.cBlueShift      = 0;
	pixelDesc.cAlphaBits      = 0;
	pixelDesc.cAlphaShift     = 0;
	pixelDesc.cAccumBits      = 64;
	pixelDesc.cAccumRedBits   = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits  = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits      = 16;
	pixelDesc.cStencilBits    = 8;
	pixelDesc.cAuxBuffers     = 0;
	pixelDesc.iLayerType      = PFD_MAIN_PLANE;
	pixelDesc.bReserved       = 0;
	pixelDesc.dwLayerMask     = 0;
	pixelDesc.dwVisibleMask   = 0;
	pixelDesc.dwDamageMask    = 0;

	m_GLPixelIndex = ChoosePixelFormat(hDC, &pixelDesc);
	if (m_GLPixelIndex == 0) // Choose default
	{
		m_GLPixelIndex = 1;
		if (DescribePixelFormat(hDC, m_GLPixelIndex,
		                        sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
		{
			return FALSE;
		}
	}

	if (!SetPixelFormat(hDC, m_GLPixelIndex, &pixelDesc))
	{
		return FALSE;
	}

	return TRUE;
}

int CEasyGenView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	HWND hWnd = GetSafeHwnd();
	HDC  hDC  = ::GetDC(hWnd);

	if (SetWindowPixelFormat(hDC) == FALSE)
	{
		return 0;
	}

	if (CreateViewGLContext(hDC) == FALSE)
	{
		return 0;
	}

	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glPolygonOffset(-0.8f, -0.8f);

	glClearColor(m_ClearColorRed, m_ClearColorGreen, m_ClearColorBlue, 1.0f);

	glClearDepth(1.0f);

	// Lights, material properties
	GLfloat ambientProperties[]  = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat diffuseProperties[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat specularProperties[] = { 1.0f, 0.5f, 1.0f, 1.0f };
//	GLfloat LightPosition[]= { 0.0f, 5.0f, -5.0f, 1.0f };				 // Light Position ( NEW )

	m_LightPos[0] = 0.f;
	m_LightPos[1] = 0.f;
	m_LightPos[2] = 0.f;
	m_LightPos[3] = 1.f;

	m_LightDir[0] = 0.f;
	m_LightDir[1] = 0.f;
	m_LightDir[2] = -1.f;
	m_LightDir[3] = 1.f;

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientProperties);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseProperties);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularProperties);
	glLightfv(GL_LIGHT1, GL_POSITION, m_LightPos);           // Position The Light
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, m_LightDir);         // Position The Light
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

	SetTimer(1, 200, NULL);

	return 0;
}

void CEasyGenView::OnPaint()
{
	// Device context for painting
	CPaintDC dc(this);

	// Model is stored in Document
	CEasyGenDoc *pDoc = (CEasyGenDoc *)GetDocument();
	ASSERT_VALID(pDoc);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// Position / translation / scale
	CNmVec3 c(1, 0, 0);
	c.Rotate_y(u_deg2rad(m_xRot));
	c.Rotate_z(-u_deg2rad(m_yRot));
	c += m_Eye;
	gluLookAt(m_Eye.x, m_Eye.y, m_Eye.z, c.x, c.y, c.z, 0, 0, 1);

//	float scale_factor = 1/512.f;
	glScalef(m_xScaling, m_yScaling, m_zScaling);

	// Start rendering...
	pDoc->RenderScene();

	// Double buffer
	SwapBuffers(dc.m_ps.hdc);
}

void CEasyGenView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();
	S_POSINFO   inf;
	CTris       *t;
	CNmVec3     p;
	int         tab;

	m_LDP = point;

	BOOL kCTRL  = nFlags & MK_CONTROL;
	BOOL kSHIFT = nFlags & MK_SHIFT;

	BOOL Refresh = FALSE;

	int mod = pDoc->GetModifier();

	switch (tab = pDoc->GetActiveTab())
	{
	case (TAB_GRID):
		break;
	case (TAB_MODIFIER):

		// Get a vertex to drag(CTRL+SHIFT+LeftBut)
		if (kCTRL && kSHIFT)
		{
			if (GetXYZ(point, p))
			{
				if (g_Grid.GetInfo(p.x, p.y, &inf))
				{
					m_VertexGot = g_Grid.GetK(inf.i, inf.j);
				}
			}
		}

		// Exclude triangle from export | to prevent erroneus exclusion
		if (!kCTRL && kSHIFT && g_Grid.OptionExcludedTrisGet())
		{
			if (GetXYZ(point, p))
			{
				t = g_Grid.GetTris(g_Grid.GetTris(p.x, p.y));

				if (t)
				{
					t->SetExcluded(!t->GetExcluded());
					pDoc->m_bInvalidateGridColor = TRUE;
					Refresh                      = TRUE;
				}
			}
		}

		break;
	case (TAB_ALPHAMAP):
		int curindex;

		curindex = g_AlphamapMan.GetCurSel();

		// paint the alphamap (CTRL+SHIFT+LeftBut)
		if (kCTRL && kSHIFT && curindex != -1)
		{
			if (GetXYZ(point, p))
			{
				if (g_Grid.GetInfo(p.x, p.y, &inf))
				{
					m_VertexGot = g_Grid.GetK(inf.i, inf.j);
//					g_Grid.SetAlphaIndex(m_VertexGot, curindex );
					g_Grid.PaintAlphaIndex(inf.i, inf.j, pDoc->m_dwPaintRadius, curindex);
					Refresh = TRUE;
				}
			}
		}

		int indexgot;

		// copy alphamap index (SHIFT+LeftBut)
		if (!kCTRL && kSHIFT)
		{
			if (GetXYZ(point, p))
			{
				if (g_Grid.GetInfo(p.x, p.y, &inf))
				{
					m_VertexGot = g_Grid.GetK(inf.i, inf.j);
					indexgot    = g_Grid.GetAlphaIndex(m_VertexGot);
					if (indexgot != g_AlphamapMan.GetCurSel())
					{
						g_AlphamapMan.SetCurSel(indexgot);
						pDoc->UpdateFormView(TAB_ALPHAMAP);
					}
				}
			}
		}

		break;
	case (TAB_MODELS):

		// MD3 - Selecting
		if (!kCTRL && kSHIFT)
		{
			CNmVec3  ray, dummy;
			float    md3_dist, hitterra_dist;
			bool     hitterra;
			S_MD3PTR *oldh = m_Md3Highlight;
			S_MD3PTR *hit_md3;

			if (GetRay(point, ray))
			{
				if (g_Md3Man.GetByRay(m_Eye, ray, hit_md3, md3_dist))
				{
					// check if is covered by terrain!!!
					hitterra = g_Grid.GetDistanceTrisRay(m_Eye, ray, dummy, hitterra_dist);

					if (!hitterra)
					{
						m_Md3Highlight = hit_md3;
					}
					else if (hitterra_dist > md3_dist)
					{
						m_Md3Highlight = hit_md3;
					}
				}
			}

			if (m_Md3Highlight != oldh)
			{
				Refresh = TRUE;
			}
		}

		break;
	}

	// rotate tris (CTRL+LeftBut)
	if ((tab == TAB_GRID || tab == TAB_MODIFIER || tab == TAB_ALPHAMAP) &&
	    mod == MOD_NONE && kCTRL && !kSHIFT)
	{
		if (GetXYZ(point, p))
		{
			if (g_Grid.GetInfo(p.x, p.y, &inf))
			{
				int          i, j, k;
				int          mt1;
				S_FACEDATAEX *f;

				mt1  = g_Grid.GetTris(p.x, p.y);
				mt1 &= ~1;

				k = mt1 / 2;
				i = k % g_Grid.GetCellsX();
				j = k / g_Grid.GetCellsX();

				f = g_Grid.GetFace(k);

				if ((f->flags & FACE_FL_TRISINV))
				{
					f->flags &= ~FACE_FL_TRISINV;
				}
				else
				{
					f->flags |= FACE_FL_TRISINV;
				}

				g_Grid.FaceOrientateTris(i, j, f->flags & FACE_FL_TRISINV);

				Refresh = TRUE;
			}
		}
	}

	if (Refresh)
	{
		InvalidateRect(NULL, FALSE);
	} //

	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}

void CEasyGenView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// Set OpenGL perspective, viewport and mode
	CSize  size(cx, cy);
	double aspect;
	aspect = (cy == 0) ? (double)size.cx : (double)size.cx / (double)size.cy;

	glViewport(0, 0, size.cx, size.cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_Prefs.m_Fov, aspect, 4.f, 65535.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDrawBuffer(GL_BACK);
}

void CEasyGenView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);

	CEasyGenDoc *pDoc = GetDocument();

	BOOL      Refresh   = FALSE;
	BOOL      UpdStatus = FALSE;
	CNmVec3   p;
	S_POSINFO inf;
	int       tab;

	int mod = pDoc->GetModifier();

	BOOL kCTRL  = nFlags & MK_CONTROL;
	BOOL kSHIFT = nFlags & MK_SHIFT;
	BOOL m_LBD  = nFlags & MK_LBUTTON;
	BOOL m_RBD  = nFlags & MK_RBUTTON;

	CPoint Ldelta = point - m_LDP;
	CPoint Rdelta = point - m_RDP;

	if (!m_LBD)
	{
		m_VertexGot = -1;
	}

	switch (tab = pDoc->GetActiveTab())
	{
	case (TAB_GRID):
		break;
	case (TAB_MODIFIER):

		//	move Modifier	CTRL + Left
		if (m_LBD && kCTRL && !kSHIFT)
		{
			float f  = (float)u_deg2rad(-m_yRot + 90);
			float dx = -(float)(Ldelta.x) * pDoc->m_modXYstep;
			float dy = (float)(Ldelta.y) * pDoc->m_modXYstep;
			pDoc->m_modY += float(sin(f) * dx + cos(f) * dy);
			pDoc->m_modX += float(cos(f) * dx - sin(f) * dy);
			g_Mod.MoveDelta(float(cos(f) * dx - sin(f) * dy), float(sin(f) * dx + cos(f) * dy));

			UpdStatus                      = TRUE;
			pDoc->m_bInvalidateGridPreview = TRUE;
			Refresh                        = TRUE;
		}

		//	inc Strength	CTRL + Right
		if (m_RBD && kCTRL && !kSHIFT && (mod != MOD_NONE))
		{
			pDoc->m_modStrength -= (float)(Rdelta.y) * 4.f;

			g_Mod.HeightDelta(-(float)(Rdelta.y) * 4.f);

			UpdStatus                      = TRUE;
			pDoc->m_bInvalidateGridPreview = TRUE;
			Refresh                        = TRUE;
		}

		// inc Radius		SHIFT + Right
		if (m_RBD && !kCTRL && kSHIFT && (mod != MOD_NONE))
		{
			pDoc->m_modRadius -= (float)(Rdelta.y) * 4.f;
			g_Mod.RadiusDelta(-(float)(Rdelta.y) * 4.f);

			if (pDoc->m_modRadius < 0)
			{
				pDoc->m_modRadius = 0;
			}

			UpdStatus                      = TRUE;
			pDoc->m_bInvalidateGridPreview = TRUE;
			Refresh                        = TRUE;
		}

		// Vertex dragging
		if (m_LBD && kCTRL && kSHIFT && (m_VertexGot != -1) && (mod == MOD_NONE))
		{
			float *z = g_Grid.GetPtr(m_VertexGot);

			*z = floor(*z - g_Prefs.m_fVertexDragStep * (float)Ldelta.y);

			if (*z < ASSUME_ZERO)
			{
				*z = 0;
			}

			m_vGenericPos.Set(g_Grid.GetX(m_VertexGot), g_Grid.GetY(m_VertexGot), *z);

			UpdStatus                      = TRUE;
			pDoc->m_bInvalidateGridPreview = TRUE;
			Refresh                        = TRUE;
		}

		break;
	case (TAB_ALPHAMAP):

		int curindex;

		curindex = g_AlphamapMan.GetCurSel();

		// Alphamap paint
		if (m_LBD && kCTRL && kSHIFT && curindex != -1)
		{
			if (GetXYZ(point, p))
			{
				if (g_Grid.GetInfo(p.x, p.y, &inf))
				{
					m_VertexGot = g_Grid.GetK(inf.i, inf.j);
//					g_Grid.SetAlphaIndex(m_VertexGot, curindex );
					g_Grid.PaintAlphaIndex(inf.i, inf.j, pDoc->m_dwPaintRadius, curindex);
					Refresh = TRUE;
				}
			}
		}

		break;
	case (TAB_MODELS):
		break;
	}

	//	rotate X/Y
	if (m_LBD && !m_RBD && !kCTRL && !kSHIFT)
	{
		m_yRot += (float)(Ldelta.x) / 2.0f; // left / right

		m_xRot += (float)(Ldelta.y) / 2.0f; // up / dn
		if (m_xRot > 85)
		{
			m_xRot = 85;
		}
		if (m_xRot < -85)
		{
			m_xRot = -85;
		}

		Refresh = TRUE;
	}

	//	pan
	if (m_RBD && !m_LBD && !kCTRL && !kSHIFT)
	{
		float f  = (float)u_deg2rad(-m_yRot + 90);
		float dx = -(float)(Ldelta.x) * 16;
		float dy = (float)(Ldelta.y) * 16;
		m_Eye.y += float(sin(f) * dx + cos(f) * dy);
		m_Eye.x += float(cos(f) * dx - sin(f) * dy);

		Refresh = TRUE;
	}

	//	up/dn
	if (m_RBD && m_LBD && !kCTRL && !kSHIFT)
	{
		m_Eye.z -= (float)(Rdelta.y) * 32;

		Refresh = TRUE;
	}

	if (Refresh)
	{
		InvalidateRect(NULL, FALSE);
	}

	if (UpdStatus)
	{
		UpdateStatusBar();
	}

	m_LDP = point;
	m_RDP = point;
}

void CEasyGenView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_VertexGot = -1;

	ReleaseCapture();

	CView::OnLButtonUp(nFlags, point);
}

void CEasyGenView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     ray;
	CNmVec3     p;
	int         tab;

	BOOL kCTRL  = nFlags & MK_CONTROL;
	BOOL kSHIFT = nFlags & MK_SHIFT;

	BOOL GotPoint = GetXYZ(point, p);
	BOOL GotRay   = GetRay(point, ray);

	m_RDP = point;

	SetCapture();

	switch (tab = pDoc->GetActiveTab())
	{
	case (TAB_GRID):
		break;
	case (TAB_MODIFIER):
		break;
	case (TAB_ALPHAMAP):
		break;
	case (TAB_MODELS):

		// MD3 - Move an Md3Highlighted model
		if (!kCTRL && kSHIFT && GotRay && m_Md3Highlight)
		{
			CNmVec3 pos;
			float   dist;

			if (g_Grid.GetDistanceTrisRay(m_Eye, ray, pos, dist))
			{
				m_Md3Highlight->m_egvPos = pos;
				InvalidateRect(NULL, FALSE);
			}
		}

		break;
	}

	CView::OnRButtonDown(nFlags, point);
}

void CEasyGenView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();

	ReleaseCapture();

	switch (pDoc->GetActiveTab())
	{
	case (TAB_GRID):
		break;
	case (TAB_MODIFIER):
		break;
	case (TAB_ALPHAMAP):
		break;
	case (TAB_MODELS):
		break;
	}

	CView::OnRButtonUp(nFlags, point);
}

void CEasyGenView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
/*
    switch (nChar)
    {
    }
*/

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEasyGenView::SetBgColor(COLORREF col)
{
	m_ClearColorRed   = GetRValue(col) / 255.f;
	m_ClearColorGreen = GetGValue(col) / 255.f;
	m_ClearColorBlue  = GetBValue(col) / 255.f;

	glClearColor(m_ClearColorRed, m_ClearColorGreen, m_ClearColorBlue, 1.0f);

	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::SetStatusText(UINT index, LPCTSTR text)
{
	CEasyGenApp *pApp       = (CEasyGenApp *)AfxGetApp();
	CMainFrame  *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	pMainFrame->SendStatusText(index, text);
}

BOOL CEasyGenView::GetRay(CPoint point, CNmVec3& ray)
{
	GLdouble mMat[16];
	GLdouble pMat[16];
	GLint    vp[4];

	glGetDoublev(GL_PROJECTION_MATRIX, pMat);
	glGetDoublev(GL_MODELVIEW_MATRIX, mMat);
	glGetIntegerv(GL_VIEWPORT, vp);

	GLdouble x1, y1, z1, x2, y2, z2;

	int realy = vp[3] - (GLint)point.y - 1;

	if ((gluUnProject((GLdouble)point.x, (GLdouble)realy, 0.0f, mMat, pMat, vp, &x1, &y1, &z1) != GL_TRUE) ||
	    (gluUnProject((GLdouble)point.x, (GLdouble)realy, 1.0f, mMat, pMat, vp, &x2, &y2, &z2) != GL_TRUE)
	    )
	{
		return FALSE;
	}

	ray.Set((float)(x2 - x1), (float)(y2 - y1), (float)(z2 - z1));
	ray.Normalize();

	return TRUE;
}

BOOL CEasyGenView::GetXYZ(CPoint point, CNmVec3& p)
{
	GLdouble mMat[16];
	GLdouble pMat[16];
	GLint    vp[4];

	glGetDoublev(GL_PROJECTION_MATRIX, pMat);
	glGetDoublev(GL_MODELVIEW_MATRIX, mMat);
	glGetIntegerv(GL_VIEWPORT, vp);

	GLdouble gl_x, gl_y, gl_z;

	int realy = vp[3] - (GLint) point.y - 1;

	glReadBuffer(GL_BACK);

	GLfloat winz;
	glReadPixels(point.x, realy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);

	GLdouble winzd = (GLdouble)winz;

	if (gluUnProject((GLdouble)point.x, (GLdouble)realy, winzd, mMat, pMat, vp, &gl_x, &gl_y, &gl_z) == GL_TRUE)
	{
		p.Set((float)gl_x, (float)gl_y, (float)gl_z);

		return TRUE;
	}

	return FALSE;
}

void CEasyGenView::SetRenderMode(int mode)
{
	m_RenderMode = (m_RenderMode & R_FL_MASK) | mode;
}

void CEasyGenView::RenderFlagToggle(int flag)
{
	if (m_RenderMode & flag)
	{
		m_RenderMode &= ~flag;
	}
	else
	{
		m_RenderMode |= flag;
	}
}

void CEasyGenView::RenderFlagSet(int flag)
{
	m_RenderMode |= flag;
}

void CEasyGenView::RenderFlagReset(int flag)
{
	m_RenderMode &= ~flag;
}

BOOL CEasyGenView::RenderFlagTest(int flag)
{
	return (m_RenderMode & flag) == flag;
}

int CEasyGenView::GetRenderMode()
{
	return m_RenderMode & (~R_FL_MASK);
}

void CEasyGenView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     ray;
	CNmVec3     p;

	BOOL kCTRL  = nFlags & MK_CONTROL;
	BOOL kSHIFT = nFlags & MK_SHIFT;

	BOOL GotPoint = GetXYZ(point, p);
	BOOL GotRay   = GetRay(point, ray);

	switch (pDoc->GetActiveTab())
	{
	case (TAB_GRID):

		// player place
		if (!kCTRL && kSHIFT && GotPoint && g_Grid.PtInGrid(p.x, p.y))
		{
			pDoc->m_vPlayer.x = p.x;
			pDoc->m_vPlayer.y = p.y;

			S_POSINFO inf;
			if (g_Grid.GetInfo(pDoc->m_vPlayer.x, pDoc->m_vPlayer.y, &inf))
			{
				pDoc->m_vPlayer.z = inf.z;
			}

			m_vGenericPos.Set(p.x, p.y, pDoc->m_vPlayer.z);
			UpdateStatusBar();
			InvalidateRect(NULL, FALSE);
		}

		break;
	case (TAB_MODIFIER):

		// modifier place
		if (kCTRL && !kSHIFT && GotPoint && g_Grid.PtInGrid(p.x, p.y))
		{
			pDoc->m_modX = p.x;
			pDoc->m_modY = p.y;

			g_Mod.PositionSet(p.x, p.y);

			UpdateStatusBar();
			InvalidateRect(NULL, FALSE);
			pDoc->m_bInvalidateGridPreview = TRUE;
		}

		break;
	case (TAB_ALPHAMAP):
		break;
	case (TAB_MODELS):

		// MD3 - Insert model
		if (kCTRL && !kSHIFT && GotRay)
		{
			CNmVec3  pos;
			float    dist;
			S_MD3PTR *md3ptr;

			if (g_Grid.GetDistanceTrisRay(m_Eye, ray, pos, dist))
			{
				md3ptr = g_Md3Man.m_Placed.Append(); // g_Md3Man.m_Placed.Append( pDoc->GetMd3SelectedTabModels() );

				if (md3ptr)
				{
					md3ptr->pMd3     = pDoc->GetMd3SelectedTabModels();
					md3ptr->m_egvPos = pos;
					InvalidateRect(NULL, FALSE);
				}
			}
		}
		break;
	}

	// CAMERA - camera place [EVERY TAB]
	if (!kSHIFT && !kCTRL && GotPoint && g_Grid.PtInGrid(p.x, p.y))
	{
		m_Eye.x = p.x;
		m_Eye.y = p.y;

		InvalidateRect(NULL, FALSE);
	}

	CView::OnLButtonDblClk(nFlags, point);
}

void CEasyGenView::OnTimer(UINT nIDEvent)
{
	CView::OnTimer(nIDEvent);
/*
    CEasyGenDoc* pDoc = GetDocument();

    m_Md3Highlight = 0;

    // upd Md3 sel
    if (nIDEvent == 1)
    {
        if ((pDoc)&&(pDoc->GetActiveTab() == TAB_MODELS))
        {
            CNmVec3 m_Eye(m_xTransl,m_yTransl,m_zTransl);
            CNmVec3 ray,dummy;
            float	dist,hitterra_dist;

            if (GetRay(m_MousePos,ray))
            {
                g_Md3Man.GetByRay(m_Eye,ray,m_Md3Highlight,dist);

                if (g_Grid.GetDistanceTrisRay(m_Eye,ray,dummy,hitterra_dist))
                {
                    if (hitterra_dist<dist)
                    {
                        m_Md3Highlight = 0;
                    }
                }
            }
        }
    }
*/
}


void CEasyGenView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();
	S_POSINFO   inf;
	CNmVec3     p;

	BOOL kCTRL  = nFlags & MK_CONTROL;
	BOOL kSHIFT = nFlags & MK_SHIFT;

	BOOL GotPoint = GetXYZ(point, p);

	switch (pDoc->GetActiveTab())
	{
	case (TAB_GRID):
		break;
	case (TAB_MODIFIER):

		// Modifier get height
		if (kCTRL && GotPoint)
		{
			if (g_Grid.GetInfo(p.x, p.y, &inf))
			{
				g_Mod.HeightSet(inf.z);
				InvalidateRect(NULL, FALSE);
				pDoc->m_bInvalidateGridPreview = TRUE;
			}
		}

		break;
	case (TAB_ALPHAMAP):
		break;
	case (TAB_MODELS):
		break;
	}

	CView::OnRButtonDblClk(nFlags, point);
}

void CEasyGenView::UpdateStatusBar()
{
	CEasyGenDoc *pDoc = GetDocument();
	CString     s;
	int         mr, ms;

	// generic pos
	s.Format("Pos = X:%1.1f Y:%1.1f Z:%1.1f",
	         m_vGenericPos.x, m_vGenericPos.y, m_vGenericPos.z);
	SetStatusText(STATUS_PLAYER, (LPCTSTR)s);

	// modifier
	if (pDoc->GetModifier() >= MODIFIERS)   // custom
	{
		mr = (int)g_Mod.GetRadius();
		ms = (int)g_Mod.GetHeight();
	}
	else                                    // integrated
	{
		mr = (int)pDoc->m_modRadius;
		ms = (int)pDoc->m_modStrength;
	}

	s.Format("Mod = X:%d Y:%d R:%d H:%d",
	         (int)pDoc->m_modX, (int)pDoc->m_modY,
	         mr, ms);

	SetStatusText(STATUS_MODIFIER, (LPCTSTR)s);
}

void CEasyGenView::OnCameraAlign()
{
	int d;
	m_xRot = 0;
	while (m_yRot < 0)
		m_yRot += 360;
	d = int(m_yRot) % 45;
	if (d < 22)
	{
		m_yRot -= d;
	}
	else
	{
		m_yRot += 45 - d;
	}
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraAlignZ()
{
	m_xRot = 0;
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraLeft()
{
	m_yRot -= 10;
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraRight()
{
	m_yRot += 10;
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraForward()
{
	float f  = (float)u_deg2rad(-m_yRot + 90);
	float dy = -32;
	m_Eye.y += float(sin(f) + cos(f) * dy);
	m_Eye.x += float(cos(f) - sin(f) * dy);
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraBackward()
{
	float f  = (float)u_deg2rad(-m_yRot + 90);
	float dy = 32;
	m_Eye.y += float(sin(f) + cos(f) * dy);
	m_Eye.x += float(cos(f) - sin(f) * dy);
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraStrafeLeft()
{
	float f  = (float)u_deg2rad(-m_yRot + 90);
	float dx = 32;
	m_Eye.y += float(sin(f) * dx);
	m_Eye.x += float(cos(f) * dx);
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraStrafeRight()
{
	float f  = (float)u_deg2rad(-m_yRot + 90);
	float dx = -32;
	m_Eye.y += float(sin(f) * dx);
	m_Eye.x += float(cos(f) * dx);
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraDown()
{
	m_Eye.z -= 32;
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraUp()
{
	m_Eye.z += 32;
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraLookup()
{
	m_xRot -= 5;
	if (m_xRot < -85)
	{
		m_xRot = -85;
	}
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraLookdown()
{
	m_xRot += 5;
	if (m_xRot > 85)
	{
		m_xRot = 85;
	}
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnCameraFloor()
{
	CEasyGenDoc *pDoc = GetDocument();
	S_POSINFO   inf;

	if (g_Grid.GetInfo(m_Eye.x, m_Eye.y, &inf))
	{
		m_Eye.z = inf.z + 50;
		InvalidateRect(NULL, FALSE);
	}
}

void CEasyGenView::OnCameraZeroh()
{
	CEasyGenDoc *pDoc = GetDocument();
	m_Eye.z = g_Grid.GetMaxHeight();
	InvalidateRect(NULL, FALSE);
}

void CEasyGenView::OnBackspace()
{
	if (m_Md3Highlight)
	{
		g_Md3Man.m_Placed.Remove(m_Md3Highlight);
		m_Md3Highlight = 0;
		InvalidateRect(NULL, FALSE);
	}
}
