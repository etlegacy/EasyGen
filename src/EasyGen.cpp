// EasyGen.cpp : Defines the class behaviors for the application.
//

#include "EasyGen.h"

#include "MainFrm.h"
#include "EasyGenDoc.h"
#include "EasyGenView.h"
#include <direct.h>
#include "Preferences.h"
#include "NmTimer.h"
#include "Modifier.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////
// GLOBALS //
/////////////////////////////////////////
/////////////////////////////////////////
float        g_AppVersion  = 1.42f;     // application version
float        g_SaveVersion = 1.4f;      // refers to egn save file format
CPreferences g_Prefs;
CGrid        g_Grid;
CGridStack   g_GridUndo;
CNmPolyList  g_World;
CModifier    g_Mod;
CAlphamapMan g_AlphamapMan;
CMd3Man      g_Md3Man;
BOOL         g_FirstDoc = TRUE;
CNmTimer     g_Timer;
//////////////////////////////////////////
//////////////////////////////////////////

float cmp_max(float f1, float f2)
{
	if (f1 < f2)
	{
		return f2;
	}
	return f1;
}

float cmp_min(float f1, float f2)
{
	if (f1 < f2)
	{
		return f1;
	}
	return f2;
}

float distance(float x1, float y1, float x2, float y2)
{
	float xx = x1 - x2;
	float yy = y1 - y2;
	return (float)sqrt(xx * xx + yy * yy);
}

float distance2(float x1, float y1, float x2, float y2)
{
	float xx = x1 - x2;
	float yy = y1 - y2;
	return (xx * xx + yy * yy);
}

