//////////////
// NEMO3D.H //
//////////////

#ifndef NEMO3D_H
#define NEMO3D_H

#include "NmVec3.h"
#include "NmVec2.h"
#include "NmStr.h"

enum POS { POS_OUT, POS_IN, POS_MID };
/*
typedef int			POS;
#define POS_OUT		0
#define POS_MID		2
#define POS_IN		1
*/
#define PIGRECO     3.1415926535897932384626433832795f
#define PIGRECO_2   1.5707963267948966192313216916398f

// scan of face points
#define SCAN_FACEPOINT(item, stop) for (S_FACEPOINT *item = m_FcPtHead; !stop && item; stop = (item = item->next) == m_FcPtHead)

// stop = false;
// scan of face points
#define SCAN_NM_FACEPOINT(fc, item, stop) for (S_FACEPOINT *item = fc->m_FcPtHead; !stop && item; stop = (item = item->next) == fc->m_FcPtHead)

// inside a member function
#define SCAN_POLY(poly)             for (poly = m_Head; poly; poly = poly->m_Next)

// outside member functions
#define SCAN_NM_POLY(pl, poly)       for (poly = pl.GetHead(); poly ; poly = poly->m_Next)

// outside member functions (pl is ptr)
#define SCAN_NM_PTR_POLY(pl, poly)   for (poly = pl->GetHead(); poly ; poly = poly->m_Next)

// (CNmPolySubList,S_PSLI*) inside member functions
#define SCAN_SPOLY(item)            for (S_PSLI *item = m_Head; item ; item = item->next)

// (CNmPolySubList,S_PSLI*) outside member functions
#define SCAN_NM_SPOLY(psl, item)     for (S_PSLI *item = psl.GetHead(); item ; item = item->next)

#define MAX_FACE_POINTS     256

/*
Example to scan poly stuff:

  {
    CNmPoly*	poly;
    CNmFace*	face;
    bool		stop = false;

    SCAN_NM_POLY(polylist,poly)
    {
        for(face = poly->m_Face; face; face = face->Next )
        {
            SCAN_NM_FACEPOINT(face,item,stop)
            {
            }
        }
    }
}

*/

#define BUILD_OPT   0x00000001 // peforms optimization
#define BUILD_LOG   0x00000002 // log to file

///////////////////////

typedef struct  S_POINT
{
	int id;

	CNmVec3 vec;

	CNmVec3 pre_n;

	S_POINT *next;
} S_POINT;

///////////////////////

typedef struct S_PLANE
{
	int id;
	CNmVec3 n;  // normal
	float d;    // normal * point
	S_PLANE *next;
} S_PLANE;

///////////////////////
class CNmFace;

//	CNmVec3			pre_edgeNormal;	// normal of the ^poly^edge (!= ^face^edge)
//	CNmVec3			pre_vxNormal;	// normal of the vertex

typedef struct S_FACEPOINT
{
	int id;
	CNmVec3 *vecptr;

	S_FACEPOINT *nearitem;
	CNmFace *nearfc;

	CNmVec3 pre_dir;            // direction of the edge
	CNmVec3 pre_dirout;         // normal pointing outside the the ^face^edge of the polygon points face

	int work;

	S_FACEPOINT *next;
	S_FACEPOINT *prev;
} S_FACEPOINT;

typedef struct S_EDGE
{
	int id;

	CNmVec3 *vecptr1;
	CNmVec3 *vecptr2;

	CNmVec3 pre_n;

	CNmFace *fcdx;
	CNmFace *fcsx;
} S_EDGE;

class CNmPolySubList;
typedef struct S_POLYTREE
{
	int id;

	S_PLANE plane;
	CNmPolySubList *pslist;

	S_POLYTREE *front;
	S_POLYTREE *back;
} S_POLYTREE;

#define FL_FACE_ISSHARED        0x00000001
#define FL_FACE_ISVISIBLE       0x00000002
#define FL_FACE_ISPORTAL        0x00000004

#define FL_FACE_CSGDISABLED     0x00008000

#define FL_FACE_CSGORDMASK      0x000F0000
#define FL_FACE_CSGORD1         0x00010000
#define FL_FACE_CSGORD2         0x00020000
#define FL_FACE_CSGORD3         0x00040000
#define FL_FACE_CSGORD4         0x00080000

