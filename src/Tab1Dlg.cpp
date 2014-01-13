// Tab1Dlg.cpp : implementation file
//

#include "EasyGen.h"
#include "Tab1Dlg.h"

#include "EasyGenView.h"
#include "EasyGenDoc.h"
#include "MainFrm.h"

#include "Grid.h"
extern CGrid g_Grid;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTab1Dlg dialog


CTab1Dlg::CTab1Dlg(CWnd *pParent /*=NULL*/)
	: CDialog(CTab1Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTab1Dlg)
	m_CellsX        = 0;
	m_CellsY        = 0;
	m_CurCellsX     = _T("");
	m_CurCellsY     = _T("");
	m_CurWidthX     = _T("");
	m_CurWidthY     = _T("");
	m_WidthX        = 0.0f;
	m_WidthY        = 0.0f;
	m_Lock          = FALSE;
	m_CellWidthX    = 0.0f;
	m_CellWidthY    = 0.0f;
	m_CurCellWidthX = _T("");
	m_CurCellWidthY = _T("");
	m_CurMaxHeight  = _T("");
	m_MaxHeight     = 0.0f;
	m_fOriginX      = 0.0f;
	m_fOriginY      = 0.0f;
	m_fOriginZ      = 0.0f;
	m_fStepBy       = 0.0f;
	//}}AFX_DATA_INIT
}

void CTab1Dlg::OnOK()
{
}

void CTab1Dlg::OnCancel()
{
}

void CTab1Dlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTab1Dlg)
	DDX_Control(pDX, IDC_STEPUP, m_butStepUp);
	DDX_Control(pDX, IDC_STEPBACKWARD, m_butStepBackward);
	DDX_Control(pDX, IDC_STEPRIGHT, m_butStepRight);
	DDX_Control(pDX, IDC_STEPLEFT, m_butStepLeft);
	DDX_Control(pDX, IDC_STEPFORWARD, m_butStepForward);
	DDX_Control(pDX, IDC_STEPDOWN, m_butStepDown);
	DDX_Control(pDX, IDC_ALIGNY, m_cboxAlignY);
	DDX_Control(pDX, IDC_ALIGNX, m_cboxAlignX);
	DDX_Text(pDX, IDC_CELLSX, m_CellsX);
	DDX_Text(pDX, IDC_CELLSY, m_CellsY);
	DDX_Text(pDX, IDC_CURCELLSX, m_CurCellsX);
	DDX_Text(pDX, IDC_CURCELLSY, m_CurCellsY);
	DDX_Text(pDX, IDC_CURWIDTHX, m_CurWidthX);
	DDX_Text(pDX, IDC_CURWIDTHY, m_CurWidthY);
	DDX_Text(pDX, IDC_WIDTHX, m_WidthX);
	DDX_Text(pDX, IDC_WIDTHY, m_WidthY);
	DDX_Check(pDX, IDC_LOCK, m_Lock);
	DDX_Text(pDX, IDC_CELLWIDTHX, m_CellWidthX);
	DDX_Text(pDX, IDC_CELLWIDTHY, m_CellWidthY);
	DDX_Text(pDX, IDC_CURCELLWIDTHX, m_CurCellWidthX);
	DDX_Text(pDX, IDC_CURCELLWIDTHY, m_CurCellWidthY);
	DDX_Text(pDX, IDC_CURMAXHEIGHT, m_CurMaxHeight);
	DDX_Text(pDX, IDC_MAXHEIGHT, m_MaxHeight);
	DDX_Text(pDX, IDC_ORIGIN_X, m_fOriginX);
	DDX_Text(pDX, IDC_ORIGIN_Y, m_fOriginY);
	DDX_Text(pDX, IDC_ORIGIN_Z, m_fOriginZ);
	DDX_Text(pDX, IDC_STEPBY, m_fStepBy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTab1Dlg, CDialog)
//{{AFX_MSG_MAP(CTab1Dlg)
ON_WM_PAINT()
ON_BN_CLICKED(IDC_RECELLS, OnReCells)
ON_BN_CLICKED(IDC_REWIDTHS, OnReWidths)
ON_BN_CLICKED(IDC_LOCK, OnLock)
ON_BN_CLICKED(IDC_RECELLWIDTHS, OnReCellWidths)
ON_BN_CLICKED(IDC_REHEIGHT, OnReMaxHeight)
ON_BN_CLICKED(IDC_ORIGIN_SET, OnOriginSet)
ON_BN_CLICKED(IDC_STEPFORWARD, OnStepForward)
ON_BN_CLICKED(IDC_STEPBACKWARD, OnStepBackward)
ON_BN_CLICKED(IDC_STEPLEFT, OnStepLeft)
ON_BN_CLICKED(IDC_STEPRIGHT, OnStepRight)
ON_BN_CLICKED(IDC_STEPUP, OnStepUp)
ON_BN_CLICKED(IDC_STEPDOWN, OnStepDown)
ON_BN_CLICKED(IDC_ORIGIN_GET, OnOriginGet)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTab1Dlg message handlers

