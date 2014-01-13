// Tab2Dlg.cpp : implementation file
//

#include "EasyGen.h"
#include "Tab2Dlg.h"

#include "EasyGenDoc.h"
#include "EasyGenView.h"

#include "Grid.h"
extern CGrid g_Grid;

#include "Modifier.h"
extern CModifier g_Mod;

#include "Preferences.h"
extern CPreferences g_Prefs;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTab2Dlg dialog


CTab2Dlg::CTab2Dlg(CWnd *pParent /*=NULL*/)
	: CDialog(CTab2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTab2Dlg)
	m_modBaseNoise    = 0.0f;
	m_modBaseScale    = 0.0f;
	m_modBaseSmooth   = 0.0f;
	m_modMode         = -1;
	m_sDescription    = _T("");
	m_fVertexDragStep = 0.0f;
	//}}AFX_DATA_INIT
}

void CTab2Dlg::OnOK()
{
}

void CTab2Dlg::OnCancel()
{
}

void CTab2Dlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTab2Dlg)
	DDX_Control(pDX, IDC_VERTEXDRAGSTEPSPIN, m_butVertexSpin);
	DDX_Control(pDX, IDC_SLSCALE, m_slScale);
	DDX_Control(pDX, IDC_SLSMOOTH, m_slSmooth);
	DDX_Control(pDX, IDC_SLNOISE, m_slNoise);
	DDX_Control(pDX, IDC_MODIFIERS, m_Modifiers);
	DDX_Text(pDX, IDC_GNOISE, m_modBaseNoise);
	DDX_Text(pDX, IDC_GSCALE, m_modBaseScale);
	DDX_Text(pDX, IDC_GSMOOTH, m_modBaseSmooth);
	DDX_Radio(pDX, IDC_MODE_ADD, m_modMode);
	DDX_Text(pDX, IDC_DESCRIPTION, m_sDescription);
	DDX_Text(pDX, IDC_VERTEXDRAGSTEP, m_fVertexDragStep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTab2Dlg, CDialog)
//{{AFX_MSG_MAP(CTab2Dlg)
ON_BN_CLICKED(IDC_MODE_ADD, OnModeAdd)
ON_BN_CLICKED(IDC_MODE_MAX, OnModeMax)
ON_BN_CLICKED(IDC_MODE_IDENT, OnModeIdent)
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_MODAPPLY, OnModApply)
ON_CBN_SELCHANGE(IDC_MODIFIERS, OnSelchangeModifiers)
ON_WM_PAINT()
ON_EN_CHANGE(IDC_VERTEXDRAGSTEP, OnChangeVertexDragStep)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTab2Dlg message handlers

void CTab2Dlg::EnableModMode(BOOL add, BOOL cmp, BOOL force)
{
	CButton *bMax   = (CButton *)GetDlgItem(IDC_MODE_MAX);
	CButton *bAdd   = (CButton *)GetDlgItem(IDC_MODE_ADD);
	CButton *bIdent = (CButton *)GetDlgItem(IDC_MODE_IDENT);
	bAdd->EnableWindow(add);
	bMax->EnableWindow(cmp);
	bIdent->EnableWindow(force);
}

void CTab2Dlg::Init()
{
	CString s;
	int     j;

	m_Modifiers.ResetContent();
	m_Modifiers.InsertString(MOD_NONE, "(none)");
	m_Modifiers.InsertString(MOD_SMOOTH, "Smooth");
	m_Modifiers.InsertString(MOD_BASE, "Base");

	j = MODIFIERS;
	for (int i = 0; i < g_Mod.GetNumModifiers(); i++)
	{
		g_Mod.GetModifierName(i, s);
		s = " > " + s;
		m_Modifiers.InsertString(j++, (LPCTSTR)s);
	}

	m_Modifiers.SetCurSel(MOD_NONE);

	EnableModMode(FALSE, FALSE, FALSE);

	m_slNoise.SetRange(0, SLIDER_NOISE_MAX, TRUE);
	m_slNoise.SetPos(0);
	m_modBaseNoise = 0;

	m_slSmooth.SetRange(0, SLIDER_SMOOTH_MAX, TRUE);
	m_slSmooth.SetPos(0);
	m_modBaseSmooth = 0;

	m_slScale.SetRange(0, SLIDER_SCALE_MAX, TRUE);
	m_slScale.SetPos(SLIDER_SCALE_MAX / 2);
	m_modBaseScale = 1;

	EnableSliders(FALSE);

	CEdit *vds = (CEdit *)GetDlgItem(IDC_VERTEXDRAGSTEP);

	m_butVertexSpin.SetBuddy((CWnd *)vds);
	m_butVertexSpin.SetRange(1, 128);
	m_fVertexDragStep = g_Prefs.m_fVertexDragStep;

	UpdateData(FALSE);

//	m_ModButtons[0].bitmap.LoadBitmap(IDB_MOD_HILL);
//	m_ModButtons[0].rect.SetRect(0,0,27,27);
}

void CTab2Dlg::EnableSliders(BOOL enable)
{
	m_slNoise.EnableWindow(enable);
	m_slSmooth.EnableWindow(enable);
	m_slScale.EnableWindow(enable);
}

CEasyGenDoc *CTab2Dlg::GetDocument()
{
	CEasyGenApp  *pApp       = (CEasyGenApp *)AfxGetApp();
	CMainFrame   *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	CEasyGenView *pView      = (CEasyGenView *)pMainFrame->m_wndSplitter.GetPane(0, 1);
	return pView->GetDocument();
}