class CNmFace
{
public:

	int id;
	int m_Flags;

	S_FACEPOINT *m_FcPtHead;
	int         m_NumFcPts;

	S_PLANE *Plane;
	CNmFace *Next;

	CNmVec3 m_pre_Origin;

	void *m_DataPtr;

	CNmStr m_sTextureName;          // application dependent

	CNmFace();
	virtual ~CNmFace();

	void        PlaneSet(S_PLANE *);

	// points stuff

	S_FACEPOINT *_FcPtAddHead();
	S_FACEPOINT *_FcPtAppend();
	S_FACEPOINT *_FcPtAddAfter(S_FACEPOINT *after);
	S_FACEPOINT *_FcPtAddBefore(S_FACEPOINT *before);
	int             _FcPtRemove(S_FACEPOINT *delme);        // returns m_NumFcPts
	S_FACEPOINT *_FcPtGetAt(int pos);                       // pos = 0..(N-1)
	bool            _FcPtTest(S_FACEPOINT *testme);

	bool            VecPtrTest(CNmVec3 *findme);
	S_FACEPOINT *FcPtPtrGetByVecPtr(CNmVec3 *findme);
	S_FACEPOINT *FcPtPtrGetByVec(CNmVec3 *findme);

	bool        PointAdd(CNmVec3 *, float tolerance);         // adding a point to the face
	bool        GetAt(int index, CNmVec3& v);    // get a point @ specified index
	int         GetNum();                       // num points

	bool        CalcNormal(CNmVec3 *);
	CNmVec3     GetOrigin();
	void        GetBox(CNmVec3 *min, CNmVec3 *max);

	void        FlagSet(int flag);
	void        FlagReset(int flag);
	bool        FlagTest(int flag);

	void        PrecalcStuff();

	void        SetDataPtr(void *dataptr);
	void        GetDataPtr(void *&dataptr);

	void        SetTextureName(const char *texname);
	const char *GetTextureName();
//	int			collidetray(CNmVec3& start,CNmVec3& dir,CNmVec3* result);		// a laser from start with dir hits the face?
};
// Face of an object points normal -:outside:- the object
// An observer over the face, outside the object, see the points ordered in anticlockwise order

///////////////////////

class CNmPolyList;

#define FL_POLY_INNER   0x00000001

class CNmPoly
{
public:
	S_PLANE *m_Plane;
	S_POINT *m_Point;
	CNmFace *m_Face;

	int m_PlaneNum;
	int m_PointNum;
	int m_FaceNum;

	S_EDGE *m_Edge;     // static array initialized after poly has been builtd
	int    m_EdgeNum;

	CNmPoly *m_Next;    // used to create lists of brushes
	CNmPoly *m_Prev;    // used to create lists of brushes

	CNmVec3 m_pre_Origin;
	CNmVec3 m_pre_Min;
	CNmVec3 m_pre_Max;
	float   m_pre_Ray;

	int m_Flags;

	char m_sMsg[256];           // used to debug

	bool m_bSolid;

	void *m_DataPtr;

private:

public:
	CNmPoly();
	virtual ~CNmPoly();
	bool        FromPoly(const CNmPoly& src);

	S_PLANE *PlaneAdd(S_PLANE *);       // must supply an existing plane, &pl
	S_PLANE *PlaneAddAlways(S_PLANE *);     // must supply an existing plane, &pl (dup check NOT performed)
	S_PLANE *PlaneGet(S_PLANE *);       // must supply an existing plane, &pl
	void        PlaneFromFaces();
	void        PlaneRemove(S_PLANE *);
	void        PlaneRemoveAll();
	bool        PlaneIsKiller(S_PLANE *); //if it's the inv of a CNmPoly's planes,it kills the poly

	S_POINT *PointAdd(CNmVec3 *);       // must supply an existing NmVec3, &pt
	S_POINT *PointGetByVec(CNmVec3 *);          // must supply an existing NmVec3, &pt
	void        PointRemove(S_POINT *);
	void        PointRemoveAll();

