// EditTemplateDlg.cpp : implementation file
//

#include "easygen.h"
#include "EditTemplateDlg.h"
#include "EasyGenDoc.h"
////////
#include "Nemo.h"
#include "NmStr.h"

#include "Preferences.h"
extern CPreferences g_Prefs;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditTemplateDlg dialog


CEditTemplateDlg::CEditTemplateDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CEditTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditTemplateDlg)
	m_Text     = _T("");
	m_sFind    = _T("");
	m_sRepwith = _T("");
	//}}AFX_DATA_INIT
}


void CEditTemplateDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditTemplateDlg)
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Text(pDX, IDC_MP_FIND, m_sFind);
	DDX_Text(pDX, IDC_MP_REPWITH, m_sRepwith);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditTemplateDlg, CDialog)
//{{AFX_MSG_MAP(CEditTemplateDlg)
ON_BN_CLICKED(IDC_REPLACEALL, OnReplaceAll)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditTemplateDlg message handlers

BOOL CEditTemplateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit *edText = (CEdit *)GetDlgItem(IDC_TEXT);

	LOGFONT logFont;
	logFont.lfHeight         = 14;
	logFont.lfWidth          = 0;
	logFont.lfEscapement     = 0;
	logFont.lfOrientation    = 0;
	logFont.lfWeight         = FW_NORMAL;
	logFont.lfItalic         = 0;
	logFont.lfUnderline      = 0;
	logFont.lfStrikeOut      = 0;
	logFont.lfCharSet        = ANSI_CHARSET;
	logFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality        = PROOF_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	strcpy(logFont.lfFaceName, "Courier New");

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&logFont);

	edText->SetFont(&m_Font);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditTemplateDlg::OnReplaceAll()
{
	UpdateData();

	m_Text.Replace(m_sFind, m_sRepwith);

	UpdateData(FALSE);
}

BOOL CEditTemplateDlg::LoadShaderPreview(LPCTSTR szShaderTemplate, LPCTSTR szMsFoldername, LPCTSTR szMsName)
{
	CNmStr filename, err;
	CNmStr tNoBlend, tBlend, tVertex;

	filename.Format("templates\\%s_noblend.shader", (const char *)szShaderTemplate);
	filename = (const char *)g_Prefs.GetFullPath((const char *)filename);
	if (!Nm_fexist((const char *)filename))
	{
		err.Format("ERROR. Couldn't find:\n\n%s", (const char *)filename);
		AfxMessageBox(err);
		return FALSE;
	}
	tNoBlend.FromFile(filename);
	RemoveTemplateComments(tNoBlend);

	filename.Format("templates\\%s_blend.shader", (const char *)szShaderTemplate);
	filename = (const char *)g_Prefs.GetFullPath((const char *)filename);
	if (!Nm_fexist((const char *)filename))
	{
		err.Format("ERROR. Couldn't find:\n\n%s", (const char *)filename);
		AfxMessageBox(err);
		return FALSE;
	}
	tBlend.FromFile(filename);
	RemoveTemplateComments(tBlend);

	filename.Format("templates\\%s_vertexremap.shader", (const char *)szShaderTemplate);
	filename = (const char *)g_Prefs.GetFullPath((const char *)filename);
	if (!Nm_fexist((const char *)filename))
	{
		err.Format("ERROR. Couldn't find:\n\n%s", (const char *)filename);
		AfxMessageBox(err);
		return FALSE;
	}
	tVertex.FromFile(filename);
	RemoveTemplateComments(tVertex);

	GenMetashaderEx(
	    m_Text,
	    szMsFoldername,
	    szMsName,
	    0,
	    (const char *)tNoBlend,
	    (const char *)tBlend,
	    (const char *)tVertex
	    );

	return TRUE;
}
