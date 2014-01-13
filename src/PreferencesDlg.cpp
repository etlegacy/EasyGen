// PreferencesDlg.cpp : implementation file
//

#include "EasyGen.h"
#include "PreferencesDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog


CPreferencesDlg::CPreferencesDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CPreferencesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferencesDlg)
	m_BmpExportBasePath       = _T("");
	m_BmpImportBasePath       = _T("");
	m_pgWidthX                = 0;
	m_pgWidthY                = 0;
	m_pgHeight                = 0;
	m_TextureFilter           = _T("");
	m_GameBasePath            = _T("");
	m_FrameAAS                = FALSE;
	m_fSlopeAngle             = 0.0f;
	m_dwUndoLevels            = 0;
	m_sWorldShadersIgnoreList = _T("");
	//}}AFX_DATA_INIT
}


void CPreferencesDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDlg)
	DDX_Control(pDX, IDC_FRAMECOLORCTRL, m_FrameColorCtrl);
	DDX_Control(pDX, IDC_BGCOLORCTRL, m_BgColorCtrl);
	DDX_Text(pDX, IDC_BITMAPEXPPATH, m_BmpExportBasePath);
	DDX_Text(pDX, IDC_BITMAPIMPPATH, m_BmpImportBasePath);
	DDX_Text(pDX, IDC_PGX, m_pgWidthX);
	DDX_Text(pDX, IDC_PGY, m_pgWidthY);
	DDX_Text(pDX, IDC_PGZ, m_pgHeight);
	DDX_Text(pDX, IDC_TEXTUREFILTER, m_TextureFilter);
	DDX_Text(pDX, IDC_GAMEPATH, m_GameBasePath);
	DDX_Check(pDX, IDC_FRAMEAAS, m_FrameAAS);
	DDX_Text(pDX, IDC_SLOPEANGLE, m_fSlopeAngle);
	DDX_Text(pDX, IDC_UNDOLEVELS, m_dwUndoLevels);
	DDV_MinMaxInt(pDX, m_dwUndoLevels, 1, 100);
	DDX_Text(pDX, IDC_SHADERSIGNORE, m_sWorldShadersIgnoreList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesDlg, CDialog)
//{{AFX_MSG_MAP(CPreferencesDlg)
ON_BN_CLICKED(IDC_BROWSE_BITMAPEXPPATH, OnBrowseBitmapExpPath)
ON_BN_CLICKED(IDC_BROWSE_BITMAPIMPPATH, OnBrowseBitmapImpPath)
ON_WM_LBUTTONDOWN()
ON_WM_PAINT()
ON_BN_CLICKED(IDC_BROWSE_GAMEPATH, OnBrowseGamePath)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg message handlers

void CPreferencesDlg::OnBrowseBitmapExpPath()
{
	CString    s;
	char       chName[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.hwndOwner = CPreferencesDlg::m_hWnd;
	bi.lpszTitle = "Select base path to export bitmaps";
	LPITEMIDLIST lpIDList = ::SHBrowseForFolder(&bi);
	::SHGetPathFromIDList(lpIDList, chName);

	s = chName;

	if (s != "")
	{
		m_BmpExportBasePath = chName;
		UpdateData(FALSE);
	}
}

void CPreferencesDlg::OnBrowseBitmapImpPath()
{
	CString    s;
	char       chName[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.hwndOwner = CPreferencesDlg::m_hWnd;
	bi.lpszTitle = "Select base path to import bitmaps from";
	LPITEMIDLIST lpIDList = ::SHBrowseForFolder(&bi);
	::SHGetPathFromIDList(lpIDList, chName);

	s = chName;

	if (s != "")
	{
		m_BmpImportBasePath = chName;
		UpdateData(FALSE);
	}
}

void CPreferencesDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;

	// Background color 3D
	m_BgColorCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	if (rect.PtInRect(point))
	{
		CColorDialog dlg(m_BgColor);
		dlg.m_cc.rgbResult = m_BgColor;
		if (dlg.DoModal() == IDOK)
		{
			m_BgColor = dlg.GetColor();
			this->InvalidateRect(&rect, FALSE);
		}
	}

	// Overlay frame color 3D
	m_FrameColorCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	if (rect.PtInRect(point))
	{
		CColorDialog dlg(m_FrameColor);
		if (dlg.DoModal() == IDOK)
		{
			m_FrameColor = dlg.GetColor();
			this->InvalidateRect(&rect, FALSE);
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CPreferencesDlg::OnPaint()
{
	// Device context for painting
	CPaintDC dc(this);

	CRect  rect;
	CBrush BrushBg, BrushFrame;

	// Background color 3D
	m_BgColorCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	BrushBg.CreateSolidBrush(m_BgColor);
	dc.FillRect(&rect, &BrushBg);

	// Overlay frame color 3D
	m_FrameColorCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	BrushFrame.CreateSolidBrush(m_FrameColor);
	dc.FillRect(&rect, &BrushFrame);
}

/*
static int __stdcall BrowseGamePathCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
{
    TCHAR szDir[MAX_PATH] = {NULL};
    CString s;
    if (uMsg == BFFM_INITIALIZED)
    {
        s = g_Prefs.m_GameBasePath;
        //  Browse dialog initialized - set it to our default folder
        wsprintf(szDir, "%s",(LPCTSTR)s);
        // set to what you want
        SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
    }
    return 0;
}
*/
void CPreferencesDlg::OnBrowseGamePath()
{
	CString    s;
	char       chName[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.hwndOwner = CPreferencesDlg::m_hWnd;
//	bi.lpfn = BrowseGamePathCallbackProc;
	bi.lpszTitle = "Select mod base path (ie baseq3) :";
	LPITEMIDLIST lpIDList = ::SHBrowseForFolder(&bi);
	::SHGetPathFromIDList(lpIDList, chName);

	s = chName;

	if (s != "")
	{
		s.MakeLower();
		s.Replace('/', '\\');
		while (s.GetAt(s.GetLength() - 1) == '\\')
			s.Delete(s.GetLength() - 1, 1);

		if (IsQuake3RootFolder(s))
		{
			CString msg;

			msg  = "You have selected this folder as work MOD folder:\r\n\r\n";
			msg += s;
			msg += "\r\n\r\nThis seems to be the Quake3 ROOT folder instead.\r\n";
			msg += "EasyGen will not be able to export data correctly if you don't browse for a MOD folder instead.\r\n\r\n";
			msg += "c:\\quake3\\baseq3\t\tis correct\r\n";
			msg += "c:\\quake3\\missionpack\tis correct\r\n\r\n";
			msg += "c:\\quake3\t\tis NOT\r\n\r\n";
			msg += "Use this folder anyway ? (the map will not work if it's the Quake3 root folder)";

			switch (AfxMessageBox(msg, MB_YESNOCANCEL))
			{
			case (IDYES):
				m_GameBasePath = chName;
				break;
			case (IDNO):
				break;
			case (IDCANCEL):
				break;
			}
			UpdateData(FALSE);
		}
		else
		{
			m_GameBasePath = chName;
			UpdateData(FALSE);
		}
	}
}

void CPreferencesDlg::OnOK()
{
	UpdateData(TRUE);

	if (m_GameBasePath != "")
	{
		CheckModSubfolders(m_GameBasePath);
	}

	CDialog::OnOK();
}