	CNmFace *FaceAdd(S_PLANE *);        // returns face just added
	CNmFace *FaceGetByPtr(S_PLANE *);   // S_PLANE* is one of this CNmPoly objects
	CNmFace *FaceGet(S_PLANE *);        // *S_PLANE is compare (d,n)
	void        FaceFlagsSet(unsigned int flags);
	void        FaceRemove(CNmFace *);
	void        FaceRemoveAll();

	void        EdgeBuild();

	void        FlagSet(int flag);
	void        FlagReset(int flag);
	bool        FlagTest(int flag);

	bool    Build(float tolerance, int flags = 0);      // you must fill plane,first,than call this.
	bool    Buildw(S_PLANE *, float tolerance, int flags = 0);    // rebuild with the new plane (add it first)
	void    LinkStuff();
	bool    IsSolid();
	void    Clear(); // empty data of brush

	void    GetOrigin(CNmVec3 *);    // store the result in CNmVec3*
	bool    GetBox(CNmVec3& min, CNmVec3& max);
	char *GetMsg();

	int     GetNumFaces();
	int     GetNumPoints();
	int     GetNumEdges();
	int     GetNumPlanes();

	void    glDrawPoints();
	void    glDrawEdges();
	void    glDrawFaces(bool CheckVisibilityFlag = true);
	void    glDrawNormals();

	void    PrecalcStuff();

	void    Move(CNmVec3& dr);
	void    RotateAxis(CNmVec3& axis, float angle);

	bool    Make(int sides, float radius, float height);
	bool    Make(CNmVec3& min, CNmVec3& max);    // does a cube
	bool    Split(S_PLANE *clipper, CNmPoly *& result, float tolerance);
	bool    Intersect(const CNmPoly *);
	bool    Merge(CNmPoly *);            // this work only if we have 2 FaceToFace exactly (points to points)
	bool    MergeAlways(CNmPoly *);  // this work when we have 2 face that share the same (inv) plane

	void    SetDataPtr(void *dataptr);
	void    GetDataPtr(void *&dataptr);

	void    SetTextureName(const char *texname);    // set to all faces
	// debug stuff
	void    Dump(const char *filename);
	void    Log();
	void    SetMsg(const char *);
};

///////////////////////

class CNmPolyList
{
public:
	CNmPoly *m_Head;

	CNmPolyList();
	virtual ~CNmPolyList();

	void        FromPolyList(CNmPolyList& src);
	bool        FromRadiantMap(const char *filename);
	bool        FromRadiantEntity(const char *radiantbuffer);
	void        ToRadiantMap(const char *filename);
	void        AddList(CNmPolyList& addme);

	int         MoveToPolyList(CNmPolyList *);  // return num items moved

	CNmPoly *AddHead();
	void        AddHead(CNmPoly *);

	CNmPoly *ExtractHead();
	CNmPoly *Extract(CNmPoly *);

	void        Remove(CNmPoly *& poly);
	void        RemoveHead();
	int         RemoveNonSolid();
	int         RemoveSmallerThan(float k);
	void        RemoveAll();

	CNmPoly *GetHead();
	int         GetNum();
	bool        GetBox(CNmVec3& min, CNmVec3& max);
	CNmPoly *GetPoly(CNmVec3& pt);          // pt inside a poly
	CNmPoly *GetPolyByPtr(CNmPoly *);

	void        Explode(float);
	void        Move(CNmVec3& dr);

	void        RecursivelySetInnerFlagByTouch(CNmPoly *startpoly);

};

/*
    Double list to manipulate sublist of polies (links to polys inside polylist)
*/
typedef struct S_PSLI
{
	int id;
	CNmPoly *poly;
	S_PSLI *next;
	S_PSLI *prev;
} S_PSLI;


class CNmPolySubList
{
public:
	S_PSLI *m_Head;

	CNmPolySubList();
	virtual ~CNmPolySubList();

	S_PSLI *AddHead();
	void        Remove(S_PSLI *);
	void        RemoveHead();
	S_PSLI *Get(CNmPoly *);

	// i should use only the members below outside implementation
	void        AddHead(CNmPoly *);

	void        Remove(CNmPoly *);
	void        RemoveAll();

	bool        GetPoly(CNmPoly *);
	S_PSLI *GetHead();
	int         GetNum();
};

///////////////////////

class CNmEngine
{
public:
	CNmPolyList m_Map;
	CNmPolyList m_Space;

	CNmEngine();
	virtual ~CNmEngine();

