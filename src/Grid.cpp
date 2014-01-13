// Grid.cpp: implementation of the CGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "EasyGen.h"
#include "Grid.h"
#include <fstream>
////////
#include <FLOAT.H>
#include "Nemo3D.h"
#include "NmVec3.h"
////////

// extern
#include "Preferences.h"
extern CPreferences g_Prefs;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
    m_GridCurrentColor	= 0;	// used to cycle combo
    m_GridCurrentColor2	= 1;	// used to cycle pairs inside a combo

//		cUP		|	cUP				cMID
//		cMID	|	(cMID+cUP)/2	(cMID+cDN)/2
//		cDN		|	cMID			cDN

    float drk = 0.25f;

    m_GridColors[0][GRID_COL_UP].set(1.f,1.f,1.f);	// WHITE
    m_GridColors[0][GRID_COL_MID].set(0.5f,0.5f,0.5f);// GRAY
    m_GridColors[0][GRID_COL_DN].set(0.f,0.f,0.f);		// BLACK

    m_GridColors[1][GRID_COL_UP].set(0.7f,drk,drk);		// RED
    m_GridColors[1][GRID_COL_MID].set(0.7f,0.7f,drk);	// YELLOW
    m_GridColors[1][GRID_COL_DN].set(drk,0.7f,drk);		// GREEN

    m_GridColors[2][GRID_COL_UP].set(0.9f,0.9f,0.9f);	// WHITE
    m_GridColors[2][GRID_COL_MID].set(drk,0.8f,0.8f);	// CYAN
    m_GridColors[2][GRID_COL_DN].set(drk,drk,0.8f);		// BLUE

    m_GridColors[3][GRID_COL_UP].set(0.9f,0.9f,0.9f);	// WHITE
    m_GridColors[3][GRID_COL_MID].set(0.5f,drk,drk);	// MARR
    m_GridColors[3][GRID_COL_DN].set(drk,0.6f,drk);		// GREEN

    m_GridColors[4][GRID_COL_UP].set(0.8f,drk,drk);		// RED
    m_GridColors[4][GRID_COL_MID].set(0.8f,drk,0.8f);	// PINK
    m_GridColors[4][GRID_COL_DN].set(drk,drk,0.8f);		// BLUE

    m_GridColors[5][GRID_COL_UP].set(drk,0.8f,drk);		// GREEN
    m_GridColors[5][GRID_COL_MID].set(drk,0.8f,0.8f);	// CYAN
    m_GridColors[5][GRID_COL_DN].set(drk,drk,0.8f);		// BLUE


    m_GridColorSet[GRID_COL_DN] = m_GridColors[m_GridCurrentColor][GRID_COL_DN];
    m_GridColorSet[GRID_COL_MID]= m_GridColors[m_GridCurrentColor][GRID_COL_MID];
    m_GridColorSet[GRID_COL_UP] = m_GridColors[m_GridCurrentColor][GRID_COL_UP];

------------
    switch (m_GridCurrentColor2)
    {
    case (0):
        m_GridColorSet[GRID_COL_DN] = m_GridColors[m_GridCurrentColor][GRID_COL_DN];
        m_GridColorSet[GRID_COL_MID]= m_GridColors[m_GridCurrentColor][GRID_COL_MID];
        m_GridColorSet[GRID_COL_UP] = m_GridColors[m_GridCurrentColor][GRID_COL_UP];
        m_GridCurrentColor2 = 1;
        break;
    case (1):
        m_GridColorSet[GRID_COL_DN] = m_GridColors[m_GridCurrentColor][GRID_COL_DN];
        m_GridColorSet[GRID_COL_MID]= (m_GridColors[m_GridCurrentColor][GRID_COL_DN] + m_GridColors[m_GridCurrentColor][GRID_COL_MID]) / 2;
        m_GridColorSet[GRID_COL_UP] = m_GridColors[m_GridCurrentColor][GRID_COL_MID];
        m_GridCurrentColor2 = 2;
        break;
    case (2):
        m_GridColorSet[GRID_COL_DN] = m_GridColors[m_GridCurrentColor][GRID_COL_MID];
        m_GridColorSet[GRID_COL_MID]= (m_GridColors[m_GridCurrentColor][GRID_COL_MID] + m_GridColors[m_GridCurrentColor][GRID_COL_UP]) / 2;
        m_GridColorSet[GRID_COL_UP] = m_GridColors[m_GridCurrentColor][GRID_COL_UP];
        m_GridCurrentColor2 = 0;
        m_GridCurrentColor = (m_GridCurrentColor+1)%GRID_COLORS3;
        break;
    }

*/
CGrid::CGrid()
{
	m_dwCellsX = 0;
	m_dwCellsY = 0;

	m_fCellWidthX = 0.0f;
	m_fCellWidthY = 0.0f;

	m_dwSizeX = 0;
	m_dwSizeY = 0;
	m_dwSizeZ = 0;

	m_dwSizeFace = 0;
	m_dwSizeTris = 0;

	m_fWidthX = 0.0f;
	m_fWidthY = 0.0f;

	m_X = NULL;
	m_Y = NULL;
	m_Z = NULL;

	for (int i = 0; i < GRID_Z_BUFFERS; i++)
	{
		m_ZBuffer[i] = NULL;
	}

	m_Alphamap = NULL;

	m_Tris = NULL;

	m_Color = NULL;
	m_Face  = NULL;

	m_vOrigin.Set(0.f, 0.f, 0.f);

	m_bMarkFlatTris     = false;
	m_bMarkSlopeTris    = false;
	m_bMarkExcludedTris = false;
}

CGrid::CGrid(CGrid & src)
{
	CGrid();
	*this = src;
}

CGrid::CGrid(int CellsX, int CellsY, float WidthX, float WidthY)
{
	CGrid();
	Set(CellsX, CellsY, WidthX, WidthY);
}

CGrid::~CGrid()
{
	Delete();
}

//
//	= GridCpy(dest,src);
//
void CGrid::operator=(const CGrid& src)
{
	int   i, m;
	CTris *tdst, *tsrc;

	Set(src.m_dwCellsX, src.m_dwCellsY, src.m_fWidthX, src.m_fWidthY);

	memcpy(m_X, src.m_X, sizeof(float) * m_dwSizeX);
	memcpy(m_Y, src.m_Y, sizeof(float) * m_dwSizeY);
	memcpy(m_Alphamap, src.m_Alphamap, sizeof(int) * m_dwSizeZ);
	memcpy(m_Color, src.m_Color, sizeof(S_COLORDATA) * m_dwSizeZ /*Color*/);
	memcpy(m_Face, src.m_Face, sizeof(S_FACEDATAEX) * m_dwSizeFace);
//	memcpy(m_Tris,src.m_Tris,sizeof(CTris)*m_dwSizeTris); NO, they're pointers !

	for (i = 0; i < GRID_Z_BUFFERS; i++)
	{
		memcpy(m_ZBuffer[i], src.m_ZBuffer[i], sizeof(float) * m_dwSizeZ);
	}

	// m_Z = ...
	SelectZBuf(GRID_Z_REAL);

	for (m = 0; m < m_dwSizeTris; m++)
	{
		tdst = &m_Tris[m];
		tsrc = &src.m_Tris[m];

		tdst->m_bExcluded = tsrc->m_bExcluded;
	}
}

void CGrid::OptionFlatTrisSet(bool state)
{
	m_bMarkFlatTris = state;
}

void CGrid::OptionFlatTrisToggle()
{
	m_bMarkFlatTris = !m_bMarkFlatTris;
}

bool CGrid::OptionFlatTrisGet()
{
	return m_bMarkFlatTris;
}

void CGrid::OptionSlopeTrisSet(bool state)
{
	m_bMarkSlopeTris = state;
}

void CGrid::OptionSlopeTrisToggle()
{
	m_bMarkSlopeTris = !m_bMarkSlopeTris;
}

bool CGrid::OptionSlopeTrisGet()
{
	return m_bMarkSlopeTris;
}

void CGrid::OptionExcludedTrisSet(bool state)
{
	m_bMarkExcludedTris = state;
}

void CGrid::OptionExcludedTrisToggle()
{
	m_bMarkExcludedTris = !m_bMarkExcludedTris;
}

bool CGrid::OptionExcludedTrisGet()
{
	return m_bMarkExcludedTris;
}

void CGrid::Set(int CellsX, int CellsY, float WidthX, float WidthY)
{
	CTris *t;
	int   i, j, k;
	float start;

	Delete();

	m_dwCellsX = CellsX;
	m_dwCellsY = CellsY;

	m_fWidthX = WidthX;
	m_fWidthY = WidthY;

	m_fCellWidthX = WidthX / (float)CellsX;
	m_fCellWidthY = WidthY / (float)CellsY;

	m_dwSizeX    = CellsX + 1;
	m_dwSizeY    = CellsY + 1;
	m_dwSizeZ    = m_dwSizeX * m_dwSizeY;
	m_dwSizeFace = m_dwCellsX * m_dwCellsY;
	m_dwSizeTris = (m_dwCellsX * m_dwCellsY) * 2;

	m_X        = new float[m_dwSizeX];
	m_Y        = new float[m_dwSizeY];
	m_Alphamap = new int[m_dwSizeZ];
	m_Color    = new S_COLORDATA[m_dwSizeZ];
	m_Face     = new S_FACEDATAEX[m_dwSizeFace];
	m_Tris     = new CTris[m_dwSizeTris];

	for (i = 0; i < GRID_Z_BUFFERS; i++)
	{
		m_ZBuffer[i] = new float[m_dwSizeZ];
	}

	// m_Z = ...
	SelectZBuf(GRID_Z_REAL);
	for (k = 0; k < m_dwSizeZ; k++)
	{
		m_Z[k] = 0.0f;
	}
	ZBufCpy(GRID_Z_PREVIEW, GRID_Z_REAL);

	for (k = 0; k < m_dwSizeZ; k++)
	{
		m_Color[k].colR = 0.0f;
		m_Color[k].colG = 0.0f;
		m_Color[k].colB = 0.0f;
	}

	int          fi, fj;
	S_FACEDATAEX *f;
	for (k = 0; k < m_dwSizeFace; k++)
	{
		fi = k % m_dwCellsX;
		fj = k / m_dwCellsX;

		f = GetFace(fi, fj);

		f->sx    = 0;
		f->sy    = 0;
		f->sxx   = 0;
		f->syy   = 0;
		f->xstep = 1;
		f->ystep = 1;
		f->flags = 0;

		if ((fi + fj) & 1)
		{
			f->flags |= FACE_FL_TRISINV;
		}
	}

	// m_Tris...
	TrisLink();

	ScanTrisSetup();
	while (ScanTris(t))
	{
		t->SetExcluded(false);
	}

	// m_Alphamap...
	for (k = 0; k < m_dwSizeZ; k++)
	{
		SetAlphaIndex(k, 0);
	}

	start = -m_fWidthX / 2;

	for (i = 0; i < m_dwSizeX; i++)
	{
		m_X[i] = start;
		start += m_fCellWidthX;
	}

	start = -m_fWidthY / 2;

	for (j = 0; j < m_dwSizeY; j++)
	{
		m_Y[j] = start;
		start += m_fCellWidthY;
	}
}

