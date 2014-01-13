// AlphamapMan.cpp: implementation of the CAlphamapMan class.
//
//////////////////////////////////////////////////////////////////////

#include <fstream>
#include "EasyGen.h"
#include "AlphamapMan.h"
#include "MainFrm.h"

#include "Texture.h"
#include "TextureMan.h"

// extern link
#include "Preferences.h"
extern CPreferences g_Prefs;

#include "AlphamapMan.h"
extern CAlphamapMan g_AlphamapMan;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlphamapMan::CAlphamapMan()
{
	Reset();
}

CAlphamapMan::~CAlphamapMan()
{

}

//
//	zoom = 1(All)..8(4)
//
void CAlphamapMan::SetZoom(int zoom)
{
	if (zoom == m_Zoom)
	{
		return;
	}

	m_Zoom = zoom;

	int xColors = (9 - zoom) * 2;

	int totColors = xColors * xColors;

	int wColor = PAL_WIDTH / xColors;

	CRect r;

	int i, j, k, x, y;

	k = 0;
	y = 0;
	for (j = 0; j < xColors; j++)
	{
		x = 0;
		for (i = 0; i < xColors; i++)
		{
			r.SetRect(x, y, x + wColor /*-1*/, y + wColor /*-1*/);
			Set(k++, r);

			x += wColor;
		}

		y += wColor;
	}
}

void CAlphamapMan::Draw(CDC *pDC)
{
	int      k;
	CRect    r;
	CTexture *tex;

	CPoint pts[3];

	int totColors = (9 - m_Zoom) * (9 - m_Zoom) * 4;

	for (k = 0; k < totColors; k++)
	{
		r = m_Pal[k].rect;
		if (m_DrawColors)
		{
			r.DeflateRect(1, 1);
			pDC->FillSolidRect(&r, m_Pal[k].col);
		}
		else if (tex = GetTex(k))
		{
			tex->m_Thumb.DrawImage(pDC->m_hDC, r.left, r.top, r.Width(), r.Height());

			// draw litte colored triangle
			CPen penCol(PS_SOLID, 1, m_Pal[k].col);
			CPen *pOldPen = pDC->SelectObject(&penCol);

			// and a solid red brush
			CBrush brushCol(m_Pal[k].col);
			CBrush *pOldBrush = pDC->SelectObject(&brushCol);

			pts[0].x = r.left;
			pts[0].y = r.bottom - 1;

			pts[1].x = r.left;
			pts[1].y = r.top + ((2 * r.Height()) / 3) - 1;

			pts[2].x = r.left + (r.Width() / 3) + 1;
			pts[2].y = r.bottom - 1;

			pDC->Polygon(pts, 3);

			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);

		}
		else //tex not available, show color
		{
			r.DeflateRect(1, 1);
			pDC->FillSolidRect(&r, m_Pal[k].col);
		}
	}

	if (m_Selected != -1)
	{
		r = m_Pal[m_Selected].rect;
		pDC->FrameRect(&r, &CBrush(RGB(255, 0, 255)));
		r.DeflateRect(1, 1);
		pDC->FrameRect(&r, &CBrush(RGB(255, 0, 255)));
	}
}

int CAlphamapMan::Get(CPoint point)
{
	int zoom = m_Zoom;

	int xColors = (9 - zoom) * 2;

	int totColors = xColors * xColors;

	for (int k = 0; k < totColors; k++)
	{
		if (m_Pal[k].rect.PtInRect(point))
		{
			return k;
		}
	}

	return -1;
}

S_ALPHAINDEXDATA *CAlphamapMan::Get(int index)
{
	return &m_Pal[index];
}

void CAlphamapMan::Set(int index, COLORREF col)
{
	m_Pal[index].col = col;
}

void CAlphamapMan::Set(int index, CTexture *tex)
{
	m_Pal[index].tex = tex;
}

void CAlphamapMan::Set(int index, CRect &rect)
{
	m_Pal[index].rect = rect;
}