	bool    MapLoad(const char *filename);
	bool    BSP();      // fills m_Space with convex areas & mark portal faces
	bool    VIS();      // fills m_Space

	bool    bsp_PrioritizeMapFacesForCSG();
};

///////////////////////
/*
template <class T> class CSList
{
public:
    T			*m_Head,*m_Tail;

                CSList();
    virtual		~CSList();

    T		*AddHead();
    T		*AddTail();
};
*/
///////////////////////

float   u_rad2deg(float radiant);
float   u_deg2rad(float degree);

void    u_FaceListAddHead(CNmFace *& head, CNmFace *fc);
CNmFace *u_FaceListRemoveHead(CNmFace *& head);
void    u_RemovePolysInPListFromPSList_NotSafe(CNmPolyList *, CNmPolySubList *);
void    u_RemovePolysInPListFromPSList_Safe(CNmPolyList *, CNmPolySubList *);

// 1 D UTILITIES

float   u1d_Min(float, float);
float   u1d_Max(float, float);

// 3 D UTILITIES ( "u3d_" does not use precalc value of poly "u3d__" uses precalc )

void    u3d_Min(CNmVec3 *, CNmVec3 *, CNmVec3 *min);
void    u3d_Max(CNmVec3 *, CNmVec3 *, CNmVec3 *max);
float   u3d_Angle(CNmVec3 *, CNmVec3 *);
float   u3d_Angle(S_PLANE *, S_PLANE *);

bool    u3d_VecCompare_Fast(CNmVec3 *, CNmVec3 *, float tolerance);   // if TRUE, use the SLOW version
bool    u3d_VecCompare(CNmVec3 *, CNmVec3 *, float tolerance);
bool    u3d_VecCompare_All(CNmVec3 *a, CNmVec3 *b, float tolerance);

bool    u3d_PolySmallerThan_Fast(CNmPoly *poly, float k);    // if TRUE, it's smaller
bool    u3d_PolySmallerThan(CNmPoly *poly, float k); // check thickness along arbitrary direction
                                                     // slow but perfect.

/*
 must be used this way (*):

 if (u3d_....Collision_Fast())	// if this gives TRUE, they still could not really collide
 if (u3d_....Collision())

*/
// COLLISION poly <-> plane (*)
bool    u3d_PolyPlaneTouch_Fast(CNmPoly *poly, S_PLANE *pl, float tolerance); // to see if DON'T collide 100%
bool    u3d_PolyPlaneIntersect(CNmPoly *poly, S_PLANE *pl, float tolerance);
POS     u3d_PolyInsidePlane(CNmPoly *poly, S_PLANE *pl, float tolerance);

// COLLISION poly <-> face  (*)
int     u3d_PolyFaceShare(CNmPoly *poly1, CNmPoly *poly2, float tolerance);
bool    u3d_PolyFaceInvShare(CNmPoly *poly1, CNmPoly *poly2, CNmFace *& fc1, CNmFace *& fc2, float tolerance);

// COLLISION box <-> box
bool    u3d_BoxCollision(CNmVec3 *min1, CNmVec3 *max1, CNmVec3 *min2, CNmVec3 *max2, float tolerance);

bool    u3d_BoxRayIntersection(CNmVec3& vMin, CNmVec3& vMax, CNmVec3& eye, CNmVec3& ray, CNmVec3& risu, float& distance);

// COLLISION poly <-> poly (*)
bool    u3d_PolyCollision_Fast(CNmPoly *, CNmPoly *, float tolerance);      // to see if DON'T collide 100%
bool    u3d__PolyCollision_Fast(CNmPoly *, CNmPoly *, float tolerance);     // to see if DON'T collide 100%
bool    u3d_PolyCollision(CNmPoly *, CNmPoly *, float tolerance);

bool    u3d_PolyFaceCollision(CNmPoly *poly, CNmFace *fc1, float tolerance);

bool    u3d_PolyInsideOrEqualPoly(CNmPoly *poly1, CNmPoly *poly2, float tolerance);
bool    u3d_PolyInsidePrismFace(CNmPoly *poly1, CNmFace *fc1, float tolerance);
bool    u3d_PolyTouch(CNmPoly *poly1, CNmPoly *poly2, CNmFace *& fc1t, CNmFace *& fc2t, float tolerance);

