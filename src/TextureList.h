// TextureList.h: interface for the CTextureList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURELIST_H__2F94D394_B81C_47C5_A933_976FC2377902__INCLUDED_)
#define AFX_TEXTURELIST_H__2F94D394_B81C_47C5_A933_976FC2377902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTexture;

class CTextureList
{
public:
	CTexture *ScanNext();
	void ScanReset();
	void RemoveAll();
	void RemoveHead();
	CTextureList();
	virtual ~CTextureList();

	CTexture *AddHead();
	CTexture *AddTail();
	CTexture *Add(LPCTSTR filename);

	CTexture *Get(CTexture *byTexPtr);
	CTexture *Get(LPCTSTR byPathName);

	CTexture *m_Head, *m_Tail;
	CTexture *m_Scanner;
};

#endif // !defined(AFX_TEXTURELIST_H__2F94D394_B81C_47C5_A933_976FC2377902__INCLUDED_)
