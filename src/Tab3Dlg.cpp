// Tab3Dlg.cpp : implementation file
//

#include "EasyGen.h"
#include "Tab3Dlg.h"

#include "EasyGenDoc.h"
#include "EasyGenView.h"

#include "AlphamapMan.h"
extern CAlphamapMan g_AlphamapMan;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTab3Dlg dialog


CTab3Dlg::CTab3Dlg(CWnd *pParent /*=NULL*/)
	: CDialog(CTab3Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTab3Dlg)
	m_bDrawTexture  = FALSE;
	m_dwPaintRadius = 0;
	//}}AFX_DATA_INIT
}

void CTab3Dlg::OnOK()
{
}

void CTab3Dlg::OnCancel()
{
}

void CTab3Dlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTab3Dlg)
	DDX_Control(pDX, IDC_SLPAINT, m_slPaintRadius);
	DDX_Control(pDX, IDC_ZOOM, m_lbZoom);
	DDX_Check(pDX, IDC_DRAWTEXTURE, m_bDrawTexture);
	DDX_Text(pDX, IDC_PAINTRADIUS, m_dwPaintRadius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTab3Dlg, CDialog)
//{{AFX_MSG_MAP(CTab3Dlg)
ON_CBN_SELCHANGE(IDC_ZOOM, OnSelchangeZoom)
ON_WM_PAINT()
ON_WM_LBUTTONUP()
ON_BN_CLICKED(IDC_DELETEINDEX, OnDeleteIndex)
ON_WM_LBUTTONDOWN()
ON_WM_RBUTTONDOWN()
ON_BN_CLICKED(IDC_DRAWTEXTURE, OnDrawTexture)
ON_BN_CLICKED(IDC_POSTERIZE, OnPosterize)
ON_WM_HSCROLL()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTab3Dlg message handlers

/*
    - Alphamap in 3D view (glUnproject is necessary)
    -




*/

void CTab3Dlg::Init()
{
	CEasyGenDoc *pDoc = GetDocument();

	m_lbZoom.ResetContent();
	m_lbZoom.InsertString(0, "( All )");
	m_lbZoom.InsertString(1, "0..195");
	m_lbZoom.InsertString(2, "0..143");
	m_lbZoom.InsertString(3, "0..99");
	m_lbZoom.InsertString(4, "0..63");
	m_lbZoom.InsertString(5, "0..35");
	m_lbZoom.InsertString(6, "0..15");
	m_lbZoom.InsertString(7, "0..3");
	m_lbZoom.SetCurSel(0);

	m_bDrawTexture = !g_AlphamapMan.m_DrawColors;

	m_slPaintRadius.SetRange(0, 12, TRUE);
	m_slPaintRadius.SetPos(0);
	m_dwPaintRadius = 0;

	UpdateData(FALSE);
}

void CTab3Dlg::OnSelchangeZoom()
{
	CEasyGenDoc *pDoc = GetDocument();

	g_AlphamapMan.SetZoom(m_lbZoom.GetCurSel() + 1);

	Invalidate();
}

CEasyGenDoc *CTab3Dlg::GetDocument()
{
	CEasyGenApp  *pApp       = (CEasyGenApp *)AfxGetApp();
	CMainFrame   *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	CEasyGenView *pView      = (CEasyGenView *)pMainFrame->m_wndSplitter.GetPane(0, 1);
	return pView->GetDocument();
}

void CTab3Dlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CEasyGenDoc *pDoc = GetDocument();

	CStatic  *pal = (CStatic *)GetDlgItem(IDC_ALPHAPALFRAME);
	CPaintDC pdc(pal);
	CBrush   blk(RGB(0, 0, 0));
	CRect    r;
	COLORREF col;
	int      sel;
	CTexture *tex;

	g_AlphamapMan.Draw(&pdc);
//	r = m_rPal;
//	r.InflateRect(1,1);
//	dc.FrameRect(&r,&blk);

	sel = g_AlphamapMan.GetCurSel();
	col = RGB(0, 0, 0);
	if (sel != -1)
	{
		col = g_AlphamapMan.GetCol(sel);
	}
	dc.FillSolidRect(&m_rCol, col);

	if ((sel != -1) && ((tex = g_AlphamapMan.GetTex(sel)) != NULL))
	{
		tex->m_Thumb.DrawImage(dc.m_hDC, m_rTex.left, m_rTex.top, SELTEX_WIDTH, SELTEX_WIDTH);
	}

	// Do not call CDialog::OnPaint() for painting messages
}