//
//	y:	0 <= x <= h
//
//  Y:	0 <= X <= H
//
//	y( x ) = Y( x * ( H / h ) )
//
/*
bool CGrid::ReCells(int cells)
{
    CGrid *newGrid;

    newGrid = new CGrid;

    *newGrid = *this;

    if (!newGrid->Set(cells,m_Width))
    {
        return false;
    }

    float ratio = float( m_Cells ) / float( newGrid->m_Cells );

    int* ij_ratio;

    ij_ratio = new int[newGrid->m_Size];

    for(int k=0;k<newGrid->m_Size;k++)
    {
        ij_ratio[k] = int(float(k)*ratio);
    }

    for(int j=0;j<newGrid->m_Size;j++)
    {
        for(int i=0;i<newGrid->m_Size;i++)
        {
            newGrid->m_Z[j][i] = m_Z[ ij_ratio[j] ][ ij_ratio[i] ];
        }
    }

    delete [] ij_ratio;

    *this = *newGrid;

    delete newGrid;

    return true;
}
*/
void CGrid::GetIndexBounds(CNmVec2 pA, CNmVec2 pB, int &j_start, int &j_end, int &i_start, int &i_end)
{
/*
    int cells_2 = m_Cells / 2;

    i_start	= (int)floor( pA.x / m_CellWidth ) + cells_2 - 1;
    i_end	= (int)ceil( pB.x / m_CellWidth ) + cells_2 + 1;
    j_start	= (int)floor( pA.y / m_CellWidth ) + cells_2 - 1;
    j_end	= (int)ceil( pB.y / m_CellWidth ) + cells_2 + 1;

    if (i_start < 0) i_start = 0;
    if (j_start < 0) j_start = 0;
    if (i_end > m_Size) i_end = m_Size;
    if (j_end > m_Size) j_end = m_Size;
*/
	i_start = m_dwSizeX - 1;
	while (i_start >= 0 && GetX(i_start) > pA.x)
	{
		i_start--;
	}
	if (i_start < 0)
	{
		i_start = 0;
	}

	i_end = 0;
	while (i_end < m_dwSizeX && GetX(i_end) < pB.x)
	{
		i_end++;
	}
	if (i_end > (m_dwSizeX - 1))
	{
		i_end = m_dwSizeX - 1;
	}

	j_start = m_dwSizeY - 1;
	while (j_start >= 0 && GetY(j_start) > pA.y)
	{
		j_start--;
	}
	if (j_start < 0)
	{
		j_start = 0;
	}

	j_end = 0;
	while (j_end < m_dwSizeY && GetY(j_end) < pB.y)
	{
		j_end++;
	}
	if (j_end > (m_dwSizeY - 1))
	{
		j_end = m_dwSizeY - 1;
	}
}

float CGrid::GetMaxHeight()
{
	int   i, j, k;
	float *z;
	float maxh = -FLT_MAX;

	ScanFullSetup();

	while (ScanFull(i, j, k, z))
	{
		if (*z > maxh)
		{
			maxh = *z;
		}
	}
/*
    for(int j=0;j<m_Size;j++)
    {
        for(int i=0;i<m_Size;i++)
        {
            if (m_Z[j][i] > maxh) maxh = m_Z[j][i];
        }
    }
*/
	return maxh;
}

float CGrid::GetMinHeight()
{
	int   i, j, k;
	float *z;
	float minh = FLT_MAX;

	ScanFullSetup();
	while (ScanFull(i, j, k, z))
	{
		if (*z < minh)
		{
			minh = *z;
		}
	}

	return minh;
}

void CGrid::GetOrigin(CNmVec3& origin)
{
	origin = m_vOrigin;
}

void CGrid::Delete()
{
	if (m_X)
	{
		delete[] m_X;
		m_X = NULL;
	}
///////////////////////
	if (m_Y)
	{
		delete[] m_Y;
		m_Y = NULL;
	}
///////////////////////
	for (int i = 0; i < GRID_Z_BUFFERS; i++)
	{
		if (m_ZBuffer[i])
		{
			delete[] m_ZBuffer[i];
			m_ZBuffer[i] = NULL;
		}
	}
///////////////////////
	if (m_Color)
	{
		delete[] m_Color;
		m_Color = NULL;
	}
///////////////////////
	if (m_Alphamap)
	{
		delete[] m_Alphamap;
		m_Alphamap = NULL;
	}
///////////////////////
	if (m_Face)
	{
		delete[] m_Face;
		m_Face = NULL;
	}
///////////////////////
	if (m_Tris)
	{
		delete[] m_Tris;
		m_Tris = NULL;
	}
}

void CGrid::SetZ(int i, int j, float z)
{
	m_Z[i + j * m_dwSizeX] = z;
}

float CGrid::GetZ(int i, int j)
{
	return m_Z[i + j * m_dwSizeX];
}

float CGrid::GetZ(int k)
{
	return m_Z[k];
}

float *CGrid::GetPtr(int i, int j)
{
	return &m_Z[i + j * m_dwSizeX];
}

float *CGrid::GetPtr(int k)
{
	return &m_Z[k];
}

float CGrid::GetX(int i)
{
	return m_X[i];
}

float CGrid::GetY(int j)
{
	return m_Y[j];
}

/*
 Usage:
    ScanRectSetup(xxx,xxx,xxx,xxx);

    while (ScanRect(i,j,k,z))
    {
    }
    ------------------------------------------
    while (ScanRectRow(j))
    {
        while (ScanRectColumn(i,k,z))
        {
        }
    }
    ------------------------------------------
*/
void CGrid::ScanRectSetup(int iStart, int jStart, int iEnd, int jEnd)
{
	m_iStart = iStart;
	m_jStart = jStart;
	m_iEnd   = iEnd;
	m_jEnd   = jEnd;

	m_i = iStart;
	m_j = jStart;
	m_k = m_i + m_j * m_dwSizeX;
}

bool CGrid::ScanRect(int &i, int &j, int &k, float *&z)
{
	if (m_j > m_jEnd)
	{
		return false;
	}

	i = m_i;
	j = m_j;
	k = m_k;
	z = &m_Z[m_k];

	m_i++;
	m_k++;

	if (m_i > m_iEnd)
	{
		m_i = m_iStart;
		m_j++;
		m_k = m_k + m_dwSizeX - m_iEnd + m_iStart - 1;
	}

	return true;
}

bool CGrid::ScanRectRow(int &j)
{
	if (m_j > m_jEnd)
	{
		return false;
	}

	j = m_j;
	m_j++;

	return true;
}

bool CGrid::ScanRectColumn(int &i, int &k, float *&z)
{
	if (m_i > m_iEnd)
	{
		m_i  = m_iStart;
		m_k += m_dwSizeX - m_iEnd + m_iStart - 1;
		return false;
	}

	i = m_i;
	k = m_k;
	z = &m_Z[k];

	m_i++;
	m_k++;

	return true;
}

void CGrid::ScanFullSetup()
{
	m_i = 0;
	m_j = 0;
	m_k = 0;
}

bool CGrid::ScanFull(int &i, int &j, int &k, float *&z)
{
	if (m_k == m_dwSizeZ)
	{
		return false;
	}

	i = m_i;
	j = m_j;
	k = m_k;
	z = &m_Z[m_k];

	m_i++;
	m_k++;

	if (m_i == m_dwSizeX)
	{
		m_i = 0;
		m_j++;
	}

	return true;
}

//
// x must be within GetX(0) and GetX(m_dwSizeX-1);
//
// if GetX(i) = x , returns i;
//
int CGrid::GetLeft_i(float x)
{
	int i = m_dwSizeX - 1;
	while ((x < GetX(i)) && (i > 0))
	{
		i--;
	}
	return i;
}

int CGrid::GetLeft_j(float y)
{
	int j = m_dwSizeY - 1;
	while ((y < GetY(j)) && (j > 0))
	{
		j--;
	}
	return j;
}

int CGrid::GetRight_i(float x)
{
	int i = 0;
	while (x > GetX(i))
	{
		i++;
	}
	return i;
}

int CGrid::GetRight_j(float y)
{
	int j = 0;
	while (y > GetY(j))
	{
		j++;
	}
	return j;
}

void CGrid::ReCellsX(int cells, bool lock, int align)
{
	CGrid *nGrid;
	int   ii, ii_start;

	int   i, j, k;
	float *z;

	CString s;

	if (cells == m_dwCellsX)
	{
		return;
	}

	nGrid = new CGrid;

	nGrid->Set(cells, m_dwCellsY, m_fWidthX, m_fWidthY);

	if (cells < m_dwCellsX)
	{
		if (lock)
		{
			nGrid->ScanFullSetup();

			while (nGrid->ScanFull(i, j, k, z))
			{
				*z = GetApproxZonX(nGrid->GetX(i), j);

				nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i * m_dwCellsX / cells, j));
			}
		} //lock
		else // lock = false
		{
			switch (align)
			{
			case ALIGN_LEFT:
				nGrid->ScanFullSetup();

				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetZ(i, j);

					nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i, j));
				}
				break;
			case ALIGN_CENTER:
				nGrid->ScanFullSetup();

				ii_start = (m_dwCellsX - nGrid->m_dwCellsX) / 2;
				ii       = ii_start;
				while (nGrid->ScanFull(i, j, k, z))
				{
					if (i == 0)
					{
						ii = ii_start;
					}

					*z = GetZ(ii, j);

					nGrid->SetAlphaIndex(i, j, GetAlphaIndex(ii, j));

					ii++;
				}
				break;
			case ALIGN_RIGHT:
				nGrid->ScanFullSetup();

				ii_start = m_dwCellsX - nGrid->m_dwCellsX;
				ii       = ii_start;
				while (nGrid->ScanFull(i, j, k, z))
				{
					if (i == 0)
					{
						ii = ii_start;
					}

					*z = GetZ(ii, j);

					nGrid->SetAlphaIndex(i, j, GetAlphaIndex(ii, j));

					ii++;
				}
				break;
			}
		} //lock
	}

	if (cells > m_dwCellsX)
	{
		if (lock)
		{
			nGrid->ScanFullSetup();

			while (nGrid->ScanFull(i, j, k, z))
			{
				*z = GetApproxZonX(nGrid->GetX(i), j);

				nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i * m_dwCellsX / cells, j));
			}
		}
		else // lock = false
		{
			switch (align)
			{
			case ALIGN_LEFT:
				ScanFullSetup();

				while (ScanFull(i, j, k, z))
				{
					nGrid->SetZ(i, j, *z);

					nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i, j));
				}
				break;
			case ALIGN_CENTER:
				ScanFullSetup();

				ii_start = (nGrid->m_dwCellsX - m_dwCellsX) / 2;
				ii       = ii_start;
				while (ScanFull(i, j, k, z))
				{
					if (i == 0)
					{
						ii = ii_start;
					}

					nGrid->SetZ(ii, j, *z);

					nGrid->SetAlphaIndex(ii, j, GetAlphaIndex(i, j));

					ii++;
				}
				break;
			case ALIGN_RIGHT:
				ScanFullSetup();

				ii_start = nGrid->m_dwCellsX - m_dwCellsX;
				ii       = ii_start;
				while (ScanFull(i, j, k, z))
				{
					if (i == 0)
					{
						ii = ii_start;
					}

					nGrid->SetZ(ii, j, *z);

					nGrid->SetAlphaIndex(ii, j, GetAlphaIndex(i, j));

					ii++;
				}
				break;
			}
		} //lock
	}

	*this = *nGrid;

	delete nGrid;
}

