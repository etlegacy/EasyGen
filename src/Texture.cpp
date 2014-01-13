// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////
#include "EasyGen.h"
#include "Texture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture::CTexture()
{
	m_glData     = NULL;
	m_glDataSize = 0;
	m_Locked     = FALSE;
	next         = NULL;

}

CTexture::~CTexture()
{
	if (m_glData)
	{
		free(m_glData);
	}
	m_glDataSize = 0;
	glDeleteTextures(1, &m_glTexID);
	m_Thumb.ClearData();
}

CTexture& CTexture::operator=(CTexture& src)
{
	m_glBPP = src.m_glBPP;

	if (m_glData)
	{
		delete m_glData;
	}
	memcpy(m_glData, src.m_glData, src.m_glDataSize);

	m_glDataSize = src.m_glDataSize;
	m_glWidth    = src.m_glWidth;
	m_glHeight   = src.m_glHeight;
	m_glTexID    = src.m_glTexID;
	m_sInfo      = src.m_sInfo;
	m_sPath      = src.m_sPath;

	m_Thumb.ClearData();
	m_Thumb = src.m_Thumb;

	m_ThumbPos = src.m_ThumbPos;

	return *this;
}

BOOL CTexture::Load(LPCTSTR filename)
{
	GLuint    type = GL_RGBA;
	CString   s, ss, ext;
	EasyImage temp;
//	S_TEXTUREIMAGE*	texture;

	BTCOLORREF    col;
	unsigned char r, g, b, a;

	int x, y;

	// Open input stream.
	if (!temp.LoadImageFromFile(filename))
	{
		return NULL;
	}

	// Load any image.

	// true, original information
	m_Thumb    = temp.CloneImage();      // later will be resized
	m_glBPP    = temp.GetBitsPerPixel();
	m_glHeight = temp.GetHeight();
	m_glWidth  = temp.GetWidth();
	m_sPath    = filename;
	m_sPath.MakeLower();
	// end


	// resize to fit texture window
	if (m_Thumb.GetWidth() > 128)
	{
		m_Thumb.ConvertTo32BPP();
		m_Thumb.Resize(128, m_Thumb.GetHeight() * 128 / m_Thumb.GetWidth());
		m_Thumb.ConvertTo24BPP();
	}

	ext = m_sPath.Right(3);
	ext.MakeUpper();

	m_sInfo.Format(
	    "%dx%d - %s - %s",
	    m_glWidth, m_glHeight, (LPCTSTR)ext, (LPCTSTR)Q3A_TexturePathFilter(filename)
	    );

	// building GL texture ...
	temp.Flip();

//	data->SplitChannels(&rc,&gc,&bc,&ac);
//	data->CombineChannels(&bc,&gc,&rc,&ac);

	for (y = 0; y < (int)m_glHeight; y++)
	{
		for (x = 0; x < (int)m_glWidth; x++)
		{
			col = temp.GetColorFromPixel(x, y);
			b   = BTGetRValue(col);
			g   = BTGetGValue(col);
			r   = BTGetBValue(col);
			a   = BTGetAValue(col);
			temp.SetColorForPixel(x, y, BTRGBA(r, g, b, a));
		}
	}

/*
    if (texture->imageData)
    {
        delete texture->imageData;
    }
*/
	m_glDataSize = temp.GetImageSize();

	m_glData = (GLubyte *)malloc(m_glDataSize);
	memcpy(m_glData, temp.GetBits(), m_glDataSize);

	temp.ClearData();

	glGenTextures(1, &m_glTexID);

	glBindTexture(GL_TEXTURE_2D, m_glTexID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (m_glBPP == 24)
	{
		type = GL_RGB;
	}

//	glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_glWidth, m_glHeight, type, GL_UNSIGNED_BYTE, m_glData);
	// finished

	return TRUE;
}

void CTexture::Lock(BOOL lock)
{
	m_Locked = lock;
}