void CTab3Dlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();
	CPoint      p;
	CString     s;
	int         cur, i;
	BOOL        mk_new = FALSE;

	// palette clicked
	if (m_rPal.PtInRect(point))
	{
		p   = point - CPoint(m_rPal.left, m_rPal.top);
		i   = g_AlphamapMan.Get(p);
		cur = g_AlphamapMan.GetCurSel();

		// got an index
		if (i != -1)
		{

			// swap i <> m_IndexGot
			if ((m_IndexGot != -1) && (m_IndexGot != i) &&
			    (m_IndexGot < g_AlphamapMan.GetLayers()) && (i < g_AlphamapMan.GetLayers()))
			{
				g_AlphamapMan.Swap(i, m_IndexGot);
				pDoc->SwapAlphaIndexes(i, m_IndexGot);

				m_IndexGot = -1;
				Invalidate();
			}
			else // add layer(index)
			if (
			    ((g_AlphamapMan.GetLayers() == 0) &&
			     (i == 0))
			    ||
			    ((i > 0) &&
			     (g_AlphamapMan.GetLayers() == i) &&
			     (g_AlphamapMan.GetCol(i - 1) != BLACK)))
			{
				mk_new = TRUE;
			}
			else // only select
			if ((i != cur) &&
			    (i < g_AlphamapMan.GetLayers()))
			{
				g_AlphamapMan.SetCurSel(i);
				// ref SELCOL
				// ref SELTEX

				Invalidate();
			}
		}
	}

	if (mk_new)
	{
		CColorDialog dlg;
		COLORREF     col;
		BOOL         ok = FALSE;

		while (!ok)
		{
			if (dlg.DoModal() == IDOK)
			{
				col = dlg.GetColor();

				if (col == BLACK)
				{
					AfxMessageBox("Any color excpet BLACK plz.");
				}
				else
				if ((g_AlphamapMan.Get(col) != -1))
				{
					AfxMessageBox("The color that you have selected already exists.");
				}
				else // do new
				{
					g_AlphamapMan.m_Layers++;
					g_AlphamapMan.SetCurSel(i);
					g_AlphamapMan.Set(i, col);
					Invalidate();
					ok = TRUE;
				}
			}
			else // abort new
			{
				ok = TRUE;
			}
		} //while
	} //mk_new

	if (m_rCol.PtInRect(point))
	{
		CColorDialog dlg;
		COLORREF     col;
		BOOL         ok = FALSE;

		cur = g_AlphamapMan.GetCurSel();

		if (cur != -1)
		{
			while (!ok)
			{
				if (dlg.DoModal() == IDOK)
				{
					col = dlg.GetColor();

					if (col == BLACK) // error
					{
						AfxMessageBox("You can't select BLACK as color for an index.");
					}
					else    // error
					if ((g_AlphamapMan.Get(col) != -1) &&
					    (g_AlphamapMan.Get(col) != cur))
					{
						AfxMessageBox("This color is already assigned.\r\n\r\n"
						              "Select another one");
					}
					else // ok modify col
					{
						g_AlphamapMan.Set(cur, col);
						pDoc->RefreshView();
						Invalidate();
						ok = TRUE;
					}
				}
				else
				{
					ok = TRUE;
				}
			} //while
		}

	}
