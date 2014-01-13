// MainFrm.cpp : implementation of the CMainFrame class
//

#include "EasyGen.h"

#include "MainFrm.h"
#include "CmdFormView.h"
#include "EasyGenView.h"
#include "TextureMan.h"

// extern
#include "Preferences.h"
extern CPreferences g_Prefs;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_CREATE()
ON_WM_CLOSE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,         // status line indicator
	ID_INDICATOR_MODIFIER,
	ID_INDICATOR_PLAYER,
	ID_INDICATOR_GRID,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static int widths[] =
{
	-1,
	200,
	300,
	350,
	400,
	450,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_dwFormWidth = 0;
}

CMainFrame::~CMainFrame()
{
}

/*
// load bitmap from resources using LoadImage;
// do not use LoadBitmap for loading true color images
HBITMAP hBitmap = (HBITMAP)LoadImage(AfxGetResourceHandle(),
MAKEINTRESOURCE(IDB_HOTTOOLBAR), IMAGE_BITMAP,
0, 0, LR_CREATEDIBSECTION);

CBitmap bmImage;
bmImage.Attach(hBitmap);

// obtain size of bitmap
BITMAP bm;
bmImage.GetBitmap(&bm);

int nWidth = bm.bmWidth;
int hHeight = bm.bmHeight;

// create empty image list and direct it to use
// 24-bit dibsection as a storage for images
img.Create(22, nHeight, ILC_COLOR24|ILC_MASK, nWidth/22, 0);

// add bitmap to the image list and specify mask color
img.Add(&bmImage, RGB(0, 255, 255));

// finally, assign image list to the toolbar
m_wndToolBar.GetToolBarCtrl().SetHotImageList(&img);
*/
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	                           | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	    !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
	    !m_wndStatusBar.SetIndicators(indicators,
	                                  sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

//	m_wndStatusBar.GetStatusBarCtrl().SetParts(sizeof(widths)/sizeof(int),widths);

	UINT style;

	style = m_wndStatusBar.GetPaneStyle(STATUS_MODIFIER);
	m_wndStatusBar.SetPaneInfo(STATUS_MODIFIER, ID_INDICATOR_MODIFIER, style, 210);

	style = m_wndStatusBar.GetPaneStyle(STATUS_PLAYER);
	m_wndStatusBar.SetPaneInfo(STATUS_PLAYER, ID_INDICATOR_PLAYER, style, 180);

	style = m_wndStatusBar.GetPaneStyle(STATUS_GRID);
	m_wndStatusBar.SetPaneInfo(STATUS_GRID, ID_INDICATOR_GRID, style, 100);

	CString apptitle, s;
	apptitle.LoadString(AFX_IDS_APP_TITLE);
	s.Format(WTITLE_FORMAT, "", g_Prefs.m_GameBasePath, apptitle);
	SetWindowText((LPCTSTR)s);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~FWS_ADDTOTITLE;   // No documentname in titlebar.
//	cs.style &= ~WS_THICKFRAME;

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext)
{
	CRect   rectform;
	CDialog *dlgform;
	int     width_form;
	int     height_form;

	if (!m_wndSplitter.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE))
	{
		TRACE("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	if (!m_wndSplitterLeft.CreateStatic(&m_wndSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(0, 0)))
	{
		m_wndSplitterLeft.DestroyWindow();
		return FALSE;
	}

	// retriving IDD_CMD rect
	dlgform = new CDialog;
	dlgform->Create(IDD_CMD, this);
	dlgform->GetWindowRect(&rectform);
	delete dlgform;

	width_form  = rectform.right - rectform.left;
	height_form = rectform.bottom - rectform.top;

	m_wndSplitter.SetColumnInfo(0, width_form, 0);

	// (CCmdFormView)
	if (!m_wndSplitterLeft.CreateView(0, 0,
	                                  RUNTIME_CLASS(CCmdFormView), CSize(width_form, height_form), pContext))
	{
		TRACE("Failed to create command view pane\n");
		return FALSE;
	}

	m_wndSplitterLeft.SetColumnInfo(0, width_form, 0);

	// (CTextureMan)
	if (!m_wndSplitterLeft.CreateView(1, 0,
	                                  RUNTIME_CLASS(CTextureMan), CSize(width_form, 200), pContext))
	{
		TRACE("Failed to create command view pane\n");
		return FALSE;
	}

	// (CEasyGenView)
	if (!m_wndSplitter.CreateView(0, 1,
	                              RUNTIME_CLASS(CEasyGenView), CSize(200, 200), pContext))
	{
		TRACE("Failed to create preview pane\n");
		return FALSE;
	}

	m_dwFormWidth = width_form;

	return TRUE;
}

void CMainFrame::SendStatusText(UINT index, LPCTSTR text)
{
	m_wndStatusBar.GetStatusBarCtrl().SetText(text, index, 0);
}

void CMainFrame::SetFormPaneHeight(int dwHeight)
{
	m_wndSplitterLeft.SetRowInfo(0, dwHeight, 0);
	m_wndSplitterLeft.RecalcLayout();

//	TRACE("->%d\n", m_dwFormWidth);
}

void CMainFrame::OnClose()
{
	int resp;

	resp = AfxMessageBox("Are you sure you want to quit?\nAll of your unsaved work will be lost.", MB_YESNOCANCEL);

	if (resp != IDYES)
	{
		return;
	}

	CFrameWnd::OnClose();
}

void CMainFrame::AdjustFormWidth()
{
	int curw, minw;

	m_wndSplitter.GetColumnInfo(0, curw, minw);
	if (curw != m_dwFormWidth)
	{
		m_wndSplitter.SetColumnInfo(0, m_dwFormWidth, 0);
		m_wndSplitter.RecalcLayout();
	}
}