void CTab1Dlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

/*
    CRect rect;

    GetClientRect(&rect);

    dc.FillSolidRect(&rect,RGB(100,100,100));
*/
	// Do not call CDialog::OnPaint() for painting messages
}

void CTab1Dlg::Init()
{
	m_Lock = TRUE;

	m_cboxAlignX.ResetContent();
	m_cboxAlignX.InsertString(ALIGN_RIGHT, "Right");
	m_cboxAlignX.InsertString(ALIGN_CENTER, "Center");
	m_cboxAlignX.InsertString(ALIGN_LEFT, "Left");

	m_cboxAlignX.SetCurSel(ALIGN_CENTER);

//	m_cboxAlignX.EnableWindow(FALSE);


	m_cboxAlignY.ResetContent();
	m_cboxAlignY.InsertString(ALIGN_TOP, "Top");
	m_cboxAlignY.InsertString(ALIGN_MIDDLE, "Middle");
	m_cboxAlignY.InsertString(ALIGN_BOTTOM, "Bottom");

	m_cboxAlignY.SetCurSel(ALIGN_MIDDLE);

//	m_cboxAlignY.EnableWindow(FALSE);

	UpdateCells(START_CELLSX, START_CELLSY);
	UpdateWidths(START_WIDTHX, START_WIDTHY);
	UpdateCellWidths(START_WIDTHX / START_CELLSX, START_WIDTHY / START_CELLSY);

	m_fStepBy = 64.f;

	UpdateData(FALSE);
}

CEasyGenDoc *CTab1Dlg::GetDocument()
{
	CEasyGenApp  *pApp       = (CEasyGenApp *)AfxGetApp();
	CMainFrame   *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	CEasyGenView *pView      = (CEasyGenView *)pMainFrame->m_wndSplitter.GetPane(0, 1);
	return pView->GetDocument();
}

void CTab1Dlg::OnReCells()
{
	CEasyGenDoc *pDoc = GetDocument();
	CString     s;

	UpdateData(TRUE);

	if ((m_CellsX < 2) || (m_CellsY < 2))
	{
		AfxMessageBox("Cells can't be less than 2");
		return;
	}

	if ((m_CellsX > GRID_MAX_CELLS) || (m_CellsY > GRID_MAX_CELLS))
	{
		s.Format("Cells can't be more than %d", GRID_MAX_CELLS);
		AfxMessageBox(s);
		return;
	}

	UINT mem = g_Grid.Memory(m_CellsX, m_CellsY);

	// Fill available memory
	MEMORYSTATUS MemStat;
	MemStat.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&MemStat);

	if (mem > ((MemStat.dwTotalPhys * 3) / 4))
	{
		s.Format("The new grid needs at least %d MB of ram.\r\n\r\n"
		         "Your system seems to have %d MB of physical ram avilable at moment.\r\n\r\n"
		         "Do you want to continue ?", mem / (1024 * 1024), MemStat.dwTotalPhys / (1024 * 1024));
		if (AfxMessageBox(s, MB_YESNOCANCEL) != IDYES)
		{
			return;
		}
	}

	pDoc->GridReCells(m_CellsX, m_CellsY, m_Lock, m_cboxAlignX.GetCurSel(), m_cboxAlignY.GetCurSel());

	UpdateCells(g_Grid.GetCellsX(), g_Grid.GetCellsY());
	UpdateCellWidths(g_Grid.GetCellWidthX(), g_Grid.GetCellWidthY());

	UpdateData(FALSE);
}

void CTab1Dlg::OnReWidths()
{
	CEasyGenDoc *pDoc = GetDocument();
	CString     s;

	UpdateData(TRUE);

	if ((m_WidthX < GRID_MIN_WIDTH) || (m_WidthY < GRID_MIN_WIDTH))
	{
		s.Format("Width can't be less than %d units", GRID_MIN_WIDTH);
		AfxMessageBox((LPCTSTR)s);
		return;
	}

	pDoc->GridReWidths(m_WidthX, m_WidthY, m_Lock, m_cboxAlignX.GetCurSel(), m_cboxAlignY.GetCurSel());

	UpdateWidths(g_Grid.GetWidthX(), g_Grid.GetWidthY());
	UpdateCellWidths(g_Grid.GetCellWidthX(), g_Grid.GetCellWidthY());

	UpdateData(FALSE);
}

void CTab1Dlg::OnOriginSet()
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     t_origin;

	UpdateData(TRUE);

	t_origin.Set(m_fOriginX, m_fOriginY, m_fOriginZ);

	pDoc->SetOrigin(t_origin);
}

void CTab1Dlg::OnLock()
{
	UpdateData(TRUE);

	if (m_Lock)
	{
//		m_cboxAlignX.EnableWindow(FALSE);
//		m_cboxAlignY.EnableWindow(FALSE);
	}
	else
	{
//		m_cboxAlignX.EnableWindow();
//		m_cboxAlignY.EnableWindow();
	}
}

