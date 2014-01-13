// ImportMapOptionsDlg.cpp : implementation file
//

#include "easygen.h"
#include "ImportMapOptionsDlg.h"
////////
#include "RadiantMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportMapOptionsDlg dialog


CImportMapOptionsDlg::CImportMapOptionsDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CImportMapOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportMapOptionsDlg)
	m_bModels     = FALSE;
	m_bWorldspawn = FALSE;
	m_sMapInfo    = _T("");
	//}}AFX_DATA_INIT
}


void CImportMapOptionsDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportMapOptionsDlg)
	DDX_Control(pDX, IDC_LC_TERRAINS, m_lcTerrains);
	DDX_Check(pDX, IDC_IMPORT_MODELS, m_bModels);
	DDX_Check(pDX, IDC_IMPORT_WORLD, m_bWorldspawn);
	DDX_Text(pDX, IDC_MAP_INFO, m_sMapInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportMapOptionsDlg, CDialog)
//{{AFX_MSG_MAP(CImportMapOptionsDlg)
ON_BN_CLICKED(IDC_IMPORT_WORLD, OnImportWorld)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportMapOptionsDlg message handlers

void CImportMapOptionsDlg::InsertItem(int i,
                                      LPCTSTR szLayers,
                                      LPCTSTR szAlphamap,
                                      LPCTSTR szShader
                                      )
{
	LVITEM  lvi;
	CString strItem;

	lvi.mask = LVIF_IMAGE | LVIF_TEXT;

	strItem      = _T(szLayers);
	lvi.iItem    = i;
	lvi.iSubItem = 0;
	lvi.pszText  = (LPTSTR)(LPCTSTR)(strItem);
	lvi.iImage   = i;
	m_lcTerrains.InsertItem(&lvi);

	strItem      = _T(szShader);
	lvi.iSubItem = 1;
	lvi.pszText  = (LPTSTR)(LPCTSTR)(strItem);
	m_lcTerrains.SetItem(&lvi);

	strItem      = _T(szAlphamap);
	lvi.iSubItem = 2;
	lvi.pszText  = (LPTSTR)(LPCTSTR)(strItem);
	m_lcTerrains.SetItem(&lvi);

	m_lcTerrains.SetItemData(i, i);
}


BOOL CImportMapOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRadiantMap map;
	CNmStr      v_classname, v_terrain, v_shader, v_alphamap, v_layers;
	CNmStr      info_message, info_filename;
	CString     sItem;

	CRect rect;
	float f;
	int   perc[] = { 20, 32, 40 };

	int i, dwItem, dwNewWidth;

	// hide warning
	CWnd *warningbmp = (CWnd *)GetDlgItem(IDC_WORLDWARNING);
	warningbmp->ShowWindow(SW_HIDE);


//	m_lcTerrains.ModifyStyle(	LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON,
//								LVS_REPORT | LVS_ALIGNTOP | LVS_SINGLESEL );
	m_lcTerrains.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_lcTerrains.InsertColumn(0, "layers", LVCFMT_LEFT, -1);
	m_lcTerrains.InsertColumn(1, "shader", LVCFMT_LEFT, -1);
	m_lcTerrains.InsertColumn(2, "alphamap", LVCFMT_LEFT, -1);
	m_lcTerrains.GetClientRect(&rect);
	f = (float)rect.Width() / 100.f;
	for (i = 0; i < 3; i++)
		m_lcTerrains.SetColumnWidth(i, int(f * (float)perc[i]));
	dwNewWidth = 0;
	for (i = 0; i < 3; i++)
		dwNewWidth += m_lcTerrains.GetColumnWidth(i);
	m_lcTerrains.SetColumnWidth(2, m_lcTerrains.GetColumnWidth(2) + rect.Width() - dwNewWidth);

	TRACE("%s\n", (LPCTSTR)m_sFilename);

	dwItem = 0;

	map.Open((LPCTSTR)m_sFilename);

	if (map.IsOpen())
	{
		while (map.ent_GetNext())
		{
			map.ent_GetValue("classname", v_classname);
			map.ent_GetValue("terrain", v_terrain);

			if (v_classname == "func_group" && v_terrain == "1")
			{
				map.ent_GetValue("shader", v_shader);
				map.ent_GetValue("alphamap", v_alphamap);
				map.ent_GetValue("layers", v_layers);

/*				sItem.Format(
                    "%s ; %s ; %s",
                    (const char*)v_layers,
                    (const char*)v_alphamap,
                    (const char*)v_shader
                );*/

				InsertItem(dwItem++,
				           (const char *)v_layers,
				           (const char *)v_alphamap,
				           (const char *)v_shader
				           );
			}

			if (v_classname == "worldspawn")
			{
				map.ent_GetValue("message", info_message);
			}
		}

		map.Close();
	}

	m_sMapInfo.Format(
	    "Filename : %s\r\n"
	    "message : %s\r\n",
	    (const char *)m_sFilename,
	    (const char *)info_message
	    );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportMapOptionsDlg::OnOK()
{
	m_dwTerrain = -1;

	if (m_lcTerrains.GetSelectedCount() == 1)
	{
		POSITION pos   = m_lcTerrains.GetFirstSelectedItemPosition();
		int      nItem = m_lcTerrains.GetNextSelectedItem(pos);

		m_dwTerrain = m_lcTerrains.GetItemData(nItem);
	}

	CDialog::OnOK();
}

void CImportMapOptionsDlg::OnImportWorld()
{
	CWnd *warningbmp = (CWnd *)GetDlgItem(IDC_WORLDWARNING);
	warningbmp->ShowWindow(SW_SHOW);
}