void CGrid::ReCellsY(int cells, bool lock, int align)
{
	CGrid *nGrid;
	int   jj_start;

	int   i, j, k;
	float *z;

	if (cells == m_dwCellsY)
	{
		return;
	}

	nGrid = new CGrid;

	nGrid->Set(m_dwCellsX, cells, m_fWidthX, m_fWidthY);

	// cells < mCells
	if (cells < m_dwCellsY)
	{
		if (lock)
		{
			nGrid->ScanFullSetup();

			while (nGrid->ScanFull(i, j, k, z))
			{
				*z = GetApproxZonY(nGrid->GetY(j), i);

				nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i, j * m_dwCellsY / cells));
			}
		} //lock
		else // lock = false
		{
			switch (align)
			{
			case ALIGN_BOTTOM:
				nGrid->ScanFullSetup();

				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetZ(i, j);

					nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i, j));
				}
				break;
			case ALIGN_MIDDLE:
				nGrid->ScanFullSetup();

				jj_start = (m_dwCellsY - nGrid->m_dwCellsY) / 2;
				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetZ(i, j + jj_start);

					nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i, j + jj_start));
				}
				break;
			case ALIGN_TOP:
				nGrid->ScanFullSetup();

				jj_start = m_dwCellsY - nGrid->m_dwCellsY;
				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetZ(i, j + jj_start);

					nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i, j + jj_start));
				}
				break;
			}
		} //lock
	}

	if (cells > m_dwCellsY)
	{
		if (lock)
		{
			nGrid->ScanFullSetup();

			while (nGrid->ScanFull(i, j, k, z))
			{
				*z = GetApproxZonY(nGrid->GetY(j), i);

				nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i, j * m_dwCellsY / cells));
			}
		}
		else // lock = false
		{
			switch (align)
			{
			case ALIGN_BOTTOM:
				ScanFullSetup();

				while (ScanFull(i, j, k, z))
				{
					nGrid->SetZ(i, j, *z);

					nGrid->SetAlphaIndex(i, j, GetAlphaIndex(i, j));
				}
				break;
			case ALIGN_MIDDLE:
				ScanFullSetup();

				jj_start = (nGrid->m_dwCellsY - m_dwCellsY) / 2;
				while (ScanFull(i, j, k, z))
				{
					nGrid->SetZ(i, j + jj_start, *z);

					nGrid->SetAlphaIndex(i, j + jj_start, GetAlphaIndex(i, j));
				}
				break;
			case ALIGN_TOP:
				ScanFullSetup();

				jj_start = nGrid->m_dwCellsY - m_dwCellsY;
				while (ScanFull(i, j, k, z))
				{
					nGrid->SetZ(i, j + jj_start, *z);

					nGrid->SetAlphaIndex(i, j + jj_start, GetAlphaIndex(i, j));
				}
				break;
			}
		} //lock
	}

	*this = *nGrid;

	delete nGrid;
}

float CGrid::GetApproxZonX(float x, int j)
{
	int   i = GetLeft_i(x);
	float z = GetZ(i, j);

	if (x == GetX(i))
	{
		return z;
	}

	if (i == (m_dwSizeX - 1))
	{
		return z;
	}

	float delta = (GetZ(i + 1, j) - z) / m_fCellWidthX;

	return (x - GetX(i)) * delta + z;
}

float CGrid::GetApproxZonY(float y, int i)
{
	int   j = GetLeft_j(y);
	float z = GetZ(i, j);

	if (y == GetY(j))
	{
		return z;
	}

	if (j == (m_dwSizeY - 1))
	{
		return z;
	}

	float delta = (GetZ(i, j + 1) - z) / m_fCellWidthY;

	return (y - GetY(j)) * delta + z;
}

void CGrid::ReWidthX(int width, bool lock, int align)
{
	CGrid *nGrid;

	float stepx, x;

	int   i, j, k;
	float *z;

	if (width == m_fWidthX)
	{
		return;
	}

	nGrid = new CGrid;

	nGrid->Set(m_dwCellsX, m_dwCellsY, (float)width, m_fWidthY);

	if (width < m_fWidthX)
	{
		if (lock)
		{
			switch (align)
			{
			case ALIGN_LEFT:
				stepx = (m_fWidthX - nGrid->m_fWidthX) / 2;

				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetApproxZonX(nGrid->GetX(i) - stepx, j);
					nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
				}
				break;
			case ALIGN_CENTER:
				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetApproxZonX(nGrid->GetX(i), j);
					nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
				}
				break;
			case ALIGN_RIGHT:
				stepx = (m_fWidthX - nGrid->m_fWidthX) / 2;

				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetApproxZonX(nGrid->GetX(i) + stepx, j);
					nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
				}
				break;
			}
		}
		else
		{
			nGrid->ScanFullSetup();
			while (nGrid->ScanFull(i, j, k, z))
			{
				*z = GetZ(k);
				nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
			}
		}
	}

	if (width > m_fWidthX)
	{
		if (lock)
		{
			switch (align)
			{
			case ALIGN_LEFT:
				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					x = nGrid->GetX(i) + (nGrid->m_fWidthX - m_fWidthX) / 2;
					if ((x >= GetX(0)) && (x <= GetX(m_dwSizeX - 1)))
					{
						*z = GetApproxZonX(x, j);
						nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
					}
				}
				break;
			case ALIGN_CENTER:
				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					x = nGrid->GetX(i);
					if ((x >= GetX(0)) && (x <= GetX(m_dwSizeX - 1)))
					{
						*z = GetApproxZonX(x, j);
						nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
					}
				}
				break;
			case ALIGN_RIGHT:
				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					x = nGrid->GetX(i) - (nGrid->m_fWidthX - m_fWidthX) / 2;
					if ((x >= GetX(0)) && (x <= GetX(m_dwSizeX - 1)))
					{
						*z = GetApproxZonX(x, j);
						nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
					}
				}
				break;
			}
		}
		else
		{
			nGrid->ScanFullSetup();

			while (nGrid->ScanFull(i, j, k, z))
			{
				*z = GetZ(k);
				nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
			}
		}
	}

	*this = *nGrid;

	delete nGrid;
}

void CGrid::ReWidthY(int width, bool lock, int align)
{
	CGrid *nGrid;

	float stepy, y;

	int   i, j, k;
	float *z;

	if (width == m_fWidthY)
	{
		return;
	}

	nGrid = new CGrid;

	nGrid->Set(m_dwCellsX, m_dwCellsY, m_fWidthX, (float)width);

	if (width < m_fWidthY)
	{
		if (lock)
		{
			switch (align)
			{
			case ALIGN_BOTTOM:
				stepy = (m_fWidthY - nGrid->m_fWidthY) / 2;

				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetApproxZonY(nGrid->GetY(j) - stepy, i);
					nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
				}
				break;
			case ALIGN_MIDDLE:
				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetApproxZonY(nGrid->GetY(j), i);
					nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
				}
				break;
			case ALIGN_TOP:
				stepy = (m_fWidthY - nGrid->m_fWidthY) / 2;

				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					*z = GetApproxZonY(nGrid->GetY(j) + stepy, i);
					nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
				}
				break;
			}
		}
		else
		{
			nGrid->ScanFullSetup();

			while (nGrid->ScanFull(i, j, k, z))
			{
				*z = GetZ(k);
				nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
			}
		}
	}

	if (width > m_fWidthY)
	{
		if (lock)
		{
			switch (align)
			{
			case ALIGN_BOTTOM:
				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					y = nGrid->GetY(j) + (nGrid->m_fWidthY - m_fWidthY) / 2;
					if ((y >= GetY(0)) && (y <= GetY(m_dwSizeY - 1)))
					{
						*z = GetApproxZonY(y, i);
						nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
					}
				}
				break;
			case ALIGN_MIDDLE:
				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					y = nGrid->GetY(j);
					if ((y >= GetY(0)) && (y <= GetY(m_dwSizeY - 1)))
					{
						*z = GetApproxZonY(y, i);
						nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
					}
				}
				break;
			case ALIGN_TOP:
				nGrid->ScanFullSetup();
				while (nGrid->ScanFull(i, j, k, z))
				{
					y = nGrid->GetY(j) - (nGrid->m_fWidthY - m_fWidthY) / 2;
					if ((y >= GetY(0)) && (y <= GetY(m_dwSizeY - 1)))
					{
						*z = GetApproxZonY(y, i);
						nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
					}
				}
				break;
			}
		}
		else
		{
			nGrid->ScanFullSetup();

			while (nGrid->ScanFull(i, j, k, z))
			{
				*z = GetZ(k);
				nGrid->SetAlphaIndex(k, GetAlphaIndex(k));
			}
		}
	}

	*this = *nGrid;

	delete nGrid;
}

void CGrid::SetColor(int k, float r, float g, float b)
{
	S_COLORDATA *col = &m_Color[k];
	col->colR = r;
	col->colG = g;
	col->colB = b;
}

void CGrid::DrawGL(S_GRIDDRAWINFO *drawinfo)
{
	float            x;
	int              i, j, jj, k, kk, ia, ib, ic;
	S_COLORDATA      *colJ, *colJJ;
	S_ALPHAINDEXDATA *a;
	CTris            *t;
	CString          s;
	float            fr_r, fr_g, fr_b, fr_a;

	fr_r = GetRValue(g_Prefs.m_FrameColor) / 255.f;
	fr_g = GetGValue(g_Prefs.m_FrameColor) / 255.f;
	fr_b = GetBValue(g_Prefs.m_FrameColor) / 255.f;
	fr_a = float(g_Prefs.m_FrameTransparency) / 255.f;
	bool r_fl_quadframe = drawinfo->dwRenderMode & R_FL_QUADFRAME;
	bool r_fl_trisframe = drawinfo->dwRenderMode & R_FL_TRISFRAME;

	drawinfo->dwRenderMode &= ~R_FL_MASK;

	float offset = 0.00005f;

	glDepthRange(offset, 1.0);

	switch (drawinfo->dwRenderMode)
	{
	case (R_WIREFRAME):
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_POLYGON_OFFSET_LINE);

		k  = 0;
		kk = m_dwSizeX;
		for (j = 0; j < m_dwSizeY - 1; j++)
		{
			glBegin(GL_QUAD_STRIP);

			jj = j + 1;
			for (i = 0; i < m_dwSizeX; i++)
			{
				colJ  = &m_Color[k++];          // (i,j)
				colJJ = &m_Color[kk++];         // (i,jj)

				x = GetX(i);
				glColor3f(colJJ->colR, colJJ->colG, colJJ->colB);
				glVertex3f(x, GetY(jj), GetZ(i, jj));
				glColor3f(colJ->colR, colJ->colG, colJ->colB);
				glVertex3f(x, GetY(j), GetZ(i, j));
			}

			glEnd();
		}
		break;
	case (R_FLAT):
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);

		glBegin(GL_TRIANGLES);

		{
			ScanTrisSetup();
			while (ScanTris(t))
			{
				UpdateTrisZ(t);

				if ((m_bMarkFlatTris && t->GetFlat()) ||
				    (m_bMarkSlopeTris && t->GetSlope()) ||
				    (m_bMarkExcludedTris && t->GetExcluded()))
				{
					// if marked tris draw it no blended, but monochrome
					colJ = &t->m_Color;
					glColor3f(colJ->colR, colJ->colG, colJ->colB);

					glVertex3f(*t->m_vx[0], *t->m_vy[0], t->m_vz[0]);
					glVertex3f(*t->m_vx[1], *t->m_vy[1], t->m_vz[1]);
					glVertex3f(*t->m_vx[2], *t->m_vy[2], t->m_vz[2]);
				}
				else
				{
					// if not marked tris draw it blended grayscale

					colJ = t->m_vcol[0];
					glColor3f(colJ->colR, colJ->colG, colJ->colB);
					glVertex3f(*t->m_vx[0], *t->m_vy[0], t->m_vz[0]);

					colJ = t->m_vcol[1];
					glColor3f(colJ->colR, colJ->colG, colJ->colB);
					glVertex3f(*t->m_vx[1], *t->m_vy[1], t->m_vz[1]);

					colJ = t->m_vcol[2];
					glColor3f(colJ->colR, colJ->colG, colJ->colB);
					glVertex3f(*t->m_vx[2], *t->m_vy[2], t->m_vz[2]);
				}
			}
		}

		glEnd();
/*
            k = 0;
            kk = m_dwSizeX;
            for(j=0;j<m_dwSizeY-1;j++)
            {
                glBegin(GL_QUAD_STRIP);

                jj = j + 1;
                for(i=0;i<m_dwSizeX;i++)
                {
                    colJ	= &m_Color[k++];
                    colJJ	= &m_Color[kk++];

                    x = GetX(i);
                    glColor3f(colJJ->colR,colJJ->colG,colJJ->colB);
                    glVertex3f(x,GetY(jj),GetZ(i,jj));
                    glColor3f(colJ->colR,colJ->colG,colJ->colB);
                    glVertex3f(x,GetY(j),GetZ(i,j));
                }

                glEnd();
            }
*/
		break;
	case (R_TEXTURE):
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);

		glBegin(GL_TRIANGLES);

		//
		// Black surface
		//

		glColor3f(0.f, 0.f, 0.f);