void CAlphamapMan::Delete(int index)
{
	m_Layers--;
	S_ALPHAINDEXDATA *a;

	for (int k = index; k < m_Layers; k++)
	{
//		m_Pal[k] = m_Pal[k+1];
		a = Get(k + 1);
		Set(k, a->col);
		Set(k, a->tex);
	}

	Set(m_Layers, (COLORREF)BLACK);
	Set(m_Layers, (CTexture *)NULL);
}

BOOL CAlphamapMan::SetCurSel(int index)
{
	m_Selected = index;

	return TRUE;
}

int CAlphamapMan::GetCurSel()
{
	return m_Selected;
}

COLORREF CAlphamapMan::GetCol(int index)
{
	return m_Pal[index].col;
}

CTexture *CAlphamapMan::GetTex(int index)
{
	return m_Pal[index].tex;
}

int CAlphamapMan::Get(COLORREF col)
{
	for (int k = 0; k < m_Layers; k++)
	{
		if (GetCol(k) == col)
		{
			return k;
		}
	}

	return -1;
}

void CAlphamapMan::ArchiveSave(CArchive &ar)
{
	CString  s;
	CTexture *tex;
	int      k;

	ar << GetLayers();

	for (k = 0; k < GetLayers(); k++)
	{
		ar << GetCol(k);

		tex = GetTex(k);
		if (tex != NULL)
		{
			s  = Q3A_TexturePathFilter(tex->m_sPath);
			s += '.';
			s += GetExt(tex->m_sPath);
			s.Replace('/', '\\');

			ar.WriteString(s);
		}
		ar << '\r' << '\n';
	}
}

/*
void CAlphamapMan::FileSave(const char* filename)
{
    FILE*	f;
    char	tag[32];
    size_t	blocksize;

    int			k;
    char		filenametex[255];
    COLORREF	col;
    CTexture	*tex;
    CString		s;

    f = fopen(filename, "ab");

    fseek(f,0,SEEK_END);

    // ALL THE OBJECT
    strcpy(tag,"ALPHAMAPMAN");
    blocksize = sizeof(int) + m_Layers * ( sizeof(COLORREF) + sizeof(char)*255 );
        fwrite((void*)tag,sizeof(char),32,f);
        fwrite(&blocksize,sizeof(size_t),1,f);

    TRACE("%d\n",blocksize);

    fwrite(&m_Layers,sizeof(int),1,f);						// w

    for(k=0;k<m_Layers;k++)
    {
        col = GetCol(k);
        fwrite(&col,sizeof(COLORREF),1,f);					// w

        tex = GetTex(k);
        if (tex != NULL)
        {
            s = Q3A_TexturePathFilter( tex->m_sPath );
            s+= '.';
            s+= GetExt( tex->m_sPath );
            s.Replace('/','\\');

            strcpy(filenametex,(LPCTSTR)s);
            fwrite(&filenametex,sizeof(char),255,f);		// w
        }
        else
        {
            strcpy(filenametex,"");
            fwrite(&filenametex,sizeof(char),255,f);		// w
        }
    }

    fclose(f);
}
*/

void CAlphamapMan::FileSave_1_3(const char *filename)
{
	std::ofstream    f;
	S_EG2BLOCKHEADER block;
	int              k;
	char             filenametex[255];
	COLORREF         col;
	CTexture         *tex;
	CString          s;

	f.open(filename, std::ios::out | std::ios::app | std::ios::binary);

	// ALL THE OBJECT
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "ALPHAMAPMAN");
	block.size = sizeof(m_Layers) + m_Layers * (sizeof(col) + sizeof(filenametex));
	f.write((const char *)&block, sizeof(S_EG2BLOCKHEADER));

	TRACE("%d\n", block.size);

	f.write((const char *)&m_Layers, sizeof(m_Layers));

	for (k = 0; k < m_Layers; k++)
	{
		col = GetCol(k);
		f.write((const char *)&col, sizeof(col));

		strcpy(filenametex, "");

		tex = GetTex(k);
		if (tex != NULL)
		{
			s  = Q3A_TexturePathFilter(tex->m_sPath);
			s += '.';
			s += GetExt(tex->m_sPath);
			s.Replace('/', '\\');

			strcpy(filenametex, (LPCTSTR)s);
		}

		f.write(filenametex, 255);

		TRACE("%s\n", filenametex);
	}

	f.close();
}