void CTab2Dlg::OnModeAdd()
{
	CEasyGenDoc *pDoc = GetDocument();
	pDoc->m_modMode                = MOD_MODE_ADD;
	pDoc->m_bInvalidateGridPreview = TRUE;
	pDoc->RefreshView();
}

void CTab2Dlg::OnModeMax()
{
	CEasyGenDoc *pDoc = GetDocument();
	pDoc->m_modMode                = MOD_MODE_MAX;
	pDoc->m_bInvalidateGridPreview = TRUE;
	pDoc->RefreshView();
}

void CTab2Dlg::OnModeIdent()
{
	CEasyGenDoc *pDoc = GetDocument();
	pDoc->m_modMode                = MOD_MODE_IDENT;
	pDoc->m_bInvalidateGridPreview = TRUE;
	pDoc->RefreshView();
}

void CTab2Dlg::UpdateSliders()
{
	CEasyGenDoc *pDoc = (CEasyGenDoc *)GetDocument();

	m_modBaseNoise       = (float)m_slNoise.GetPos() / (float)SLIDER_NOISE_MAX;
	pDoc->m_modBaseNoise = (float)m_slNoise.GetPos() / (float)SLIDER_NOISE_MAX;

	m_modBaseSmooth       = (float)m_slSmooth.GetPos();
	pDoc->m_modBaseSmooth = (float)m_slSmooth.GetPos() * 2 + 1;

	m_modBaseScale       = (float)m_slScale.GetPos() * 2.f / SLIDER_SCALE_MAX;
	pDoc->m_modBaseScale = m_modBaseScale;

	pDoc->m_bInvalidateGridPreview = TRUE;

	UpdateData(FALSE);
}

void CTab2Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	CEasyGenDoc *pDoc = GetDocument();

	switch (nSBCode)
	{
	case TB_BOTTOM:
		UpdateSliders();
		break;
	case TB_ENDTRACK:
		UpdateSliders();
		break;
	case TB_LINEDOWN:
		UpdateSliders();
		break;
	case TB_LINEUP:
		UpdateSliders();
		break;
	case TB_PAGEDOWN:
		UpdateSliders();
		break;
	case TB_PAGEUP:
		UpdateSliders();
		break;
	case TB_THUMBPOSITION:
		UpdateSliders();
		break;
	case TB_THUMBTRACK:
		UpdateSliders();
		break;
	case TB_TOP:
		UpdateSliders();
		break;
	default:
	{}
	}

	// Update Noise/Smooth
	pDoc->RefreshView();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTab2Dlg::OnModApply()
{
	CEasyGenDoc *pDoc = GetDocument();
	if (pDoc->GetModifier() != MOD_NONE)
	{
		pDoc->UndoGet(&g_Grid);
	}
	pDoc->GridModApply(g_Grid);
	pDoc->m_bInvalidateGridPreview = TRUE;
	pDoc->RefreshView();
}

void CTab2Dlg::UpdateModifier()
{
	CEasyGenDoc *pDoc = GetDocument();

	pDoc->ModSelect(m_Modifiers.GetCurSel());

	UpdateData(TRUE);

	EnableSliders(FALSE);

	EnableModMode(FALSE, FALSE, FALSE);

	switch (pDoc->GetModifier())
	{
	case MOD_NONE:
		m_sDescription = "";
		break;
	case MOD_BASE:
		EnableSliders(TRUE);
		m_slNoise.SetPos(0);
		m_slSmooth.SetPos(0);
		m_slScale.SetPos(SLIDER_SCALE_MAX / 2);
		m_sDescription = "Play with the three sliders to adjust:\r\n\r\n"
		                 "Noise\r\nSmoothness\r\nHeight\r\n\r\n"
		                 "... of the whole terrain.";
		break;
	case MOD_SMOOTH:
		m_sDescription = "Used to smooth an area.\r\n\r\n"
		                 "Increasing modifier's height increases the strenght of the smooth filter.";
		break;
	}

	if (pDoc->GetModifier() >= MODIFIERS)
	{
		S_MODIFIER *m = g_Mod.GetCurrentModifier();

		BOOL enAdd   = m->flags & MOD_FLAG_ADD;
		BOOL enMax   = m->flags & MOD_FLAG_MAX;
		BOOL enIdent = m->flags & MOD_FLAG_IDENT;

		EnableModMode(enAdd, enMax, enIdent);

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

		m_sDescription = m->description;
	}

	UpdateData(FALSE);
}

void CTab2Dlg::OnSelchangeModifiers()
{
	CEasyGenDoc *pDoc = GetDocument();
	UpdateModifier();
	pDoc->m_bInvalidateGridPreview = TRUE;
	pDoc->RefreshView();
}

void CTab2Dlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
/*
    CRect rect;

    m_ModButtonsFrame.GetWindowRect(&rect);

    dc.BitBlt(
            rect.left + m_ModButtons[0].rect.left,
            rect.top + m_ModButtons[0].rect.top,
            28,
            28,
            m_ModButtons[0].bitmap.
*/
	// Do not call CDialog::OnPaint() for painting messages
}

void CTab2Dlg::OnChangeVertexDragStep()
{
	UpdateData();

	g_Prefs.m_fVertexDragStep = m_fVertexDragStep;
}
