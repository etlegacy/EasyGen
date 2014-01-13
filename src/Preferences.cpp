// Preferences.cpp: implementation of the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

#include "EasyGen.h"
#include "Preferences.h"

#include "PreferencesDlg.h"
#include "Nemo.h"

// extern link
extern CPreferences g_Prefs;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPreferences::CPreferences()
{
	m_sCurrentPath = "";

	m_GameBasePath      = "";
	m_BmpImportBasePath = "";
	m_BmpExportBasePath = "";
	m_dwUndoLevels      = 4;
	m_PlayerWidthX      = -1;
	m_PlayerWidthY      = -1;
	m_PlayerHeight      = -1;

	m_fSlopeAngle     = 45.f;
	m_fVertexDragStep = 4.f;

	m_sWorldShadersIgnoreList = "";
}

CPreferences::~CPreferences()
{

}

BOOL CPreferences::LoadINI(LPCTSTR filename)
{
	CString section = "general";
	CString ini     = filename;
	char    buf[255];
	CString s;
	int     i, r, g, b;

	if (!Nm_fexist(ini))
	{
		return false;
	}

	GetPrivateProfileString(section, "GameBasePath", "", buf, 255, ini);
	m_GameBasePath = buf;

	GetPrivateProfileString(section, "BmpImportBasePath", "", buf, 255, ini);
	m_BmpImportBasePath = buf;

	GetPrivateProfileString(section, "BmpExportBasePath", "", buf, 255, ini);
	m_BmpExportBasePath = buf;

	GetPrivateProfileString(section, "TextureFilterExt", "jpg;tga", buf, 255, ini);
	m_TextureFilter = buf;

	GetPrivateProfileString(section, "BackgroundColor3D", "128 128 128", buf, 255, ini);
	i = sscanf(buf, "%d %d %d", &r, &g, &b);
	if (i == 3 && i != EOF)
	{
		m_BgColor = RGB(r, g, b);
	}

	GetPrivateProfileString(section, "OverlayFrameColor3D", "255 255 255", buf, 255, ini);
	i = sscanf(buf, "%d %d %d", &r, &g, &b);
	if (i == 3 && i != EOF)
	{
		m_FrameColor = RGB(r, g, b);
	}

	m_FrameTransparency = GetPrivateProfileInt(section, "OverlayFrameTransparency3D", 64, ini);

	m_FrameAAS = GetPrivateProfileInt(section, "OverlayFrameAAS3D", 1, ini) == 1;

	m_Fov = GetPrivateProfileInt(section, "Fov", 90, ini);

	m_dwUndoLevels = GetPrivateProfileInt(section, "Undo", 4, ini);

	m_fSlopeAngle = (float)GetPrivateProfileInt(section, "SlopeMax", 45, ini);

	m_IsQuake3 = GetPrivateProfileInt(section, "IsQuake3", 0, ini) == 1;

	GetPrivateProfileString(section, "SteepTriangleColor", "255 255 0", buf, 255, ini);
	i = sscanf(buf, "%d %d %d", &r, &g, &b);
	if (i == 3 && i != EOF)
	{
		m_colSteepTris.Set(float(r) / 255.f, float(g) / 255.f, float(b) / 255.f);
	}

	GetPrivateProfileString(section, "FlatTriangleColor", "255 0 0", buf, 255, ini);
	i = sscanf(buf, "%d %d %d", &r, &g, &b);
	if (i == 3 && i != EOF)
	{
		m_colFlatTris.Set(float(r) / 255.f, float(g) / 255.f, float(b) / 255.f);
	}

	GetPrivateProfileString(section, "ExcludedTriangleColor", "255 0 0", buf, 255, ini);
	i = sscanf(buf, "%d %d %d", &r, &g, &b);
	if (i == 3 && i != EOF)
	{
		m_colExcludedTris.Set(float(r) / 255.f, float(g) / 255.f, float(b) / 255.f);
	}

	GetPrivateProfileString(section, "PlayerSize", "32 32 56", buf, 255, ini);
	i = sscanf(buf, "%d %d %d", &r, &g, &b);
	if (i == 3 && i != EOF)
	{
		m_PlayerWidthX = r;
		m_PlayerWidthY = g;
		m_PlayerHeight = b;
	}

	GetPrivateProfileString(section, "WorldShadersIgnore", (const char *)m_sWorldShadersIgnoreList, buf, 255, ini);
	m_sWorldShadersIgnoreList = buf;

	return true;
/*
    CString record,workrecord,tag,index;
    BYTE r,g,b;
    char nums[11] = "0123456789";

    HANDLE hFile = CreateFile(	(LPCTSTR)filename,
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    CFile file((int)hFile);

    CArchive ini(&file,CArchive::load);

    //

    m_WholeINI = "";

    while (ini.ReadString(record))
    {

        m_WholeINI += record;
        m_WholeINI += "\r\n";

        workrecord = record;

        RecordPurge(workrecord);

        if	(
                (workrecord.GetLength() <= 1) ||
                (workrecord.Find('=')==-1) ||
                ( (workrecord.GetAt(0)=='/') && (workrecord.GetAt(1)=='/') )
            )
        {
            continue;
        }

        // "tag=index"
        tag = RecordGetTag(workrecord);
        index = RecordGetIndex(workrecord);

        if (tag == "GameBasePath")
        {
            index.MakeLower();
            m_GameBasePath = index;
            m_WholeINI.Replace(record,"[GameBasePath]");
        } else
        if (tag == "BmpImportBasePath")
        {
            index.MakeLower();
            m_BmpImportBasePath = index;
            m_WholeINI.Replace(record,"[BmpImportBasePath]");
        } else
        if (tag == "BmpExportBasePath")
        {
            index.MakeLower();
            m_BmpExportBasePath = index;
            m_WholeINI.Replace(record,"[BmpExportBasePath]");
        } else
        if (tag == "TextureFilterExt")
        {
            index.MakeLower();
            m_TextureFilter = index;
            m_WholeINI.Replace(record,"[TextureFilterExt]");
        } else
        if (tag == "BackgroundColor3D")
        {
            if (index.GetLength()>=5)
            {
                r = (BYTE)IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                g = (BYTE)IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                b = (BYTE)IntExtract(index);
                m_BgColor = RGB(r,g,b);
                m_WholeINI.Replace(record,"[BackgroundColor3D]");
            }
        } else
        if (tag == "OverlayFrameColor3D")
        {
            if (index.GetLength()>=5)
            {
                r = (BYTE)IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                g = (BYTE)IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                b = (BYTE)IntExtract(index);
                m_FrameColor = RGB(r,g,b);
                m_WholeINI.Replace(record,"[OverlayFrameColor3D]");
            }
        } else
        if (tag == "OverlayFrameTransparency3D")
        {
            m_FrameTransparency = IntExtract(index);
            m_WholeINI.Replace(record,"[OverlayFrameTransparency3D]");
        } else
        if (tag == "OverlayFrameAAS3D")
        {
            m_FrameAAS = IntExtract(index) == 1;
            m_WholeINI.Replace(record,"[OverlayFrameAAS3D]");
        } else
        if (tag == "Fov")
        {
            m_Fov = (float)IntExtract(index);
            m_WholeINI.Replace(record,"[Fov]");
        } else
        if (tag == "SlopeMax")
        {
            m_fSlopeAngle = (float)IntExtract(index);
            m_WholeINI.Replace(record,"[SlopeMax]");
        } else
        if (tag == "IsQuake3")
        {
            m_IsQuake3 = IntExtract(index) == 1;
            m_WholeINI.Replace(record,"[IsQuake3]");
        } else
        if (tag == "SteepTriangleColor")
        {
            if (index.GetLength()>=5)
            {
                r = (BYTE)IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                g = (BYTE)IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                b = (BYTE)IntExtract(index);
                m_colSteepTris.Set(float(r)/255.f,float(g)/255.f,float(b)/255.f);
                m_WholeINI.Replace(record,"[SteepTriangleColor]");
            }
        } else
        if (tag == "FlatTriangleColor")
        {
            if (index.GetLength()>=5)
            {
                r = (BYTE)IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                g = (BYTE)IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                b = (BYTE)IntExtract(index);
                m_colFlatTris.Set(float(r)/255.f,float(g)/255.f,float(b)/255.f);
                m_WholeINI.Replace(record,"[FlatTriangleColor]");
            }
        } else
        if (tag == "PlayerSize")
        {
            if (index.GetLength()>=5)
            {
                m_PlayerWidthX = IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                m_PlayerWidthY = IntExtract(index);
                index.Delete(0,index.FindOneOf(nums));
                m_PlayerHeight = IntExtract(index);
                m_WholeINI.Replace(record,"[PlayerSize]");
            }
        }
    }

    //

    ini.Close();

    file.Close();

    CloseHandle(hFile);

    return TRUE;
*/
}