//			glColor3f(1.f,1.f,1.f);

		ScanTrisSetup();
		while (ScanTris(t))
		{
			UpdateTrisZ(t);

/*				if ( m_bMarkFlatTris&&t->GetFlat() || m_bMarkSlopeTris&&t->GetSlope() || m_bMarkSlopeTris&&t->GetExcluded() )
                {
                    colJ	= &t->m_gColor;
                    glColor3f(colJ->colR,colJ->colG,colJ->colB);
                }*/

			glVertex3f(*t->m_vx[0], *t->m_vy[0], t->m_vz[0]);
			glVertex3f(*t->m_vx[1], *t->m_vy[1], t->m_vz[1]);
			glVertex3f(*t->m_vx[2], *t->m_vy[2], t->m_vz[2]);
		}

		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		glDisable(GL_POLYGON_OFFSET_LINE);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//
// L'errore stava in m_Face ...
//

		ScanTrisSetup();
		while (ScanTris(t))
		{
			UpdateTrisZ(t);

			glNormal3f(t->m_Normal.x, t->m_Normal.y, t->m_Normal.z);

			ia = GetAlphaIndex(t->m_vk[0]);
			ib = GetAlphaIndex(t->m_vk[1]);
			ic = GetAlphaIndex(t->m_vk[2]);

			if ((ia != ib) && (ia != ic) && (ib != ic))
			{
				continue;     // 3 triangles meet -> ERROR
			}

			if ((ia == ib) && (ib == ic))
			{
				a = drawinfo->pAlphamapManager->Get(ia);
				if (a->tex)
				{
					glBindTexture(GL_TEXTURE_2D, a->tex->m_glTexID);
				}

				t->DrawAlphaGL(1.f, 1.f, 1.f);

				continue;
			}

			if (ia != ib)
			{
				if (ia == ic)
				{
					a = drawinfo->pAlphamapManager->Get(ia);
					if (a->tex)
					{
						glBindTexture(GL_TEXTURE_2D, a->tex->m_glTexID);
					}

					t->DrawAlphaGL(1.f, 0.f, 1.f);

					a = drawinfo->pAlphamapManager->Get(ib);
					if (a->tex)
					{
						glBindTexture(GL_TEXTURE_2D, a->tex->m_glTexID);
					}

					t->DrawAlphaGL(0.f, 1.f, 0.f);
				}
				else if (ib == ic)     // ib == ic
				{
					a = drawinfo->pAlphamapManager->Get(ib);
					if (a->tex)
					{
						glBindTexture(GL_TEXTURE_2D, a->tex->m_glTexID);
					}

					t->DrawAlphaGL(0.f, 1.f, 1.f);

					a = drawinfo->pAlphamapManager->Get(ia);
					if (a->tex)
					{
						glBindTexture(GL_TEXTURE_2D, a->tex->m_glTexID);
					}

					t->DrawAlphaGL(1.f, 0.f, 0.f);
				}
			}
			else     // ia == ib
			{
				if (ia != ic)     // ia == ib , != ic
				{
					a = drawinfo->pAlphamapManager->Get(ia);
					if (a->tex)
					{
						glBindTexture(GL_TEXTURE_2D, a->tex->m_glTexID);
					}

					t->DrawAlphaGL(1.f, 1.f, 0.f);

					a = drawinfo->pAlphamapManager->Get(ic);
					if (a->tex)
					{
						glBindTexture(GL_TEXTURE_2D, a->tex->m_glTexID);
					}

					t->DrawAlphaGL(0.f, 0.f, 1.f);
				}
			}
		}

		glDisable(GL_BLEND);            // Turn Blending Off
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		break;
	case (R_ALPHAMAP):
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);

		glBegin(GL_TRIANGLES);

		ScanTrisSetup();
		while (ScanTris(t))
		{
			UpdateTrisZ(t);

			a = drawinfo->pAlphamapManager->Get(GetAlphaIndex(t->m_vk[0]));
			glColor3ub(GetRValue(a->col), GetGValue(a->col), GetBValue(a->col));
			glVertex3f(*t->m_vx[0], *t->m_vy[0], t->m_vz[0]);

			a = drawinfo->pAlphamapManager->Get(GetAlphaIndex(t->m_vk[1]));
			glColor3ub(GetRValue(a->col), GetGValue(a->col), GetBValue(a->col));
			glVertex3f(*t->m_vx[1], *t->m_vy[1], t->m_vz[1]);

			a = drawinfo->pAlphamapManager->Get(GetAlphaIndex(t->m_vk[2]));
			glColor3ub(GetRValue(a->col), GetGValue(a->col), GetBValue(a->col));
			glVertex3f(*t->m_vx[2], *t->m_vy[2], t->m_vz[2]);
		}

		glEnd();
		break;
	}

//	glDepthRange (0.0, 1.0 - offset);
	glPushMatrix();
	glTranslatef(0, 0, 2);

	if ((r_fl_quadframe || r_fl_trisframe) && (drawinfo->dwRenderMode != R_WIREFRAME))
	{
//		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
//		glEnable(GL_POLYGON_OFFSET_LINE);

		if (drawinfo->bAASWires)
		{
			glEnable(GL_LINE_SMOOTH);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(fr_r, fr_g, fr_b, fr_a);

		for (j = 0; j < m_dwSizeY; j++)
		{
			glBegin(GL_LINE_STRIP);

			for (i = 0; i < m_dwSizeX; i++)
			{
				glVertex3f(GetX(i), GetY(j), GetZ(i, j));
			}

			glEnd();
		} //j

		for (i = 0; i < m_dwSizeX; i++)
		{
			glBegin(GL_LINE_STRIP);

			for (j = 0; j < m_dwSizeY; j++)
			{
				glVertex3f(GetX(i), GetY(j), GetZ(i, j));
			}

			glEnd();
		} //i

		if (r_fl_trisframe)
		{
			ScanTrisSetup();
			while (ScanTris(t))
			{
				UpdateTrisZ(t);

				glBegin(GL_LINES);

				// a-c
				glVertex3f(*t->m_vx[0], *t->m_vy[0], t->m_vz[0]);
				glVertex3f(*t->m_vx[2], *t->m_vy[2], t->m_vz[2]);

				glEnd();

				ScanTris(t); // 1 yes, 1 no
			}
		}

		if (drawinfo->bAASWires)
		{
			glDisable(GL_LINE_SMOOTH);
		}

		glDisable(GL_BLEND);
//		glDisable(GL_POLYGON_OFFSET_LINE);
	} // OverlayFrame3D

	glPopMatrix();
//	glDepthRange (0.0, 1.0);
}

//
//
// if factor = 0 , texture natural
// if factor >= 1 , texture fit itself to <factor> cells
// if factor < -1 , texture fit itself to |<factor>| times inside a cell
//
//
void CGrid::TextureStretch(int factor)
{
	int          i, j;
	float        x, xx, y, yy;
	float        sx, sy, xstep, ystep, sxstart, systart;
	S_FACEDATAEX *f;

	if (factor >= 1)
	{
		xstep = 1.f / float(factor);
		ystep = 1.f / float(factor);
	}
	else if (factor < -1)
	{
		xstep = (float)fabs((float)factor);
		ystep = (float)fabs((float)factor);
	}

	sxstart = -xstep * (m_dwCellsX / 2);
	systart = -ystep * (m_dwCellsY / 2);

	sy = systart;

	for (j = 0; j < m_dwCellsY; j++)
	{
		y  = GetY(j);
		yy = GetY(j + 1);

		sx = sxstart;

		for (i = 0; i < m_dwCellsX; i++)
		{
			x  = GetX(i);
			xx = GetX(i + 1);

			f        = GetFace(i, j);
			f->sx    = sx;
			f->sy    = sy;
			f->sxx   = sx + xstep;
			f->syy   = sy + ystep;
			f->xstep = xstep;
			f->ystep = ystep;

			sx += xstep;
		}

		sy += ystep;
	}
}


void CGrid::CalcTrisNormals()
{
	CTris *t;

	ScanTrisSetup();
	while (ScanTris(t))
	{
		UpdateTrisZ(t);

		t->CalcNormal();
	}
}

void CGrid::SelectZBuf(int buf)
{
	m_Z = m_ZBuffer[buf];
}

void CGrid::ZBufCpy(int dest, int src)
{
	memcpy(m_ZBuffer[dest], m_ZBuffer[src], sizeof(float) * m_dwSizeZ);
}

int CGrid::GetAlphaIndex(int i, int j)
{
	return m_Alphamap[i + j * m_dwSizeX];
}

int CGrid::GetAlphaIndex(int k)
{
	return m_Alphamap[k];
}

void CGrid::SetAlphaIndex(int i, int j, int index)
{
	m_Alphamap[i + j * m_dwSizeX] = index;
}

void CGrid::SetAlphaIndex(int k, int index)
{
	m_Alphamap[k] = index;
}

void CGrid::PaintAlphaIndex(int i_center, int j_center, int radius, int index)
{
	int i, j;

	for (j = j_center - radius; j <= j_center + radius; j++)
		for (i = i_center - radius; i <= i_center + radius; i++)
		{
			if (
			    i >= 0 && j >= 0 && i < m_dwSizeX && j < m_dwSizeY &&
			    sqrt((float)(i - i_center) * (i - i_center) + (float)(j - j_center) * (j - j_center)) <= (float)radius
			    )
			{
				m_Alphamap[i + j * m_dwSizeX] = index;
			}
		}
}

void CGrid::ArchiveSave(CArchive &ar)
{
	ar << m_dwCellsX << m_dwCellsY << m_fWidthX << m_fWidthY;

	for (int i = 0; i < GRID_Z_BUFFERS; i++)
	{
		ar.Write(m_ZBuffer[i], sizeof(float) * m_dwSizeZ);
	}

	ar.Write(m_Alphamap, sizeof(int) * m_dwSizeZ);

	ar.Write(m_Face, sizeof(S_FACEDATAEX) * m_dwSizeFace);
}

/*
char	TAG[32];			//	"SIZEXY"
int		BLOCKSIZE;			//	8
char	DATA[BLOCKSIZE];	//  m_dwSizeX, m_dwSizeY
:
:
:
*/
/*
void CGrid::FileSave(const char* filename)
{
    FILE*	f;
    char	blockname[32];
    size_t	blocksize;

    CTris*	t;
    bool	pair;
    S_FACEDATAEX* face;

    f = fopen(filename, "ab");

    fseek(f,0,SEEK_END);

    // m_dwCellsX, m_dwCellsY
    strcpy(blockname,"GRID_CELLS");
    blocksize = sizeof(int)*2;
        fwrite((void*)blockname,sizeof(char),32,f);
        fwrite(&blocksize,sizeof(size_t),1,f);

    fwrite(&m_dwCellsX,sizeof(int),1,f);
    fwrite(&m_dwCellsY,sizeof(int),1,f);

    // m_fWidthX, m_fWidthY
    strcpy(blockname,"GRID_WIDTHS");
    blocksize = sizeof(float)*2;
        fwrite((void*)blockname,sizeof(char),32,f);
        fwrite(&blocksize,sizeof(size_t),1,f);

    fwrite(&m_fWidthX,sizeof(float),1,f);
    fwrite(&m_fWidthY,sizeof(float),1,f);

    // m_ZBuffer[][]
    strcpy(blockname,"GRID_Z");
    blocksize = sizeof(float) * m_dwSizeZ * GRID_Z_BUFFERS;
        fwrite((void*)blockname,sizeof(char),32,f);
        fwrite(&blocksize,sizeof(size_t),1,f);

    for(int i=0;i<GRID_Z_BUFFERS;i++)
    {
        fwrite(m_ZBuffer[i],sizeof(float),m_dwSizeZ,f);
    }

    // m_Alphamap[]
    strcpy(blockname,"GRID_ALPHAMAP");
    blocksize = sizeof(int)*m_dwSizeZ;
        fwrite((void*)blockname,sizeof(char),32,f);
        fwrite(&blocksize,sizeof(size_t),1,f);

    fwrite(m_Alphamap,sizeof(int),m_dwSizeZ,f);

    // m_Face[]
    pair = true;
    ScanTrisSetup();
    while (ScanTris(t))
    {
        face = (S_FACEDATAEX*)t->GetFacePtr();

        if (pair)
        {
            if (t->GetExcluded())
                face->flags	|= FACE_FL_EXTRIS0;
            else
                face->flags	&= ~FACE_FL_EXTRIS0;
        }
        else
        {
            if (t->GetExcluded())
                face->flags	|= FACE_FL_EXTRIS1;
            else
                face->flags	&= ~FACE_FL_EXTRIS1;
        }

        pair = !pair;
    }

    strcpy(blockname,"GRID_FACE");
    blocksize = sizeof(S_FACEDATAEX)*m_dwSizeFace;
        fwrite((void*)blockname,sizeof(char),32,f);
        fwrite(&blocksize,sizeof(size_t),1,f);

    fwrite(m_Face,sizeof(S_FACEDATAEX),m_dwSizeFace,f);

    fclose(f);
}
*/