void CAlphamapMan::FileSave_1_4(const char *filename)
{
	FileSave_1_3(filename);
}

void CAlphamapMan::ArchiveLoad(CArchive &ar)
{
	CTextureMan *pTMan = (CTextureMan *)GetTextureView();
	COLORREF    col;
	CString     s, tpn;
	int         k;

	Reset();

	ar >> m_Layers;

	for (k = 0; k < m_Layers; k++)
	{
		ar >> col;
		Set(k, col);

		ar.ReadString(s);

		tpn  = g_Prefs.m_GameBasePath;
		tpn += "\\textures\\";
		tpn += s;

		tpn.Replace('/', '\\');
		tpn.Replace("\\\\", "\\");

		Set(k, pTMan->AddTexture(tpn));
	}
}

/*
bool CAlphamapMan::FileLoad(const char* filename)
{
    CTextureMan* pTMan = (CTextureMan*)GetTextureView();

    FILE*	f;
    char	blockname[32];
    size_t	blocksize;

    int			k;
    char		filenametex[255];
    COLORREF	col;

    CString	s,tpn;

    Reset();

    f = fopen(filename, "rb");

    if (f == NULL) return false;

    fseek(f,sizeof(S_EASYGENHEADER),SEEK_SET);

    while (fread(blockname,sizeof(char),32,f))
    {
        fread(&blocksize,sizeof(size_t),1,f);

        if (strcmp(blockname,"ALPHAMAPMAN") == 0)
        {
            fread(&m_Layers,sizeof(int),1,f);

            for(k=0;k<m_Layers;k++)
            {
                fread(&col,sizeof(COLORREF),1,f);
                Set(k,col);

                fread(filenametex,sizeof(char),255,f);

                tpn = g_Prefs.m_GameBasePath;
                tpn+= "\\textures\\";
                tpn+= filenametex;

                tpn.Replace('/','\\');
                tpn.Replace("\\\\","\\");

                Set(k, pTMan->AddTexture( tpn ) );
            }
        }
        else
        {
            TRACE( " BLOCKNAME UNKNOWN = %s\n", blockname);
            TRACE( " BLOCKSIZE = %d\n", blocksize);
            fseek(f,blocksize,SEEK_CUR);
        }
    }//while

    fclose(f);

    return true;
}
*/

bool CAlphamapMan::FileLoad_1_3(const char *filename)
{
	CTextureMan *pTMan = (CTextureMan *)GetTextureView();

	std::ifstream    f;
	S_EG2BLOCKHEADER block;

	int      k;
	char     filenametex[255];
	COLORREF col;

	CString s, tpn;

	Reset();

	f.open(filename, std::ios::in | std::ios::binary);

	if (!f.is_open())
	{
		return false;
	}

	f.ignore(sizeof(S_EASYGENHEADER));

	while (1)
	{
		f.read((char *)&block, sizeof(S_EG2BLOCKHEADER));

		if (f.gcount() == 0) //!= sizeof(S_EG2BLOCKHEADER))
		{
			break;
		}

		if (f.gcount() != sizeof(S_EG2BLOCKHEADER))
		{
			TRACE("*** Corrupted eg2 save file\n");
			break;
		}

		TRACE(" BLOCKNAME = [%s]\n", block.name);
		TRACE(" BLOCKSIZE = %d\n", block.size);

		if (strcmp(block.name, "ALPHAMAPMAN") == 0)
		{
			f.read((char *)&m_Layers, sizeof(m_Layers));

			TRACE("%d\n", m_Layers);

			for (k = 0; k < m_Layers; k++)
			{
				f.read((char *)&col, sizeof(col));
				f.read(filenametex, 255);

				TRACE("%s\n", filenametex);

				Set(k, col);

				tpn  = g_Prefs.m_GameBasePath;
				tpn += "\\textures\\";
				tpn += filenametex;
				tpn.MakeLower();

				tpn.Replace('/', '\\');
				tpn.Replace("\\\\", "\\");

				Set(k, pTMan->AddTexture(tpn));
			}
		}
		else
		{
			TRACE(" BLOCKNAME UNKNOWN = %s\n", block.name);
			TRACE(" BLOCKSIZE = %d\n", block.size);
			f.ignore(block.size);
		}
	} //while

	f.close();

	return true;
}