//
//	s : "TAG=INDEX"
//
// NO ERROR checking
CString CPreferences::RecordGetTag(CString s)
{
	return s.Left(s.Find('='));
}

//
//	s : "TAG=INDEX"
//
// NO ERROR checking
CString CPreferences::RecordGetIndex(CString s)
{
	return s.Right(s.GetLength() - s.Find('=') - 1);
}

//
// "     Text    = wooo      o      ,  oo        "
//
// "Text=wooo      o      ,  oo"
//
void CPreferences::RecordPurge(CString &s)
{
	s.TrimLeft();
	s.TrimRight();

	while (s.Replace(" =", "="))
		;
	while (s.Replace("= ", "="))
		;
	while (s.Replace(" \r\n", "\r\n"))
		;                               // EOL trimming

	s.Replace("\t", "");
}

int CPreferences::IntExtract(CString &s)
{
	char buf[32];
	int  i = 0;

	while ((i < s.GetLength()) && (s.GetAt(i) >= '0') && (s.GetAt(i) <= '9'))
	{
		buf[i] = s.GetAt(i);
		i++;
	}

	s.Delete(0, i);

	buf[i] = '\0';

	return atoi(buf);
}

void CPreferences::InputPreferences()
{
	CPreferencesDlg m_PrefDlg;

	m_PrefDlg.m_GameBasePath            = m_GameBasePath;
	m_PrefDlg.m_BmpExportBasePath       = m_BmpExportBasePath;
	m_PrefDlg.m_BmpImportBasePath       = m_BmpImportBasePath;
	m_PrefDlg.m_BgColor                 = m_BgColor;
	m_PrefDlg.m_FrameColor              = m_FrameColor;
	m_PrefDlg.m_FrameAAS                = m_FrameAAS;
	m_PrefDlg.m_pgWidthX                = m_PlayerWidthX;
	m_PrefDlg.m_pgWidthY                = m_PlayerWidthY;
	m_PrefDlg.m_pgHeight                = m_PlayerHeight;
	m_PrefDlg.m_fSlopeAngle             = m_fSlopeAngle;
	m_PrefDlg.m_TextureFilter           = m_TextureFilter;
	m_PrefDlg.m_dwUndoLevels            = m_dwUndoLevels;
	m_PrefDlg.m_sWorldShadersIgnoreList = m_sWorldShadersIgnoreList;

	if (m_PrefDlg.DoModal() != IDOK)
	{
		return;
	}

	m_GameBasePath            = m_PrefDlg.m_GameBasePath;
	m_BmpExportBasePath       = m_PrefDlg.m_BmpExportBasePath;
	m_BmpImportBasePath       = m_PrefDlg.m_BmpImportBasePath;
	m_BgColor                 = m_PrefDlg.m_BgColor;
	m_FrameColor              = m_PrefDlg.m_FrameColor;
	m_FrameAAS                = m_PrefDlg.m_FrameAAS;
	m_PlayerWidthX            = m_PrefDlg.m_pgWidthX;
	m_PlayerWidthY            = m_PrefDlg.m_pgWidthY;
	m_PlayerHeight            = m_PrefDlg.m_pgHeight;
	m_fSlopeAngle             = m_PrefDlg.m_fSlopeAngle;
	m_TextureFilter           = m_PrefDlg.m_TextureFilter;
	m_dwUndoLevels            = m_PrefDlg.m_dwUndoLevels;
	m_sWorldShadersIgnoreList = m_PrefDlg.m_sWorldShadersIgnoreList;

	CString pcfg = g_Prefs.GetFullPath("EasyGen.ini");

	SaveINI(pcfg);
}