//		if (aindex) g_AlphamapMan.m_Selected = aindex;

	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CTab3Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString s;

	CStatic *pal = (CStatic *)GetDlgItem(IDC_ALPHAPALFRAME);
	CStatic *col = (CStatic *)GetDlgItem(IDC_SELCOLFRAME);
	CStatic *tex = (CStatic *)GetDlgItem(IDC_SELTEXFRAME);

	CRect mainr, palr, colr, texr;
	GetWindowRect(&mainr);

	pal->GetWindowRect(&palr);
	palr.left  -= mainr.left;
	palr.top   -= mainr.top;
	palr.right  = palr.left + PAL_WIDTH;
	palr.bottom = palr.top + PAL_WIDTH;
	pal->MoveWindow(palr.left, palr.top, PAL_WIDTH, PAL_WIDTH);
	m_rPal = palr;

	col->GetWindowRect(&colr);
	colr.left  -= mainr.left;
	colr.top   -= mainr.top;
	colr.right  = colr.left + SELCOL_WIDTH;
	colr.bottom = colr.top + SELCOL_WIDTH;
	col->MoveWindow(colr.left, colr.top, SELCOL_WIDTH, SELCOL_WIDTH);
	m_rCol = colr;


	tex->GetWindowRect(&texr);
	texr.left  -= mainr.left;
	texr.top   -= mainr.top;
	texr.right  = texr.left + SELTEX_WIDTH;
	texr.bottom = texr.top + SELTEX_WIDTH;
	tex->MoveWindow(texr.left, texr.top, SELTEX_WIDTH, SELTEX_WIDTH);
	m_rTex = texr;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTab3Dlg::RefreshPal()
{
	CEasyGenDoc *pDoc = GetDocument();
	CStatic     *pal  = (CStatic *)GetDlgItem(IDC_ALPHAPALFRAME);
	CPaintDC    pdc(pal);

	CDC     tmp;
	CBitmap memBM;
	CBitmap *pOldBM;
	CRect   r;

	tmp.CreateCompatibleDC(&pdc);
	memBM.CreateCompatibleBitmap(&pdc, m_rPal.Width(), m_rPal.Height());

	pOldBM = tmp.SelectObject(&memBM);

	r.SetRect(0, 0, m_rPal.Width() - 1, m_rPal.Height() - 1);

	tmp.FillRect(r, &CBrush(RGB(255, 255, 255)));

	//draw memory contents into memory DC tmp
	g_AlphamapMan.Draw(&tmp);

	pdc.BitBlt(0, 0, m_rPal.Width(), m_rPal.Height(), &tmp, 0, 0, SRCCOPY);
	tmp.SelectObject(pOldBM);
	memBM.DeleteObject();
	tmp.DeleteDC();

//	InvalidateRect(&m_rPal,FALSE);
}

void CTab3Dlg::OnDeleteIndex()
{
	CEasyGenDoc *pDoc = GetDocument();

	int cursel = g_AlphamapMan.GetCurSel();

	if (cursel == -1)
	{
		return;
	}

	if (AfxMessageBox("Delete selected index ?", MB_YESNO) != IDYES)
	{
		return;
	}

	pDoc->DeleteAlphaIndex(cursel);

	g_AlphamapMan.Delete(cursel);

	g_AlphamapMan.SetCurSel(-1);

	Invalidate();
}

void CTab3Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();

	CPoint p;
	if (m_rPal.PtInRect(point))
	{
		p          = point - CPoint(m_rPal.left, m_rPal.top);
		m_IndexGot = g_AlphamapMan.Get(p);

		// got an index
//		if (i != -1)
		{
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CTab3Dlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CEasyGenDoc *pDoc = GetDocument();

	m_bDrawTexture = !m_bDrawTexture;

	g_AlphamapMan.m_DrawColors = !m_bDrawTexture;

	Invalidate();

	UpdateData(FALSE);

	CDialog::OnRButtonDown(nFlags, point);
}

void CTab3Dlg::OnDrawTexture()
{
	CEasyGenDoc *pDoc = GetDocument();

	UpdateData(TRUE);

	g_AlphamapMan.m_DrawColors = !m_bDrawTexture;

	Invalidate();
}

void CTab3Dlg::OnPosterize()
{
	CEasyGenDoc *pDoc = GetDocument();
	CString     s;
	int         levels;

	GetDlgItemText(IDC_LEVELS, s);

	if (s == "")
	{
		AfxMessageBox("Fill in the Levels (Lvls) the number of levels for Posterize (2..255)");
		return;
	}

	levels = GetDlgItemInt(IDC_LEVELS);

	if ((levels < 2) || (levels > 255))
	{
		AfxMessageBox("Levels must be in the range 2..255");
		return;
	}

	if (AfxMessageBox("This command will destroy current palette and alphamap.\r\n\r\nDo you wish to continue ?", MB_YESNO) != IDYES)
	{
		return;
	}

	pDoc->Posterize(levels);
}

int CTab3Dlg::GetPaintRadius()
{
	UpdateData();
	return m_slPaintRadius.GetPos();
}

void CTab3Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
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

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTab3Dlg::UpdateSliders()
{
	CEasyGenDoc *pDoc = (CEasyGenDoc *)GetDocument();

	pDoc->UpdateFormView(TAB_ALPHAMAP, FALSE);

	m_dwPaintRadius = m_slPaintRadius.GetPos();

	UpdateData(FALSE);
}
