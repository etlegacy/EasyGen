// CmdFormView.cpp : implementation file
//

#include "EasyGen.h"
#include "CmdFormView.h"

#include "MainFrm.h"
#include "EasyGenView.h"
#include "EasyGenDoc.h"

#include "Tab1Dlg.h"
#include "Tab2Dlg.h"
#include "Tab3Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdFormView

IMPLEMENT_DYNCREATE(CCmdFormView, CFormView)

CCmdFormView::CCmdFormView()
	: CFormView(CCmdFormView::IDD)
{
	//{{AFX_DATA_INIT(CCmdFormView)
	m_TabButtons = 0;
	//}}AFX_DATA_INIT
}

CCmdFormView::~CCmdFormView()
{
}

void CCmdFormView::DoDataExchange(CDataExchange *pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdFormView)
	DDX_Radio(pDX, IDC_TABBUTTON, m_TabButtons);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdFormView, CFormView)
//{{AFX_MSG_MAP(CCmdFormView)
ON_BN_CLICKED(IDC_TABBUTTON, OnTabButton)
ON_BN_CLICKED(IDC_TABBUTTON2, OnTabButton2)
ON_BN_CLICKED(IDC_TABBUTTON3, OnTabButton3)
ON_BN_CLICKED(IDC_TABBUTTON4, OnTabButton4)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdFormView diagnostics

#ifdef _DEBUG
void CCmdFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdFormView message handlers

CEasyGenDoc *CCmdFormView::GetDocument()
{
	CEasyGenApp  *pApp       = (CEasyGenApp *)AfxGetApp();
	CMainFrame   *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	CEasyGenView *pView      = (CEasyGenView *)pMainFrame->m_wndSplitter.GetPane(0, 1);
	return pView->GetDocument();
}

CView *CCmdFormView::GetRenderView()
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

void CCmdFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_Tab1.DestroyWindow();
	m_Tab2.DestroyWindow();
	m_Tab3.DestroyWindow();
	m_Tab4.DestroyWindow();

	m_Tab1.Create(IDD_TAB1, this);  // Grid		TAB
	m_Tab2.Create(IDD_TAB2, this);  // Modifier	TAB
	m_Tab3.Create(IDD_TAB3, this);  // Alphamap	TAB
	m_Tab4.Create(IDD_TAB4, this);  // Models	TAB

	m_Tab1.ShowWindow(SW_HIDE);
	m_Tab2.ShowWindow(SW_HIDE);
	m_Tab3.ShowWindow(SW_HIDE);
	m_Tab4.ShowWindow(SW_HIDE);

	m_Tab1.Init();
	m_Tab2.Init();
	m_Tab3.Init();
	m_Tab4.Init();

	TabShow(TAB_GRID);
}

void CCmdFormView::TabShow(int tabindex)
{
	CEasyGenApp *pApp = (CEasyGenApp *)AfxGetApp();
	CMainFrame  *pFrm;
	CStatic     *stat = (CStatic *)GetDlgItem(IDC_TABCENTER);
	RECT        rect, dlgrect;
	CDialog     *dlg;

	if (pApp)
	{
		pFrm = (CMainFrame *)pApp->m_pMainWnd;
	}

	m_TabActive = tabindex;

	m_Tab1.ShowWindow(SW_HIDE);
	m_Tab2.ShowWindow(SW_HIDE);
	m_Tab3.ShowWindow(SW_HIDE);
	m_Tab4.ShowWindow(SW_HIDE);

	stat->GetWindowRect(&rect);
	ScreenToClient(&rect);

	switch (tabindex)
	{
	case TAB_GRID:
	{
		dlg = &m_Tab1;
		break;
	}
	case TAB_MODIFIER:
	{
		dlg = &m_Tab2;
		break;
	}
	case TAB_ALPHAMAP:
	{
		dlg = &m_Tab3;
		break;
	}
	case TAB_MODELS:
	{
		dlg = &m_Tab4;
		break;
	}
	default:
		dlg = &m_Tab1;
	}

	dlg->ShowWindow(SW_SHOW);
	dlg->SetWindowPos(NULL, rect.left, rect.top, -1, -1, SWP_NOOWNERZORDER | SWP_NOSIZE);

	dlg->GetWindowRect(&dlgrect);
	ScreenToClient(&dlgrect);
	pFrm->SetFormPaneHeight(dlgrect.bottom + 4);

	pFrm->AdjustFormWidth();

//	GetClientRect(&thisrect);
//	SetWindowPos(NULL,-1,-1, thisrect.right, dlgrect.bottom + 8, SWP_NOOWNERZORDER | SWP_NOMOVE);
}

// GRID
void CCmdFormView::OnTabButton()
{
	CEasyGenDoc *pDoc = (CEasyGenDoc *)GetDocument();

	TabShow(TAB_GRID);

	pDoc->ModifierNone();
	pDoc->RefreshView();
}

// MODIFIER
void CCmdFormView::OnTabButton2()
{
	CEasyGenDoc *pDoc = (CEasyGenDoc *)GetDocument();

	TabShow(TAB_MODIFIER);

	pDoc->RefreshView();
}

// ALPHAMAP
void CCmdFormView::OnTabButton3()
{
	CEasyGenDoc  *pDoc  = (CEasyGenDoc *)GetDocument();
	CEasyGenView *pView = (CEasyGenView *)pDoc->GetRenderView();

	TabShow(TAB_ALPHAMAP);

	pDoc->ModifierNone();

	int curmode = pView->GetRenderMode();

	if (curmode != R_ALPHAMAP && curmode != R_TEXTURE)
	{
		pView->SetRenderMode(R_ALPHAMAP);
	}

	pDoc->RefreshView();
}

// MODELS
void CCmdFormView::OnTabButton4()
{
	CEasyGenDoc *pDoc = (CEasyGenDoc *)GetDocument();

	TabShow(TAB_MODELS);

	pDoc->ModifierNone();
	pDoc->RecalcModelsZ();
	pDoc->RefreshView();
}
