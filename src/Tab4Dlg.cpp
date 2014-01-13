// Tab4Dlg.cpp : implementation file
//

#include "EasyGen.h"
#include "Tab4Dlg.h"
////////
#include "NmStr.h"

// extern link
#include "Preferences.h"
extern CPreferences g_Prefs;

#include "Md3.h"
extern CMd3Man g_Md3Man;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTab4Dlg dialog


CTab4Dlg::CTab4Dlg(CWnd *pParent /*=NULL*/)
	: CDialog(CTab4Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTab4Dlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTab4Dlg::OnOK()
{
}

void CTab4Dlg::OnCancel()
{
}

void CTab4Dlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTab4Dlg)
	DDX_Control(pDX, IDC_MD3LIST, m_lbMd3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTab4Dlg, CDialog)
//{{AFX_MSG_MAP(CTab4Dlg)
ON_LBN_SELCHANGE(IDC_MD3LIST, OnSelchangeMd3List)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTab4Dlg message handlers

void CTab4Dlg::Init()
{
	CNmStr s;
	int    pos;
	int    ins;

	m_lbMd3.ResetContent();

	pos = g_Prefs.m_GameBasePath.GetLength() + strlen(_T("\\models\\"));

	CMd3 *md3;
	g_Md3Man.m_List.ScanReset();
	while (g_Md3Man.m_List.Scan(md3))
	{
		s = md3->m_sName;
		s.Delete(0, pos);
		s.Delete(s.GetLength() - 4, 4);

		ins = m_lbMd3.InsertString(-1, s);   // Bottom

		m_lbMd3.SetItemDataPtr(ins, (void *)md3);
	}

	m_Md3Selected = 0;
}

void CTab4Dlg::OnSelchangeMd3List()
{
	CNmStr  s;
	CString text;

	UpdateData();

	int sel = m_lbMd3.GetCurSel();

	m_Md3Selected = (CMd3 *)m_lbMd3.GetItemDataPtr(sel);

	g_Md3Man.m_lbSelected = m_Md3Selected;

	InvalidateRect(NULL, FALSE);
//	if (md3) AfxMessageBox( md3->m_sName );
}