void CGrid::FileSave_1_3(const char *filename)
{
	std::ofstream    f;
	S_EG2BLOCKHEADER block;
	char             tris_prop;
	CTris            *t;

	f.open(filename, std::ios::out | std::ios::binary | std::ios::app);

	// m_dwCellsX, m_dwCellsY
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "GRID_CELLS");
	block.size = sizeof(int) * 2;
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	f.write((char *)&m_dwCellsX, sizeof(m_dwCellsX));
	f.write((char *)&m_dwCellsY, sizeof(m_dwCellsY));

	// m_fWidthX, m_fWidthY
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "GRID_WIDTHS");
	block.size = sizeof(float) * 2;
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	f.write((char *)&m_fWidthX, sizeof(m_fWidthX));
	f.write((char *)&m_fWidthY, sizeof(m_fWidthY));

	// m_vOrigin
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "GRID_ORIGIN");
	block.size = sizeof(float) * 3;
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	f.write((char *)&m_vOrigin.x, sizeof(m_vOrigin.x));
	f.write((char *)&m_vOrigin.y, sizeof(m_vOrigin.y));
	f.write((char *)&m_vOrigin.z, sizeof(m_vOrigin.z));

	// m_ZBuffer[][]
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "GRID_Z");
	block.size = sizeof(float) * m_dwSizeZ;
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	f.write((char *)m_ZBuffer[GRID_Z_REAL], sizeof(float) * m_dwSizeZ);

	// m_Alphamap[]
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "GRID_ALPHAMAP");
	block.size = sizeof(int) * m_dwSizeZ;
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	f.write((char *)m_Alphamap, sizeof(int) * m_dwSizeZ);

	// m_Face[]
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "GRID_FACE");
	block.size = sizeof(S_FACEDATAEX) * m_dwSizeFace;
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	f.write((char *)m_Face, sizeof(S_FACEDATAEX) * m_dwSizeFace);

	// CTris options
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "GRID_TRISPROPERTIES");
	block.size = sizeof(tris_prop) * m_dwSizeTris;
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	ScanTrisSetup();
	while (ScanTris(t))
	{
		tris_prop = 0;

		if (t->m_bExcluded)
		{
			tris_prop |= 0x01;
		}
//		if (t->m_bA)		tris_prop |= 0x02;
//		if (t->m_bB)		tris_prop |= 0x04;
//		if (t->m_bC)		tris_prop |= 0x08;

		TRACE("tris_prop = %c\n", tris_prop);

		f.write((char *)&tris_prop, sizeof(tris_prop));
	}

	f.close();
}

void CGrid::FileSave_1_4(const char *filename)
{
	std::ofstream    f;
	S_EG2BLOCKHEADER block;
	char             tris_prop;
	CTris            *t;

	f.open(filename, std::ios::out | std::ios::binary | std::ios::app);

	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "GRID");
	block.size =
	    sizeof(m_dwCellsX) +
	    sizeof(m_dwCellsY) +
	    sizeof(m_fWidthX) +
	    sizeof(m_fWidthY) +
	    sizeof(m_vOrigin.x) + sizeof(m_vOrigin.y) + sizeof(m_vOrigin.z) +
	    sizeof(float) * m_dwSizeZ +
	    sizeof(int) * m_dwSizeZ +
	    sizeof(S_FACEDATAEX) * m_dwSizeFace +
	    sizeof(tris_prop) * m_dwSizeTris;
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	// m_dwCellsX, m_dwCellsY
	f.write((char *)&m_dwCellsX, sizeof(m_dwCellsX));
	f.write((char *)&m_dwCellsY, sizeof(m_dwCellsY));

	// m_fWidthX, m_fWidthY
	f.write((char *)&m_fWidthX, sizeof(m_fWidthX));
	f.write((char *)&m_fWidthY, sizeof(m_fWidthY));

	// m_vOrigin
	f.write((char *)&m_vOrigin.x, sizeof(m_vOrigin.x));
	f.write((char *)&m_vOrigin.y, sizeof(m_vOrigin.y));
	f.write((char *)&m_vOrigin.z, sizeof(m_vOrigin.z));

	// m_ZBuffer[][]
	f.write((char *)m_ZBuffer[GRID_Z_REAL], sizeof(float) * m_dwSizeZ);

	// m_Alphamap[]
	f.write((char *)m_Alphamap, sizeof(int) * m_dwSizeZ);

	// m_Face[]
	f.write((char *)m_Face, sizeof(S_FACEDATAEX) * m_dwSizeFace);

	// CTris options
	ScanTrisSetup();
	while (ScanTris(t))
	{
		tris_prop = 0;

		if (t->m_bExcluded)
		{
			tris_prop |= 0x01;
		}
//		if (t->m_bA)		tris_prop |= 0x02;
//		if (t->m_bB)		tris_prop |= 0x04;
//		if (t->m_bC)		tris_prop |= 0x08;

		TRACE("tris_prop = %c\n", tris_prop);

		f.write((char *)&tris_prop, sizeof(tris_prop));
	}

	f.close();
}

void CGrid::ArchiveLoad_1_0(CArchive &ar)
{
	S_FACEDATA *Face_1_0;

	ar >> m_dwCellsX >> m_dwCellsY >> m_fWidthX >> m_fWidthY;

	Set(m_dwCellsX, m_dwCellsY, m_fWidthX, m_fWidthY);

	for (int i = 0; i < GRID_Z_BUFFERS; i++)
	{
		ar.Read(m_ZBuffer[i], sizeof(float) * m_dwSizeZ);
	}

	ar.Read(m_Alphamap, sizeof(int) * m_dwSizeZ);
//	ar.Read(m_Face,sizeof(S_FACEDATA)*m_dwSizeFace); [old]

	Face_1_0 = new S_FACEDATA[m_dwSizeFace];
	ar.Read(Face_1_0, sizeof(S_FACEDATA) * m_dwSizeFace);

	int          k;
	S_FACEDATAEX *dat;
	S_FACEDATA   *datold;
	for (k = 0; k < m_dwSizeFace; k++)
	{
		dat    = &m_Face[k];
		datold = &Face_1_0[k];

		dat->sx    = datold->sx;
		dat->sxx   = datold->sxx;
		dat->sy    = datold->sy;
		dat->syy   = datold->syy;
		dat->xstep = datold->xstep;
		dat->ystep = datold->ystep;

//		dat->flags ... from previous versions are standard inverted 1 by 1
	}

	delete[] Face_1_0;
}

int CGrid::GetK(int i, int j)
{
	return i + j * m_dwSizeX;
}

void CGrid::CalcColors()
{
	CNmVec3 colUP, colDN, colMID, colZ;
	CNmVec3 min;
	int     i, j, k;
	float   *z;
	float   maxh, c;
	CTris   *t;

	// gradient, always
	maxh = GetMaxHeight();

	if (maxh == 0.f)
	{
		maxh = 1.f;
	}

	colUP.Set(1.f, 1.f, 1.f);
	colMID.Set(0.5f, 0.5f, 0.5f);
	colDN.Set(0.f, 0.f, 0.f);

	ScanFullSetup();
	while (ScanFull(i, j, k, z))
	{
		c = *z / maxh;

		colZ = colDN + (colUP - colDN) * c;

		SetColor(k, colZ.x, colZ.y, colZ.z);
	}
	// gradient, always - END

//	if (m_bMarkFlatTris||m_bMarkSlopeTris||m_bMarkExcludedTris)
	{
		ScanTrisSetup();
		while (ScanTris(t))
		{
			UpdateTrisZ(t);

			t->SetColor(1.f, 1.f, 1.f); // to draw blended texture correctly

			// flat tris
			if (m_bMarkFlatTris)
			{
				if (t->CalcFlat())
				{
					t->SetColor(g_Prefs.m_colFlatTris.x, g_Prefs.m_colFlatTris.y, g_Prefs.m_colFlatTris.z);
				}
			}

			// steep tris
			if (m_bMarkSlopeTris)
			{
				t->CalcNormal();
				if (t->CalcSlope(g_Prefs.m_fSlopeAngle))
				{
					t->SetColor(g_Prefs.m_colSteepTris.x, g_Prefs.m_colSteepTris.y, g_Prefs.m_colSteepTris.z);
				}
			}

			// excluded tris
			if (m_bMarkExcludedTris)
			{
				if (t->GetExcluded())
				{
					t->SetColor(g_Prefs.m_colExcludedTris.x, g_Prefs.m_colExcludedTris.y, g_Prefs.m_colExcludedTris.z);
				}
			}
		}
	}
}

void CGrid::TrisLink()
{
	int          tris = 0;
	S_FACEDATAEX *f;
	int          i, j, k;
	float        *z;

	ScanRectSetup(0, 0, m_dwSizeX - 2, m_dwSizeY - 2);

	while (ScanRect(i, j, k, z))
	{
		f = GetFace(i, j);

		FaceOrientateTris(i, j, bool(f->flags & FACE_FL_TRISINV));
	}
}

CTris *CGrid::GetTris(int m)
{
	if ((m >= 0) && (m < m_dwSizeTris))
	{
		return &m_Tris[m];
	}
	else
	{
		return NULL;
	}
}

void CGrid::GetTris(int fi, int fj, CTris *& t0, CTris *& t1)
{
	int m = (fi + fj * m_dwCellsX) * 2;

	t0 = &m_Tris[m];
	t1 = &m_Tris[m + 1];
}

//
// Loads corresponding Z depending on which zbuffer is active
//
void CGrid::UpdateTrisZ(CTris *t)
{
	t->SetZ(GetZ(t->m_vk[0]), GetZ(t->m_vk[1]), GetZ(t->m_vk[2]));
}

void CGrid::ScanTrisSetup()
{
	m_m = 0;
}

bool CGrid::ScanTris(CTris *&t)
{
	if (m_m == m_dwSizeTris)
	{
		return false;
	}

	t = GetTris(m_m++);

	return true;
}

bool CGrid::AlphamapFromFile(LPCTSTR filename)
{
	EasyImage  pcx;
	BTCOLORREF col;
	int        x, y;

	if (!pcx.LoadImageFromFile(filename))
	{
		return false;
	}

	if ((pcx.GetHeight() != m_dwSizeY) || (pcx.GetWidth() != m_dwSizeX))
	{
/*		CString s;
        s.Format(	"Alphmap size (%d %d) does not match the actual grid size (%d %d)\r\n\r\n"
                    "Alphamap cannot be imported. Resize the grid first.",pcx.GetWidth()-1,pcx.GetHeight()-1,m_dwSizeX-1,m_dwSizeY-1);
        AfxMessageBox( s );*/
		pcx.ClearData();
		return false;
	}

	pcx.Flip();

	for (y = 0; y < pcx.GetHeight(); y++)
	{
		for (x = 0; x < pcx.GetWidth(); x++)
		{
			col = pcx.GetColorFromPixel(x, y);
			SetAlphaIndex(x, y, pcx.GetIndexFromColor(col));
		}
	}

	pcx.ClearData();

	return true;
}

float CGrid::GetWidthX()
{
	return m_fWidthX;
}

float CGrid::GetWidthY()
{
	return m_fWidthY;
}

float CGrid::GetCellWidthX()
{
	return m_fCellWidthX;
}