void CPreferences::SaveINI(LPCTSTR filename)
{
	CString ini     = filename;
	CString section = "general";
	CString s;

	WritePrivateProfileString(section, "GameBasePath", m_GameBasePath, ini);

	WritePrivateProfileString(section, "BmpImportBasePath", m_BmpImportBasePath, ini);
	WritePrivateProfileString(section, "BmpExportBasePath", m_BmpExportBasePath, ini);

	WritePrivateProfileString(section, "TextureFilterExt", m_TextureFilter, ini);

	s.Format("%d %d %d", m_PlayerWidthX, m_PlayerWidthY, m_PlayerHeight);
	WritePrivateProfileString(section, "PlayerSize", s, ini);

	s.Format("%d %d %d", GetRValue(m_BgColor), GetGValue(m_BgColor), GetBValue(m_BgColor));
	WritePrivateProfileString(section, "BackgroundColor3D", s, ini);

	s.Format("%d %d %d", GetRValue(m_FrameColor), GetGValue(m_FrameColor), GetBValue(m_FrameColor));
	WritePrivateProfileString(section, "OverlayFrameColor3D", s, ini);

	s.Format("%d", m_FrameTransparency);
	WritePrivateProfileString(section, "OverlayFrameTransparency3D", s, ini);

	s.Format("%d", (int)m_FrameAAS);
	WritePrivateProfileString(section, "OverlayFrameAAS3D", s, ini);

	s.Format("%d", (int)m_Fov);
	WritePrivateProfileString(section, "Fov", s, ini);

	s.Format("%d", (int)m_dwUndoLevels);
	WritePrivateProfileString(section, "Undo", s, ini);

	s.Format("%d", (int)m_fSlopeAngle);
	WritePrivateProfileString(section, "SlopeMax", s, ini);

	s.Format("%d %d %d", int(m_colSteepTris.x * 255.f), int(m_colSteepTris.y * 255.f), int(m_colSteepTris.z * 255.f));
	WritePrivateProfileString(section, "SteepTriangleColor", s, ini);

	s.Format("%d %d %d", int(m_colFlatTris.x * 255.f), int(m_colFlatTris.y * 255.f), int(m_colFlatTris.z * 255.f));
	WritePrivateProfileString(section, "FlatTriangleColor", s, ini);

	s.Format("%d %d %d", int(m_colExcludedTris.x * 255.f), int(m_colExcludedTris.y * 255.f), int(m_colExcludedTris.z * 255.f));
	WritePrivateProfileString(section, "ExcludedTriangleColor", s, ini);

	s.Format("%d", (int)m_IsQuake3);
	WritePrivateProfileString(section, "IsQuake3", s, ini);

	WritePrivateProfileString(section, "WorldShadersIgnore", m_sWorldShadersIgnoreList, ini);

/*	HANDLE hFile = CreateFile(	(LPCTSTR)filename,
                                GENERIC_WRITE,
                                FILE_SHARE_READ,
                                NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }

    CFile file((int)hFile);

    CArchive ini(&file,CArchive::store);

    WholeINI = m_WholeINI;

    record = "GameBasePath = " + m_GameBasePath;
    if (!WholeINI.Replace("[GameBasePath]",record))
    {
        WholeINI += record + "\r\n";
    }

    record = "BmpImportBasePath = " + m_BmpImportBasePath;
    if (!WholeINI.Replace("[BmpImportBasePath]",record))
    {
        WholeINI += record + "\r\n";
    }

    record = "BmpExportBasePath = " + m_BmpExportBasePath;
    if (!WholeINI.Replace("[BmpExportBasePath]",record))
    {
        WholeINI += record + "\r\n";
    }

    record = "TextureFilterExt = " + m_TextureFilter;
    if (!WholeINI.Replace("[TextureFilterExt]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("PlayerSize = %d,%d,%d",m_PlayerWidthX,m_PlayerWidthY,m_PlayerHeight);
    if (!WholeINI.Replace("[PlayerSize]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("BackgroundColor3D = %d,%d,%d",GetRValue(m_BgColor),GetGValue(m_BgColor),GetBValue(m_BgColor));
    if (!WholeINI.Replace("[BackgroundColor3D]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("OverlayFrameColor3D = %d,%d,%d",GetRValue(m_FrameColor),GetGValue(m_FrameColor),GetBValue(m_FrameColor));
    if (!WholeINI.Replace("[OverlayFrameColor3D]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("OverlayFrameTransparency3D = %d", m_FrameTransparency);
    if (!WholeINI.Replace("[OverlayFrameTransparency3D]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("OverlayFrameAAS3D = %d", (int)m_FrameAAS);
    if (!WholeINI.Replace("[OverlayFrameAAS3D]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("Fov = %d", (int)m_Fov);
    if (!WholeINI.Replace("[Fov]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("SlopeMax = %d", (int)m_fSlopeAngle);
    if (!WholeINI.Replace("[SlopeMax]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("SteepTriangleColor = %1.0f,%1.0f,%1.0f",m_colSteepTris.x*255.f,m_colSteepTris.y*255.f,m_colSteepTris.z*255.f);
    if (!WholeINI.Replace("[SteepTriangleColor]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("FlatTriangleColor = %1.0f,%1.0f,%1.0f",m_colFlatTris.x*255.f,m_colFlatTris.y*255.f,m_colFlatTris.z*255.f);
    if (!WholeINI.Replace("[FlatTriangleColor]",record))
    {
        WholeINI += record + "\r\n";
    }

    record.Format("IsQuake3 = %d", (int)m_IsQuake3);
    if (!WholeINI.Replace("[IsQuake3]",record))
    {
        WholeINI += record + "\r\n";
    }

    ini.WriteString(WholeINI);

    ini.Close();

    file.Close();

    CloseHandle(hFile);
*/
}