void glCube(float x, float y, float z, float sideX, float sideY, float height)
{
	float sx  = sideX / 2;
	float sxx = -sx;
	float sy  = sideY / 2;
	float syy = -sy;
	float hh  = height;

	glPushMatrix();

	glTranslated(x, y, z);

	glBegin(GL_QUADS);
	glVertex3d(sx, sy, hh);
	glVertex3d(sxx, sy, hh);
	glVertex3d(sxx, syy, hh);
	glVertex3d(sx, syy, hh);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3d(sxx, sy, 0);
	glVertex3d(sx, sy, 0);
	glVertex3d(sx, syy, 0);
	glVertex3d(sxx, syy, 0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3d(sx, sy, 0);
	glVertex3d(sx, sy, hh);
	glVertex3d(sx, syy, hh);
	glVertex3d(sx, syy, 0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3d(sxx, syy, 0);
	glVertex3d(sxx, syy, hh);
	glVertex3d(sxx, sy, hh);
	glVertex3d(sxx, sy, 0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3d(sxx, syy, 0);
	glVertex3d(sx, syy, 0);
	glVertex3d(sx, syy, hh);
	glVertex3d(sxx, syy, hh);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3d(sxx, sy, hh);
	glVertex3d(sx, sy, hh);
	glVertex3d(sx, sy, 0);
	glVertex3d(sxx, sy, 0);
	glEnd();

	glPopMatrix();
}

void LogToTxt(LPCTSTR filename, LPCTSTR msg)
{
	CString fp;

	fp = g_Prefs.GetFullPath(filename);

	HANDLE hLog = CreateFile(fp,
	                         GENERIC_WRITE,
	                         FILE_SHARE_READ,
	                         NULL,
	                         CREATE_ALWAYS,
	                         FILE_ATTRIBUTE_NORMAL,
	                         NULL);

	CFile logFile(hLog);

	CArchive ar(&logFile, CArchive::store, 8192);

	ar.WriteString(msg);

	ar.Close();

	logFile.Close();

	CloseHandle(hLog);
}

CString Q3A_TexturePathFilter(CString path)
{
	CString s, ss;
	int     i;

	path.Replace('\\', '/');

	// remove ext
	i = path.ReverseFind('.');
	path.Delete(i, path.GetLength() - i);

	s = path;

	// get texture name + /
	i  = s.ReverseFind('/');
	ss = s.Right(s.GetLength() - i);

	s = s.Left(i);

	// get texture dir
	i = s.ReverseFind('/');
	s = s.Right(s.GetLength() - i - 1);

	return s + ss;
}

CString GetExt(CString fpn)
{
	int i = fpn.ReverseFind('.');

	return fpn.Right(fpn.GetLength() - i - 1);
}

CString GetFileNameNoExtNoPath(CString fpn)
{
	int i = fpn.ReverseFind('.');

	if (i != -1)
	{
		fpn.Delete(i, fpn.GetLength() - i);
	}

	i = fpn.ReverseFind('\\');

	if (i != -1)
	{
		fpn.Delete(0, i + 1);
	}

	return fpn;
}

CString GetFileNameNoPath(CString fpn)
{
	int i;

	i = fpn.ReverseFind('\\');

	if (i != -1)
	{
		fpn.Delete(0, i + 1);
	}

	return fpn;
}

BOOL IsQuake3RootFolder(LPCTSTR pathname)
{
	BOOL    ok;
	CString s;
	FILE    *f;
	s = pathname;
	while (s.GetAt(s.GetLength() - 1) == '\\')
		s.Delete(s.GetAt(s.GetLength() - 1), 1);
	s += "\\quake3.exe";

	ok = (f = fopen(s, "rb")) != NULL;

	if (ok)
	{
		fclose(f);
	}

	return ok;
}

/*

  Osservando una faccia di fronte, i 3 vertici vanno dati in senso orario
*/
void  q3r_cube(CString &s, int x0, int y0, int z0, int x1, int y1, int z1, LPCTSTR texture)
{
	CString q;
	s = "{\r\n";
	// 1
	q.Format("( %d %d %d ) ( %d %d %d ) ( %d %d %d ) ", x0, y0, z0, x0, y0, z1, x1, y0, z1);
	q += texture;
	q += " 0 0 0 0.500000 0.500000 0 0 0\r\n";
	s += q;
	// 2
	q.Format("( %d %d %d ) ( %d %d %d ) ( %d %d %d ) ", x1, y0, z0, x1, y0, z1, x1, y1, z1);
	q += texture;
	q += " 0 0 0 0.500000 0.500000 0 0 0\r\n";
	s += q;
	// 3
	q.Format("( %d %d %d ) ( %d %d %d ) ( %d %d %d ) ", x1, y1, z0, x1, y1, z1, x0, y1, z1);
	q += texture;
	q += " 0 0 0 0.500000 0.500000 0 0 0\r\n";
	s += q;
	// 4
	q.Format("( %d %d %d ) ( %d %d %d ) ( %d %d %d ) ", x0, y1, z0, x0, y1, z1, x0, y0, z1);
	q += texture;
	q += " 0 0 0 0.500000 0.500000 0 0 0\r\n";
	s += q;
	// 5
	q.Format("( %d %d %d ) ( %d %d %d ) ( %d %d %d ) ", x0, y0, z1, x0, y1, z1, x1, y1, z1);
	q += texture;
	q += " 0 0 0 0.500000 0.500000 0 0 0\r\n";
	s += q;
	// 6
	q.Format("( %d %d %d ) ( %d %d %d ) ( %d %d %d ) ", x0, y0, z0, x1, y0, z0, x1, y1, z0);
	q += texture;
	q += " 0 0 0 0.500000 0.500000 0 0 0\r\n";
	s += q;

	s += "}\r\n";
}

void CheckModSubfolders(CString& modfolder)
{
	CString msg;
	CString maps_f, scripts_f, tex_f;

	if (modfolder == "")
	{
		return;
	}

	maps_f    = modfolder + "\\maps";
	scripts_f = modfolder + "\\scripts";
	tex_f     = modfolder + "\\textures";

	BOOL maps_b    = _chdir(LPCTSTR(maps_f)) == 0;
	BOOL scripts_b = _chdir(LPCTSTR(scripts_f)) == 0;
	BOOL tex_b     = _chdir(LPCTSTR(tex_f)) == 0;

	if ((!maps_b) || (!scripts_b) || (!tex_b))
	{
		msg = "The following folders don't exist and need to be created :\r\n\r\n";
		if (!maps_b)
		{
			msg += maps_f;
			msg += "\r\n";
		}
		if (!scripts_b)
		{
			msg += scripts_f;
			msg += "\r\n";
		}
		if (!tex_b)
		{
			msg += tex_f;
			msg += "\r\n";
		}
		msg += "\r\n";
		msg += "Do you let EasyGen creating them for you now ?";

		if (AfxMessageBox(msg, MB_YESNOCANCEL) == IDYES)
		{
			if (!maps_b)   // notfound
			{
				_mkdir(LPCTSTR(maps_f));
			}
			if (!scripts_b)   // notfound
			{
				_mkdir(LPCTSTR(scripts_f));
			}
			if (!tex_b)   // notfound
			{
				_mkdir(LPCTSTR(tex_f));

				AfxMessageBox("The texture folder has been created because you had not one yet.\r\n\r\n"
				              "In order to load some textures to apply to your terrain you must now\r\n"
				              "fill that folder with other subfolders and put the textures you're going\r\n"
				              "to use in them.\r\n\r\n"
				              "After that you can load a texture set from the menù Textures");
			}
		} // YES
	}
}

// ;this is a comment
void RemoveTemplateComments(CNmStr& templ)
{
	int pos_pt, pos_n;

	while ((pos_pt = templ.Find(';')) != -1)
	{
		pos_n = templ.Find('\n', pos_pt);
		if (pos_n == -1)
		{
			pos_n = templ.GetLength() - 1;
		}

		templ.Delete(pos_pt, pos_n - pos_pt + 1);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CEasyGenApp

BEGIN_MESSAGE_MAP(CEasyGenApp, CWinApp)
//{{AFX_MSG_MAP(CEasyGenApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyGenApp construction

CEasyGenApp::CEasyGenApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEasyGenApp object

CEasyGenApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEasyGenApp initialization

BOOL CEasyGenApp::InitInstance()
{
	//	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	//This initialization is no longer needed on the latest compilers
	/*
	#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
	#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
	#endif
	*/

	//Init the devil image library
	ilInit();
	iluInit();
	ilutInit();
	ilEnable(IL_FILE_OVERWRITE);

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
//	SetRegistryKey(_T("EasyGen"));

//	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// get EasyGen current directory ("e:\\vc++\\EasyGen\\")
	g_Prefs.AcquireBasePath();

	// defaults if INI not found or errors
	g_Prefs.SetDefaults();

	CString pcfg;
	pcfg = g_Prefs.GetFullPath("EasyGen.ini");

	if (!g_Prefs.LoadINI((LPCTSTR)pcfg))
	{
		AfxMessageBox("Since this seems to be the first time you run EasyGen,\r\n\r\n"
		              "please browse for a work MOD directory in the next dialog");

		g_Prefs.InputPreferences();

		if (g_Prefs.m_GameBasePath == "")
		{
			AfxMessageBox("EasyGen can't work properly if you don't specify a work MOD directory (ie: baseq3\\)\r\n\r\n"
			              "EasyGen now quitting ... bye bye.");
			return FALSE;
		}

//		g_Prefs.SaveINI("EasyGen.cfg");

//		CString msg;
//		msg.LoadString(IDS_ININOTFOUND);
//		AfxMessageBox((LPCTSTR)msg);
	}
	else // cfg exists but mod dir does not
	{
		g_Prefs.SaveINI(pcfg);

		if (g_Prefs.m_GameBasePath == "")
		{
			AfxMessageBox("Work MOD directory not set, please use the next dialog.");

			g_Prefs.InputPreferences();

			if (g_Prefs.m_GameBasePath == "")
			{
				AfxMessageBox("EasyGen can't work properly if you don't specify a work MOD folder (ie: baseq3\\)\r\n\r\n"
				              "EasyGen now quitting ... bye bye.");
				return FALSE;
			}
		}
	}

	g_Prefs.m_TextureFilter.MakeLower();

	// Loading modifiers
	CString modf = g_Prefs.GetFullPath("modifiers\\");
	g_Mod.LoadFolder((LPCTSTR)modf);

	// Loading models...
	Md3_InitNormals();
	g_Md3Man.LoadModels(g_Prefs.m_GameBasePath);

	g_Timer.Init();

	g_GridUndo.SetSize(g_Prefs.m_dwUndoLevels);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate *pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
	    IDR_MAINFRAME,
	    RUNTIME_CLASS(CEasyGenDoc),
	    RUNTIME_CLASS(CMainFrame),       // main SDI frame window
	    RUNTIME_CLASS(CEasyGenView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
	{
		return FALSE;
	}

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
//	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString m_Author;
	CString m_Email;
	CString m_Site;
	CString m_Version;
	CString m_help;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_Author  = _T("");
	m_Email   = _T("");
	m_Site    = _T("");
	m_Version = _T("");
	m_help    = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_APPAUTHOR, m_Author);
	DDX_Text(pDX, IDC_APPEMAIL, m_Email);
	DDX_Text(pDX, IDC_APPSITE, m_Site);
	DDX_Text(pDX, IDC_APPVERSION, m_Version);
	DDX_Text(pDX, IDC_CMDLIST, m_help);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CEasyGenApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CEasyGenApp message handlers


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Version.Format("%1.2f fixed", g_AppVersion);
	while (m_Version.GetAt(m_Version.GetLength() - 1) == '0')
	{
		m_Version = m_Version.Left(m_Version.GetLength() - 1);
	}

	m_Author = "Francesco Bancalà";
	m_Email  = "bancaf@libero.it";
	m_Site   = "http://digilander.libero.it/ilbanca/";

	m_help  = "note: Grid,Modifier,Alphamap and Models commands work only when the specified TAB is active..";
	m_help += "AlphamapPaint\t\tCTRL+SHIFT+LeftClk.";
	m_help += "AlphamapGet\t\tSHIFT+LeftClk.";
	m_help += "CameraLookUp\t\tA.";
	m_help += "CameraLookDown\t\tZ.";
	m_help += "CameraUp\t\tD.";
	m_help += "CameraDown\t\tC.";
	m_help += "CameraTerrain\t\tPGDN.";
	m_help += "CameraSky\t\tPGUP.";
	m_help += "CameraAlign\t\tEND.";
	m_help += "CameraAlignZ\t\tHOME.";
	m_help += "CameraStrafeLeft\t\tcomma.";
	m_help += "CameraStrafeRight\t\tdot.";
	m_help += "CameraMove\t\tArrows.";
	m_help += "MD3Insert\t\tCTRL+LeftDblClk.";
	m_help += "MD3Select\t\tSHIFT+LeftClk.";
	m_help += "MD3MoveSelected\t\tSHIFT+RightClk.";
	m_help += "MD3DeleteSelected\tBackspace.";
	m_help += "ModifierPlace\t\tCTRL+LeftDblClk.";
	m_help += "ModifierMove\t\tCTRL+LeftClk.";
	m_help += "ModifierHeight\t\tCTRL+RightClk.";
	m_help += "ModifierHeightGet\t\tCTRL+RightDblClk.";
	m_help += "ModifierRadius\t\tSHIFT+RightClk.";
	m_help += "ModifierApply\t\tSPACEBAR.";
	m_help += "ModifierNone\t\tESC.";
	m_help += "ModifierHide/Show\t\tH.";
	m_help += "PlayerPlace\t\tSHIFT+LeftDblClk.";
	m_help += "SwapTris\t\t\tCTRL+LeftClk.";
	m_help += "ToggleAxis\t\tX.";
	m_help += "ToggleWorld\t\tW.";
	m_help += "_InEyeLook\t\tMOUSE.";
	m_help += "_InEyeMove\t\tSHIFT+LeftClk.";
	m_help += "ToggleCulling\t\tC.";
	m_help += "ToggleSmoothing\t\tS.";
	m_help += "TriangleExclude\t\tSHIFT+LeftClk.";
	m_help += "TextureFit\t\t\tKP_*.";
	m_help += "TextureScaleUP\t\tKP_PLUS.";
	m_help += "TextureScaleDN\t\tKP_MINUS.";
	m_help += "VertexRaise\t\tCTRL+SHIFT+LeftClk.";
	m_help += "ViewRotate\t\tLeftClk.";
	m_help += "ViewPan\t\t\tRightClk.";
	m_help += "ViewRaise\t\tLeftClk+RightClk.";
	m_help += "ViewJump\t\tLeftDblClk.";
	m_help += ".";

	m_help.Replace(".", "\r\n");

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