float CGrid::GetCellWidthY()
{
	return m_fCellWidthY;
}

int CGrid::GetSizeX()
{
	return m_dwSizeX;
}

int CGrid::GetSizeY()
{
	return m_dwSizeY;
}

int CGrid::GetSizeZ()
{
	return m_dwSizeZ;
}

int CGrid::GetCellsX()
{
	return m_dwCellsX;
}

int CGrid::GetCellsY()
{
	return m_dwCellsY;
}

unsigned int CGrid::Memory(unsigned int cellsx, unsigned int cellsy)
{
	unsigned int sizex, sizey, sizez, sizecolor, sizeface, sizetris;

	unsigned int memory;

	sizex     = cellsx + 1;
	sizey     = cellsy + 1;
	sizez     = sizex * sizey;
	sizecolor = sizez;
	sizeface  = cellsx * cellsy;
	sizetris  = (cellsx * cellsy) * 2;

	memory = 0;

	memory += sizeof(float) * sizex;   // x
	memory += sizeof(float) * sizey;   // y
	memory += sizeof(int) * sizez;     // alphamap
	memory += sizeof(S_COLORDATA) * sizecolor;     // m_Color
//	memory+= sizeof(S_FACEDATA)*sizeface;		// m_Face [old]
	memory += sizeof(S_FACEDATAEX) * sizeface;     // m_Face
	memory += sizeof(CTris) * sizetris;            // m_Tris

	for (int i = 0; i < GRID_Z_BUFFERS; i++)
	{
		memory += sizeof(float) * sizez;           // m_ZBuffer[i]
	}

	return memory;
}

bool CGrid::GetInfo(float x, float y, S_POSINFO *inf)
{
	int     i, ii, j, jj, trisi;
	CNmVec3 eye, ray, risu;
	float   dummy;
	CTris   *tris;
	S_PLANE plane;

	if (
	    (x < GetX(0)) ||
	    (x > GetX(m_dwSizeX - 1)) ||
	    (y < GetY(0)) ||
	    (y > GetY(m_dwSizeY - 1))
	    )
	{
		return false;
	}

	i  = GetLeft_i(x);
	j  = GetLeft_j(y);
	ii = GetRight_i(x);
	jj = GetRight_j(y);

	if (i != ii)
	{
		if ((x - GetX(i)) > (m_fCellWidthX / 2))
		{
			i = ii;
		}
	}

	if (j != jj)
	{
		if ((y - GetY(j)) > (m_fCellWidthY / 2))
		{
			j = jj;
		}
	}

	inf->i = i;
	inf->j = j;
//	inf->z = GetZ(i,j);

	trisi = GetTris(x, y);

	if (trisi == -1)
	{
		return false;
	}

	tris = GetTris(trisi);

	if (!tris)
	{
		return false;
	}

	tris->GetPlane(&plane);

	eye.Set(x, y, 100000.f);
	ray.Set(0, 0, -1);

	if (u3d_PlaneRayIntersection(&plane, &eye, &ray, &risu, &dummy))
	{
		inf->z = risu.z;
	}

	return true;
}

void CGrid::SetMaxHeight(float h)
{
	float maxh = GetMaxHeight();
	int   i, j, k;
	float *z;
	float ratio;

	if ((maxh == 0.0f) || (h == 0.0f))
	{
		return;
	}

	ratio = h / maxh;

	ScanFullSetup();
	while (ScanFull(i, j, k, z))
	{
		*z = *z * ratio;
	}
}

void CGrid::SetOrigin(CNmVec3& origin)
{
	m_vOrigin = origin;
}

bool IsValidBrushToImport(CNmPoly *poly)
{
	if (poly->GetNumFaces() != 5)
	{
		return false;
	}

	return true;
}

bool CGrid::FromPolyList(CNmPolyList& polys)
{
	CTris   *tris;
	CTris   *t0;
	CTris   *t1;
	CNmPoly *poly;
	CNmVec3 t_min, t_max, p_min, p_max, t_origin;
	float   t_minz, t_maxz, t_floor;

	t_min.Set(FLT_MAX, FLT_MAX, FLT_MAX);
	t_max.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	SCAN_NM_POLY(polys, poly)
	{
		if (!IsValidBrushToImport(poly))
		{
			continue;
		}

		poly->GetBox(p_min, p_max);

		u3d_Min(&p_min, &t_min, &t_min);
		u3d_Max(&p_max, &t_max, &t_max);
	}

	t_origin = (t_min + t_max) / 2;

	// p_min, p_max are the bounding box of a Brush

	Set((int)floor((t_max.x - t_min.x) / (p_max.x - p_min.x) + 0.5f),
	    (int)floor((t_max.y - t_min.y) / (p_max.y - p_min.y) + 0.5f),
	    t_max.x - t_min.x,
	    t_max.y - t_min.y);

	// exclude all triangles
	for (int m = 0; m < m_dwSizeTris; m++)
	{
		tris = GetTris(m);
		tris->SetExcluded(true);
	}

	CNmFace *face;
	CNmVec3 n, v[3];
	bool    stop;
	int     i, j, vi;

	// get t_minz, t_maxz
	t_minz = FLT_MAX;
	t_maxz = -FLT_MAX;
	SCAN_NM_POLY(polys, poly)
	{
		if (!IsValidBrushToImport(poly))
		{
			continue;
		}

		for (face = poly->m_Face; face; face = face->Next)
		{
			if (!face->CalcNormal(&n))
			{
				break;
			}

			if (n.z > 0.001f)
			{
				stop = false;
				SCAN_NM_FACEPOINT(face, item2, stop)
				{
					if (item2->vecptr->z < t_minz)
					{
						t_minz = item2->vecptr->z;
					}

					if (item2->vecptr->z > t_maxz)
					{
						t_maxz = item2->vecptr->z;
					}

				}
			}
		}
	}

	TRACE("T MIN MAX %1.0f %1.0f\n", t_minz, t_maxz);

	t_floor    = floor(t_minz) - int(floor(t_minz)) % 64;
	t_origin.z = t_floor;

	// Sets Z
	// Orientates triangles
	// Enables triangles
	SelectZBuf(GRID_Z_REAL);
	SCAN_NM_POLY(polys, poly)
	{
		if (!IsValidBrushToImport(poly))
		{
			continue;
		}

		for (face = poly->m_Face; face; face = face->Next)
		{
			if (!face->CalcNormal(&n))
			{
				break;
			}

			if (n.z > 0.001f)
			{

				// Z
				vi   = 0;
				stop = false;
				SCAN_NM_FACEPOINT(face, item, stop)
				{
					i = (int)floor((item->vecptr->x - t_min.x) / (p_max.x - p_min.x) + 0.5f);
					j = (int)floor((item->vecptr->y - t_min.y) / (p_max.y - p_min.y) + 0.5f);

					v[vi++] = *item->vecptr;

//					SetZ(i,j, item->vecptr->z - t_minz );
					SetZ(i, j, item->vecptr->z - t_floor);
				}

				CNmVec3      v_med, vtmp, pt1, pt2;
				int          fi, fj, i_incl;
				S_FACEDATAEX *gface;

				// pt medio del triangolo
				v_med = (v[0] + v[1] + v[2]) / 3 - t_origin;

				fi = GetLeft_i(v_med.x);
				fj = GetLeft_j(v_med.y);

				gface = GetFace(fi, fj);

				i_incl = -1;

				for (i = 0; i < 3; i++)
				{
					vtmp = v[i] - v[(i + 2) % 3];

					if (fabs(vtmp.x) > 0.001f && fabs(vtmp.y) > 0.001f)
					{
						// inclinato
						i_incl = i;
						FaceOrientateTris(fi, fj, vtmp.x * vtmp.y > 0);
						break;
					}
				}

				// enable triangle
				GetTris(fi, fj, t0, t1);

				pt1 = (v[i_incl] + (v[0] + v[1] + v[2]) / 3) / 2 - t_origin;
				pt2 = (v[(i_incl + 2) % 3] + (v[0] + v[1] + v[2]) / 3) / 2 - t_origin;

				if (t0->PtInTris(pt1.x, pt1.y) && t0->PtInTris(pt2.x, pt2.y))
				{
					t0->SetExcluded(false);
				}
				else if (t1->PtInTris(pt1.x, pt1.y) && t1->PtInTris(pt2.x, pt2.y))
				{
					t1->SetExcluded(false);
				}

				break;
			}
		}
	}

	SetOrigin(t_origin);

	return true;
}

/*
 (XY view)
     _____
  D	|  |  |
  C	|__|__|
 (0)  A  B

  if (swap)		A <-> B
  if (!swap)	A <-  B  (B does not change)

  if (full)		C D computed
  if (!full)	C	 computed
*/
void CGrid::FlipH(bool swap, bool full)
{
	int   i, j, k;
	int   ii;
	float *z;
	int   f_alpha;
	float f_z;

	UINT  flags_i;
	UINT  flags_ii;
	CTris *t0_i, *t1_i, *t0_ii, *t1_ii;
	bool  b0_i, b1_i;

	int halfy = GetSizeY() / 2;

	ScanRectSetup(0, 0, GetSizeX() / 2 - 1, GetSizeY() - 1);
	while (ScanRect(i, j, k, z))
	{
		ii = m_dwSizeX - i - 1;

		if (full || ((!full) && (j < halfy)))
		{
			f_alpha = GetAlphaIndex(i, j);
			f_z     = *z;

			SetAlphaIndex(i, j, GetAlphaIndex(ii, j));
			SetZ(i, j, GetZ(ii, j));

			if (swap)
			{
				SetAlphaIndex(ii, j, f_alpha);
				SetZ(ii, j, f_z);
			} //swap
		} //full
	}

	halfy = GetCellsY() / 2;

	// m_Face && m_Tris
	ScanRectSetup(0, 0, GetCellsX() / 2 - 1, GetCellsY() - 1);
	while (ScanRect(i, j, k, z))
	{
		ii = m_dwCellsX - i - 1;

		if (full || ((!full) && (j < halfy)))
		{
			// face flags
			flags_i  = GetFace(i, j)->flags;
			flags_ii = GetFace(ii, j)->flags;

			if (flags_ii & FACE_FL_TRISINV)
			{
				GetFace(i, j)->flags &= ~FACE_FL_TRISINV;
			}
			else
			{
				GetFace(i, j)->flags |= FACE_FL_TRISINV;
			}

			FaceOrientateTris(i, j, bool(GetFace(i, j)->flags & FACE_FL_TRISINV));

			// tris
			GetTris(i, j, t0_i, t1_i);
			GetTris(ii, j, t0_ii, t1_ii);

			b0_i = t0_i->m_bExcluded;
			b1_i = t1_i->m_bExcluded;

			// inverted 0<>1, dued to FACE_FL_TRISINV.TGA
			t0_i->m_bExcluded = t1_ii->m_bExcluded;
			t1_i->m_bExcluded = t0_ii->m_bExcluded;

			if (swap)
			{
				if (flags_i & FACE_FL_TRISINV)
				{
					GetFace(ii, j)->flags &= ~FACE_FL_TRISINV;
				}
				else
				{
					GetFace(ii, j)->flags |= FACE_FL_TRISINV;
				}

				FaceOrientateTris(ii, j, bool(GetFace(ii, j)->flags & FACE_FL_TRISINV));

				// inverted 0<>1, dued to FACE_FL_TRISINV.TGA
				t0_ii->m_bExcluded = b1_i;
				t1_ii->m_bExcluded = b0_i;
			} //swap??
		} //full?
	}
}

