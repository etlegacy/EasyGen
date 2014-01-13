// BMPImportSetupDlg.cpp : implementation file
//

#include "EasyGen.h"
#include "BMPImportSetupDlg.h"

// extern link
#include "Preferences.h"
extern CPreferences g_Prefs;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMPImportSetupDlg dialog


CBMPImportSetupDlg::CBMPImportSetupDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CBMPImportSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBMPImportSetupDlg)
	m_Height       = 0.0f;
	m_sInformation = _T("");
	//}}AFX_DATA_INIT

	m_BTCData   = NULL;
	m_sFilename = _T("");
}


void CBMPImportSetupDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBMPImportSetupDlg)
	DDX_Text(pDX, IDC_GRIDHEIGHT, m_Height);
	DDX_Text(pDX, IDC_HMAP_INFORMATION, m_sInformation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBMPImportSetupDlg, CDialog)
//{{AFX_MSG_MAP(CBMPImportSetupDlg)
ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBMPImportSetupDlg message handlers

void CBMPImportSetupDlg::OnOK()
{
	UpdateData(TRUE);

	if (m_Height < 0.f)
	{
		AfxMessageBox("Height must be positive");
		return;
	}

	if (m_sFilename == "")
	{
		AfxMessageBox("Cough cough.\n\nAnd the heightmap ???");
		return;
	}

	CDialog::OnOK();
}

void CBMPImportSetupDlg::OnBrowse()
{
	CFileDialog fd(
	    TRUE,
	    NULL,
	    NULL,
	    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	    "BMP image (*.bmp)|*.bmp|TGA image (*.tga)|*.tga||",
	    NULL
	    );

	fd.m_ofn.lpstrTitle = "Import - Bitmap";

	g_Prefs.m_BmpImportBasePath == "" ?
	fd.m_ofn.lpstrInitialDir                                                       = g_Prefs.m_sCurrentPath :
	                                                      fd.m_ofn.lpstrInitialDir = g_Prefs.m_BmpImportBasePath;

	if (fd.DoModal() == IDOK)
	{
		if (m_BTCObj.LoadImageFromFile(fd.GetPathName()))
		{

			m_BTCData = &m_BTCObj;

			m_sFilename = fd.GetPathName().Right(fd.GetPathName().GetLength() - fd.GetPathName().ReverseFind('\\') - 1);

			m_sInformation.Format(
			    "FILENAME : %s\r\n"
			    "SIZE : %d x %d\r\n"
			    "BBP : %d\r\n",
			    m_sFilename,
			    m_BTCData->GetWidth(), m_BTCData->GetHeight(),
			    m_BTCData->GetBitsPerPixel()
			    );

			UpdateData(FALSE);

			Invalidate();
		}
	}

}

void CBMPImportSetupDlg::OnPaint()
{
	CPaintDC dc(this);    // device context for painting
	CRect    frameImage;
	CBrush   brImage(RGB(0, 0, 0));
	int      dx, dy, imax;

	// frame
	frameImage = m_rectImage;
	frameImage.InflateRect(2, 2);
	dc.FrameRect((LPRECT)frameImage, &brImage);

	if (m_BTCData)
	{
		CStatic *help = (CStatic *)GetDlgItem(IDC_HMAP_HELP);
		help->ShowWindow(SW_HIDE);

		dx = m_rectImage.right - m_rectImage.left;
		dy = m_rectImage.bottom - m_rectImage.top;

		if (m_BTCData->GetWidth() != m_BTCData->GetHeight())
		{
			imax = max(m_BTCData->GetWidth(), m_BTCData->GetHeight());

			dx = (dx * m_BTCData->GetWidth()) / imax;
			dy = (dy * m_BTCData->GetHeight()) / imax;
		}

		m_BTCData->DrawImage(dc.m_hDC, m_rectImage.left, m_rectImage.top, dx, dy);
	}

}

BOOL CBMPImportSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CStatic *hmap = (CStatic *)GetDlgItem(IDC_IMAGE);

	hmap->ShowWindow(SW_HIDE);
	hmap->GetWindowRect(&m_rectImage);
	ScreenToClient(&m_rectImage);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