/*
E' uguale alla 1.3
*/
bool CAlphamapMan::FileLoad_1_4(const char *filename)
{
	return FileLoad_1_3(filename);
}

CView *CAlphamapMan::GetTextureView()
{
	CEasyGenApp *pApp = (CEasyGenApp *)AfxGetApp();
	if (!pApp)
	{
		return NULL;
	}
	CMainFrame *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	if (!pMainFrame)
	{
		return NULL;
	}
	CView *pView = (CView *)pMainFrame->m_wndSplitterLeft.GetPane(1, 0);
	return pView;
}

BOOL CAlphamapMan::IsTextureInUse(CTexture *tex)
{
	int k;

	for (k = 0; k < m_Layers; k++)
	{
		if (GetTex(k) == tex)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CAlphamapMan::Reset()
{
	int              k;
	S_ALPHAINDEXDATA a;

	for (k = 0; k < 256; k++)
	{
		a.index = k;
		a.col   = RGB(0, 0, 0);
//		a.rect	: use SetZoom(...)
		a.tex = NULL;

		m_Pal[k] = a;
	}

	m_Layers = 0;

	m_Zoom = 0;
	SetZoom(1);

	m_Selected = -1;

	m_DrawColors = TRUE;
}

void CAlphamapMan::FromFile(LPCTSTR filename)
{
	EasyImage  pcx;
	BTCOLORREF col;
	BOOL       done;

	if (!pcx.LoadImageFromFile(filename))
	{
		return;
	}

	Reset();

	done = FALSE;
	while ((!done) && (m_Layers < 256))
	{
		col = pcx.GetColorFromIndex(m_Layers);

		if (col == BTRGB(0, 0, 0))
		{
			done = TRUE;
		}
		else
		{
			Set(m_Layers++, (COLORREF)col);
		}
	}

	pcx.ClearData();
	//delete &pcx;
}

bool CAlphamapMan::GenMetashaderList(CNmListStr& list, const char *szMsFolder, const char *szMs)
{
	CNmStr ms;
	int    l, m;

	list.RemoveAll();

	for (l = 0; l < GetLayers(); l++)
	{
		ms.Format("textures/%s/%s_%d", szMsFolder, szMs, l);
		list.Append(ms);
	}

	for (l = 0; l < g_AlphamapMan.GetLayers() - 1; l++)
	{
		for (m = l + 1; m < g_AlphamapMan.GetLayers(); m++)
		{
			ms.Format("textures/%s/%s_%dto%d", szMsFolder, szMs, l, m);
			list.Append(ms);
		}
	}

	return true;
}

void CAlphamapMan::Swap(int a, int b)
{
	S_ALPHAINDEXDATA aux = *Get(a);

	Set(a, GetCol(b));
	Set(a, GetTex(b));

	Set(b, aux.col);
	Set(b, aux.tex);
}

int CAlphamapMan::GetLayers()
{
	return m_Layers;
}

void CAlphamapMan::FromData(EasyImage &pcx)
{
	BTCOLORREF col;
	BOOL       done;

	Reset();

	done = FALSE;
	while ((!done) && (m_Layers < 256))
	{
		col = pcx.GetColorFromIndex(m_Layers);

		if (col == BTRGB(0, 0, 0))
		{
			done = TRUE;
		}
		else
		{
			Set(m_Layers++, (COLORREF)col);
		}
	}
}
