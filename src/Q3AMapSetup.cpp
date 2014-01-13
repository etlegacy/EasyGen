// Q3AMapSetup.cpp : implementation file
//

#include "EasyGen.h"
#include "EasyGenDoc.h"
#include "Q3AMapSetup.h"
#include "Nemo.h"
#include "NmList.h"
// extern link

#include "Preferences.h"
extern CPreferences g_Prefs;

#include "AlphamapMan.h"
extern CAlphamapMan g_AlphamapMan;

#include "Grid.h"
extern CGrid g_Grid;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQ3AMapSetup dialog


CQ3AMapSetup::CQ3AMapSetup(CWnd *pParent /*=NULL*/)
	: CDialog(CQ3AMapSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQ3AMapSetup)
	m_TextureNotTop        = _T("");
	m_MetashaderName       = _T("");
	m_MetashaderFilename   = _T("");
	m_AlphamapFilename     = _T("");
	m_MS_Clouds            = FALSE;
	m_ComboTexture         = _T("");
	m_BoxIt                = FALSE;
	m_PlayerStart          = FALSE;
	m_enableBreath         = FALSE;
	m_enableDust           = FALSE;
	m_gridsize             = _T("");
	m_bSaveMap             = FALSE;
	m_bSavePcx             = FALSE;
	m_bSaveShader          = FALSE;
	m_bThinMesh            = FALSE;
	m_bInsert              = FALSE;
	m_MetashaderFoldername = _T("");
	m_bModels              = FALSE;
	m_bExportAllTris       = FALSE;
	//}}AFX_DATA_INIT
}


void CQ3AMapSetup::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQ3AMapSetup)
	DDX_Control(pDX, IDC_SHADERTEMPLATE, m_lbTemplatelist);
	DDX_Control(pDX, IDC_EDITTEMPLATE, m_butEditSelected);
	DDX_Text(pDX, IDC_TEXNOTTOP, m_TextureNotTop);
	DDX_Text(pDX, IDC_METASHADERNAME, m_MetashaderName);
	DDX_Text(pDX, IDC_METASHADERFILENAME, m_MetashaderFilename);
	DDX_Text(pDX, IDC_ALPHAMAPFILENAME, m_AlphamapFilename);
	DDX_Check(pDX, IDC_MS_CLOUDS, m_MS_Clouds);
	DDX_CBString(pDX, IDC_COMBO_TEXTOP, m_ComboTexture);
	DDX_Check(pDX, IDC_BOXIT, m_BoxIt);
	DDX_Check(pDX, IDC_PLAYERSTART, m_PlayerStart);
	DDX_Check(pDX, IDC_ENABLEBREATH, m_enableBreath);
	DDX_Check(pDX, IDC_ENABLEDUST, m_enableDust);
	DDX_Text(pDX, IDC_LIGHTGRIDSIZE, m_gridsize);
	DDX_Check(pDX, IDC_SAVE_MAP, m_bSaveMap);
	DDX_Check(pDX, IDC_SAVE_PCX, m_bSavePcx);
	DDX_Check(pDX, IDC_SAVE_SHADER, m_bSaveShader);
	DDX_Check(pDX, IDC_THINMESH, m_bThinMesh);
	DDX_Check(pDX, IDC_INSERT, m_bInsert);
	DDX_Text(pDX, IDC_METASHADERFOLDER, m_MetashaderFoldername);
	DDX_Check(pDX, IDC_MODELS, m_bModels);
	DDX_Check(pDX, IDC_EXPORTALLTRIS, m_bExportAllTris);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQ3AMapSetup, CDialog)
//{{AFX_MSG_MAP(CQ3AMapSetup)
ON_BN_CLICKED(IDC_BUT_METASHADER, OnButMetashader)
ON_BN_CLICKED(IDC_BUT_ALPHAMAP, OnButAlphamap)
ON_BN_CLICKED(IDC_EDITTEMPLATE, OnEditShader)
ON_CBN_SELCHANGE(IDC_SHADERTEMPLATE, OnSelchangeShaderTemplate)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQ3AMapSetup message handlers

