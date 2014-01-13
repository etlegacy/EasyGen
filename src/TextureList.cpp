// TextureList.cpp: implementation of the CTextureList class.
//
//////////////////////////////////////////////////////////////////////

#include "EasyGen.h"
#include "TextureList.h"

#include "Texture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureList::CTextureList()
{
	m_Head = NULL;
	m_Tail = NULL;
}

CTextureList::~CTextureList()
{
	while (m_Head)
		RemoveHead();
}

CTexture *CTextureList::Add(LPCTSTR filename)
{
	CTexture *texture;

	// if already loaded
	if ((texture = Get(filename)) != NULL)
	{
		return texture;
	}

	texture = AddHead();

	if (texture->Load(filename))
	{
		return texture;
	}
	else
	{
		RemoveHead();
		return NULL;
	}
}

CTexture *CTextureList::AddHead()
{
	CTexture *im;

	im = new CTexture;

	im->next = m_Head;

	if (m_Head == NULL)
	{
		m_Tail = im;
	}

	m_Head = im;

	return im;
}

CTexture *CTextureList::AddTail()
{
	if (m_Tail == NULL)
	{
		return AddHead();
	}

	m_Tail->next = new CTexture;

	m_Tail = m_Tail->next;

	return m_Tail;
}

void CTextureList::RemoveHead()
{
	if (m_Head == NULL)
	{
		return;
	}

	CTexture *delme = m_Head;

	m_Head = m_Head->next;

	if (m_Head == NULL)
	{
		m_Tail = NULL;
	}

	delete delme;
}

void CTextureList::RemoveAll()
{
//	while (m_Head) RemoveHead();

	CTexture *app, *prec;

	prec = NULL;

	for (CTexture *tex = m_Head; tex; )
	{
		if (!tex->m_Locked)
		{
			if (prec != NULL)
			{
				app        = tex->next;
				prec->next = app;
				if (m_Tail == tex)
				{
					m_Tail = prec;
				}
				delete tex;
				tex = app;
			}
			else
			{
				app    = tex->next;
				m_Head = app;
				delete tex;
				tex = app;
			}
		}
		else
		{
			prec = tex;
			tex  = tex->next;
		}
	}
}

void CTextureList::ScanReset()
{
	m_Scanner = m_Head;
}

CTexture *CTextureList::ScanNext()
{
	CTexture *getit = m_Scanner;

	if (m_Scanner)
	{
		m_Scanner = m_Scanner->next;
	}

	return getit;
}


CTexture *CTextureList::Get(LPCTSTR byPathName)
{
	CTexture *tex;

	ScanReset();

	while (tex = ScanNext())
	{
		if (tex->m_sPath == byPathName)
		{
			return tex;
		}
	}

	return NULL;
}

CTexture *CTextureList::Get(CTexture *byTexPtr)
{
	CTexture *tex;

	ScanReset();

	while (tex = ScanNext())
	{
		if (tex == byTexPtr)
		{
			return tex;
		}
	}

	return NULL;
}
