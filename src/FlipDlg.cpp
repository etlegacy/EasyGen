// FlipDlg.cpp : implementation file
//

#include "EasyGen.h"
#include "FlipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlipDlg dialog


CFlipDlg::CFlipDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CFlipDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlipDlg)
	m_rMode    = -1;
	m_bEnlarge = FALSE;
	//}}AFX_DATA_INIT
}


void CFlipDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlipDlg)
	DDX_Radio(pDX, IDC_FLIPA, m_rMode);
	DDX_Check(pDX, IDC_ENLARGE, m_bEnlarge);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlipDlg, CDialog)
//{{AFX_MSG_MAP(CFlipDlg)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlipDlg message handlers
