// Md3.cpp: implementation of the CMd3 class.
//
//////////////////////////////////////////////////////////////////////
#include "EasyGen.h"
#include "Md3.h"
#include <fstream>
////////
#include <stdio.h>
#include <math.h>
#include <GL\gl.h>
#include <GL\glu.h>
////////
#include "Nemo3D.h"
#include "NmParser.h"

// extern
#include "Preferences.h"
extern CPreferences g_Prefs;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

float anorms[256][256][3];

void Md3_InitNormals()
{
	int   i, j;
	float alpha, beta;
	float pi = 3.1415926535f;

	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			alpha           = 2 * i * pi / 255;
			beta            = 2 * j * pi / 255;
			anorms[i][j][0] = cos(beta) * sin(alpha);
			anorms[i][j][1] = sin(beta) * sin(alpha);
			anorms[i][j][2] = cos(alpha);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMd3::CMd3()
{
	m_BoneFrames = 0;
	m_Meshes     = 0;
	m_Tags       = 0;
	m_Links      = 0;
}

CMd3::~CMd3()
{
	int i;

	if (m_BoneFrames)
	{
		delete[] m_BoneFrames;
	}

	if (m_Meshes)
	{
		for (i = 0; i < m_Header.numMeshes; i++)
		{
			delete[] m_Meshes[i].Skins;
			delete[] m_Meshes[i].Triangle;
			delete[] m_Meshes[i].TexCoord;
			delete[] m_Meshes[i].Vertex;
		}
		delete[] m_Meshes;
	}

	if (m_Tags)
	{
		delete[] m_Tags;
	}

	if (m_Links)
	{
		delete[] m_Links;
	}
}

// use after md3 has benn loaded
void CMd3::CalcStuff()
{
	float v[3];

	m_vMin.Set(10000, 10000, 10000);
	m_vMax.Set(-10000, -10000, -10000);

	for (int m = 0; m < m_Header.numMeshes; m++)
	{
		for (int i = 0; i < m_Meshes[m].MeshHeader.numVertexes; i++)
		{
			v[0] = (float)m_Meshes[m].Vertex[i].Vertex[0] / 64.f;
			v[1] = (float)m_Meshes[m].Vertex[i].Vertex[1] / 64.f;
			v[2] = (float)m_Meshes[m].Vertex[i].Vertex[2] / 64.f;

			if (v[0] < m_vMin.x)
			{
				m_vMin.x = v[0];
			}
			if (v[1] < m_vMin.y)
			{
				m_vMin.y = v[1];
			}
			if (v[2] < m_vMin.z)
			{
				m_vMin.z = v[2];
			}

			if (v[0] > m_vMax.x)
			{
				m_vMax.x = v[0];
			}
			if (v[1] > m_vMax.y)
			{
				m_vMax.y = v[1];
			}
			if (v[2] > m_vMax.z)
			{
				m_vMax.z = v[2];
			}
		}
	}

	m_vOrigin = (m_vMax + m_vMin) / 2.f;
	m_vRay    = m_vMax - m_vMin;
}

bool CMd3::LoadModel(const char *filename)
{
	FILE *f;
	int  i;
	int  MeshOffset;

	f = fopen(filename, "rb");

	if (!f)
	{
		return false;
	}

	m_sName = filename;
	m_sName.MakeLower();

	fread(&m_Header, sizeof(m_Header), 1, f);

	if ((m_Header.ID[0] != 'I') || (m_Header.Version != 15))
	{
		fclose(f);
		return false;
	}

	// read boneframes
	m_BoneFrames = new S_MD3_BONEFRAME[m_Header.numBoneFrames];
	fread(m_BoneFrames, sizeof(S_MD3_BONEFRAME), m_Header.numBoneFrames, f);

	// read tags
	m_Tags = new S_MD3_TAG[m_Header.numBoneFrames * m_Header.numTags];
	fread(m_Tags, sizeof(S_MD3_TAG), m_Header.numBoneFrames * m_Header.numTags, f);

	// init links
	m_Links = new CMd3 *[m_Header.numTags];
	for (i = 0; i < m_Header.numTags; i++)
	{
		m_Links[i] = 0;
	}

	// read meshes
	m_Meshes = new S_MESH[m_Header.numMeshes];

	MeshOffset = ftell(f);

	for (i = 0; i < m_Header.numMeshes; i++)
	{
		fseek(f, MeshOffset, 0);
		fread(&m_Meshes[i].MeshHeader, sizeof(S_MESH_HEADER), 1, f);

		// Load the Skins
		m_Meshes[i].Skins = new S_MESH_SKIN[m_Meshes[i].MeshHeader.numSkins];
		fread(m_Meshes[i].Skins, 68, m_Meshes[i].MeshHeader.numSkins, f);

		// Triangles
		fseek(f, MeshOffset + m_Meshes[i].MeshHeader.triStart, 0);
		m_Meshes[i].Triangle = new S_MD3_TRIANGLE[m_Meshes[i].MeshHeader.numTriangles];
		fread(m_Meshes[i].Triangle, sizeof(S_MD3_TRIANGLE), m_Meshes[i].MeshHeader.numTriangles, f);

		// Texture Coordiantes
		fseek(f, MeshOffset + m_Meshes[i].MeshHeader.TexVectorStart, 0);
		m_Meshes[i].TexCoord = new S_MD3_TEXCOORD[m_Meshes[i].MeshHeader.numVertexes];
		fread(m_Meshes[i].TexCoord, sizeof(S_MD3_TEXCOORD), m_Meshes[i].MeshHeader.numVertexes, f);

		// Vertices
		fseek(f, MeshOffset + m_Meshes[i].MeshHeader.VertexStart, 0);
		m_Meshes[i].Vertex = new S_MD3_VERTEX[m_Meshes[i].MeshHeader.numVertexes * m_Meshes[i].MeshHeader.numMeshFrames];
		fread(m_Meshes[i].Vertex, sizeof(S_MD3_VERTEX), m_Meshes[i].MeshHeader.numVertexes * m_Meshes[i].MeshHeader.numMeshFrames, f);

		MeshOffset += m_Meshes[i].MeshHeader.MeshSize;
	}

	fclose(f);

	// set the start, end frame
	m_Header.numBoneFrames--;
	m_startFrame = 0;
	m_endFrame   = m_Header.numBoneFrames;

	CalcStuff();

//	CNmStr s;
//	s.Format("%f %f %f", m_vMax.x - m_vMin.x, m_vMax.y - m_vMin.y, m_vMax.z - m_vMin.z );
//	s.TestMsg();

	return true;
}


void CMd3::glDraw(float x, float y, float z)
{
	int   i, j;
	int   triangleNum, currentFrame, currentMesh, currentOffsetVertex, currentVertex, indexVertex;
	int   normU, normV;
	float s, t;
	float v[3];
	float n[3];

	glPushMatrix();
	glTranslatef(x, y, z);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(.8f, .8f, .8f);

	currentMesh  = 0;
	currentFrame = 0;

	for (currentMesh = 0; currentMesh < m_Header.numMeshes; currentMesh++)
	{
		currentOffsetVertex = currentFrame * m_Meshes[currentMesh].MeshHeader.numVertexes;
		triangleNum         = m_Meshes[currentMesh].MeshHeader.numTriangles;

//	if (Meshes[currentMesh].settexture)
//	{
//		glBindTexture(GL_TEXTURE_2D, Meshes[currentMesh].texture);
//	}
		for (i = 0; i < triangleNum; i++)
		{
			glBegin(GL_TRIANGLES);

			for (j = 2; j >= 0; j--)
			{
				currentVertex = m_Meshes[currentMesh].Triangle[i].Vertex[j];

				indexVertex = currentOffsetVertex + currentVertex;

				v[0] = (float)m_Meshes[currentMesh].Vertex[indexVertex].Vertex[0] / 64.f;
				v[1] = (float)m_Meshes[currentMesh].Vertex[indexVertex].Vertex[1] / 64.f;
				v[2] = (float)m_Meshes[currentMesh].Vertex[indexVertex].Vertex[2] / 64.f;

				normU = m_Meshes[currentMesh].Vertex[indexVertex].Normal[0];
				normV = m_Meshes[currentMesh].Vertex[indexVertex].Normal[1];

				n[0] = anorms[normU][normV][0];
				n[1] = anorms[normU][normV][1];
				n[2] = anorms[normU][normV][2];

				s = m_Meshes[currentMesh].TexCoord[currentVertex].Coord[0];
				t = m_Meshes[currentMesh].TexCoord[currentVertex].Coord[1];

				glTexCoord2f(s, 1 - t);

				glNormal3f(n[0], n[1], n[2]);
				glVertex3f(v[0], v[1], v[2]);
			}

			glEnd();
		}
	}

	glPopMatrix();
}

///////////////////////////////////////////////////////////////
// CMd3List
///////////////////////////////////////////////////////////////

CMd3List::CMd3List()
{
	m_Head = 0;
	m_Tail = 0;
}

CMd3List::~CMd3List()
{
	RemoveAll();
}

CMd3 *CMd3List::Get(const char *name)
{
	CMd3 *item;

	ScanReset();
	while (Scan(item))
	{
		if (item->m_sName == name)
		{
			return item;
		}
	}

	return 0;
}

void CMd3List::RemoveAll()
{
	while (m_Head)
		_Remove(m_Head);
}

void CMd3List::ScanReset()
{
	m_Scan = m_Head;
}

bool CMd3List::Scan(CMd3 *& item)
{
	bool res = false;

	if (m_Scan)
	{
		res    = true;
		item   = m_Scan;
		m_Scan = m_Scan->m_pNext;
	}

	return res;
}

CMd3 *CMd3List::Append()
{
	return _AddTail();
}

CMd3 *CMd3List::_AddHead()
{
	CMd3 *nitem = new CMd3;

	if (m_Head == 0)
	{
		nitem->m_pNext = 0;
		nitem->m_pPrev = 0;

		m_Head = nitem;
		m_Tail = nitem;

		return nitem;
	}

	nitem->m_pNext = m_Head;
	nitem->m_pPrev = 0;

	m_Head->m_pPrev = nitem;
	m_Head          = nitem;

	return nitem;
}

CMd3 *CMd3List::_AddTail()
{
	if (m_Tail == 0)
	{
		return _AddHead();
	}

	CMd3 *nitem = new CMd3;

	nitem->m_pNext = 0;
	nitem->m_pPrev = m_Tail;

	m_Tail->m_pNext = nitem;
	m_Tail          = nitem;

	return nitem;
}

CMd3 *CMd3List::_AddAfter(CMd3 *item)
{
	if (item == m_Tail)
	{
		return _AddTail();
	}

	CMd3 *nitem = new CMd3;

	nitem->m_pNext = item->m_pNext;
	nitem->m_pPrev = item;

	item->m_pNext->m_pPrev = nitem;
	item->m_pNext          = nitem;

	return nitem;
}

CMd3 *CMd3List::_AddBefore(CMd3 *item)
{
	if (item == m_Head)
	{
		return _AddHead();
	}

	CMd3 *nitem = new CMd3;

	nitem->m_pNext = item;
	nitem->m_pPrev = item->m_pPrev;

	item->m_pPrev->m_pNext = nitem;
	item->m_pPrev          = nitem;

	return nitem;
}

bool CMd3List::_Remove(CMd3 *item)
{
	if (item == 0)
	{
		return false;
	}

	if ((m_Head == m_Tail) && (m_Head == item))
	{
		m_Head = 0;
		m_Tail = 0;
	}
	else if (item == m_Head)
	{
		m_Head          = m_Head->m_pNext;
		m_Head->m_pPrev = 0;
	}
	else if (item == m_Tail)
	{
		m_Tail          = m_Tail->m_pPrev;
		m_Tail->m_pNext = 0;
	}
	else    // generic
	{
		item->m_pNext->m_pPrev = item->m_pPrev;
		item->m_pPrev->m_pNext = item->m_pNext;
	}

	delete item;

	return true;
}

///////////////////////////////////////////////////////////////
// CMd3ListPtr
///////////////////////////////////////////////////////////////

CMd3ListPtr::CMd3ListPtr()
{
	m_Head = 0;
	m_Tail = 0;
}

CMd3ListPtr::~CMd3ListPtr()
{
	RemoveAll();
}

void CMd3ListPtr::RemoveAll()
{
	while (m_Head)
		Remove(m_Head);
}

void CMd3ListPtr::ScanReset()
{
	m_Scan = m_Head;
//	m_Scan = m_Tail;
}

bool CMd3ListPtr::Scan(S_MD3PTR *& item)
{
	bool res = false;

	if (m_Scan)
	{
		res    = true;
		item   = m_Scan;
		m_Scan = m_Scan->next;
	}
/*
    if (m_Scan)
    {
        res = true;
        item = m_Scan;
        m_Scan = m_Scan->prev;
    }
*/
	return res;
}

S_MD3PTR *CMd3ListPtr::Append()
{
	return _AddTail();
}

S_MD3PTR *CMd3ListPtr::_AddHead()
{
	S_MD3PTR *nitem = new S_MD3PTR;

	if (m_Head == 0)
	{
		nitem->next = 0;
		nitem->prev = 0;

		m_Head = nitem;
		m_Tail = nitem;

		return nitem;
	}

	nitem->next = m_Head;
	nitem->prev = 0;

	m_Head->prev = nitem;
	m_Head       = nitem;

	return nitem;
}

S_MD3PTR *CMd3ListPtr::_AddTail()
{
	if (m_Tail == 0)
	{
		return _AddHead();
	}

	S_MD3PTR *nitem = new S_MD3PTR;

	nitem->next = 0;
	nitem->prev = m_Tail;

	m_Tail->next = nitem;
	m_Tail       = nitem;

	return nitem;
}

S_MD3PTR *CMd3ListPtr::_AddAfter(S_MD3PTR *item)
{
	if (item == m_Tail)
	{
		return _AddTail();
	}

	S_MD3PTR *nitem = new S_MD3PTR;

	nitem->next = item->next;
	nitem->prev = item;

	item->next->prev = nitem;
	item->next       = nitem;

	return nitem;
}

S_MD3PTR *CMd3ListPtr::_AddBefore(S_MD3PTR *item)
{
	if (item == m_Head)
	{
		return _AddHead();
	}

	S_MD3PTR *nitem = new S_MD3PTR;

	nitem->next = item;
	nitem->prev = item->prev;

	item->prev->next = nitem;
	item->prev       = nitem;

	return nitem;
}

bool CMd3ListPtr::Remove(S_MD3PTR *item)
{
	if (item == 0)
	{
		return false;
	}

	if ((m_Head == m_Tail) && (m_Head == item))
	{
		m_Head = 0;
		m_Tail = 0;
	}
	else if (item == m_Head)
	{
		m_Head       = m_Head->next;
		m_Head->prev = 0;
	}
	else if (item == m_Tail)
	{
		m_Tail       = m_Tail->prev;
		m_Tail->next = 0;
	}
	else    // generic
	{
		item->next->prev = item->prev;
		item->prev->next = item->next;
	}

	delete item;

	return true;
}

///////////////////////////////////////////////////////////////
// CMd3Man
///////////////////////////////////////////////////////////////

CMd3Man::CMd3Man()
{
	m_lbSelected = 0;
}

CMd3Man::~CMd3Man()
{
	m_List.RemoveAll();
}

bool CMd3Man::FileLoad_1_3(const char *filename)
{
	std::ifstream    f;
	S_EG2BLOCKHEADER block;

	S_MD3PTR *item;
	CNmStr   s, relname;
	char     relnamepchr[255];
	int      k, count;
	float    x, y, z;

	CMd3   *md3;
	CNmStr name;

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

		if (strcmp(block.name, "MODELS") == 0)
		{
			f.read((char *)&count, sizeof(count));

			for (k = 0; k < count; k++)
			{
				f.read(relnamepchr, sizeof(relnamepchr));
				f.read((char *)&x, sizeof(x));
				f.read((char *)&y, sizeof(y));
				f.read((char *)&z, sizeof(z));

				name  = g_Prefs.m_GameBasePath;
				name += "\\models\\";
				name += relnamepchr;

				md3 = m_List.Get(name);

				if (md3)
				{
					item       = m_Placed.Append();
					item->pMd3 = md3;

					item->m_egvPos.x = x;
					item->m_egvPos.y = y;
					item->m_egvPos.z = z;
				}
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

bool CMd3Man::FileLoad_1_4(const char *filename)
{
	return FileLoad_1_3(filename);
}

void CMd3Man::FileSave_1_3(const char *filename)
{
	std::ofstream    f;
	S_EG2BLOCKHEADER block;

	S_MD3PTR *item;
	CNmStr   s, relname;
	char     relnamepchr[255];
	int      pos;
	int      count;

	f.open(filename, std::ios::out | std::ios::binary | std::ios::app);

	// count items.........
	count = 0;
	m_Placed.ScanReset();
	while (m_Placed.Scan(item))
	{
		count++;
	}

	// Models placed
	ZeroMemory(&block, sizeof(block));
	strcpy(block.name, "MODELS");
	block.size = sizeof(count) + count * (sizeof(relnamepchr) + sizeof(float) * 3);
	f.write((char *)&block, sizeof(S_EG2BLOCKHEADER));

	pos = g_Prefs.m_GameBasePath.GetLength() + strlen(_T("\\models\\"));

	f.write((char *)&count, sizeof(count));

	m_Placed.ScanReset();
	while (m_Placed.Scan(item))
	{
		relname = item->pMd3->m_sName;
		relname.Delete(0, pos);

		strcpy(relnamepchr, (const char *)relname);

		f.write(relnamepchr, sizeof(relnamepchr));
		f.write((char *)&item->m_egvPos.x, sizeof(item->m_egvPos.x));
		f.write((char *)&item->m_egvPos.y, sizeof(item->m_egvPos.y));
		f.write((char *)&item->m_egvPos.z, sizeof(item->m_egvPos.z));
	}

	f.close();
}

void CMd3Man::FileSave_1_4(const char *filename)
{
	FileSave_1_3(filename);
}

int CMd3Man::SearchFiles(const char *moddir)
{
	CNmStr pathstart = moddir;

	if ((pathstart[pathstart.GetLength() - 1] != '\\') || (pathstart[pathstart.GetLength() - 1] != '/'))
	{
		pathstart += "\\";
	}

	pathstart += "models\\";

	SearchFilesRec(pathstart);

	return m_NamesList.GetNum();
}

// Fill in m_NamesList (!!!)
//
// dir = "c:\\quake3\\baseq3\\models\\room\\"
//
void CMd3Man::SearchFilesRec(const char *dir)
{
	CNmStr folder = dir;
	CNmStr mask;
	CNmStr filename, ext, subfolder;

	mask = folder + "*.*";

	WIN32_FIND_DATA data;
	HANDLE          hFind = FindFirstFile(_T(mask), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (lstrcmp(data.cFileName, _T(".")) == 0 ||
			    lstrcmp(data.cFileName, _T("..")) == 0)
			{
				continue;
			}

			if  (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				subfolder  = folder;
				subfolder += data.cFileName;
				subfolder += "\\";
				// "c:\\...\\models\\mapobjects\\"
				SearchFilesRec(subfolder);
			}
			else
			{
				// "c:\\...\\models\\mapobjects\\bath.md3"
				filename = folder + data.cFileName;

				ext = filename.Right(3);
				ext.MakeLower();

				if (ext == "md3")
				{
					m_NamesList.Append(filename);
				}
			}

		}
		while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

bool CMd3Man::GetByRay(CNmVec3& eye, CNmVec3& ray, S_MD3PTR *& risu, float& distance)
{
	S_MD3PTR *item;
	S_MD3PTR *hit_md3ptr;
	float    hit_dist, dist;
	CNmVec3  r, pvett, min, max;

	hit_md3ptr = 0;

	m_Placed.ScanReset();
	while (m_Placed.Scan(item))
	{
		r = item->m_egvPos + item->pMd3->m_vOrigin - eye;

		// is in front?
		if ((r * ray) > 0)
		{
//			pvett = r^ray;
//			f = pvett.GetLength();

			min = item->m_egvPos + item->pMd3->m_vMin;
			max = item->m_egvPos + item->pMd3->m_vMax;

			// hit
//			if (f<64.f)
			if (u3d_BoxRayIntersection(min, max, eye, ray, pvett, dist))
			{
				// is behind another one
				if (((hit_md3ptr) && (r.GetLength() < hit_dist)) ||
				    (!hit_md3ptr))
				{
					hit_md3ptr = item;
					hit_dist   = r.GetLength();
				}
			}
		} // is in front
	}

	if (!hit_md3ptr)
	{
		risu     = 0;
		distance = -1;
		return false;
	}

	risu     = hit_md3ptr;
	distance = hit_dist;

	return true;
}

bool CMd3Man::IsPlaced(CMd3 *md3)
{
	S_MD3PTR *item;

	m_Placed.ScanReset();
	while (m_Placed.Scan(item))
	{
		if (item->pMd3 == md3)
		{
			return true;
		}
	}

	return false;
}

bool CMd3Man::IsPlaced(const char *md3name)
{
	S_MD3PTR *item;

	m_Placed.ScanReset();
	while (m_Placed.Scan(item))
	{
		if (item->pMd3->m_sName == md3name)
		{
			return true;
		}
	}

	return false;
}

bool CMd3Man::LoadModels(const char *moddir)
{
	CNmStr filename;
	CMd3   *md3;

	m_NamesList.RemoveAll();

	if (SearchFiles(moddir) < 1)
	{
		return false;
	}

	m_NamesList.ScanReset();

	while (m_NamesList.Scan(filename))
	{
		md3 = m_List.Append();

		md3->LoadModel(filename);
	}

	return true;
//	m_lsTemp.RemoveAll(); ???
}

void CMd3Man::ResetNewMap()
{
	m_lbSelected = 0;
	m_Placed.RemoveAll();
}

void CMd3Man::SavePlaced(const char *filename)
{
	S_MD3PTR *item;
	FILE     *f;
	CNmStr   s, relname;
	int      pos;

	f = fopen(filename, "wb");

	pos = g_Prefs.m_GameBasePath.GetLength() + strlen(_T("\\models\\"));

	m_Placed.ScanReset();
	while (m_Placed.Scan(item))
	{
		relname = item->pMd3->m_sName;
		relname.Delete(0, pos);

		s.Format("\"%s\" %1.0f %1.0f %1.0f\r\n",
		         relname.GetBuffer(),
		         item->m_egvPos.x,
		         item->m_egvPos.y,
		         item->m_egvPos.z);
		fwrite(s, 1, s.GetLength(), f);
	}

	fclose(f);
}

bool CMd3Man::LoadPlaced(const char *filename)
{
	S_MD3PTR  *item;
	CMd3      *md3;
	CNmParser p;
	CNmStr    name;
	char      *buf = 0;

	if (!p.BufferFromFile(filename))
	{
		return false;
	}

	while (!p.EOP())
	{
		p.XGo('\"');
		p.XString(&buf, '\"');
		p.XGo('\"');

		name  = g_Prefs.m_GameBasePath;
		name += "\\models\\";
		name += buf;

		delete[] buf;
		buf = 0;

		md3 = m_List.Get(name);

		if (md3)
		{
			item       = m_Placed.Append();
			item->pMd3 = md3;

			item->m_egvPos.x = p.XFloat();
			item->m_egvPos.y = p.XFloat();
			item->m_egvPos.z = p.XFloat();
		}

		p.XGo('\n');
	}

	return true;
}