/*
 (XY view)
     _____
  B	|  |  |
  A	|__|__|
 (0)  C  D

  if (swap)		A <-> B
  if (!swap)	A <-  B  (B does not change)

  if (full)		C D computed
  if (!full)	C	 computed
*/
void CGrid::FlipV(bool swap, bool full)
{
	int   i, j, k;
	int   jj;
	float *z;
	int   f_alpha;
	float f_z;

	UINT  flags_j;
	UINT  flags_jj;
	CTris *t0_j, *t1_j, *t0_jj, *t1_jj;
	bool  b0_j, b1_j;

	int halfx = GetSizeX() / 2;

	ScanRectSetup(0, 0, GetSizeX() - 1, GetSizeY() / 2 - 1);
	while (ScanRect(i, j, k, z))
	{
		jj = m_dwSizeY - j - 1;

		if (full || ((!full) && (i < halfx)))
		{
			f_alpha = GetAlphaIndex(i, j);
			f_z     = *z;

			SetAlphaIndex(i, j, GetAlphaIndex(i, jj));
			SetZ(i, j, GetZ(i, jj));

			if (swap)
			{
				SetAlphaIndex(i, jj, f_alpha);
				SetZ(i, jj, f_z);
			} //swap??
		} //full?
	}

	halfx = GetCellsX() / 2;

	// m_Face & m_Tris
	ScanRectSetup(0, 0, GetCellsX() - 1, GetCellsY() / 2 - 1);
	while (ScanRect(i, j, k, z))
	{
		jj = m_dwCellsY - j - 1;

		if (full || ((!full) && (i < halfx)))
		{
			// face
			flags_j  = GetFace(i, j)->flags;
			flags_jj = GetFace(i, jj)->flags;

			if (flags_jj & FACE_FL_TRISINV)
			{
				GetFace(i, j)->flags &= ~FACE_FL_TRISINV;
			}
			else
			{
				GetFace(i, j)->flags |= FACE_FL_TRISINV;
			}

			FaceOrientateTris(i, j, bool(GetFace(i, j)->flags & FACE_FL_TRISINV));

			// tris
			GetTris(i, j, t0_j, t1_j);
			GetTris(i, jj, t0_jj, t1_jj);

			b0_j = t0_j->m_bExcluded;
			b1_j = t1_j->m_bExcluded;

			t0_j->m_bExcluded = t0_jj->m_bExcluded;
			t1_j->m_bExcluded = t1_jj->m_bExcluded;

			if (swap)
			{
				if (flags_j & FACE_FL_TRISINV)
				{
					GetFace(i, jj)->flags &= ~FACE_FL_TRISINV;
				}
				else
				{
					GetFace(i, jj)->flags |= FACE_FL_TRISINV;
				}

				FaceOrientateTris(i, jj, bool(GetFace(i, jj)->flags & FACE_FL_TRISINV));

				t0_jj->m_bExcluded = b0_j;
				t1_jj->m_bExcluded = b1_j;
			} //swap??
		} //full?
	}
}

bool CGrid::PtInGrid(float x, float y)
{
	return (x <= GetX(GetSizeX() - 1)) && (x >= GetX(0)) &&
	       (y <= GetY(GetSizeY() - 1)) && (y >= GetY(0));
}

bool CGrid::AlphamapFromData(EasyImage &pcx)
{
	BTCOLORREF col;
	int        x, y;

	if ((pcx.GetHeight() != m_dwSizeY) || (pcx.GetWidth() != m_dwSizeX))
	{
		return false;
	}

	for (y = 0; y < pcx.GetHeight(); y++)
	{
		for (x = 0; x < pcx.GetWidth(); x++)
		{
			col = pcx.GetColorFromPixel(x, y);
			SetAlphaIndex(x, y, pcx.GetIndexFromColor(col));
		}
	}

	return true;
}

int CGrid::GetTris(float x, float y)
{
	CTris *t;
	int   m = 0;

	ScanTrisSetup();
	while (ScanTris(t))
	{
		if (t->PtInTris(x, y))
		{
			return m;
		}

		m++;
	}

	return -1;
}

/*
CTris* CGrid::GetTris0(int ci, int cj)
{
    int ck = (ci + cj * m_dwCellsX) * 2;
    return &m_Tris[ ck ];
}

CTris* CGrid::GetTris0(int ck)
{
    return &m_Tris[ ck * 2 ];
}

CTris* CGrid::GetTris1(int ci, int cj)
{
    int ck = (ci + cj * m_dwCellsX) * 2;
    return &m_Tris[ ck + 1 ];
}

CTris* CGrid::GetTris1(int ck)
{
    return &m_Tris[ (ck * 2) + 1 ];
}
*/
int CGrid::GetTrisCount()
{
	return m_dwSizeTris;
}

void CGrid::FaceOrientateTris(int ci, int cj, bool invert)
{
	int          kf, k;
	S_FACEDATAEX *f;
	CTris        *t;

	k  = ci + cj * m_dwSizeX;
	kf = ci + cj * m_dwCellsX;

	f = GetFace(ci, cj);

/*	t0 = GetTris0(kf);
    t1 = GetTris1(kf);

    t0->SetMisc(&f->xstep,	&f->ystep, (void*)f);
    t0->Seta( &m_X[ci],	  &m_Y[cj],	k,		&f->sx,	 &f->sy, &m_Color[k]);
    t0->Setb( &m_X[ci+1], &m_Y[cj],	k+1,	&f->sxx, &f->sy, &m_Color[k+1]);

    t1->SetMisc(&f->xstep,	&f->ystep, (void*)f);
    t1->Seta( &m_X[ci+1], &m_Y[cj+1], k+m_dwSizeX+1, &f->sxx, &f->syy, &m_Color[k+m_dwSizeX+1]);
    t1->Setb( &m_X[ci],	  &m_Y[cj+1], k+m_dwSizeX,   &f->sx,  &f->syy, &m_Color[k+m_dwSizeX]);

    if (invert)
    {
        t0->Setc(	&m_X[ci+1],	&m_Y[cj+1],	k+m_dwSizeX+1,
                    &f->sxx,	&f->syy,	&m_Color[k+m_dwSizeX+1]);
        t1->Setc(	&m_X[ci],	&m_Y[cj],	k,
                    &f->sx,		&f->sy,		&m_Color[k]);
    }
    else
    {
        t0->Setc(	&m_X[ci],	&m_Y[cj+1],	k+m_dwSizeX,
                    &f->sx,		&f->syy,	&m_Color[k+m_dwSizeX]);
        t1->Setc(	&m_X[ci+1],	&m_Y[cj],	k+1,
                    &f->sxx,	&f->sy,		&m_Color[k+1]);
    }*/
//	if (inv) [old]

	// invert = FACE_FL_TRISINV is ON

	if (invert)
	{
		t = GetTris(kf * 2);
		t->SetMisc(&f->xstep, &f->ystep, (void *)f);

		t->SetVertex(0, &m_X[ci + 1], &m_Y[cj + 1], k + m_dwSizeX + 1,
		             &f->sxx, &f->syy, &m_Color[k + m_dwSizeX + 1]);

		t->SetVertex(1, &m_X[ci], &m_Y[cj + 1], k + m_dwSizeX,
		             &f->sx, &f->syy, &m_Color[k + m_dwSizeX]);

		t->SetVertex(2, &m_X[ci], &m_Y[cj], k,
		             &f->sx, &f->sy, &m_Color[k]);


		t = GetTris((kf * 2) + 1);
		t->SetMisc(&f->xstep, &f->ystep, (void *)f);

		t->SetVertex(0, &m_X[ci], &m_Y[cj], k,
		             &f->sx, &f->sy, &m_Color[k]);

		t->SetVertex(1, &m_X[ci + 1], &m_Y[cj], k + 1,
		             &f->sxx, &f->sy, &m_Color[k + 1]);

		t->SetVertex(2, &m_X[ci + 1], &m_Y[cj + 1], k + m_dwSizeX + 1,
		             &f->sxx, &f->syy, &m_Color[k + m_dwSizeX + 1]);

	}
	else
	{
		t = GetTris(kf * 2);
		t->SetMisc(&f->xstep, &f->ystep, (void *)f);

		t->SetVertex(0, &m_X[ci], &m_Y[cj + 1], k + m_dwSizeX,
		             &f->sx, &f->syy, &m_Color[k + m_dwSizeX]);

		t->SetVertex(1, &m_X[ci], &m_Y[cj], k,
		             &f->sx, &f->sy, &m_Color[k]);

		t->SetVertex(2, &m_X[ci + 1], &m_Y[cj], k + 1,
		             &f->sxx, &f->sy, &m_Color[k + 1]);


		t = GetTris((kf * 2) + 1);
		t->SetMisc(&f->xstep, &f->ystep, (void *)f);

		t->SetVertex(0, &m_X[ci + 1], &m_Y[cj], k + 1,
		             &f->sxx, &f->sy, &m_Color[k + 1]);

		t->SetVertex(1, &m_X[ci + 1], &m_Y[cj + 1], k + m_dwSizeX + 1,
		             &f->sxx, &f->syy, &m_Color[k + m_dwSizeX + 1]);

		t->SetVertex(2, &m_X[ci], &m_Y[cj + 1], k + m_dwSizeX,
		             &f->sx, &f->syy, &m_Color[k + m_dwSizeX]);

	}
}

void CGrid::ArchiveLoad_1_1(CArchive &ar)
{
	int          k, fi, fj;
	S_FACEDATAEX *face;

	ar >> m_dwCellsX >> m_dwCellsY >> m_fWidthX >> m_fWidthY;

	Set(m_dwCellsX, m_dwCellsY, m_fWidthX, m_fWidthY);

	for (int i = 0; i < GRID_Z_BUFFERS; i++)
	{
		ar.Read(m_ZBuffer[i], sizeof(float) * m_dwSizeZ);
	}

	ar.Read(m_Alphamap, sizeof(int) * m_dwSizeZ);
	ar.Read(m_Face, sizeof(S_FACEDATAEX) * m_dwSizeFace);

	// Orientate tris
	for (k = 0; k < m_dwSizeFace; k++)
	{
		fi = k % m_dwCellsX;
		fj = k / m_dwCellsX;

		face = GetFace(fi, fj);

		FaceOrientateTris(fi, fj, bool(face->flags & FACE_FL_TRISINV));
	}
}