void CTab1Dlg::UpdateCells(int CellsX, int CellsY)
{
	m_CellsX = CellsX;
	m_CellsY = CellsY;

	m_CurCellsX.Format("%d", CellsX);
	m_CurCellsY.Format("%d", CellsY);

	UpdateData(FALSE);
}

void CTab1Dlg::UpdateWidths(float WidthX, float WidthY)
{
	m_WidthX = WidthX;
	m_WidthY = WidthY;

	m_CurWidthX.Format("%d", (int)WidthX);
	m_CurWidthY.Format("%d", (int)WidthY);

	UpdateData(FALSE);
}

void CTab1Dlg::UpdateCellWidths(float cwX, float cwY)
{
	m_CellWidthX = cwX;
	m_CellWidthY = cwY;

	m_CurCellWidthX.Format("%d", (int)cwX);
	m_CurCellWidthY.Format("%d", (int)cwY);

	UpdateData(FALSE);
}

void CTab1Dlg::OnReCellWidths()
{
	CEasyGenDoc *pDoc = GetDocument();

	UpdateData(TRUE);

	if ((m_CellWidthX < 1) || (m_CellWidthY < 1))
	{
		AfxMessageBox("CellWidths can't be lesser than 1");
		return;
	}

	pDoc->GridReCellWidths(m_CellWidthX, m_CellWidthY, m_Lock, m_cboxAlignX.GetCurSel(), m_cboxAlignY.GetCurSel());

	UpdateWidths(g_Grid.GetWidthX(), g_Grid.GetWidthY());
	UpdateCellWidths(m_CellWidthX, m_CellWidthY);
}

BOOL CTab1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_butStepUp.SetIcon(IDI_STEP_UP);
	m_butStepDown.SetIcon(IDI_STEP_DOWN);
	m_butStepLeft.SetIcon(IDI_STEP_LEFT);
	m_butStepRight.SetIcon(IDI_STEP_RIGHT);
	m_butStepForward.SetIcon(IDI_STEP_FORWARD);
	m_butStepBackward.SetIcon(IDI_STEP_BACKWARD);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTab1Dlg::OnReMaxHeight()
{
	CEasyGenDoc *pDoc = GetDocument();

	UpdateData(TRUE);

	if (m_MaxHeight < 0)
	{
		AfxMessageBox("MaxHeight can't be lesser than 0");
		return;
	}

	pDoc->GridReMaxHeight(m_MaxHeight);

	UpdateMaxHeight(m_MaxHeight);
}

void CTab1Dlg::UpdateMaxHeight(float maxh)
{
	m_MaxHeight = maxh;

	m_CurMaxHeight.Format("%d", (int)maxh);

	UpdateData(FALSE);
}

void CTab1Dlg::UpdateOrigin(CNmVec3& origin)
{
	m_fOriginX = origin.x;
	m_fOriginY = origin.y;
	m_fOriginZ = origin.z;

	UpdateData(FALSE);
}

void CTab1Dlg::OnStepForward()
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     t_origin;

	UpdateData();
	g_Grid.GetOrigin(t_origin);
	t_origin.y += m_fStepBy;
	UpdateOrigin(t_origin);
	pDoc->SetOrigin(t_origin);
}

void CTab1Dlg::OnStepBackward()
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     t_origin;

	UpdateData();
	g_Grid.GetOrigin(t_origin);
	t_origin.y -= m_fStepBy;
	UpdateOrigin(t_origin);
	pDoc->SetOrigin(t_origin);
}

void CTab1Dlg::OnStepLeft()
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     t_origin;

	UpdateData();
	g_Grid.GetOrigin(t_origin);
	t_origin.x -= m_fStepBy;
	UpdateOrigin(t_origin);
	pDoc->SetOrigin(t_origin);
}

void CTab1Dlg::OnStepRight()
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     t_origin;

	UpdateData();
	g_Grid.GetOrigin(t_origin);
	t_origin.x += m_fStepBy;
	UpdateOrigin(t_origin);
	pDoc->SetOrigin(t_origin);
}

void CTab1Dlg::OnStepUp()
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     t_origin;

	UpdateData();
	g_Grid.GetOrigin(t_origin);
	t_origin.z += m_fStepBy;
	UpdateOrigin(t_origin);
	pDoc->SetOrigin(t_origin);
}

void CTab1Dlg::OnStepDown()
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     t_origin;

	UpdateData();
	g_Grid.GetOrigin(t_origin);
	t_origin.z -= m_fStepBy;
	UpdateOrigin(t_origin);
	pDoc->SetOrigin(t_origin);
}

void CTab1Dlg::OnOriginGet()
{
	CEasyGenDoc *pDoc = GetDocument();
	CNmVec3     t_origin;

	g_Grid.GetOrigin(t_origin);
	UpdateOrigin(t_origin);
}