void CPreferences::AcquireBasePath()
{
	TCHAR szBuffer[_MAX_PATH];
	VERIFY(::GetModuleFileName(AfxGetInstanceHandle(), szBuffer, _MAX_PATH));
	m_sCurrentPath = (CString)szBuffer;
	m_sCurrentPath = m_sCurrentPath.Left(m_sCurrentPath.ReverseFind('\\') + 1);
}

void CPreferences::SetDefaults()
{
	m_GameBasePath            = "";
	m_BmpExportBasePath       = "";
	m_BmpImportBasePath       = "";
	m_BgColor                 = RGB(120, 120, 120);
	m_FrameColor              = RGB(255, 255, 255);
	m_FrameTransparency       = 64;
	m_FrameAAS                = true;
	m_Fov                     = 100;
	m_dwUndoLevels            = 4;
	m_IsQuake3                = false;
	m_PlayerWidthX            = 32;
	m_PlayerWidthY            = 32;
	m_PlayerHeight            = 56;
	m_fSlopeAngle             = 45.f;
	m_fVertexDragStep         = 4.f; // not in INI
	m_colFlatTris             = CNmVec3(0.9f, 0.f, 0.f);
	m_colSteepTris            = CNmVec3(1.f, 1.f, 0.f);
	m_colExcludedTris         = CNmVec3(0.8f, 0.f, 0.8f);
	m_TextureFilter           = "jpg;tga";
	m_sWorldShadersIgnoreList = "common/clip;common/hint;common/areaportal;common/clusterportal";
}

CString CPreferences::GetFullPath(const char *relativefilename)
{
	return CString(m_sCurrentPath + relativefilename);
}
