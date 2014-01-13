// Md3.h: interface for the CMd3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD3_H__6D9355EE_59DC_4E19_888E_E6EC0C58F9C6__INCLUDED_)
#define AFX_MD3_H__6D9355EE_59DC_4E19_888E_E6EC0C58F9C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gl\gl.h>
#include <gl\glu.h>

#include "NmStr.h"
#include "NmList.h"
#include "NmVec3.h"

////////////////////////////////////////////////
// Md3
////////////////////////////////////////////////

typedef struct
{
	char ID[4];
	int Version;
	char Filename[68];
	int numBoneFrames;
	int numTags;
	int numMeshes;
	int numMaxSkins;
	int headerLength;
	int TagStart;
	int TagEnd;
	int FileSize;
} S_MD3_HEADER;

typedef struct
{
	float mins[3];
	float maxs[3];
	float Position[3];
	float Scale;
	char Creator[16];
} S_MD3_BONEFRAME;

typedef struct
{
	int FirstFrame;
	int numFrames;
	int LoopingFrames;
	int FPS;
} S_MD3_ANIM;

typedef float S_MD3_ROTATIONMATRIX[3][3];

typedef float S_MD3_VECTOR[3];

typedef struct
{
	char Name[64];
	S_MD3_VECTOR Position;
	S_MD3_ROTATIONMATRIX Rotation;
} S_MD3_TAG;

typedef struct
{
	int Vertex[3];
} S_MD3_TRIANGLE;

typedef struct
{
	float Coord[2];
} S_MD3_TEXCOORD;

typedef struct
{
	signed short Vertex[3];
	unsigned char Normal[2];
} S_MD3_VERTEX;

typedef struct
{
	char ID[4];
	char Name[68];
	int numMeshFrames;
	int numSkins;
	int numVertexes;
	int numTriangles;
	int triStart;
	int headerSize;
	int TexVectorStart;
	int VertexStart;
	int MeshSize;
} S_MESH_HEADER;

typedef char S_MESH_SKIN[68];

typedef struct
{
	S_MESH_HEADER MeshHeader;
	S_MESH_SKIN *Skins;
	S_MD3_TRIANGLE *Triangle;
	S_MD3_TEXCOORD *TexCoord;
	S_MD3_VERTEX *Vertex;
	GLuint Texture;
	bool SetTexture;
} S_MESH;

class CMd3
{
public:
	S_MD3_HEADER    m_Header;
	S_MD3_BONEFRAME *m_BoneFrames;
	S_MD3_TAG       *m_Tags;
	S_MESH          *m_Meshes;
	CMd3            **m_Links;

	int m_startFrame;
	int m_endFrame;

	// Ezgen stuff
	CNmStr  m_sName;                // fullpath name of file (md3 included)
	CNmVec3 m_vMin, m_vMax;         // mesh[0] bounding box (relative)
	CNmVec3 m_vRay;                 // x,y,z Widths
	CNmVec3 m_vOrigin;              // Origin

	// List stuff
	CMd3 *m_pNext;
	CMd3 *m_pPrev;

/*
    int		m_frame;
    int		nextFrame;
    int		FPS;
    GLfloat	Poll;
    GLfloat	LastUpdate;
    int		TexNr;
    int		TexInf[100];
*/
	bool LoadModel(const char *filename);
	void glDraw(float, float, float);
//	void DrawModel();
//	void DrawSkeleton(CMd3 TMD3Model);
//	void UpdateFrame(GLfloat glFLoat);
//	void LinkModel( const char* tagname, CMd3 MD3Model);
//	void LoadSkin( const char* Imagepath, const char* filename);

	CMd3();
	virtual ~CMd3();
private:
	void    CalcStuff();
};

void Md3_InitNormals();

////////////////////////////////////////////////
// Md3 List
////////////////////////////////////////////////

class CMd3List
{
public:
	CMd3List();
	virtual ~CMd3List();

	CMd3 *Append();

	CMd3 *Get(const char *name);
	void    RemoveAll();

	void    ScanReset();
	bool    Scan(CMd3 *& item);

private:
	CMd3 *_AddHead();
	CMd3 *_AddTail();
	CMd3 *_AddAfter(CMd3 *item);
	CMd3 *_AddBefore(CMd3 *item);
	bool  _Remove(CMd3 *item);

	CMd3 *m_Head;
	CMd3 *m_Tail;
	CMd3 *m_Scan;
};

////////////////////////////////////////////////
// Md3 List Ptr
////////////////////////////////////////////////

typedef struct S_MD3PTR
{
	CMd3 *pMd3;
	CNmVec3 m_egvPos;           // EasyGen position over the mesh

	S_MD3PTR *next;
	S_MD3PTR *prev;
} S_MD3PTR;

class CMd3ListPtr
{
public:
	CMd3ListPtr();
	virtual ~CMd3ListPtr();

	S_MD3PTR *Append();

	void    RemoveAll();

	void    ScanReset();
	bool    Scan(S_MD3PTR *& item);
	bool    Remove(S_MD3PTR *item);

private:
	S_MD3PTR *_AddHead();
	S_MD3PTR *_AddTail();
	S_MD3PTR *_AddAfter(S_MD3PTR *item);
	S_MD3PTR *_AddBefore(S_MD3PTR *item);

	S_MD3PTR *m_Head;
	S_MD3PTR *m_Tail;
	S_MD3PTR *m_Scan;
};

////////////////////////////////////////////////
// Md3 Manager
////////////////////////////////////////////////

class CMd3Man
{
public:
	CMd3Man();
	virtual ~CMd3Man();

	bool    LoadModels(const char *moddir);
	void    ResetNewMap();
	bool    GetByRay(CNmVec3& eye, CNmVec3& ray, S_MD3PTR *& risu, float& distance);
	void    SavePlaced(const char *filename);   // old
	bool    LoadPlaced(const char *filename);   // old

	bool    FileLoad_1_3(const char *filename);     // m_Placed
	void    FileSave_1_3(const char *filename);     // m_Placed
	bool    FileLoad_1_4(const char *filename);     // m_Placed
	void    FileSave_1_4(const char *filename);     // m_Placed

	bool    IsPlaced(CMd3 *md3);
	bool    IsPlaced(const char *md3name);

	CMd3        *m_lbSelected; // md3 selected from the list box
	CMd3List    m_List;     // md3 loaded (all)
	CMd3ListPtr m_Placed;   // md3 placed over the mesh (ptr)
	CNmListStr  m_NamesList; // md3 filenames list (all)
private:
	int     SearchFiles(const char *moddir);
	void    SearchFilesRec(const char *dir);
};

#endif // !defined(AFX_MD3_H__6D9355EE_59DC_4E19_888E_E6EC0C58F9C6__INCLUDED_)