BOOL CQ3AMapSetup::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox *textopcombo = (CComboBox *)GetDlgItem(IDC_COMBO_TEXTOP);

	textopcombo->ResetContent();
	textopcombo->InsertString(0, "common/terrain");
	textopcombo->InsertString(1, "common/terrain2");
	textopcombo->SetCurSel(0);
	textopcombo->LimitText(1024);

	if (!g_Prefs.m_IsQuake3)
	{
		CButton *clouds = (CButton *)GetDlgItem(IDC_MS_CLOUDS);

		clouds->ShowWindow(SW_HIDE);
	}

	CNmStr     templliststr;
	CString    s;
	CNmListStr templlist;
	templliststr.FromFile((const char *)g_Prefs.GetFullPath("templates\\templatelist.txt"));
	templliststr.Replace("\r\n", "\n");
	templlist.FromString((const char *)templliststr, '\n');

	m_butEditSelected.EnableWindow(FALSE);

	m_lbTemplatelist.ResetContent();
	for (int i = 0; i < templlist.GetNum(); i++)
	{
		s = (const char *)templlist.GetAt(i);
		s.TrimLeft();
		s.TrimRight();
		if (
		    s != "" &&
		    (
		        (s.GetLength() >= 2 && s[0] != '/' && s[1] != '/')
		        ||
		        s.GetLength() == 1
		    )
		    )
		{

			m_lbTemplatelist.AddString((const char *)s);
		}
	}
	m_lbTemplatelist.SetCurSel(0);
	m_lbTemplatelist.GetWindowText(s);
	if (s != "")
	{
		m_butEditSelected.EnableWindow(TRUE);

		m_EditTemplateDlg.LoadShaderPreview(
		    (LPCTSTR)s,
		    (LPCTSTR)m_MetashaderFoldername,
		    (LPCTSTR)m_MetashaderName
		    );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQ3AMapSetup::OnButMetashader()
{
	UpdateData(TRUE);

	CFileDialog fd(FALSE,
	               "shader",
	               "",
	               OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	               "Shader files (*.shader)|*.shader||",
	               NULL);

	CString s, scripts;

	scripts  = g_Prefs.m_GameBasePath;
	scripts += "\\scripts";

	scripts.MakeLower();
	while (scripts.Replace("\\\\", "\\"))
		;

	fd.m_ofn.lpstrInitialDir = scripts;

	if (!m_bSaveShader)
	{
		fd.m_ofn.Flags &= ~OFN_OVERWRITEPROMPT;
	}

	if (fd.DoModal() == IDOK)
	{
		s = fd.GetPathName();

		s.MakeLower();

		if (s.Find(scripts, 0) == -1)
		{
			CString msg;
			msg  = "You must locate metashader under ";
			msg += scripts;
			AfxMessageBox(msg);
			UpdateData(FALSE);

			CString log;
			log  = scripts + "\r\n";
			log += "should be contained in \r\n";
			log += s;
			LogToTxt("Error_exp_shader.txt", log);

			return;
		}

//		s.Replace((LPCTSTR)scripts,"");

		m_MetashaderFilename = GetFileNameNoPath(s);
	}

	UpdateData(FALSE);
}

void CQ3AMapSetup::OnButAlphamap()
{
	UpdateData(TRUE);

	CFileDialog fd(FALSE,
	               "",
	               "",
	               OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	               "PCX image (*.pcx)|*.pcx|PNG image (*.png)|*.png|BMP image (*.bmp)|*.bmp||",
	               NULL);

	CString s, alpha;

	alpha = g_Prefs.m_GameBasePath;

	alpha.MakeLower();
	while (alpha.Replace("\\\\", "\\"))
		;

	fd.m_ofn.lpstrInitialDir = alpha;

	if (!m_bSavePcx)
	{
		fd.m_ofn.Flags &= ~OFN_OVERWRITEPROMPT;
	}

	if (fd.DoModal() == IDOK)
	{
		s = fd.GetPathName();

		s.MakeLower();

		if (s.Find(alpha, 0) == -1)
		{
			CString msg;
			msg  = "You must locate alphamap somewhere under ";
			msg += alpha;
			AfxMessageBox(msg);
			UpdateData(FALSE);

			CString log;
			log  = alpha + "\r\n";
			log += "should be contained in \r\n";
			log += s;
			LogToTxt("Error_exp_alphamap.txt", log);

			return;
		}

		alpha += '\\';
		s.Replace((LPCTSTR)alpha, "");

//		m_AlphamapFilename = GetFileNameNoPath( s );
		m_AlphamapFilename = s;
	}

	UpdateData(FALSE);
}

void CQ3AMapSetup::OnOK()
{
	CString sErr;

	UpdateData();

	if ((m_bSaveMap) && (!m_bSaveShader) && (!m_bSavePcx) &&
	    ((m_MetashaderFilename == "") ||
	     (m_MetashaderName == "") ||
	     (m_MetashaderFoldername == "") ||
	     (m_AlphamapFilename == ""))
	    )
	{
		AfxMessageBox("You have decided to save only the map file.\n\n"
		              "You can do that but you must supply a metashader name\n"
		              "and browse for an existing metashader script\n"
		              "and alphamap image before continuing.");
		return;
	}

	sErr = "";
	if ((m_MetashaderFilename == "") && m_bSaveShader)
	{
		sErr = "You must supply a filename for the shader";
	}
	if ((m_MetashaderName == "") && m_bSaveShader)
	{
		sErr = "You must supply a valid name for the shader\n\nex: textures/mymap/terrain";
	}
	if ((m_MetashaderFoldername == "") && m_bSaveShader)
	{
		sErr = "You must supply a valid name for the shader\n\nex: textures/mymap/terrain";
	}
	if ((m_AlphamapFilename == "") && m_bSavePcx)
	{
		sErr = "You must supply a valid filename for the alphamap";
	}

	if (sErr != "")
	{
		AfxMessageBox((LPCTSTR)sErr);
		return;
	}

	CString shtempl;
	m_lbTemplatelist.GetWindowText(shtempl);
	if (shtempl == "")
	{
		AfxMessageBox("Select a shader template.");
		m_lbTemplatelist.SetFocus();
		return;
	}

	CDialog::OnOK();
}

void CQ3AMapSetup::OnEditShader()
{
	CString sSelected;

	UpdateData();

	m_lbTemplatelist.GetWindowText(sSelected);

	if (sSelected == "")
	{
		return;
	}

	m_EditTemplateDlg.DoModal();
}

void CQ3AMapSetup::OnSelchangeShaderTemplate()
{
	CNmStr  filename, tNoBlend, tBlend, tVertex, sSGEN, sTGEN, sTCMOD;
	CString err;
	CString sSelected;

	UpdateData();

	m_lbTemplatelist.GetWindowText(sSelected);

	m_sShaderTemplate = sSelected;

	if (sSelected == "")
	{
		return;
	}

	m_butEditSelected.EnableWindow(TRUE);

	m_EditTemplateDlg.LoadShaderPreview(
	    (LPCTSTR)sSelected,
	    (LPCTSTR)m_MetashaderFoldername,
	    (LPCTSTR)m_MetashaderName
	    );
}

CString CQ3AMapSetup::GetShaderTemplate()
{
	return m_sShaderTemplate;
}