float   u3d_det(CNmVec3 *, CNmVec3 *, CNmVec3 *);                            // are the rows of the matrix 3x3

void    u3d_PlaneInvert(S_PLANE *pl);
bool    u3d_PlaneCompare(S_PLANE *pl1, S_PLANE *pl2, float tolerance);
bool    u3d_PlaneInverted(S_PLANE *pl1, S_PLANE *pl2, float tolerance); // same place, but inv n's?

void    u3d_PlanePointProjection(S_PLANE *pl, CNmVec3 *v, CNmVec3 *risu);
bool    u3d_PlaneRayIntersection(S_PLANE *pl, CNmVec3 *v, CNmVec3 *dir, CNmVec3 *risu, float *distance);

bool    u3d_PlanesToPoint(S_PLANE *, S_PLANE *, S_PLANE *, CNmVec3 *);
POS     u3d_PointInsidePlane(CNmVec3 *, S_PLANE *, float *, float tolerance);
bool    u3d_PointInPlanes(CNmVec3 *, S_PLANE *, float tolerance);
bool    u3d_PointInsideOrEqualPoly(CNmVec3 *, CNmPoly *, float tolerance);
bool    u3d_PointInsidePoly(CNmVec3 *, CNmPoly *, float tolerance);

bool    u3d_PointInsideOrEqualFace(CNmVec3 *, CNmFace *, float tolerance);
bool    u3d__PointInsideOrEqualFace(CNmVec3 *, CNmFace *, float tolerance);
bool    u3d_PointInsideFace(CNmVec3 *, CNmFace *, float tolerance);

bool    u3d_EdgeIntersect(S_FACEPOINT *, S_FACEPOINT *, float tolerance);
bool    u3d_EdgePlaneIntersect(CNmVec3 *a, CNmVec3 *b, S_PLANE *pl, CNmVec3 *risu, float tolerance);
bool    u3d_EdgeFaceIntersect(CNmVec3 *a, CNmVec3 *b, CNmFace *fc, CNmVec3 *risu, float tolerance);
//bool	u3d_EdgeIntersectOrEqual(S_FACEPOINT*,S_FACEPOINT*,float tolerance);

bool    u3d_FaceInsideFace(CNmFace *fc1, CNmFace *fc2, float tolerance);
bool    u3d_FaceInsideOrEqualFace(CNmFace *fc1, CNmFace *fc2, float tolerance);
bool    u3d_FaceInsidePoly(CNmFace *fc1, CNmPoly *poly1, float tolerance);
bool    u3d_FaceInsideOrEqualPoly(CNmFace *fc1, CNmPoly *poly1, float tolerance);
bool    u3d_FaceIntersect(CNmFace *fc1, CNmFace *fc2, float tolerance);
bool    u3d_FaceToFace(CNmFace *fc1, CNmFace *fc2, float tolerance); // are two faces one appicciata all'altra ?
bool    u3d_FaceShareFace(CNmFace *fc1, CNmFace *fc2, float tolerance); // are two faces sharing some space ?
bool    u3d__FaceShareFace(CNmFace *fc1, CNmFace *fc2, float tolerance);
bool    u3d_SquareFaceRayIntersection(CNmVec3& a, CNmVec3& b, CNmVec3& c, CNmVec3& d,
                                      CNmVec3& eye, CNmVec3& ray, CNmVec3& risu, float& distance);

//int		u3d_SegmentPlaneClip(CNmVec3&,CNmVec3&,S_PLANE*);

CNmVec3 u3d_PlaneCollideTray(S_PLANE&, CNmVec3& start, CNmVec3& dir);

bool    u3d_FaceCSGPoly(CNmFace *fc1, CNmPoly *poly1, float tolerance);

int     u3d_CSGSafe(CNmPoly *hard, CNmPoly *, CNmPolyList *, float tolerance);   // do a merge if needed ath end
int     u3d_CSG(CNmPoly *hard, CNmPoly *, CNmPolyList *, float tolerance);       // basic CSG (use at your own risk)

void    u3d_glCube(float x, float y, float z, float sideX, float sideY, float height);
void    u3d_glCube(CNmVec3& min, CNmVec3& max);

#endif // !defined(POLY_H)