void CTab4Dlg::DrawXZ(CDC *pDC, CMd3 *md3)
{
	int    i, j;
	int    triangleNum, currentFrame, currentMesh, currentOffsetVertex, currentVertex, indexVertex;
	float  vf[2], wf[2];
	int    vi[2], wi[2];
	float  d;
	CPen   pensel(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush BgCol(RGB(192, 192, 192));

	pDC->SelectObject(&BgCol);
	pDC->SelectObject(&pensel);

	pDC->FillSolidRect(0, 0, VIEWXZ_WIDTH, VIEWXZ_WIDTH, RGB(192, 192, 192));

	if (!md3)
	{
		return;
	}

	(md3->m_vRay.x) > (md3->m_vRay.z) ? d = md3->m_vRay.x : d = md3->m_vRay.z;

	currentFrame = 0;
	for (currentMesh = 0; currentMesh < md3->m_Header.numMeshes; currentMesh++)
	{
		currentOffsetVertex = currentFrame * md3->m_Meshes[currentMesh].MeshHeader.numVertexes;

		triangleNum = md3->m_Meshes[currentMesh].MeshHeader.numTriangles;

		for (i = 0; i < triangleNum; i++)
		{
			for (j = 0; j < 3; j++)
			{
				currentVertex = md3->m_Meshes[currentMesh].Triangle[i].Vertex[j];
				indexVertex   = currentOffsetVertex + currentVertex;
				vf[0]         = (float)md3->m_Meshes[currentMesh].Vertex[indexVertex].Vertex[0] / 64.f;
				vf[1]         = (float)md3->m_Meshes[currentMesh].Vertex[indexVertex].Vertex[2] / 64.f;

				currentVertex = md3->m_Meshes[currentMesh].Triangle[i].Vertex[(j + 1) % 3];
				indexVertex   = currentOffsetVertex + currentVertex;
				wf[0]         = (float)md3->m_Meshes[currentMesh].Vertex[indexVertex].Vertex[0] / 64.f;
				wf[1]         = (float)md3->m_Meshes[currentMesh].Vertex[indexVertex].Vertex[2] / 64.f;

				vi[0] = (int)ceil((vf[0] - md3->m_vMin.x) * VIEWXZ_WIDTH / d);
				vi[1] = VIEWXZ_WIDTH - (int)ceil((vf[1] - md3->m_vMin.z) * VIEWXZ_WIDTH / d);
				wi[0] = (int)ceil((wf[0] - md3->m_vMin.x) * VIEWXZ_WIDTH / d);
				wi[1] = VIEWXZ_WIDTH - (int)ceil((wf[1] - md3->m_vMin.z) * VIEWXZ_WIDTH / d);

				pDC->MoveTo(vi[0], vi[1]);
				pDC->LineTo(wi[0], wi[1]);
			}
		}
	}
}

void CTab4Dlg::DrawYZ(CDC *pDC, CMd3 *md3)
{
	int    i, j;
	int    triangleNum, currentFrame, currentMesh, currentOffsetVertex, currentVertex, indexVertex;
	float  vf[2], wf[2];
	int    vi[2], wi[2];
	float  d;
	CRgn   rgn;
	CPen   pensel(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush BgCol(RGB(192, 192, 192));

	pDC->SelectObject(&BgCol);
	pDC->SelectObject(&pensel);

	pDC->FillSolidRect(0, 0, VIEWYZ_WIDTH, VIEWYZ_WIDTH, RGB(192, 192, 192));

	if (!md3)
	{
		return;
	}

	(md3->m_vRay.y) > (md3->m_vRay.z) ? d = md3->m_vRay.y : d = md3->m_vRay.z;

	currentFrame = 0;
	for (currentMesh = 0; currentMesh < md3->m_Header.numMeshes; currentMesh++)
	{
		currentOffsetVertex = currentFrame * md3->m_Meshes[currentMesh].MeshHeader.numVertexes;

		triangleNum = md3->m_Meshes[currentMesh].MeshHeader.numTriangles;

		for (i = 0; i < triangleNum; i++)
		{
			for (j = 0; j < 3; j++)
			{
				currentVertex = md3->m_Meshes[currentMesh].Triangle[i].Vertex[j];
				indexVertex   = currentOffsetVertex + currentVertex;
				vf[0]         = (float)md3->m_Meshes[currentMesh].Vertex[indexVertex].Vertex[1] / 64.f;
				vf[1]         = (float)md3->m_Meshes[currentMesh].Vertex[indexVertex].Vertex[2] / 64.f;

				currentVertex = md3->m_Meshes[currentMesh].Triangle[i].Vertex[(j + 1) % 3];
				indexVertex   = currentOffsetVertex + currentVertex;
				wf[0]         = (float)md3->m_Meshes[currentMesh].Vertex[indexVertex].Vertex[1] / 64.f;
				wf[1]         = (float)md3->m_Meshes[currentMesh].Vertex[indexVertex].Vertex[2] / 64.f;

				vi[0] = int((vf[0] - md3->m_vMin.y) * VIEWYZ_WIDTH / d );
				vi[1] = VIEWYZ_WIDTH - int((vf[1] - md3->m_vMin.z) * VIEWYZ_WIDTH / d );
				wi[0] = int((wf[0] - md3->m_vMin.y) * VIEWYZ_WIDTH / d );
				wi[1] = VIEWYZ_WIDTH - int((wf[1] - md3->m_vMin.z) * VIEWYZ_WIDTH / d );

				pDC->MoveTo(vi[0], vi[1]);
				pDC->LineTo(wi[0], wi[1]);
			}
		}
	}
}

void CTab4Dlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CStatic  *xz = (CStatic *)GetDlgItem(IDC_VIEWXZ);
	CStatic  *yz = (CStatic *)GetDlgItem(IDC_VIEWYZ);
	CPaintDC xzDC(xz);
	CPaintDC yzDC(yz);
	CBrush   brframe(RGB(0, 0, 0));
	CRect    rect;

	rect = m_ViewXZ;
	rect.InflateRect(1, 1);
	dc.FrameRect(&rect, &brframe);

	rect = m_ViewYZ;
	rect.InflateRect(1, 1);
	dc.FrameRect(&rect, &brframe);

	if (m_Md3Selected)
	{
		DrawXZ(&xzDC, m_Md3Selected);
		DrawYZ(&yzDC, m_Md3Selected);
	}

	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CTab4Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CStatic *xz = (CStatic *)GetDlgItem(IDC_VIEWXZ);
	CStatic *yz = (CStatic *)GetDlgItem(IDC_VIEWYZ);

	xz->GetWindowRect(&m_ViewXZ);
	ScreenToClient(&m_ViewXZ);
	m_ViewXZ.right  = m_ViewXZ.left + VIEWXZ_WIDTH;
	m_ViewXZ.bottom = m_ViewXZ.top + VIEWXZ_WIDTH;

	yz->GetWindowRect(&m_ViewYZ);
	ScreenToClient(&m_ViewYZ);
	m_ViewYZ.right  = m_ViewYZ.left + VIEWYZ_WIDTH;
	m_ViewYZ.bottom = m_ViewYZ.top + VIEWYZ_WIDTH;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CMd3 *CTab4Dlg::GetSelected()
{
	return m_Md3Selected;
}