/*
bool CGrid::FileLoad(const char* filename)
{
    FILE*	f;
    char	blockname[32];
    size_t	blocksize;
    int		cells[2] = { -1, -1 };
    float	widths[2] = { -1, -1 };
    int		fi,fj,k;
    fpos_t	pos;
    bool	pair;
    CTris*	t;
    S_FACEDATAEX*	face;

    f = fopen(filename, "rb");

    if (f == NULL) return false;

    fseek(f,sizeof(S_EASYGENHEADER),SEEK_SET);

    while (fread(blockname,sizeof(char),32,f))
    {
        fread(&blocksize,sizeof(size_t),1,f);

        TRACE( " BLOCKNAME = [%s]\n", blockname);
        TRACE( " BLOCKSIZE = %d\n", blocksize);

        if (strcmp(blockname,"GRID_CELLS") == 0)
        {
            fread(cells,sizeof(int),2,f);
        }
        else if (strcmp(blockname,"GRID_WIDTHS") == 0)
        {
            fread(widths,sizeof(int),2,f);

            if ((cells[0]!=-1)&&(cells[1]!=-1)&&(widths[0]!=-1)&&(widths[1]!=-1))
            {
                Set(cells[0],cells[1],widths[0],widths[1]);
            }
            else
            {
                TRACE("Bad file format. Couldn't load the Grid");
                return false;
            }
        }
        else if (strcmp(blockname,"GRID_Z") == 0)
        {
            for(int i=0;i<GRID_Z_BUFFERS;i++)
            {
                fread(m_ZBuffer[i],sizeof(float),m_dwSizeZ,f);
            }
        }
        else if (strcmp(blockname,"GRID_ALPHAMAP") == 0)
        {
            fread(m_Alphamap,sizeof(int),m_dwSizeZ,f);
        }
        else if (strcmp(blockname,"GRID_FACE") == 0)
        {
            fread(m_Face,sizeof(S_FACEDATAEX),m_dwSizeFace,f);

            // Orientate tris
            for(k=0;k<m_dwSizeFace;k++)
            {
                fi = k % m_dwCellsX;
                fj = k / m_dwCellsX;
                face = GetFace(fi,fj);
                FaceOrientateTris(fi,fj,(face->flags & FACE_FL_TRISINV)==FACE_FL_TRISINV);
            }

            // Excluded triangles
            pair = true;
            ScanTrisSetup();
            while (ScanTris(t))
            {
                face = (S_FACEDATAEX*)t->GetFacePtr();

                if (pair)
                {
                    if ((face->flags & FACE_FL_EXTRIS0)==FACE_FL_EXTRIS0)
                        t->SetExcluded(true);
                    else
                        t->SetExcluded(false);
                }
                else
                {
                    if ((face->flags & FACE_FL_EXTRIS1)==FACE_FL_EXTRIS1)
                        t->SetExcluded(true);
                    else
                        t->SetExcluded(false);
                }

                pair = !pair;
            }//while
        }//FACE
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

bool CGrid::FileLoad_1_3(const char *filename)
{
	std::ifstream    f;
	S_EG2BLOCKHEADER block;
	int              cells[2]  = { -1, -1 };
	float            widths[2] = { -1, -1 };
	int              fi, fj, k;
	S_FACEDATAEX     *face;
	CTris            *t;
	char             tris_prop;

	f.open(filename, std::ios::in | std::ios::binary);

	if (!f.is_open())
	{
		return false;
	}

	f.ignore(sizeof(S_EASYGENHEADER));

	while (1)
	{
		f.read((char *)&block, sizeof(S_EG2BLOCKHEADER));

		if (f.gcount() == 0)
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

		if (strcmp(block.name, "GRID_CELLS") == 0)
		{
			f.read((char *)cells, sizeof(cells));
		}
		else if (strcmp(block.name, "GRID_WIDTHS") == 0)
		{
			f.read((char *)widths, sizeof(widths));

			if ((cells[0] != -1) && (cells[1] != -1) && (widths[0] != -1) && (widths[1] != -1))
			{
				Set(cells[0], cells[1], widths[0], widths[1]);
			}
			else
			{
				TRACE("Bad file format. Couldn't load the Grid");
				return false;
			}
		}
		else if (strcmp(block.name, "GRID_ORIGIN") == 0)
		{
			f.read((char *)&m_vOrigin.x, sizeof(m_vOrigin.x));
			f.read((char *)&m_vOrigin.y, sizeof(m_vOrigin.y));
			f.read((char *)&m_vOrigin.z, sizeof(m_vOrigin.z));
		}
		else if (strcmp(block.name, "GRID_Z") == 0)
		{
			f.read((char *)m_ZBuffer[GRID_Z_REAL], sizeof(float) * m_dwSizeZ);
		}
		else if (strcmp(block.name, "GRID_ALPHAMAP") == 0)
		{
			f.read((char *)m_Alphamap, sizeof(int) * m_dwSizeZ);
		}
		else if (strcmp(block.name, "GRID_FACE") == 0)
		{
			f.read((char *)m_Face, sizeof(S_FACEDATAEX) * m_dwSizeFace);

			// Orientate tris
			for (k = 0; k < m_dwSizeFace; k++)
			{
				fi   = k % m_dwCellsX;
				fj   = k / m_dwCellsX;
				face = GetFace(fi, fj);
				FaceOrientateTris(fi, fj, bool(face->flags & FACE_FL_TRISINV));
			}
		} //FACE
		else if (strcmp(block.name, "GRID_TRISPROPERTIES") == 0)
		{
			ScanTrisSetup();
			while (ScanTris(t))
			{
				f.read((char *)&tris_prop, sizeof(tris_prop));

				TRACE("%c\n", tris_prop);

				t->m_bExcluded = bool(tris_prop & 0x01);
			}
		} //TRISPROPERTIES
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

bool CGrid::FileLoad_1_4(const char *filename)
{
	std::ifstream    f;
	S_EG2BLOCKHEADER block;
	int              cells[2]  = { -1, -1 };
	float            widths[2] = { -1, -1 };
	int              fi, fj, k;
	S_FACEDATAEX     *face;
	CTris            *t;
	char             tris_prop;

	f.open(filename, std::ios::in | std::ios::binary);

	if (!f.is_open())
	{
		return false;
	}

	f.ignore(sizeof(S_EASYGENHEADER));

	while (1)
	{
		f.read((char *)&block, sizeof(S_EG2BLOCKHEADER));

		if (f.gcount() == 0)
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

		if (strcmp(block.name, "GRID") == 0)
		{
			f.read((char *)cells, sizeof(cells));
			f.read((char *)widths, sizeof(widths));
			if ((cells[0] != -1) && (cells[1] != -1) && (widths[0] != -1) && (widths[1] != -1))
			{
				Set(cells[0], cells[1], widths[0], widths[1]);
			}
			else
			{
				TRACE("Bad file format. Couldn't load the Grid");
				return false;
			}
			f.read((char *)&m_vOrigin.x, sizeof(m_vOrigin.x));
			f.read((char *)&m_vOrigin.y, sizeof(m_vOrigin.y));
			f.read((char *)&m_vOrigin.z, sizeof(m_vOrigin.z));
			f.read((char *)m_ZBuffer[GRID_Z_REAL], sizeof(float) * m_dwSizeZ);
			f.read((char *)m_Alphamap, sizeof(int) * m_dwSizeZ);
			f.read((char *)m_Face, sizeof(S_FACEDATAEX) * m_dwSizeFace);

			// Orientate tris
			for (k = 0; k < m_dwSizeFace; k++)
			{
				fi   = k % m_dwCellsX;
				fj   = k / m_dwCellsX;
				face = GetFace(fi, fj);
				FaceOrientateTris(fi, fj, bool(face->flags & FACE_FL_TRISINV));
			}

			// Triangles properties
			ScanTrisSetup();
			while (ScanTris(t))
			{
				f.read((char *)&tris_prop, sizeof(tris_prop));

				TRACE("%c\n", tris_prop);

				t->m_bExcluded = bool(tris_prop & 0x01);
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

// eye: origin of ray
// ray: normalized ray direction
//
bool CGrid::GetDistanceTrisRay(CNmVec3& eye, CNmVec3& ray, CNmVec3& risu, float& distance)
{
	CTris   *hit_tris, t_tris;
	CNmVec3 hit_risu, t_risu;
	float   hit_dist, t_dist;
	CTris   *t;
	CNmVec3 a, b, c, ba, cb, ac, ain, bin, cin;
	S_PLANE plane;

	hit_tris = 0;

	ScanTrisSetup();
	while (ScanTris(t))
	{
		UpdateTrisZ(t);

		a.Set(*t->m_vx[0], *t->m_vy[0], t->m_vz[0]);
		b.Set(*t->m_vx[1], *t->m_vy[1], t->m_vz[1]);
		c.Set(*t->m_vx[2], *t->m_vy[2], t->m_vz[2]);

		plane.n = (c - a) ^ (b - a);
		plane.n.Normalize();
		plane.d = plane.n * a;

		if ((plane.n * ray) < 0)
		{
			continue;
		}

		// calc point intersection / distance
		if (u3d_PlaneRayIntersection(&plane, &eye, &ray, &t_risu, &t_dist))
		{
			// is in front ?
			if (t_dist > 0)
			{
				ba = b - a;
				cb = c - b;
				ac = a - c;

				ain = ba ^ plane.n;
				bin = cb ^ plane.n;
				cin = ac ^ plane.n;

				// is inside?
				if ((((t_risu - a) * ain) > 0) && (((t_risu - b) * bin) > 0) && (((t_risu - c) * cin) > 0))
				{
					if (((hit_tris) && (t_dist < hit_dist)) ||
					    (!hit_tris))
					{
						hit_tris = t;
						hit_dist = t_dist;
						hit_risu = t_risu;
					}
				} // inside

			} //in front
		}
	}

	if (!hit_tris)
	{
		return false;
	}

	// hit_tris is available too...
	risu     = hit_risu;
	distance = hit_dist;

	return true;
}

S_FACEDATAEX *CGrid::GetFace(int fi, int fj)
{
	return &m_Face[fi + fj * m_dwCellsX];
}

S_FACEDATAEX *CGrid::GetFace(int fk)
{
	return &m_Face[fk];
}

void CGrid::ResetDefault()
{
	Set(START_CELLSX, START_CELLSY, START_WIDTHX, START_WIDTHY);

	SelectZBuf(GRID_Z_REAL);

	OptionFlatTrisSet(false);
	OptionSlopeTrisSet(false);
	OptionExcludedTrisSet(false);

	for (int k = 0; k < m_dwSizeZ; k++)
	{
		SetAlphaIndex(k, 0);
	}

	m_TextureStretchFactor = 1;
	m_vOrigin.Set(0.f, 0.f, 0.f);
}

////////////////
////////////////

CGridStack::CGridStack()
{
	m_dwSize   = 1;
	m_dwPos    = 0;
	m_Stack    = new CGrid *[1];
	m_Stack[0] = NULL;
}

CGridStack::CGridStack(int dwSize)
{
	m_dwSize = dwSize;
	m_dwPos  = 0;
	m_Stack  = new CGrid *[dwSize];
	for (int i = 0; i < dwSize; i++)
		m_Stack[i] = NULL;
}

CGridStack::~CGridStack()
{
	Reset();
	delete[] m_Stack;
}

bool CGridStack::IsEmpty()
{
	return m_dwPos == 0;
}

void CGridStack::Push(CGrid *grid)
{
	int i;

	if (m_dwPos == m_dwSize)
	{
		// so fuori
		delete m_Stack[0];

		for (i = 0; i < m_dwSize - 1; i++)
		{
			m_Stack[i] = m_Stack[i + 1];
		}

		m_dwPos--;

		m_Stack[m_dwPos] = NULL;
	}

	m_Stack[m_dwPos]  = new CGrid;
	*m_Stack[m_dwPos] = *grid;

	m_dwPos++;
}

void CGridStack::Pop(CGrid *grid)
{
	if (m_dwPos == 0)
	{
		return;
	}

	m_dwPos--;

	*grid = *m_Stack[m_dwPos];

	delete m_Stack[m_dwPos];
	m_Stack[m_dwPos] = NULL;
}

void CGridStack::Reset()
{
	int i;

	for (i = 0; i < m_dwSize; i++)
	{
		if (m_Stack[i] != NULL)
		{
			delete m_Stack[i];
			m_Stack[i] = NULL;
		}
	}

	m_dwPos = 0;
}

void CGridStack::SetSize(int dwSize)
{
	Reset();
	delete[] m_Stack;

	m_dwSize = dwSize;
	m_dwPos  = 0;
	m_Stack  = new CGrid *[dwSize];
	for (int i = 0; i < dwSize; i++)
		m_Stack[i] = NULL;
}

void CGrid::GetSTGen(int &sGen, int &tGen)
{
	if (m_TextureStretchFactor == 0)
	{
		sGen = (int)GetCellWidthX();
		tGen = (int)GetCellWidthY();
	}
	else if (m_TextureStretchFactor >= 1)
	{
		sGen = (int)(GetCellWidthX() * m_TextureStretchFactor);
		tGen = (int)(GetCellWidthY() * m_TextureStretchFactor);
	}
	else if (m_TextureStretchFactor < -1)
	{
		sGen = (int)fabs(GetCellWidthX() / m_TextureStretchFactor);
		tGen = (int)fabs(GetCellWidthY() / m_TextureStretchFactor);
	}
}
/*
TextureScaleUP()
*/
int CGrid::TextureScaleUP()
{
	m_TextureStretchFactor++;

	if (m_TextureStretchFactor == -1)
	{
		m_TextureStretchFactor = 1;
	}

	TextureStretch(m_TextureStretchFactor);

	return m_TextureStretchFactor;
}

int CGrid::TextureScaleDN()
{
	m_TextureStretchFactor--;

	if (m_TextureStretchFactor == 0)
	{
		m_TextureStretchFactor = -2;
	}

	TextureStretch(m_TextureStretchFactor);

	return m_TextureStretchFactor;
}

int CGrid::TextureScaleFit()
{
	m_TextureStretchFactor = 1;

	TextureStretch(m_TextureStretchFactor);

	return m_TextureStretchFactor;
}


void CGrid::TextureScaleRefresh()
{
	TextureStretch(m_TextureStretchFactor);
}
