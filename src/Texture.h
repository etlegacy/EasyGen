// Texture.h: interface for the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__198B99ED_C167_4A0B_9E98_DBE6958887C9__INCLUDED_)
#define AFX_TEXTURE_H__198B99ED_C167_4A0B_9E98_DBE6958887C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTexture
{
public:
	CTexture();
	virtual ~CTexture();

	CTexture& operator=(CTexture& src);

	BOOL Load(LPCTSTR filename);

	EasyImage m_Thumb;
	CRect     m_ThumbPos;

	GLuint  m_glBPP;
	GLuint  m_glWidth;
	GLuint  m_glHeight;
	GLuint  m_glTexID;
	GLubyte *m_glData;
	GLuint  m_glDataSize;

	CString m_sInfo;
	CString m_sPath;

	BOOL m_Locked;
	void Lock(BOOL lock = TRUE);

	CTexture *next;
};

#endif // !defined(AFX_TEXTURE_H__198B99ED_C167_4A0B_9E98_DBE6958887C9__INCLUDED_)
