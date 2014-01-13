// Preferences.h: interface for the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREFERENCES_H__3167691D_33B2_42EC_B331_F1892881817F__INCLUDED_)
#define AFX_PREFERENCES_H__3167691D_33B2_42EC_B331_F1892881817F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NmVec3.h"
#include "NmList.h"

class CPreferences
{
public:
	CString GetFullPath(const char *relativefilename);
	void SetDefaults();
	void AcquireBasePath();
	void SaveINI(LPCTSTR filename);
	void InputPreferences();
	int IntExtract(CString &s);
	void RecordPurge(CString &s);
	CString RecordGetIndex(CString s);
	CString RecordGetTag(CString s);
	CPreferences();
	virtual ~CPreferences();
	BOOL LoadINI(LPCTSTR filename);

	CString m_sCurrentPath;         // with ending '\\'

	CString m_GameBasePath;         // without ending '\\'
	CString m_BmpImportBasePath;
	CString m_BmpExportBasePath;

	int m_dwUndoLevels;

	int m_PlayerWidthX;
	int m_PlayerWidthY;
	int m_PlayerHeight;

	float m_fSlopeAngle;

	float m_fVertexDragStep;

	CString  m_TextureFilter;
	COLORREF m_BgColor;

	CNmVec3 m_colSteepTris;
	CNmVec3 m_colFlatTris;
	CNmVec3 m_colExcludedTris;

	CString m_sWorldShadersIgnoreList;

	COLORREF m_FrameColor;
	int      m_FrameTransparency;
	bool     m_FrameAAS;

	float m_Fov;

	bool m_IsQuake3;
};

#endif // !defined(AFX_PREFERENCES_H__3167691D_33B2_42EC_B331_F1892881817F__INCLUDED_)
