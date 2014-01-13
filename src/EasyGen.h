// EasyGen.h : main header file for the EASYGEN application
//

#if !defined(AFX_EASYGEN_H__E8A7EA6F_8C4F_4A45_B3C1_8B47AD8D1E14__INCLUDED_)
#define AFX_EASYGEN_H__E8A7EA6F_8C4F_4A45_B3C1_8B47AD8D1E14__INCLUDED_

#define _WIN32_WINNT 0x0501

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <fstream>

#include "EasyImage.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include "resource.h"       // main symbols

#include <gl\gl.h>
#include <gl\glu.h>
////////
#include <io.h>
#include <stdio.h>
#include <shlobj.h>
#include <math.h>
////////
#include "Nemo3D.h"

#include "EgDefines.h"
#include "extern.h"

/////////////////////////////////////
/////////////////////////////////////
extern float g_AppVersion;
extern float g_SaveVersion;
/////////////////////////////////////
/////////////////////////////////////

struct S_MOD_BUTTON
{
	CRect rect;
	CBitmap bitmap;
};

struct S_EASYGENHEADER
{
	char appname[8];
	float version;
};

struct S_EG2BLOCKHEADER
{
	char name[32];
	int size;
};

struct S_COLORDATA
{
	float colR, colG, colB;
};

struct S_EGN_POLYFACE
{
	CNmVec3 f_color;
};

class CTexture;
// m_SizeZ

float cmp_max(float f1, float f2);
float cmp_min(float f1, float f2);
float distance(float x1, float y1, float x2, float y2);
float distance2(float x1, float y1, float x2, float y2);
void  q3r_cube(CString &s, int x0, int y0, int z0, int x1, int y1, int z1, LPCTSTR texture);

void glCube(float x, float y, float z, float sideX, float sideY, float height);

void LogToTxt(LPCTSTR filename, LPCTSTR msg);

CString Q3A_TexturePathFilter(CString path);
CString GetExt(CString fpn);
CString GetFileNameNoExtNoPath(CString fpn);
CString GetFileNameNoPath(CString fpn);

BOOL IsQuake3RootFolder(LPCTSTR pathname);
void CheckModSubfolders(CString& modfolder);

void RemoveTemplateComments(CNmStr& templ);
/////////////////////////////////////////////////////////////////////////////
// CEasyGenApp:
// See EasyGen.cpp for the implementation of this class
//

class CEasyGenApp : public CWinApp
{
public:
	CEasyGenApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasyGenApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEasyGenApp)
	afx_msg void OnAppAbout();
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYGEN_H__E8A7EA6F_8C4F_4A45_B3C1_8B47AD8D1E14__INCLUDED_)
