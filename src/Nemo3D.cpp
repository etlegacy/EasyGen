////////////////
// NEMO3D.CPP //
////////////////
#include "Nemo3D.h"
////////
#include <windows.h>
#include <math.h>
#include <string.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "Nemo.h"
#include "NmVec3.h"
#include "NmVec2.h"
#include "NmParser.h"

/*
Notes:

  About Precalculated values. CNmPoly, CNmFace and S_FACEPOINT have some members variables to store
  precalculated values of common use (origin,ray,min,max etc etc).

  There some functions that have the 'use precalc' version that does the same job faster. Tht
  function has 'u3d__' in front instead of 'u3d_'.

  CNmPoly.PrecalcStuff() is never called by default so you must call it when you want do a precalc of
  variables.

  CNmPoly.FromPoly automatically copies precalc values from a CNmPoly object to another instead.

*/

//#define LOG_BSP
//#define LOG_BSP_BASIC

//#define LOG_VIS
//#define LOG_VIS_BASIC

//#define LOG_BUILD

//#define LOG_CSG

//#define LOG_POLYATTACCATI
//#define LOG_POLYFACECOLLISION
//#define LOG_POLYPLANEINTERSECT

//#define CONSOLE_MODE
#ifndef CONSOLE_MODE
#endif

/*
    Imagine the points as balls of ray = WORLD_GRID_2
*/
#define WORLD_GRID      0.05f
#define WORLD_GRID_2    0.025f

#define CSG_TOLERANCE   -0.01f      // put 0 and won't work anything :(

#define CLEAR_ALL       0x00000007 //
#define CLEAR_PLANES    0x00000001 //
#define CLEAR_POINTS    0x00000002 //
#define CLEAR_FACES     0x00000004 //
#define CLEAR_NOPLANES  0x00000006

#define DETBYNmVec3(a, b, c, det) { __asm fld c.y \
		                            __asm fld b.y \
		                            __asm fld c.x \
		                            __asm fld b.x \
		                            __asm fld c.z \
		                            __asm fld b.z \
		                            __asm fld c.y \
		                            __asm fld b.y \
		                            __asm fmul st, st(3) \
		                            __asm fxch \
		                            __asm fmul st, st(2) \
		                            __asm fsub \
		                            __asm fstp st(7) \
		                            __asm fmul st, st(3) \
		                            __asm fxch \
		                            __asm fmul st, st(2) \
		                            __asm fsub \
		                            __asm fstp st(6) \
		                            __asm fmulp st(3), st \
		                            __asm fmul \
		                            __asm fsub \
		                            __asm fmul a.z \
		                            __asm fxch st(2) \
		                            __asm fmul a.y \
		                            __asm faddp st(2), st \
		                            __asm fmul a.x \
		                            __asm fadd \
		                            __asm fstp det }


// -> Look for FIXME , for probable errors
// -> Look for TESTME , for stuff that need to be tested

////////////////
//// CNmFace CLASS
////////////////

CNmFace::CNmFace()
{
	id = -1;

	m_Flags = 0;

	m_FcPtHead = NULL;
	m_NumFcPts = 0;

	Plane = NULL;
	Next  = NULL;

	m_DataPtr = NULL;

	m_sTextureName = "";
}

CNmFace::~CNmFace()
{
	while (m_FcPtHead)
		_FcPtRemove(m_FcPtHead);
}

void CNmFace::FlagSet(int flag)
{
	m_Flags |= flag;
}

void CNmFace::FlagReset(int flag)
{
	m_Flags &= ~flag;
}

bool CNmFace::FlagTest(int flag)
{
	return  (m_Flags & flag);

}

bool CNmFace::GetAt(int index, CNmVec3& v)
{
	if (index >= m_NumFcPts || m_NumFcPts == 0)
	{
		return false;
	}

	S_FACEPOINT *item = m_FcPtHead;

	while (index != 0)
	{
		index--;
		item = item->next;
	}

	v = *item->vecptr;

	return true;
}

S_FACEPOINT *CNmFace::_FcPtAddHead()
{
	S_FACEPOINT *item = new S_FACEPOINT;

	m_NumFcPts++ ;

	if (m_FcPtHead == NULL)     // no items
	{
		item->nearfc   = NULL;
		item->nearitem = NULL;

		item->next = item;
		item->prev = item;
	}
	else
	{
		item->nearfc   = NULL;
		item->nearitem = NULL;

		item->next = m_FcPtHead;
		item->prev = m_FcPtHead->prev;

		m_FcPtHead->prev->next = item;
		m_FcPtHead->prev       = item;
	}

	m_FcPtHead = item;

	return item;
}

S_FACEPOINT *CNmFace::_FcPtAppend()
{
	S_FACEPOINT *item;

	if (m_FcPtHead == NULL)     // no items
	{
		item = new S_FACEPOINT;

		m_NumFcPts++;

		item->nearfc   = NULL;
		item->nearitem = NULL;

		item->next = item;
		item->prev = item;

		m_FcPtHead = item;
	}
	else
	{
		item = _FcPtAddBefore(m_FcPtHead);
	}

	return item;
}

S_FACEPOINT *CNmFace::_FcPtAddAfter(S_FACEPOINT *after)
{
	if (!after)
	{
		return NULL;
	}

	S_FACEPOINT *item = new S_FACEPOINT;

	m_NumFcPts++;

	item->nearfc   = NULL;
	item->nearitem = NULL;

	item->next = after->next;
	item->prev = after;

	after->next->prev = item;
	after->next       = item;

	return item;
}

S_FACEPOINT *CNmFace::_FcPtAddBefore(S_FACEPOINT *before)
{
	if (!before)
	{
		return NULL;
	}

	S_FACEPOINT *item = new S_FACEPOINT;

	m_NumFcPts++;

	item->nearfc   = NULL;
	item->nearitem = NULL;

	item->next = before;
	item->prev = before->prev;

	before->prev->next = item;
	before->prev       = item;

	return item;
}

int CNmFace::_FcPtRemove(S_FACEPOINT *delme)
{
	if (!delme)
	{
		return m_NumFcPts;
	}
	if (!m_FcPtHead)
	{
		return m_NumFcPts;
	}

	m_NumFcPts--;

	if (m_FcPtHead == m_FcPtHead->next)
	{
		m_FcPtHead = NULL;
	}
	else
	{
		if (m_FcPtHead == delme)
		{
			m_FcPtHead = delme->next;
		}

		delme->next->prev = delme->prev;
		delme->prev->next = delme->next;
	}

	delete delme;

	return m_NumFcPts;
}

S_FACEPOINT *CNmFace::_FcPtGetAt(int pos)
{
	S_FACEPOINT *item = m_FcPtHead;

	if (!item)
	{
		return NULL;
	}

	for (int i = 0; i < pos; i++)
	{
		item = item->next;
	}

	return item;
}

bool CNmFace::_FcPtTest(S_FACEPOINT *testme)        // FIXME , what of the two versions i'm going to use ?
{
//	pointer only scanning

	bool stop = false;

	SCAN_FACEPOINT(item, stop)
	{
		if (item == testme)
		{
			return true;
		}
	}

	return false;
/*
//	numerical|pointer scanning

    S_FACEPOINT* item = m_FcPtHead;

    for(int i=0; i<m_NumFcPts; i++)
    {
        if (testme == item) return true;

        item = item->next;
    }

    return false;
*/
}

bool CNmFace::VecPtrTest(CNmVec3 *findme)
{
	bool stop = false;

	SCAN_FACEPOINT(item, stop)
	{
		if (item->vecptr == findme)
		{
			return true;
		}
	}

	return false;
}

S_FACEPOINT *CNmFace::FcPtPtrGetByVec(CNmVec3 *findme)
{
	bool stop = false;

	SCAN_FACEPOINT(item, stop)
	{
		if (u3d_VecCompare_All(item->vecptr, findme, WORLD_GRID))
		{
			return item;
		}
	}

	return NULL;
}

S_FACEPOINT *CNmFace::FcPtPtrGetByVecPtr(CNmVec3 *findme)
{
	bool stop = false;

	SCAN_FACEPOINT(item, stop)
	{
		if (item->vecptr == findme)
		{
			return item;
		}
	}

	return NULL;
}

bool CNmFace::CalcNormal(CNmVec3 *n)
{
	CNmVec3 a, ba, ca, best;
	float   angle, tmp;
	bool    stop;

	if (m_NumFcPts < 3)
	{
		return false;
	}

	a  = *m_FcPtHead->vecptr;
	ba = *m_FcPtHead->next->vecptr - a;

	angle = PIGRECO_2 + 0.2f; // the best is NEAREST to 90° | PIGRECO/2 , so angle is the distance form 90°

	stop = false;

	for (S_FACEPOINT *item = m_FcPtHead->next->next ; !stop && item; stop = (item = item->next) == m_FcPtHead)
	{
		ca = *item->vecptr - a;

		tmp = (float)fabs(u3d_Angle(&ba, &ca) - PIGRECO_2);

		if (tmp <= angle)
		{
			angle = tmp;
			best  = ca;
		}
	}

//	g_LogDlg.Log("Best angle:",angle);
//	g_LogDlg.Log("Points:",m_NumFcPts);

	*n = ba ^ best;
	n->Normalize();

	return true;
}

// Is assumed "right" insertion
// Is assumed point is coplanar with face's plane
// All new points are external to current face perimeter!
bool CNmFace::PointAdd(CNmVec3 *vec, float tolerance)
{
	CNmVec3 v, q;
	bool    stop;

	// if already exits
	if (VecPtrTest(vec))
	{
		return false;
	}

	// 0-1 points
	if (m_NumFcPts < 2)
	{
		_FcPtAddHead()->vecptr = vec;
		return true;
	}

	stop = false;
	SCAN_FACEPOINT(item, stop)
	{
		v = *item->next->vecptr - *item->vecptr;
		v = Plane->n ^ v;
		q = *vec - *item->vecptr;
		if ((q * v) < tolerance)
		{
			_FcPtAddAfter(item)->vecptr = vec;
			return true;
		}
	}

	return false;
}

void CNmFace::PlaneSet(S_PLANE *spl)
{
	Plane = spl;
}

void CNmFace::GetBox(CNmVec3 *min, CNmVec3 *max)
{
	CNmVec3 *vec;
	bool    stop;

	if (m_NumFcPts == 0)
	{
		return;
	}

	*min = *m_FcPtHead->vecptr;
	*max = *min;

	stop = m_FcPtHead == m_FcPtHead->next;
	SCAN_FACEPOINT(item, stop)
	{
		vec = item->vecptr;

		if (vec->x < min->x)
		{
			min->x = vec->x;
		}
		if (vec->y < min->y)
		{
			min->y = vec->y;
		}
		if (vec->z < min->z)
		{
			min->z = vec->z;
		}
		if (vec->x > max->x)
		{
			max->x = vec->x;
		}
		if (vec->y > max->y)
		{
			max->y = vec->y;
		}
		if (vec->z > max->z)
		{
			max->z = vec->z;
		}
	}
}

int CNmFace::GetNum()
{
	return m_NumFcPts;
}

CNmVec3 CNmFace::GetOrigin()
{
	CNmVec3 v(0, 0, 0);
	bool    stop;

	if (m_NumFcPts == 0)
	{
		return v;
	}

	stop = false;

	SCAN_FACEPOINT(item, stop)
	{
		v += *item->vecptr;
	}
	return v /= float(m_NumFcPts);
}


void CNmFace::PrecalcStuff()
{
	S_FACEPOINT *item;
	int         i;

	m_pre_Origin = GetOrigin();

	item = m_FcPtHead;
	for (i = 0; i < m_NumFcPts; i++)
	{
		item->pre_dir = *item->next->vecptr - *item->vecptr;
		item->pre_dir.Normalize();

		item->pre_dirout = (*item->next->vecptr - *item->vecptr) ^ Plane->n;
		item->pre_dirout.Normalize();

		item = item->next;
	}
}

void CNmFace::SetDataPtr(void *dataptr)
{
	m_DataPtr = dataptr;
}

void CNmFace::GetDataPtr(void *&dataptr)
{
	dataptr = m_DataPtr;
}

void CNmFace::SetTextureName(const char *texname)
{
	if (texname == NULL)
	{
		return;
	}
	m_sTextureName = texname;
}

const char *CNmFace::GetTextureName()
{
	return m_sTextureName.GetBuffer();
}

////////////////
//// CNmFace CLASS - END
////////////////

////////////////
//// CNmPoly CLASS
////////////////

CNmPoly::CNmPoly()
{
	m_Plane = NULL;
	m_Point = NULL;
	m_Face  = NULL;
	m_Next  = NULL;
	m_Prev  = NULL;

	m_PlaneNum = 0;
	m_PointNum = 0;
	m_FaceNum  = 0;

	m_Edge    = NULL;
	m_EdgeNum = 0;

	m_Flags = 0;

	m_bSolid = false;

	m_DataPtr = NULL;

	strcpy(m_sMsg, "");
}

CNmPoly::~CNmPoly()
{
	Clear();
}

void CNmPoly::Clear()
{
	m_bSolid = false;

	PlaneRemoveAll();
	PointRemoveAll();
	FaceRemoveAll();

	if (m_Edge)
	{
		delete[] m_Edge;
		m_Edge = NULL;
	}

	strcpy(m_sMsg, "");
}

// Add plane 'pl' to m_Plane list.
//	duplicate planes check is performed
S_PLANE *CNmPoly::PlaneAdd(S_PLANE *pl)
{
	S_PLANE *aux = PlaneGet(pl);

	if (aux)
	{
		return aux;
	}

	m_PlaneNum++;

	aux       = new S_PLANE;
	aux->d    = pl->d;
	aux->n    = pl->n;
	aux->next = m_Plane;
	m_Plane   = aux;

	return aux;
}

// Add plane 'pl' to m_Plane list.
//	duplicate planes check is NOT performed
S_PLANE *CNmPoly::PlaneAddAlways(S_PLANE *pl)
{
	m_PlaneNum++;

	S_PLANE *aux = new S_PLANE;
	aux->d    = pl->d;
	aux->n    = pl->n;
	aux->next = m_Plane;
	m_Plane   = aux;

	return aux;
}

S_PLANE *CNmPoly::PlaneGet(S_PLANE *plane)
{
	S_PLANE *pl;
	for (pl = m_Plane; pl; pl = pl->next)
	{
		if (u3d_PlaneCompare(plane, pl, WORLD_GRID))
		{
			return pl;
		}
	}
	return NULL;
}

bool CNmPoly::PlaneIsKiller(S_PLANE *killer)
{
	S_PLANE *pl;

	for (pl = m_Plane; pl; pl = pl->next)
	{
		if (u3d_PlaneInverted(killer, pl, WORLD_GRID))
		{
			return true;
		}
	}

	return false;
}

void CNmPoly::PlaneRemove(S_PLANE *what)
{
	if (!m_Plane)
	{
		return;
	}
	if (!what)
	{
		return;
	}

	if (what == m_Plane)
	{
		m_Plane = m_Plane->next;
	}
	else
	{
		S_PLANE *pl = m_Plane;
		while (pl->next != what)
			pl = pl->next;
		pl->next = what->next;
	}

	m_PlaneNum--;

	delete what;
}

void CNmPoly::PlaneRemoveAll()
{
	while (m_Plane)
		PlaneRemove(m_Plane);
}

S_POINT *CNmPoly::PointGetByVec(CNmVec3 *p)
{
	S_POINT *pt;
	for (pt = m_Point; pt; pt = pt->next)
	{
		if (u3d_VecCompare_All(&pt->vec, p, WORLD_GRID))
		{
			return pt;
		}
	}
	return NULL;
}

// No duplicate points are Added
S_POINT *CNmPoly::PointAdd(CNmVec3 *p)
{
	S_POINT *aux = PointGetByVec(p);
	if (aux)
	{
		return aux;
	}

	m_PointNum++;

	aux       = new S_POINT;
	aux->vec  = *p;
	aux->next = m_Point;
	m_Point   = aux;
	return aux;
}

void CNmPoly::PointRemove(S_POINT *what)
{
	if (!m_Point)
	{
		return;
	}
	if (!what)
	{
		return;
	}
	if (what == m_Point)
	{
		m_Point = m_Point->next;
	}
	else
	{
		S_POINT *aux = m_Point;
		while (aux->next != what)
			aux = aux->next;
		aux->next = what->next;
	}

	m_PointNum--;

	delete what;
}

void CNmPoly::PointRemoveAll()
{
	while (m_Point)
		PointRemove(m_Point);
}

CNmFace *CNmPoly::FaceAdd(S_PLANE *pl)
{
	CNmFace *fc = FaceGetByPtr(pl);     // FIXME , I hope we are SURE pl is of this CNmPoly object
	// or we could have multiple faces with different planes (?)
	if (!fc)
	{

		m_FaceNum++;

		// add head
		fc = new CNmFace;
		fc->PlaneSet(pl);
		fc->Next = m_Face;
		m_Face   = fc;
	}

	return fc;
}

void CNmPoly::FaceRemove(CNmFace *what)
{
//	if (!m_Face) return;	// FIXME
	if (!what)
	{
		return;
	}
	if (what == m_Face)
	{
		m_Face = m_Face->Next;
	}
	else
	{
		CNmFace *aux = m_Face;
		while (aux->Next != what)
			aux = aux->Next;
		aux->Next = what->Next;
	}

	m_FaceNum--;

	delete what;
}

void CNmPoly::FaceRemoveAll()
{
	while (m_Face)
		FaceRemove(m_Face);
}

void CNmPoly::EdgeBuild()
{
	CNmFace     *fc;
	S_EDGE      *eg;
	S_FACEPOINT *item;
	S_FACEPOINT *itemnear;
	int         i;
	int         j;

	if (m_Edge != NULL)
	{
		delete[] m_Edge;
	}

	m_EdgeNum = 0;

	// clear work flag
	for (fc = m_Face; fc; fc = fc->Next)
	{
		item = fc->m_FcPtHead;
		for (i = 0; i < fc->m_NumFcPts; i++)
		{
			item->work = 0;
			item       = item->next;
		}

		m_EdgeNum += fc->m_NumFcPts;
	}

	m_EdgeNum /= 2;

	m_Edge = new S_EDGE[m_EdgeNum];

	// gathering edges
	// all facepoints are scanned, scanned first and never scanned before are
	// used to calc Edges
	j = 0;

	for (fc = m_Face; fc; fc = fc->Next)
	{
		item = fc->m_FcPtHead;
		for (i = 0; i < fc->m_NumFcPts; i++)
		{
			if (item->work == 0)
			{
				item->work = 1;

				itemnear       = item->nearitem;
				itemnear->work = 1;

				eg = &m_Edge[j++];

				eg->vecptr1 = item->vecptr;
				eg->vecptr2 = item->next->vecptr;

				eg->fcsx = fc;
				eg->fcdx = item->nearfc;            // FIXME (should be ok)
			}

			item = item->next;
		}
	}
}

void CNmPoly::FlagSet(int flag)
{
	m_Flags |= flag;
}

void CNmPoly::FlagReset(int flag)
{
	m_Flags &= ~flag;
}

bool CNmPoly::FlagTest(int flag)
{
	return  (m_Flags & flag);

}

CNmFace *CNmPoly::FaceGetByPtr(S_PLANE *pl)
{
	for (CNmFace *fc = m_Face; fc; fc = fc->Next)
		if (fc->Plane == pl)
		{
			return fc;
		}
	return NULL;
}

void CNmPoly::FaceFlagsSet(unsigned int flags)
{
	for (CNmFace *fc = m_Face; fc; fc = fc->Next)
	{
		fc->FlagSet(flags);
	}
}

CNmFace *CNmPoly::FaceGet(S_PLANE *pl)
{
	for (CNmFace *fc = m_Face; fc; fc = fc->Next)
	{
		if (u3d_PlaneCompare(pl, fc->Plane, WORLD_GRID))
		{
			return fc;
		}
	}
	return NULL;
}

/*
  1. Ripulisce il brush, lasciando i PLANE
  2. Per ogni *PLANE, aggiunge una *FACE
>>> pulisce ed esce se si sono caricate meno di 4 *FACE
  3. Calcola e aggiunge tutti i possibili punti di intersezione tra 3 *FACE
>>> pulisce ed esce se sono usciti meno di 4 *POINT
>>> pulisce ed esce se trova una *FACE con 1 o 2 POINTS[]
  4. Se BUILD_OPT elimina *PLANE di *FACE con 0 POINTS[], e quindi anche la *FACE stessa
*/

bool CNmPoly::Build(float tolerance, int flags)
{
	CNmVec3 punto;
	S_POINT *pt;
	S_PLANE *p;
	S_PLANE *q;
	CNmFace *fc;
	CNmFace *fc1;
	CNmFace *fc2;
	CNmFace *fc3;
	CNmFace *tmp;
	bool    inside;


	m_bSolid = false;

	PointRemoveAll();
	FaceRemoveAll();

	if (m_PlaneNum < 4)
	{
		#ifdef LOG_BUILD
			#ifndef CONSOLE_MODE
		g_LogDlg.Log("Build: few planes:", m_PlaneNum);
			#endif
		#endif

		PlaneRemoveAll();

		return false;
	}

	// check if 2 planes are one the inverse of the other
	// if so, build() will be 100% unsuccesfull, so we quit.
	for (p = m_Plane ; p->next ; p = p->next)      // FIXME ? seems ok
	{
		for (q = p->next ; q ; q = q->next)
		{
			if (u3d_PlaneInverted(p, q, tolerance)) // FIXME, should be always WORLD_GRID ?
			{
				#ifdef LOG_BUILD
					#ifndef CONSOLE_MODE
				g_LogDlg.Log("Build: planes were inverted:", p->n, q->n);
					#endif
				#endif

				PlaneRemoveAll();

				return false;
			}
		}
	}

	// adding the faces (we can't find duplicate plane, so #planes = #faces)
	for (p = m_Plane ; p ; p = p->next)
	{
		FaceAdd(p);
	}

	// scanning 3 to 3 the face's planes to calc the intersect point
	for (fc1 = m_Face ; fc1->Next->Next ; fc1 = fc1->Next)
	{
		for (fc2 = fc1->Next ; fc2->Next ; fc2 = fc2->Next)
		{
			for (fc3 = fc2->Next ; fc3 ; fc3 = fc3->Next)
			{

				if (u3d_PlanesToPoint(fc1->Plane, fc2->Plane, fc3->Plane, &punto))
				{
					inside = true;

					// fast u3d_PointInPlanes()
					for (fc = m_Face; fc; fc = fc->Next)
					{
						if (fc == fc1)
						{
							continue;          // we now point is inside fc1,fc2,fc3
						}
						if (fc == fc2)
						{
							continue;          // cause they have generated it
						}
						if (fc == fc3)
						{
							continue;
						}

						if (((punto * fc->Plane->n) - fc->Plane->d) > tolerance)
						{
							inside = false;
							break;
						}
					}

					if (inside)
					{
						pt = PointAdd(&punto); // to main point list,if not exists

						fc1->PointAdd(&pt->vec, 0);  //to face...if the case
						fc2->PointAdd(&pt->vec, 0);  //to face...if the case
						fc3->PointAdd(&pt->vec, 0);  //to face...if the case
					}
				}

			} //for fc3
		} //for fc2
	} //for fc1

	for (fc = m_Face; fc; )
	{
		if (fc->m_NumFcPts < 3)
		{
			#ifdef LOG_BUILD
				#ifndef CONSOLE_MODE
			g_LogDlg.Log("Build: face with 1|2 points: ", fc->Plane->n, fc->Plane->n * fc->Plane->d);
				#endif
			#endif

			tmp = fc->Next;
			PlaneRemove(fc->Plane); // NEW
			FaceRemove(fc);
			fc = tmp;
		}
		else
		{
			fc = fc->Next;
		}
	} //for

	if (m_FaceNum < 4)
	{
		#ifdef LOG_BUILD
			#ifndef CONSOLE_MODE
		g_LogDlg.Log("Build: few faces:", m_FaceNum);
			#endif
		#endif

		PointRemoveAll();
		PlaneRemoveAll();
		FaceRemoveAll();

		return false;
	}

	if (m_PointNum < 4)
	{
		#ifdef LOG_BUILD
			#ifndef CONSOLE_MODE
		g_LogDlg.Log("Build: few points:", m_PointNum);
			#endif
		#endif

		PointRemoveAll();
		PlaneRemoveAll();
		FaceRemoveAll();

		return false;
	}

	m_bSolid = true;

	LinkStuff();

	EdgeBuild();

	return true;
}

/*
Buildw() can be applied only to poly already SOLID and consistent.
*/
bool CNmPoly::Buildw(S_PLANE *pl, float tolerance, int flags)
{
	CNmVec3 punto;
	S_POINT *tmp;
	S_POINT *pt;
	CNmFace *fc;
	CNmFace *fctmp;
	CNmFace *fc1;
	CNmFace *fc2;
	CNmFace *fcnew;
	S_PLANE *newpl;
	float   dist;
	bool    inside;


	if (!m_bSolid)
	{
		return false;
	}

	m_bSolid = false;

	if (m_PlaneNum < 3)
	{
		PointRemoveAll();
		PlaneRemoveAll();
		FaceRemoveAll();

		return false;
	}

	if (PlaneGet(pl))   // adding an existing plane is fast and not consume cpu
	{                   // FIXME but you must be sure the poly was m_bSolid.. yes it was, or we exit
		m_bSolid = true;

		return true;
	}

	if (PlaneIsKiller(pl))  // found an plane that is the inverse of 'pl'
	{

		PointRemoveAll();
		PlaneRemoveAll();
		FaceRemoveAll();

		return false;
	}

	newpl = PlaneAddAlways(pl); // don't need to check if exist, done before this cmd

	// removing extra points(faces,planes) outside new plane
	for (pt = m_Point; pt; )
	{
		if (!u3d_PointInsidePlane(&pt->vec, newpl, &dist, tolerance))
		{
			// remove from faces and remove face if remains with 0 points
			for (fc = m_Face; fc; )
			{
				if (fc->_FcPtRemove(fc->FcPtPtrGetByVecPtr(&pt->vec)) == 0)
				{
					fctmp = fc->Next;   // face completely CUT AWAY (0 points)
					PlaneRemove(fc->Plane);
					FaceRemove(fc);     // if remains with 1 - 2 points can still be
					fc = fctmp;         // solid, some points could be added after
				}
				else
				{
					fc = fc->Next;
				}
			}

			// remove from poly
			tmp = pt->next;
			PointRemove(pt);
			pt = tmp;
		}
		else
		{
			pt = pt->next;
		}
	}

	// !!!	I know 'newpl' was added to head, so I can start scanning from m_Plane->next
	//		because m_Plane == newpl

	fcnew = FaceAdd(newpl);

	if (m_FaceNum < 4)
	{
		#ifdef LOG_BUILD
			#ifndef CONSOLE_MODE
		g_LogDlg.Log("Buildw: few faces:", m_FaceNum);
			#endif
		#endif

		PointRemoveAll();
		PlaneRemoveAll();
		FaceRemoveAll();

		return false;
	}

	// calculating new points

	for (fc1 = m_Face->Next ; fc1->Next ; fc1 = fc1->Next)
	{
		for (fc2 = fc1->Next ; fc2 ; fc2 = fc2->Next)
		{
			if (u3d_PlanesToPoint(fc1->Plane, fc2->Plane, fcnew->Plane, &punto))
			{

				inside = true;

				// start from m_Plane->next cause m_Plane is newpl and I could not check for it
				for (fc = m_Face->Next; fc; fc = fc->Next)
				{
					if (fc == fc1)
					{
						continue;
					}
					if (fc == fc2)
					{
						continue;
					}

					if (((punto * fc->Plane->n) - fc->Plane->d) > tolerance)
					{
						inside = false;
						break;
					}
				}

				if (inside)
				{
					pt = PointAdd(&punto);

					fc1->PointAdd(&pt->vec, 0);
					fc2->PointAdd(&pt->vec, 0);
					fcnew->PointAdd(&pt->vec, 0);
				}
			}
		} //for
	} //for

	for (fc = m_Face; fc; )
	{
		if (fc->m_NumFcPts < 3)
		{
			fctmp = fc->Next;
			PlaneRemove(fc->Plane);
			FaceRemove(fc);
			fc = fctmp;
		}
		else
		{
			fc = fc->Next;
		}
	} //for

	if (m_FaceNum < 4)
	{
		#ifdef LOG_BUILD
			#ifndef CONSOLE_MODE
		g_LogDlg.Log("Buildw: few faces:", m_FaceNum);
			#endif
		#endif
		PointRemoveAll();
		PlaneRemoveAll();
		FaceRemoveAll();
		return false;
	}

	if (m_PointNum < 4)
	{
		PointRemoveAll();
		PlaneRemoveAll();
		FaceRemoveAll();
		return false;
	}

	m_bSolid = true;        // before precalc !

	LinkStuff();

	EdgeBuild();

	return true;
}

void CNmPoly::LinkStuff()       // TESTME
{
	CNmFace     *fc;
	CNmFace     *fc1;
	CNmFace     *fc2;
	S_FACEPOINT *item;
	S_FACEPOINT *item1;
	S_FACEPOINT *item2;
	int         i;
	int         i1;

	// reset links
	for (fc = m_Face; fc; fc = fc->Next)
	{
		item = fc->m_FcPtHead;
		for (i = 0; i < fc->m_NumFcPts; i++)
		{
			item->nearitem = NULL;
			item->nearfc   = NULL;

			item = item->next;
		}
	}

	for (fc1 = m_Face; fc1->Next; fc1 = fc1->Next)
	{

		item1 = fc1->m_FcPtHead;
		for (i1 = 0; i1 < fc1->m_NumFcPts; i1++)
		{
			if (item1->nearfc) // already set
			{
				item1 = item1->next;
				continue;
			}

			for (fc2 = m_Face; fc2; fc2 = fc2->Next)
			{
				if (fc1 == fc2)
				{
					continue;
				}

				if (item2 = fc2->FcPtPtrGetByVecPtr(item1->next->vecptr))
				{
					if (item2->next->vecptr == item1->vecptr)
					{
						// link!
						item1->nearitem = item2;
						item2->nearitem = item1;

						item1->nearfc = fc2;
						item2->nearfc = fc1;

						break;
					}
				}
			}

			item1 = item1->next;
		} //item1

	} //fc1


/*
    for(fc1=m_Face;fc1->Next;fc1=fc1->Next)
    {
        for(fc2=fc1->Next;fc2;fc2=fc2->Next)
        {
            if (fc1==fc2) continue;

            item1 = fc1->m_FcPtHead;
            for(i1=0;i1<fc1->m_NumFcPts;i1++)
            {
                if (item1->nearfc) // already set
                {
                    item1 = item1->next;
                    continue;
                }

                if (item2 = fc2->FcPtPtrGetByVecPtr(item1->next->vecptr))
                if (item2->next->vecptr == item1->vecptr)
                {
                    // link!
                    item1->nearitem = item2;
                    item2->nearitem = item1;

                    item1->nearfc	= fc2;
                    item2->nearfc	= fc1;
                }

                item1 = item1->next;
            }
        }//fc2
    }//fc1
*/

/*
    for(fc1=m_Face;fc1->Next;fc1=fc1->Next)
    {
            item1 = fc1->m_FcPtHead;
            for(i1=0;i1<fc1->m_NumFcPts;i1++)
            {
                if (!item1->nearfc) // already set
                {
                    AfxMessageBox("merdA");
                    Log();
                }

                if (!item1->nearitem) // already set
                {
                    AfxMessageBox("merdB");
                }


                item1 = item1->next;
            }
    }//fc1
*/
}

bool CNmPoly::IsSolid()
{
	return m_bSolid;
}

/*
bool CNmPoly::IsSmallerThan(float k)
{
    CNmVec3 a,b;

    if (!GetBox(a,b)) return false;

    b -= a;

    return (b.x<k) || (b.y<k) || (b.z<k);	// FIXME , || or &&, || is better
}
*/

void CNmPoly::GetOrigin(CNmVec3 *origin)
{
	origin->Set(0, 0, 0);
	if (!m_Point)
	{
		return;
	}
	*origin = m_Point->vec;
	int i = 1;
	for (S_POINT *p = m_Point->next; p; p = p->next)
	{
		*origin += p->vec;
		i++;
	}
	*origin /= float(i);
}

void CNmPoly::glDrawPoints()
{
	S_POINT *pt;

	glBegin(GL_POINTS);

	for (pt = m_Point; pt; pt = pt->next)
	{
		glVertex3f(pt->vec.x, pt->vec.y, pt->vec.z);
	}

	glEnd();
}

void CNmPoly::glDrawEdges()
{
	CNmVec3 *v1;
	CNmVec3 *v2;
	CNmVec3 v;
	CNmFace *face;
	S_EDGE  *eg;
	int     i;
/*
    for(face = m_Face; face; face = face->Next )
    {

        glBegin(GL_LINE_LOOP);

        for(i=0;i<face->m_NumFcPts;i++)
        {
            v1 = face->_FcPtGetAt(i)->vecptr;
            glVertex3f( v1->x, v1->y, v1->z );
        }

        glEnd();

    }// for
*/


	glBegin(GL_LINES);

	for (i = 0; i < m_EdgeNum; i++)
	{
		eg = &m_Edge[i];

		v1 = eg->vecptr1;
		v2 = eg->vecptr2;

		glVertex3f(v1->x, v1->y, v1->z);
		glVertex3f(v2->x, v2->y, v2->z);
	}

	glEnd();

}

void CNmPoly::glDrawFaces(bool CheckVisibilityFlag)
{
	CNmVec3 *v;
	CNmVec3 n;
	CNmFace *fc;
	S_PLANE pl;
	int     i;

	for (fc = m_Face; fc; fc = fc->Next)
	{
		if (CheckVisibilityFlag && !fc->FlagTest(FL_FACE_ISVISIBLE))
		{
			continue;
		}

		n = fc->Plane->n;
//		n.Invert();

		glBegin(GL_POLYGON);

		glNormal3f(n.x, n.y, n.z);

		for (i = 0; i < fc->m_NumFcPts; i++)
		{
			v = fc->_FcPtGetAt(i)->vecptr;
			glVertex3f(v->x, v->y, v->z);
		}

		glEnd();

	} // for
}

void CNmPoly::glDrawNormals()
{
	CNmVec3 u, w;
	CNmFace *face;

	for (face = m_Face; face; face = face->Next)
	{

		u = face->GetOrigin();

		w  = u;
		w += face->Plane->n * 16.f;

		glBegin(GL_LINES);

		glVertex3f(u.x, u.y, u.z);
		glVertex3f(w.x, w.y, w.z);

		glEnd();

	} // for
}

bool CNmPoly::GetBox(CNmVec3& min, CNmVec3& max)
{
	if (!m_bSolid)
	{
		return false;
	}
	if (!m_Point)
	{
		return false;
	}

	S_POINT *pt;

	min = m_Point->vec;
	max = m_Point->vec;
	for (pt = m_Point->next; pt; pt = pt->next)
	{
		if (pt->vec.x < min.x)
		{
			min.x = pt->vec.x;
		}
		if (pt->vec.y < min.y)
		{
			min.y = pt->vec.y;
		}
		if (pt->vec.z < min.z)
		{
			min.z = pt->vec.z;
		}
		if (pt->vec.x > max.x)
		{
			max.x = pt->vec.x;
		}
		if (pt->vec.y > max.y)
		{
			max.y = pt->vec.y;
		}
		if (pt->vec.z > max.z)
		{
			max.z = pt->vec.z;
		}
	}

	return true;
}

char *CNmPoly::GetMsg()
{
	return m_sMsg;
}

int CNmPoly::GetNumFaces()
{
	return m_FaceNum;
}

int CNmPoly::GetNumPoints()
{
	return m_PointNum;
}

int CNmPoly::GetNumEdges()
{
	return m_EdgeNum;
}

int CNmPoly::GetNumPlanes()
{
	return m_PlaneNum;
}

void CNmPoly::Dump(const char *filename)
{
	// ...
}

bool CNmPoly::FromPoly(const CNmPoly &src)
{
	CNmFace     *auxfc;
	CNmFace     *fcsrc;
	S_FACEPOINT *item;
	S_FACEPOINT *item1;
	S_PLANE     *pl;
	S_POINT     *pt;
	int         i;

	Clear();

	for (pt = src.m_Point; pt; pt = pt->next)
	{
		PointAdd(&pt->vec);
	}

	for (pl = src.m_Plane; pl; pl = pl->next)
	{
		PlaneAdd(pl);
	}

	for (fcsrc = src.m_Face; fcsrc; fcsrc = fcsrc->Next)
	{
		auxfc = FaceAdd(PlaneGet(fcsrc->Plane));

		// copy precalc CNmFace stuff
		auxfc->m_pre_Origin = fcsrc->m_pre_Origin;

		item1 = fcsrc->m_FcPtHead;
		for (i = 0; i < fcsrc->m_NumFcPts; i++)
		{
			item = auxfc->_FcPtAppend();

			// copy precalc S_FACEPOINT stuff
			item->pre_dir    = item1->pre_dir;
			item->pre_dirout = item1->pre_dirout;

			item->vecptr = &PointGetByVec(item1->vecptr)->vec;

			item1 = item1->next;
		}
	}


	// copy precalc CNmPoly stuff
	m_pre_Origin = src.m_pre_Origin;
	m_pre_Min    = src.m_pre_Min;
	m_pre_Max    = src.m_pre_Max;
	m_pre_Ray    = src.m_pre_Ray;

	m_Flags = src.m_Flags;

	m_bSolid = src.m_bSolid;

	if (m_bSolid)
	{
		LinkStuff();

		EdgeBuild();
	}

	return true;
}

void CNmPoly::Move(CNmVec3& dr)
{
	CNmFace *fc;
	S_POINT *pt;
	for (pt = m_Point; pt; pt = pt->next)
	{
		pt->vec += dr;
	}
	for (fc = m_Face; fc; fc = fc->Next)
	{
		fc->Plane->d = ((fc->Plane->n * fc->Plane->d) + dr) * fc->Plane->n;
		//pl->d = ((pl->n * pl->d) + dr) * pl->n;
	}
}

bool CNmPoly::Make(int sides, float radius, float height)
{
	S_PLANE pl;
	CNmVec3 r, n;
	int     i;

	Clear();

	r.Set(radius, 0, 0);
	r.RotateAxis(CNmVec3(0, 0, 1), PIGRECO / sides);
	n.Set(1, 0, 0);

	for (i = 0; i < sides; i++)
	{
		pl.n = n;
		pl.d = pl.n * r;

		PlaneAdd(&pl);

		n.RotateAxis(CNmVec3(0, 0, 1), PIGRECO * 2 / sides);
		r.RotateAxis(CNmVec3(0, 0, 1), PIGRECO * 2 / sides);
	}

	pl.n.Set(0, 0, -1);
	pl.d = pl.n * CNmVec3(0, 0, -height / 2);
	PlaneAdd(&pl);

	pl.n.Set(0, 0, 1);
	pl.d = pl.n * CNmVec3(0, 0, height / 2);
	PlaneAdd(&pl);

	return Build(WORLD_GRID);
}

bool CNmPoly::Make(CNmVec3& min, CNmVec3& max)
{
	S_PLANE pl;

	Clear();

	pl.n.Set(0, -1, 0);
	pl.d = pl.n * min;
	PlaneAdd(&pl);

	pl.n.Set(1, 0, 0);
	pl.d = pl.n * CNmVec3(max.x, min.y, min.z);
	PlaneAdd(&pl);

	pl.n.Set(0, 1, 0);
	pl.d = pl.n * CNmVec3(max.x, max.y, min.z);
	PlaneAdd(&pl);

	pl.n.Set(-1, 0, 0);
	pl.d = pl.n * CNmVec3(min.x, max.y, min.z);
	PlaneAdd(&pl);

	pl.n.Set(0, 0, 1);
	pl.d = pl.n * max;
	PlaneAdd(&pl);

	pl.n.Set(0, 0, -1);
	pl.d = pl.n * min;
	PlaneAdd(&pl);

	return Build(WORLD_GRID);
}

void CNmPoly::SetTextureName(const char *texname)
{
	CNmFace *face;

	for (face = m_Face; face; face = face->Next)
	{
		face->SetTextureName(texname);
	}
}

bool CNmPoly::Split(S_PLANE *clipper, CNmPoly *& result, float tolerance)
{
	S_PLANE pl;
	pl.n = clipper->n;
	pl.d = clipper->d;

	if (result == NULL)
	{
		result = new CNmPoly;
	}

	result->FromPoly(*this);

	Buildw(&pl, tolerance);

	u3d_PlaneInvert(&pl);

	result->Buildw(&pl, tolerance);

	return true;
}
/*
bool CNmPoly::CSG(const CNmPoly* cutter, CNmPolyList* results)
{
//	if (!collide(b)) return;
//	con.out("Entering subtraction...");
    CNmPoly	*work = new CNmPoly;
    CNmPoly	*result = NULL;
    S_PLANE *pl;
    char	msg[256];

    work->FromPoly( *this );	// work = *this

    for(pl=cutter->m_Plane;pl;pl=pl->next)
    {
        result = results->AddHead();
        work->Split( *pl, result );

        sprintf(msg,"Created by CSG plane: %6.3f %6.3f %6.3f , %6.3f %6.3f %6.3f",
                                            pl->n.x*pl->d,pl->n.y*pl->d,pl->n.z*pl->d,	pl->n.x,pl->n.y,pl->n.z);
        result->SetMsg(msg);

        result->PlaneRemoveUnused();

        if (!work->IsSolid()) break;	// nothing to csg anymore... quit
        work->PlaneRemoveUnused();
    }//for

    delete work;

    return true;
}
*/

bool CNmPoly::Intersect(const CNmPoly *poly)
{
	CNmFace *fc;

	for (fc = poly->m_Face; fc; fc = fc->Next)
	{
		PlaneAdd(fc->Plane);
	}

	Build(WORLD_GRID);

	return IsSolid();
}

bool CNmPoly::Merge(CNmPoly *poly)  // FIXME could be done faster but it's hard and long
{
	CNmPoly     *npoly = new CNmPoly;
	CNmFace     *fc1;
	CNmFace     *fc2;
	S_FACEPOINT *item1;
	S_FACEPOINT *item2;
	float       angle;
	int         i;
	bool        ok   = false;
	bool        fail = false;

	npoly->FromPoly(*poly);

	for (fc1 = m_Face; fc1; fc1 = fc1->Next)
	{
		for (fc2 = npoly->m_Face; fc2; fc2 = fc2->Next)
		{
			if (u3d_FaceToFace(fc1, fc2, WORLD_GRID))
			{
				item1 = fc1->m_FcPtHead;
				item2 = fc2->FcPtPtrGetByVec(item1->next->vecptr);
				for (i = 0; i < fc1->m_NumFcPts; i++)
				{
					angle  = u3d_Angle(fc1->Plane, item1->nearfc->Plane);
					angle += u3d_Angle(fc2->Plane, item2->nearfc->Plane);

					if (angle > (PIGRECO + 0.0001))
					{
						fail = true;
						break;
					}

					item1 = item1->next;
					item2 = item2->prev;    // !!! inverse scan direction
				}

				if (!fail)
				{
					PlaneRemove(fc1->Plane);        // here don't remove anything because it will be rebuildt
					npoly->PlaneRemove(fc2->Plane);   npoly->FaceRemove(fc2);
					ok = true;
					break;
				}
			}

		} //fc2

		if (ok || fail)
		{
			break;
		}

	} //fc1

	if ((!ok) || fail)
	{
		delete npoly;
		return false;
	}

	for (fc2 = npoly->m_Face; fc2; fc2 = fc2->Next)
	{
		PlaneAdd(fc2->Plane);
	}

	Build(WORLD_GRID);

	delete npoly;

	return IsSolid();
}

/*
Ok we have a strange function.

  If this and poly don't touch but share a face (inv) plane, the build start anyway and the final
  result will not be solid

  But if they share also some face space a special brush will be created with all poly and this
  face planes except the ones that were shared.

  So a good thing it's to check if this and poly *roughly* collide, if so proceed with MergeAlways()
*/
bool CNmPoly::MergeAlways(CNmPoly *poly)
{
	CNmFace *fc;
	CNmFace *fc1;
	CNmFace *fc2;
	CNmPoly *app1;
	bool    planeshare;

	planeshare = false;

	for (fc1 = m_Face; fc1; fc1 = fc1->Next)
	{
		for (fc2 = poly->m_Face; fc2; fc2 = fc2->Next)
		{
			if (u3d_PlaneInverted(fc1->Plane, fc2->Plane, WORLD_GRID))  // FIXME
			{
				planeshare = true;
				break;
			}
		}
		if (planeshare)
		{
			break;
		}
	}

	if (!planeshare)
	{
		return false;
	}

	app1 = new CNmPoly;

	// gathering this planes except fc1
	for (fc = m_Face; fc != fc1; fc = fc->Next)
	{
		app1->PlaneAdd(fc->Plane);
	}

	for (fc = fc1->Next; fc; fc = fc->Next)
	{
		app1->PlaneAdd(fc->Plane);
	}

	// gathering poly planes except fc2
	for (fc = poly->m_Face; fc != fc2; fc = fc->Next)
	{
		app1->PlaneAdd(fc->Plane);
	}

	for (fc = fc2->Next; fc; fc = fc->Next)
	{
		app1->PlaneAdd(fc->Plane);
	}

	bool result = app1->Build(WORLD_GRID);

	FromPoly(*app1);

	delete app1;

	return result;
}

void CNmPoly::Log()
{
/*	CString s;
    CNmFace* fc;
    S_POINT* pt;
    int i,j;

    g_LogDlg.Log("Msg: ",m_sMsg);

    // planes info
    g_LogDlg.Log("Planes =",m_PlaneNum);

    // faces info
    g_LogDlg.Log("Faces =",m_FaceNum);

    i = 0;
    for(fc = m_Face;fc;fc=fc->Next)
    {
        s.Format(" Face[%d].Points =",i);
        g_LogDlg.Log(s,fc->m_NumFcPts);

        for(j=0;j<fc->m_NumFcPts;j++)
            g_LogDlg.Log("\t\t",*fc->_FcPtGetAt(j)->vecptr);
        i++;
    }

    // points info
    g_LogDlg.Log("Points =",m_PointNum);
    for(pt=m_Point;pt;pt=pt->next)
        g_LogDlg.Log(" ",pt->vec);
*/
}

void CNmPoly::SetMsg(const char *msg)
{
	strcpy(m_sMsg, msg);
}

void CNmPoly::PrecalcStuff()
{
	S_POINT *pt;
	CNmFace *fc;
	float   r;

	// precalc Origin
	GetOrigin(&m_pre_Origin);

	// precalc Box
	GetBox(m_pre_Min, m_pre_Max);

	// precal Ray
	m_pre_Ray = 0;
	for (pt = m_Point; pt; pt = pt->next)
	{
		r = (pt->vec - m_pre_Origin).GetLength();

		if (r > m_pre_Ray)
		{
			m_pre_Ray = r;
		}
	}

	for (fc = m_Face; fc; fc = fc->Next)
	{
		fc->PrecalcStuff();
	}
}

void CNmPoly::RotateAxis(CNmVec3 &axis, float angle)
{
	S_POINT *pt;

	for (pt = m_Point; pt; pt = pt->next)
	{
		pt->vec.RotateAxis(axis, angle);
	}

	PlaneFromFaces();
}

void CNmPoly::PlaneFromFaces()
{
	CNmFace *fc;

	for (fc = m_Face; fc; fc = fc->Next)
	{
		if (fc->m_NumFcPts < 3)
		{
			continue;
		}

		if (!fc->CalcNormal(&fc->Plane->n))
		{
//			g_LogDlg.Log("Couldn't calc normal for face");
		}

//		fc->Plane->p = fc->Points[0]->p;
		fc->Plane->d = *fc->m_FcPtHead->vecptr * fc->Plane->n;


//		fc->Plane->n = (fc->Points[1]->p - fc->Points[0]->p) ^ (fc->Points[2]->p - fc->Points[0]->p);
//		fc->Plane->n.Normalize();
	}
}

void CNmPoly::SetDataPtr(void *dataptr)
{
	m_DataPtr = dataptr;
}

void CNmPoly::GetDataPtr(void *&dataptr)
{
	dataptr = m_DataPtr;
}

////////////////
//// CNmPoly CLASS - END
////////////////

////////////////////
//// CNmPolyLIST CLASS
////////////////////

CNmPolyList::CNmPolyList()
{
	m_Head = NULL;
}

CNmPolyList::~CNmPolyList()
{
	RemoveAll();
}

void CNmPolyList::FromPolyList(CNmPolyList& src)
{
	CNmPoly *poly;
	RemoveAll();

	for (poly = src.m_Head; poly; poly = poly->m_Next)
	{
		AddHead()->FromPoly(*poly);
	}
}

bool CNmPolyList::FromRadiantMap(const char *filename)
{
	char      ch;
	char      *pchr;
	CNmVec3   a, b, c;
	S_PLANE   pl;
	CNmPoly   *poly;
	CNmParser str;

	if (!str.BufferFromFile(filename))
	{
		return false;
	}

	RemoveAll();

	str.SeekStart();

	ch = str.XChar();

	if (ch != '{')
	{
		return false;
	}

	while (1)
	{
		if (str.Find('{') == NULL)
		{
			break;
		}

		str.XGo('{');

		// patchDef2 found
		if (*(pchr = str.FindOneOf("{}")) == '{')
		{
			str.XGo('{');
			str.XGo('}');
			str.XGo('}');
			continue;
		}

		poly = AddHead();

		while (1)
		{
			//str_clear(buf,' ');
			str.XClear(' ');

			if (str.GetChar() == '}')
			{
				break;
			}

			//str_purgex(buf,'(');
			str.XGo('(');

			a.x = str.XFloat();
			a.y = str.XFloat();
			a.z = str.XFloat();

			//str_purgex(buf,'(');
			str.XGo('(');

			b.x = str.XFloat();
			b.y = str.XFloat();
			b.z = str.XFloat();

			//str_purgex(buf,'(');
			str.XGo('(');

			c.x = str.XFloat();
			c.y = str.XFloat();
			c.z = str.XFloat();

			//str_purgex(buf,')');
			str.XGo(')');

			char *par = str.Find('('); // potrebbe non esserci
			char *brk = str.Find('}'); // c'è di sicuro

			if ((par == NULL) || (par > brk))
			{
				str.XGoOn('}'); // new brush || end of map
			}
			else
			{
				str.XGoOn('('); // new plane
			}

			// add poly

			pl.n = (c - a) ^ (b - a);
//			pl.n = (b-c)^(a-c);
			pl.n.Normalize();
//			pl.p = a;
			pl.d = pl.n * a;

			poly->PlaneAdd(&pl);
		} // end parse brush

		poly->Build(WORLD_GRID);
	}

	return true;
}

bool CNmPolyList::FromRadiantEntity(const char *radiantbuffer)
{
	char      ch;
	CNmVec3   a, b, c;
	S_PLANE   pl;
	CNmPoly   *poly = 0;
	CNmParser str;
	CNmStr    tex;
	int       dbg;

	RemoveAll();

	str.BufferCopy(radiantbuffer);

	str.SeekStart();

	ch = str.XChar();

	if (ch != '{')
	{
		return false;
	}

	dbg = 0;

	while (1)
	{
		if (str.Find('{') == NULL)
		{
			break;
		}

		str.XGo('{');

		// patchDef2 found
		if (*str.FindOneOf("{}") == '{')
		{
			str.XGo('{');
			str.XGo('}');
			str.XGo('}');
			continue;
		}

//		poly = AddHead();
		poly = new CNmPoly;

		tex = "";

		while (1)
		{
			str.XClear(' ');

			if (str.GetChar() == '}')
			{
				break;
			}

			str.XGo('(');

			a.x = str.XFloat();
			a.y = str.XFloat();
			a.z = str.XFloat();

			str.XGo('(');

			b.x = str.XFloat();
			b.y = str.XFloat();
			b.z = str.XFloat();

			str.XGo('(');

			c.x = str.XFloat();
			c.y = str.XFloat();
			c.z = str.XFloat();

			str.XGo(')');

			// get texture name
			str.XClear(' ');
			str.XString(tex, ' ');

			char *par = str.Find('('); // potrebbe non esserci
			char *brk = str.Find('}'); // c'è di sicuro

			if ((par == NULL) || (par > brk))
			{
				str.XGoOn('}'); // new brush || end of map
			}
			else
			{
				str.XGoOn('('); // new plane
			}

			// add poly

			pl.n = (c - a) ^ (b - a);
//			pl.n = (b-c)^(a-c);
			pl.n.Normalize();
//			pl.p = a;
			pl.d = pl.n * a;

			poly->PlaneAdd(&pl);
		} // end parse brush

		if (poly->Build(WORLD_GRID))
		{
			poly->SetTextureName((const char *)tex);
			AddHead(poly);
		}
		else
		{
			delete poly;
			poly = 0;
		}
	}

	return true;
}

void CNmPolyList::ToRadiantMap(const char *filename)
{
	CNmPoly *poly;
	CNmFace *fc;
	CNmVec3 v;
	int     i;
	FILE    *map;
	char    s[1024];

	map = fopen(filename, "wb");

	strcpy(s, "{\r\n\"classname\" \"worldspawn\"\r\n");
	fwrite(s, 1, strlen(s), map);

	SCAN_POLY(poly)
	{
		strcpy(s, "{\r\n");
		fwrite(s, 1, strlen(s), map);

		for (fc = poly->m_Face; fc; fc = fc->Next)
		{
			for (i = 2; i >= 0; i--)
			{
				v = *fc->_FcPtGetAt(i)->vecptr;
				sprintf(s, "( %1.0f %1.0f %1.0f ) ", v.x, v.y, v.z);
				fwrite(s, 1, strlen(s), map);
			}

			strcpy(s, "common/caulk 0 -32 0 1 1 0 0 0\r\n");
			fwrite(s, 1, strlen(s), map);
		}

		strcpy(s, "}\r\n");
		fwrite(s, 1, strlen(s), map);
	}

	strcpy(s, "}\r\n");
	fwrite(s, 1, strlen(s), map);

	fclose(map);
}

void CNmPolyList::AddList(CNmPolyList& addme)
{
	CNmPoly *poly;

	for (poly = addme.m_Head; poly; poly = poly->m_Next)
	{
		AddHead()->FromPoly(*poly);
	}
}

// recursively infect by touch
void CNmPolyList::RecursivelySetInnerFlagByTouch(CNmPoly *startpoly)
{
	CNmPoly *p;
	CNmFace *fc1;
	CNmFace *fc2;

	startpoly->FlagSet(FL_POLY_INNER);

	SCAN_POLY(p)
	{
		if (p == startpoly)
		{
			continue;
		}

		if (!p->FlagTest(FL_POLY_INNER))
		{
			if (u3d_PolyCollision_Fast(startpoly, p, 0.05f)) // tolerance MUST be > 0
			{
				if (u3d_PolyFaceInvShare(startpoly, p, fc1, fc2, WORLD_GRID))
				{
					if (u3d_FaceShareFace(fc1, fc2, WORLD_GRID))
					{
						RecursivelySetInnerFlagByTouch(p);
					}
				}
			}
		}
	}
}

/*
    S_PSLI*	item = new S_PSLI;

    item->poly = NULL;
    item->next = m_Head;
    item->prev = NULL;

    if (m_Head)
    {
        m_Head->prev = item;
    }

    m_Head = item;

    return item;
*/
CNmPoly *CNmPolyList::AddHead()
{
	CNmPoly *poly = new CNmPoly;

	poly->m_Next = m_Head;
	poly->m_Prev = NULL;

	if (m_Head)
	{
		m_Head->m_Prev = poly;
	}

	m_Head = poly;

	return poly;
}

void CNmPolyList::AddHead(CNmPoly *existing)
{
	existing->m_Next = m_Head;
	existing->m_Prev = NULL;

	if (m_Head)
	{
		m_Head->m_Prev = existing;
	}

	m_Head = existing;
}

/*
    if (!m_Head) return;

    S_PSLI* item = m_Head;

    m_Head = m_Head->next;

    if (m_Head)
    {
        m_Head->prev = NULL;
    }

    delete item
*/
CNmPoly *CNmPolyList::ExtractHead()
{
	if (!m_Head)
	{
		return NULL;
	}

	CNmPoly *poly = m_Head;

	m_Head = m_Head->m_Next;

	if (m_Head)
	{
		m_Head->m_Prev = NULL;
	}

	poly->m_Prev = NULL;
	poly->m_Next = NULL;

	return poly;
}

/*
    if (item == m_Head)
    {
        RemoveHead();
        return;
    }

    item->prev->next = item->next;

    if (item->next)
    {
        item->next->prev = item->prev;
    }

    delete item;
*/
CNmPoly *CNmPolyList::Extract(CNmPoly *poly)
{
	if (poly == m_Head)
	{
		return ExtractHead();
	}

	poly->m_Prev->m_Next = poly->m_Next;

	if (poly->m_Next)
	{
		poly->m_Next->m_Prev = poly->m_Prev;
	}

	poly->m_Prev = NULL;
	poly->m_Next = NULL;

	return poly;
}


bool CNmPolyList::GetBox(CNmVec3& min, CNmVec3& max)
{
	CNmPoly *poly;
	CNmVec3 minp, maxp;

	if (!m_Head)
	{
		return false;
	}

	m_Head->GetBox(min, max);

	for (poly = m_Head->m_Next; poly; poly = poly->m_Next)
	{
		poly->GetBox(minp, maxp);

		u3d_Min(&min, &minp, &min);
		u3d_Max(&max, &maxp, &max);
	}

	return true;
}

CNmPoly *CNmPolyList::GetHead()
{
	return m_Head;
}

int CNmPolyList::GetNum()
{
	CNmPoly *poly;
	int     i = 0;

	SCAN_POLY(poly)
	{
		i++;
	}

	return i;
}

CNmPoly *CNmPolyList::GetPoly(CNmVec3& pt)
{
	CNmPoly *poly;

	SCAN_POLY(poly)
	{
		if (u3d_PointInsideOrEqualPoly(&pt, poly, WORLD_GRID))
		{
			return poly;
		}
	}

	return NULL;
}

CNmPoly *CNmPolyList::GetPolyByPtr(CNmPoly *polytofind)
{
	CNmPoly *poly;

	SCAN_POLY(poly)
	{
		if (poly == polytofind)
		{
			return poly;
		}
	}

	return NULL;
}

void CNmPolyList::Move(CNmVec3& dr)
{
	CNmPoly *poly;

	SCAN_POLY(poly)
	{
		poly->Move(dr);
	}
}

int CNmPolyList::MoveToPolyList(CNmPolyList *dst)
{
	int i = 0;
	while (m_Head)
	{
		i++;
		dst->AddHead(ExtractHead());
	}
	return i;
}

void CNmPolyList::Remove(CNmPoly *& poly)
{
	if (poly == m_Head)
	{
		RemoveHead();
		return;
	}

	poly->m_Prev->m_Next = poly->m_Next;

	if (poly->m_Next)
	{
		poly->m_Next->m_Prev = poly->m_Prev;
	}

	delete poly;

	poly = NULL;
}

void CNmPolyList::RemoveAll()
{
	while (m_Head)
		RemoveHead();
}

void CNmPolyList::RemoveHead()
{
	if (!m_Head)
	{
		return;
	}

	CNmPoly *poly = m_Head;

	m_Head = m_Head->m_Next;

	if (m_Head)
	{
		m_Head->m_Prev = NULL;
	}

	delete poly;
}

int CNmPolyList::RemoveNonSolid()
{
/*
    CNmPoly*	poly;
    int i = 0;

    SCAN_POLY(poly)
    {
        if (!poly->IsSolid())
        {
            i++;
            Remove(poly);
            poly = m_Head;
        }
    }

    return i;
*/
	CNmPolySubList psli;
	S_PSLI         *item;
	CNmPoly        *poly;
	int            i = 0;

	SCAN_POLY(poly)
	{
		if (!poly->IsSolid())
		{
			i++;
			psli.AddHead(poly);
		}
	}

	for (item = psli.GetHead(); item; item = item->next)
	{
		Remove(item->poly);
	}

	return i;
}

int CNmPolyList::RemoveSmallerThan(float k)
{
	CNmPoly        *poly;
	CNmPolySubList psli;
	S_PSLI         *item;
	int            i = 0;

	SCAN_POLY(poly)
	{
		if ((u3d_PolySmallerThan_Fast(poly, k)) ||  // faster (calc a cube)
		    u3d_PolySmallerThan(poly, k))           // slower but perfect, along any direction
		{
			i++;
			psli.AddHead(poly);
//			Remove(poly);
//			poly = m_Head;
		}
	}

	for (item = psli.m_Head; item; item = item->next)
	{
		Remove(item->poly);
	}

	return i;
}

void CNmPolyList::Explode(float factor)
{
	CNmPoly *poly;
	CNmVec3 min, max, plorigin, origin;

	GetBox(min, max);

	plorigin = (min + max) / 2;

	SCAN_POLY(poly)
	{
		poly->GetOrigin(&origin);
		poly->Move((origin - plorigin) * (factor - 1));
	}
}

////////////////////
//// CNmPolyLIST CLASS - END
////////////////////

///////////////////////
//// CNmPolySUBLIST CLASS
///////////////////////
/*
CNmPolySubList::CNmPolySubList()
{
    m_Head	= NULL;
    m_Tail	= NULL;
    m_Scan	= NULL;
}

CNmPolySubList::~CNmPolySubList()
{
}

S_PSLI* CNmPolySubList::AddHead()
{
    S_PSLI* item = new S_PSLI;

    item->poly	= NULL;
    item->next	= m_Head;

    if (m_Head == NULL)
    {
        m_Tail = item;
    }

    m_Head = item;

    return item;
}

S_PSLI*	CNmPolySubList::AddTail()
{
    if (m_Tail == NULL)	// m_Head also is NULL
    {
        return AddHead();
    }

    m_Tail->next	= new S_PSLI;
    m_Tail			= m_Tail->next;

    m_Tail->poly	= NULL;
    m_Tail->next	= NULL;	// already NULL, but never known

    return m_Tail;
}

S_PSLI* CNmPolySubList::Get(CNmPoly* poly)
{
    S_PSLI* item;

    for(item = m_Head;item;item=item->next)
    {
        if (item->poly == poly) return item;
    }

    return NULL;
}

void CNmPolySubList::AddHead(CNmPoly* poly)
{
    S_PSLI* item = AddHead();

    item->poly	= poly;
}

void CNmPolySubList::AddTail(CNmPoly* poly)
{
    S_PSLI* item = AddTail();

    item->poly	= poly;
}

bool CNmPolySubList::GetPoly(CNmPoly* poly)
{
    return ( Get(poly) != NULL );
}

void CNmPolySubList::Remove(CNmPoly* poly)
{
    S_PSLI* item = Get(poly);

    if (!item) return;

    if (item == m_Head)
    {
        RemoveHead();
        return;
    }

    S_PSLI *aux = m_Head;
    while (aux->next != item) aux = aux->next;
    aux->next = item->next;

    if (item == m_Tail)
    {
        m_Tail = aux;
    }

    delete item;
}

void CNmPolySubList::RemoveAll()
{
    while (m_Head) RemoveHead();
}

void CNmPolySubList::RemoveHead()
{
    if (m_Head == NULL) return;

    S_PSLI* delme = m_Head;

    m_Head = m_Head->next;

    if (m_Head == NULL)
    {
        m_Tail = NULL;
    }

    delete delme;
}
*/

CNmPolySubList::CNmPolySubList()
{
	m_Head = NULL;
}

CNmPolySubList::~CNmPolySubList()
{
	RemoveAll();
}

S_PSLI *CNmPolySubList::AddHead()
{
	S_PSLI *item = new S_PSLI;

	item->poly = NULL;
	item->next = m_Head;
	item->prev = NULL;

	if (m_Head)
	{
		m_Head->prev = item;
	}

	m_Head = item;

	return item;
}

S_PSLI *CNmPolySubList::Get(CNmPoly *poly)
{
	S_PSLI *item;

	for (item = m_Head; item; item = item->next)
	{
		if (item->poly == poly)
		{
			return item;
		}
	}

	return NULL;
}

void CNmPolySubList::AddHead(CNmPoly *poly)
{
	S_PSLI *item = AddHead();

	item->poly = poly;
}

bool CNmPolySubList::GetPoly(CNmPoly *poly)
{
	return (Get(poly) != NULL);
}

S_PSLI *CNmPolySubList::GetHead()
{
	return m_Head;
}

int CNmPolySubList::GetNum()
{
	int i = 0;

	SCAN_SPOLY(item)
	{
		i++;
	}

	return i;
}

void CNmPolySubList::Remove(CNmPoly *poly)
{
	Remove(Get(poly));
}

void CNmPolySubList::Remove(S_PSLI *item)
{
	if (item == m_Head)
	{
		RemoveHead();
		return;
	}

	item->prev->next = item->next;

	if (item->next)
	{
		item->next->prev = item->prev;
	}

	delete item;
}

void CNmPolySubList::RemoveAll()
{
	while (m_Head)
		RemoveHead();
}

void CNmPolySubList::RemoveHead()
{
	if (!m_Head)
	{
		return;
	}

	S_PSLI *item = m_Head;

	m_Head = m_Head->next;

	if (m_Head)
	{
		m_Head->prev = NULL;
	}

	delete item;
}

///////////////////////
//// CNmPolySUBLIST CLASS - END
///////////////////////

//////////////////
//// CNmEngine CLASS
//////////////////

CNmEngine::CNmEngine()
{
}

CNmEngine::~CNmEngine()
{
}

bool CNmEngine::MapLoad(const char *filename)
{
	return m_Map.FromRadiantMap(filename);
}

#define BSP_CSG             // csg space cube with polymaps
#define BSP_REMOVEOUTSIDE   // remove portals outside map
#define BSP_MERGE           // merge mergeable portals
#define BSP_MAPVISIBLE      // mark visible faces of polymaps
#define BSP_GATHERPLANES    //
#define BSP_DEFINEPORTALS   // portals are polySpace's faces that separates convex areas

// fills m_Space with portals nodes
bool CNmEngine::BSP()
{
	CNmPolyList    tmpList;
	CNmPolyList    csgList;
	CNmPolySubList subList;
	CNmFace        *fc;
	CNmFace        *fc1;
	CNmFace        *fc2;
	CNmPoly        *polyMap;
	CNmPoly        *polySpace;
	CNmPoly        *poly;
	CNmPoly        maincube;
	CNmVec3        min, max;
	CNmPoly        planes;  //NEW
	int            i, npl, k;
	bool           log = false;

	int timeSpaceCSG    = 0;
	int timeSpaceRemove = 0;
	int timeSpaceMerge  = 0;
	int timeSpacePortal = 0;
	int timeTotal       = 0;

	// FaceToFace phase
	bool    restart;
	CNmVec3 start(0, 0, 0);

	i   = 0;
	npl = 0;

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("");
	g_LogDlg.Log("*** BSP START ***");
	g_LogDlg.Log("");
		#else
	printf("");
	printf("*** BSP START ***");
	printf("");
		#endif
	#endif

//	timeTotal = TimerGetTime();

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Map polys:", m_Map.GetNum());
		#else
	printf("");
		#endif
	#endif

	// optimizing map (should be already ok if loaded from q3r map)
	// error poly, small poly...
	subList.RemoveAll();

	SCAN_NM_POLY(m_Map, polyMap)
	{
		if ((!polyMap->IsSolid()) /*|| u3d_PolySmallerThan(polymap,WORLD_GRID)*/)
		{
			subList.AddHead(polyMap);
		}
	}

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Removed from Map polys:", subList.GetNum());
		#else
	printf("");
		#endif
	#endif

	u_RemovePolysInPListFromPSList_NotSafe(&m_Map, &subList);

	subList.RemoveAll();

	///////////////////////

	// building main Space cube
	m_Map.GetBox(min, max);

	m_Space.RemoveAll();

	// starting poly is a simple cube
	m_Space.AddHead()->Make(min, max);

	maincube.FromPoly(*m_Space.m_Head);

#ifdef BSP_CSG

//	timeSpaceCSG = TimerGetTime();

	////////////////////////////////////////
	// Disabling FaceToFace faces of polyMap
	// (should be disabled outside faces too) FIXME

	bsp_PrioritizeMapFacesForCSG();

	// CSG m_Space with m_Map polymaps
	SCAN_NM_POLY(m_Map, polyMap)
	{
		#ifdef LOG_BSP
			#ifndef CONSOLE_MODE
		g_LogDlg.Log("BSP: Processing polymap #fc: ", polyMap->m_FaceNum);
			#else
		printf("BSP: Processing polymap #fc: %d", polySpace->m_FaceNum);
			#endif
		#endif

		csgList.RemoveAll();

		SCAN_NM_POLY(m_Space, polySpace)
		{
			if (u3d_PolyCollision_Fast(polyMap, polySpace, WORLD_GRID))
			{
				u3d_CSGSafe(polyMap, polySpace, &csgList, 0);

				#ifdef LOG_BSP
					#ifndef CONSOLE_MODE
				SCAN_NM_POLY(csgList, poly)
				{
					g_LogDlg.Log("BSP:  CSG gen poly #fc: ", (int)poly->m_FaceNum);
				}
					#else
				printf("BSP:  CSG gen poly #fc: %d", polySpace->m_FaceNum);
					#endif
				#endif

				subList.AddHead(polySpace); // polySpace to remove (there are the results from its csg)
			}
		}

		u_RemovePolysInPListFromPSList_NotSafe(&m_Space, &subList);

		subList.RemoveAll();

		k = csgList.MoveToPolyList(&m_Space);

		#ifdef LOG_BSP
			#ifndef CONSOLE_MODE
		g_LogDlg.Log("BSP: Move to Space list #polys: ", k);
			#else
		printf("BSP: Move to Space list #polys: %d", k);
			#endif
		#endif
	}

//	timeSpaceCSG = TimerGetTime() - timeSpaceCSG;

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Space polys after CSG: ", m_Space.GetNum());
		#else
	printf("Space polys after CSG: %d", m_Space.GetNum());
		#endif
	#endif

#endif

	///////////

#ifdef BSP_REMOVEOUTSIDE


//	timeSpaceRemove = TimerGetTime();

	CNmPoly *startpoly;

	////////////////////////
	// Removing extra polySpace outside the Map

	startpoly = m_Space.GetPoly(CNmVec3(32, 32, 32));

	if (!startpoly)
	{
//		AfxMessageBox("Startpoly not found");
		#ifdef LOG_BSP_BASIC
			#ifndef CONSOLE_MODE
		g_LogDlg.Log("* Startpoly not found: ");
			#else
		printf("* Startpoly not found: ");
			#endif
		#endif
	}
//	else
//	{
	m_Space.RecursivelySetInnerFlagByTouch(startpoly);
//	}

	SCAN_NM_POLY(m_Space, polySpace)
	{
		if (polySpace->FlagTest(FL_POLY_INNER))
		{
			polySpace->FlagReset(FL_POLY_INNER);
		}
		else
		{
			subList.AddHead(polySpace);
		}
	}

	u_RemovePolysInPListFromPSList_NotSafe(&m_Space, &subList);

	subList.RemoveAll();

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Space polys after outside reduced: ", m_Space.GetNum());
		#else
	printf("Space polys after outside reduced: %d", m_Space.GetNum());
		#endif
	#endif

#endif

//	timeSpaceRemove = TimerGetTime() - timeSpaceRemove;

	///////////

#ifdef BSP_MERGE

//	timeSpaceMerge = TimerGetTime();

	CNmPoly *merge;
	CNmPoly *tmp;

	// we must check if some polySpace now need to be merged. CSG could have cut more than
	// needed.

	restart = true;

	while (restart)
	{
		restart = false;

		for (polySpace = m_Space.GetHead(); polySpace->m_Next ; polySpace = polySpace->m_Next)
		{
			for (poly = polySpace->m_Next; poly ; poly = poly->m_Next)
			{
				if (u3d_PolyCollision_Fast(polySpace, poly, 0.05f))   // tolerance MUST be > 0
				{
					if (u3d_PolyFaceInvShare(polySpace, poly, fc1, fc2, WORLD_GRID))
					{
						if (u3d_FaceShareFace(fc1, fc2, WORLD_GRID))
//				if (u3d_FaceToFace(fc1,fc2,WORLD_GRID))	// encapsulate insde Merge()
						{
							merge = new CNmPoly;

							merge->FromPoly(*polySpace);

							if (merge->Merge(poly))
							{
								restart = true;

								polySpace->FromPoly(*merge);

								tmp = poly->m_Prev;

								m_Space.Remove(poly);

								poly = tmp;

//						if (poly == polySpace)
//						{
//							break;
//						}
							}

							delete merge;
						}
					}
				}
			}

			if (!polySpace->m_Next) // cause i just removed 'poly' that was the last.
			{
				break;
			}

		} //for

	} //while

//	timeSpaceMerge = TimerGetTime() - timeSpaceMerge;

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Space polys after MERGE: ", m_Space.GetNum());
		#else
	printf("Space polys after MERGE: %d", m_Space.GetNum());
		#endif
	#endif

#endif

	///////////

#ifdef BSP_MAPVISIBLE

	SCAN_NM_POLY(m_Map, polyMap)
	{

		SCAN_NM_POLY(m_Space, polySpace)
		{
			if (u3d_PolyTouch(polyMap, polySpace, fc1, fc2, WORLD_GRID))
			{
				fc1->FlagSet(FL_FACE_ISVISIBLE);
			}
		}

	}

#endif

	/////////////

#ifdef BSP_GATHERPLANES

	planes.Clear();

	int ptot = 0;

	SCAN_NM_POLY(m_Map, polyMap)
	{
		for (fc = polyMap->m_Face; fc; fc = fc->Next)
		{
			if (fc->FlagTest(FL_FACE_ISVISIBLE))
			{
				ptot++;

				if (!planes.PlaneIsKiller(fc->Plane))
				{
					if (!planes.PlaneGet(fc->Plane))
					{
						planes.PlaneAddAlways(fc->Plane);
					}
				}
			}
		}
	}

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Gathered planes: ", planes.m_PlaneNum);
		#else
	printf("Gathered planes: %d", planes.m_PlaneNum);
		#endif
	#endif

#endif

	//////////////

#ifdef  BSP_DEFINEPORTALS

//	timeSpacePortal = TimerGetTime();

	// make the m_Space visible for output
	SCAN_NM_POLY(m_Space, polySpace)
	{
		for (fc = polySpace->m_Face; fc; fc = fc->Next)
		{
			fc->FlagSet(FL_FACE_ISPORTAL);
		}
	}

	// if a polySpace face touches a polyMap, it's not a portal
	SCAN_NM_POLY(m_Space, polySpace)
	{
		SCAN_NM_POLY(m_Map, polyMap)
		{
			if (u3d_PolyCollision_Fast(polyMap, polySpace, 0.05f))    // tolerance MUST be > 0
			{
				if (u3d_PolyFaceInvShare(polyMap, polySpace, fc1, fc2, WORLD_GRID))
				{
					if (u3d_FaceShareFace(fc1, fc2, WORLD_GRID))
					{
						fc2->FlagReset(FL_FACE_ISPORTAL);
					}
				}
			}
		}
	}


	// FaceToFace portals ? if so , remains only one
/*	for(polySpace = m_Space.GetHead(); polySpace->m_Next ; polySpace = polySpace->m_Next )
    {
        for(poly = polySpace->m_Next; poly ; poly = poly->m_Next )
        {
            if (u3d_PolyCollision_Fast(poly,polySpace,0.05))	// tolerance MUST be > 0
            if (u3d_PolyFaceInvShare(poly,polySpace,fc1,fc2,WORLD_GRID))
            if (fc1->FlagTest( FL_FACE_ISPORTAL ) && fc2->FlagTest( FL_FACE_ISPORTAL ))
            if (u3d_FaceToFace(fc1,fc2,WORLD_GRID))
            {
                fc2->FlagReset( FL_FACE_ISPORTAL );
            }
        }
    }
*/

	// make the m_Space portals visible for output debug
	SCAN_NM_POLY(m_Space, polySpace)
	{
		for (fc = polySpace->m_Face; fc; fc = fc->Next)
		{
			if (fc->FlagTest(FL_FACE_ISPORTAL))
			{
				fc->FlagSet(FL_FACE_ISVISIBLE);
			}
		}
	}

//	timeSpacePortal = TimerGetTime() - timeSpacePortal;

#endif

	//////////////

//	timeTotal = TimerGetTime() - timeTotal;

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Elaspsed (timeSpaceCSG) ms:", timeSpaceCSG);
	g_LogDlg.Log("Elaspsed (timeSpaceRemove) ms:", timeSpaceRemove);
	g_LogDlg.Log("Elaspsed (timeSpaceMerge) ms:", timeSpaceMerge);
	g_LogDlg.Log("Elaspsed (timeSpacePortal) ms:", timeSpacePortal);
	g_LogDlg.Log("Elaspsed (TOTAL) ms:", timeTotal);
		#else
	printf("Elaspsed (timeSpaceCSG) ms: %d", timeSpaceCSG);
	printf("Elaspsed (timeSpaceRemove) ms: %d", timeSpaceRemove);
	printf("Elaspsed (timeSpaceMerge) ms: %d", timeSpaceMerge);
	printf("Elaspsed (timeSpacePortal) ms: %d", timeSpacePortal);
	printf("Elaspsed (TOTAL) ms: %d", timeTotal);
		#endif
	#endif

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Space polys: ", m_Space.GetNum());
		#else
	printf("Space polys: %d", m_Space.GetNum());
		#endif
	#endif

	#ifdef LOG_BSP_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("");
	g_LogDlg.Log("*** BSP END ***");
	g_LogDlg.Log("");
		#else
	printf("");
	printf("*** BSP END ***");
	printf("");
		#endif
	#endif

	return true;
}

/*
face with PRIOR1 csg for first

face with PRIOR4 csg for last
*/
bool CNmEngine::bsp_PrioritizeMapFacesForCSG()
{
	CNmPoly *polyMap;
	CNmPoly *poly;
	CNmFace *fc;
	CNmFace *fc1;
	CNmFace *fc2;
	CNmFace *fctmp;

	CNmFace **ord1;
	CNmFace **ord2;
	CNmFace **ord3;
	CNmFace **ord4;
	CNmFace **unord;
	int     nord1;
	int     nord2;
	int     nord3;
	int     nord4;
	int     nunord;

	int i;

	// resetting all PRIOR flags
	for (polyMap = m_Map.GetHead(); polyMap ; polyMap = polyMap->m_Next)
	{
		for (fc = polyMap->m_Face; fc; fc = fc->Next)
		{
			fc->FlagReset(FL_FACE_CSGORDMASK);
		}
	}

	// Aligned x|y|z faces get ORD1
	for (polyMap = m_Map.GetHead(); polyMap ; polyMap = polyMap->m_Next)
	{
		for (fc = polyMap->m_Face; fc; fc = fc->Next)
		{
			if ((fabs(fc->Plane->n.x) > 0.999) || (fabs(fc->Plane->n.y) > 0.999) || (fabs(fc->Plane->n.z) > 0.999))
			{
				fc->FlagSet(FL_FACE_CSGORD1);
			}
		}
	}

	// FaceToFace faces get ORD3
	for (polyMap = m_Map.GetHead(); polyMap->m_Next ; polyMap = polyMap->m_Next)
	{
		for (poly = polyMap->m_Next; poly ; poly = poly->m_Next)
		{
			if (u3d_PolyCollision_Fast(polyMap, poly, 0.05f)) // tolerance MUST be > 0
			{
				if (u3d_PolyFaceInvShare(polyMap, poly, fc1, fc2, WORLD_GRID))
				{
					if (!fc1->FlagTest(FL_FACE_CSGORD3) && !fc2->FlagTest(FL_FACE_CSGORD3)) // already set
					{
						if (u3d_FaceToFace(fc1, fc2, WORLD_GRID))
						{
							// going to lowest order, give bad result the csg, so delete past prior
							// it's better to have less BAD cuts rather then more good-looking ones
							fc1->FlagSet(FL_FACE_CSGORD3);
							fc2->FlagSet(FL_FACE_CSGORD3);
						}
					}
				}
			}
		}
	}


	// REordering the faces then quit
	for (polyMap = m_Map.GetHead(); polyMap ; polyMap = polyMap->m_Next)
	{
		ord1  = new CNmFace *[polyMap->m_FaceNum];
		ord2  = new CNmFace *[polyMap->m_FaceNum];
		ord3  = new CNmFace *[polyMap->m_FaceNum];
		ord4  = new CNmFace *[polyMap->m_FaceNum];
		unord = new CNmFace *[polyMap->m_FaceNum];

		nord1  = 0;
		nord2  = 0;
		nord3  = 0;
		nord4  = 0;
		nunord = 0;

		for (fc = polyMap->m_Face; fc; )
		{
			fctmp = fc->Next;

			if (fc->FlagTest(FL_FACE_CSGORD4))
			{
				ord4[nord4++] = fc;
			}
			else
			if (fc->FlagTest(FL_FACE_CSGORD3))
			{
				ord3[nord3++] = fc;
			}
			else
			if (fc->FlagTest(FL_FACE_CSGORD2))
			{
				ord2[nord2++] = fc;
			}
			else
			if (fc->FlagTest(FL_FACE_CSGORD1))
			{
				ord1[nord1++] = fc;
			}
			else
			{
				unord[nunord++] = fc;
			}

			fc = fctmp;

		}

		polyMap->m_Face = NULL;

		for (i = 0; i < nunord; i++)
			u_FaceListAddHead(polyMap->m_Face, unord[i]);
		for (i = 0; i < nord4; i++)
			u_FaceListAddHead(polyMap->m_Face, ord4[i]);
		for (i = 0; i < nord3; i++)
			u_FaceListAddHead(polyMap->m_Face, ord3[i]);
		for (i = 0; i < nord2; i++)
			u_FaceListAddHead(polyMap->m_Face, ord2[i]);
		for (i = 0; i < nord1; i++)
			u_FaceListAddHead(polyMap->m_Face, ord1[i]);

		delete[] ord1;
		delete[] ord2;
		delete[] ord3;
		delete[] ord4;
		delete[] unord;

	} //polyMap

	return true;
}

//#define VIS_FACETOFACE

bool CNmEngine::VIS()
{
	CNmPolyList csgList;

	int timeTotal           = 0;
	int timeSpaceFaceToFace = 0;


	#ifdef LOG_VIS_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("");
	g_LogDlg.Log("*** VIS START ***");
	g_LogDlg.Log("");
		#else
	printf("");
	printf("*** VIS START ***");
	printf("");
		#endif
	#endif

//	timeTotal = TimerGetTime();


#ifdef VIS_FACETOFACE

	///////////////////
	// FaceToFace phase
	//
	// during this phase polySpace inside the Map will be cutted properly to make them
	// perfectly met (combacianti 'face to face')
	///////////////////

	restart = true;

//	timeSpaceFaceToFace = TimerGetTime();

	if (m_Space.GetHead())
	{
		while (restart)
		{
			restart = false;

			// scanning all polySpace
			for (polySpace = m_Space.GetHead(); polySpace->m_Next ; polySpace = polySpace->m_Next)
			{

				for (poly = polySpace->m_Next; poly ; poly = poly->m_Next)
				{

					if (u3d_PolyCollision_Fast(polySpace, poly, 0.05)) // tolerance MUST be > 0
					{
						if (u3d_PolyFaceInvShare(polySpace, poly, fc1, fc2, WORLD_GRID))
						{
							if (u3d_FaceShareFace(fc1, fc2, WORLD_GRID)) // FIXME, less polySpace obtained if used, should not be so
							{
								if (!u3d_FaceToFace(fc1, fc2, WORLD_GRID))
								{

									mergedalw = new CNmPoly;

									mergedalw->FromPoly(*polySpace);

									if (mergedalw->MergeAlways(poly)) // ok, now the good part :)
									{

										// sorry but we must restart all the scan when we reach the end
										restart = true;

										// let CSG polySpace with mergedalw
										if (!u3d_PolyInsidePoly(polySpace, mergedalw, WORLD_GRID))
										{
											u3d_CSGSafe(mergedalw, polySpace, &csgList, WORLD_GRID);
										}

										// let CSG poly with mergedalw
										if (!u3d_PolyInsidePoly(poly, mergedalw, WORLD_GRID))
										{
											u3d_CSGSafe(mergedalw, poly, &csgList, WORLD_GRID);
										}

										split1 = new CNmPoly;

										mergedalw->Split(fc1->Plane, split1, WORLD_GRID);

										polySpace->FromPoly(*split1);
										poly->FromPoly(*mergedalw);

										delete split1;

										csgList.MoveToPolyList(&m_Space);

									}

									delete mergedalw;

								} // if collision fast

							}
						}
					}
				} //for poly

			} //for polySpace

		} // while (restart)

	}
//	timeSpaceFaceToFace = TimerGetTime() - timeSpaceFaceToFace;

	#ifdef LOG_VIS_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Space polys after FACETOFACE: ", m_Space.GetNum());
		#else
	printf("Space polys after FACETOFACE: %d", m_Space.GetNum());
		#endif
	#endif

#endif  // VIS_FACETOFACE

	///////////////////

//	timeTotal = TimerGetTime() - timeTotal;

	#ifdef LOG_VIS_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Elaspsed (timeSpaceFaceToFace) ms:", timeSpaceFaceToFace);
	g_LogDlg.Log("Elaspsed (TOTAL) ms:", timeTotal);
		#else
	printf("Elaspsed (timeSpaceFaceToFace) ms: %d", timeSpaceFaceToFace);
	printf("Elaspsed (TOTAL) ms: %d", timeTotal);
		#endif
	#endif

	#ifdef LOG_VIS_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("Space polys: ", m_Space.GetNum());
		#else
	printf("Space polys: %d", m_Space.GetNum());
		#endif
	#endif

	#ifdef LOG_VIS_BASIC
		#ifndef CONSOLE_MODE
	g_LogDlg.Log("");
	g_LogDlg.Log("*** VIS END ***");
	g_LogDlg.Log("");
		#else
	printf("");
	printf("*** VIS END ***");
	printf("");
		#endif
	#endif

	return true;
}

//////////////////
//// CNmEngine CLASS - END
//////////////////

///////////////////////////////
//// MISCELLANEOUS 3D UTILITIES
///////////////////////////////

float u_rad2deg(float radiant)
{
	return (radiant * 57.29577951308232286464772187173366546630859375f);
}             // 180/M_PI

float u_deg2rad(float degree)
{
	return (degree * 0.0174532925199432954743716805978692718781530857086181640625f);
}             // M_PI/180

void u_RemovePolysInPListFromPSList_NotSafe(CNmPolyList *polylist, CNmPolySubList *polysublist)
{
	for (S_PSLI *item = polysublist->GetHead(); item ; item = item->next)
	{
		polylist->Remove(item->poly);
	}
}

void u_RemovePolysInPListFromPSList_Safe(CNmPolyList *polylist, CNmPolySubList *polysublist)
{
	for (S_PSLI *item = polysublist->GetHead(); item ; item = item->next)
	{
		if (polylist->GetPolyByPtr(item->poly))
		{
			polylist->Remove(item->poly);
		}
	}
}

void u_FaceListAddHead(CNmFace *& head, CNmFace *fc)
{
	fc->Next = head;
	head     = fc;
}

CNmFace *u_FaceListRemoveHead(CNmFace *& head)
{
	if (!head)
	{
		return NULL;
	}

	CNmFace *tmp = head;

	head = head->Next;

	tmp->Next = NULL;

	return tmp;
}

////////////////
// 1 DIMENSIONAL
////////////////

float   u1d_Min(float a, float b)
{
	return (a < b) ? a : b;
}

float   u1d_Max(float a, float b)
{
	return (a < b) ? b : a;
}

////////////////
// 2 DIMENSIONAL
////////////////

bool u2d_VecCompare_All(CNmVec2 *a, CNmVec2 *b, float tolerance)
{
	CNmVec2 c = *a - *b;

	if (c.x < 0)
	{
		c.x = -c.x;
	}
	if (c.y < 0)
	{
		c.y = -c.y;
	}

	if ((c.x > tolerance) ||
	    (c.y > tolerance) ||
	    ((c.x + c.y) > tolerance)
	    )
	{
		return false;
	}

	float tol_2 = tolerance * tolerance;

	return (c.x * c.x + c.y * c.y) < tol_2;
}


////////////////
// 3 DIMENSIONAL
////////////////


void u3d_Min(CNmVec3 *a, CNmVec3 *b, CNmVec3 *min)
{
	min->x = u1d_Min(a->x, b->x);
	min->y = u1d_Min(a->y, b->y);
	min->z = u1d_Min(a->z, b->z);
}

void u3d_Max(CNmVec3 *a, CNmVec3 *b, CNmVec3 *max)
{
	max->x = u1d_Max(a->x, b->x);
	max->y = u1d_Max(a->y, b->y);
	max->z = u1d_Max(a->z, b->z);
}

float u3d_Angle(CNmVec3 *a, CNmVec3 *b)
{
	float l = a->GetLength() * b->GetLength();

	if (l == 0)
	{
		return 0;
	}

	return acosf((*a * *b) / l);
}

float u3d_Angle(S_PLANE *pl1, S_PLANE *pl2)
{
	return PIGRECO - u3d_Angle(&pl1->n, &pl2->n);    // FIXME, could be precalc for S_FACEPOINT with nearfc
}

// if false, use the SLOW version
bool u3d_VecCompare_Fast(CNmVec3 *a, CNmVec3 *b, float tolerance)
{
	CNmVec3 c = *a - *b;
	if (c.x < 0)
	{
		c.x = -c.x;
	}
	if (c.y < 0)
	{
		c.y = -c.y;
	}
	if (c.z < 0)
	{
		c.z = -c.z;
	}
	return (c.x + c.y + c.z) < tolerance;
}

bool u3d_VecCompare(CNmVec3 *a, CNmVec3 *b, float tolerance)
{
	return (*a - *b).GetLength() < tolerance;
}

bool u3d_VecCompare_All(CNmVec3 *a, CNmVec3 *b, float tolerance)
{
	CNmVec3 c = *a - *b;

	if (c.x < 0)
	{
		c.x = -c.x;
	}
	if (c.y < 0)
	{
		c.y = -c.y;
	}
	if (c.z < 0)
	{
		c.z = -c.z;
	}

	if ((c.x > tolerance) ||
	    (c.y > tolerance) ||
	    (c.z > tolerance) ||
	    ((c.x + c.y) > tolerance) ||
	    ((c.x + c.z) > tolerance) ||
	    ((c.y + c.z) > tolerance) ||
	    ((c.x + c.y + c.z) > tolerance)
	    )
	{
		return false;
	}

	float tol_2 = tolerance * tolerance;

	return (c.x * c.x + c.y * c.y + c.z * c.z) < tol_2;
}

bool u3d_BoxCollision(CNmVec3 *min1, CNmVec3 *max1, CNmVec3 *min2, CNmVec3 *max2, float tolerance)
{
	CNmVec3 maxbox;
	CNmVec3 min, max;

/*
    maxbox = *max1 - *min1 + *max2 - *min2;

    u3d_Min(min1,min2,&min);
    u3d_Max(max1,max2,&max);


    return ((max.x - min.x) < maxbox.x + tolerance) &&
                ((max.y - min.y) < maxbox.y + tolerance) &&
                    ((max.z - min.z) < maxbox.z + tolerance);
*/
	maxbox.x = max1->x - min1->x + max2->x - min2->x;
	min.x    = (min1->x < min2->x) ? min1->x : min2->x;
	max.x    = (max1->x < max2->x) ? max2->x : max1->x;
	if ((max.x - min.x) > maxbox.x + tolerance)
	{
		return false;
	}

	maxbox.y = max1->y - min1->y + max2->y - min2->y;
	min.y    = (min1->y < min2->y) ? min1->y : min2->y;
	max.y    = (max1->y < max2->y) ? max2->y : max1->y;
	if ((max.y - min.y) > maxbox.y + tolerance)
	{
		return false;
	}

	maxbox.z = max1->z - min1->z + max2->z - min2->z;
	min.z    = (min1->z < min2->z) ? min1->z : min2->z;
	max.z    = (max1->z < max2->z) ? max2->z : max1->z;
	if ((max.z - min.z) > maxbox.z + tolerance)
	{
		return false;
	}

	return true;
}

bool u3d_FaceCSGPoly(CNmFace *fc1, CNmPoly *poly1, float tolerance)
{
	CNmVec3 forigin;
	CNmFace *fc;
	S_FACEPOINT *item;
	int i;

	for (fc = poly1->m_Face; fc; fc = fc->Next)
	{
		if (u3d_PlaneCompare(fc1->Plane, fc->Plane, tolerance))
		{
			return false;
		}
	}

	forigin = fc1->GetOrigin();

	if (u3d_PointInsidePoly(&forigin, poly1, tolerance))
	{
		return true;
	}

	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		if (u3d_PointInsidePoly(item->vecptr, poly1, tolerance))
		{
			return true;
		}

		item = item->next;
	}

//	if (u3d_FaceInsidePoly(fc1,poly1,tolerance))
//	{
//		return true;
//	}

	for (fc = poly1->m_Face; fc; fc = fc->Next)
	{
		if (u3d_FaceIntersect(fc1, fc, tolerance))
		{
			return true;
		}
	}

	return false;
}

int u3d_CSGSafe(CNmPoly *polyhard, CNmPoly *poly, CNmPolyList *results, float tolerance)
{
	CNmPoly *work   = new CNmPoly;
	CNmPoly *result = NULL;
	S_PLANE *pl;
	CNmFace *fc;
	CNmPolyList csglist, tmplist;
	CNmPolySubList sublist;
	CNmPoly *poly1;
	CNmPoly *poly2;
	CNmPoly *merged;
	bool restart;
	int created = 0;

	work->FromPoly(*poly);      // work = *this

	for (fc = polyhard->m_Face; fc; fc = fc->Next)
	{
//		if (!u3d_PolyFaceCollision(poly,fc,tolerance)) continue; // FIXME, does not work

		pl = fc->Plane;

		result = csglist.AddHead();

		work->Split(pl, result, WORLD_GRID);

		if (!work->IsSolid())
		{
			break;                      // nothing to csg anymore... quit
		}
	} //for

	delete work;

	csglist.RemoveNonSolid();

	if (csglist.GetHead() == NULL)
	{
		return 0;
	}

	restart = true;
	while (restart)
	{
		restart = false;
		for (poly1 = csglist.GetHead(); poly1->m_Next; poly1 = poly1->m_Next)
		{
			for (poly2 = poly1->m_Next; poly2; poly2 = poly2->m_Next)
			{
				merged = new CNmPoly;

				merged->FromPoly(*poly1);

/*				if (merged->Merge( poly2 ))		// check itself for FaceToFace
                {
                    restart = true;

                    poly1->FromPoly( *merged );

                    work = poly2->m_Prev;

                    csglist.Remove( poly2 );

                    poly2 = work;
                }

                delete merged;
*/

				if (merged->Merge(poly2))       // check itself for FaceToFace
				{
					csglist.Remove(poly1);
					csglist.Remove(poly2);
					csglist.AddHead(merged);
					restart = true;
					break;
				}
				else
				{
					delete merged;
				}

			}

/*
            if (!poly1->m_Next)
            {
                break;
            }
*/
			if (restart)
			{
				break;
			}
		}
	} //while

	created = csglist.GetNum();

	csglist.MoveToPolyList(results);

	return created;
}

int u3d_CSG(CNmPoly *polyhard, CNmPoly *poly, CNmPolyList *results, float tolerance)
{
	CNmPoly *work   = new CNmPoly;
	CNmPoly *result = NULL;
	S_PLANE *pl;
	CNmFace *fc;
	CNmPolyList csglist, tmplist;
	CNmPolySubList sublist;
	int created  = 0;
	bool collide = false;

	// complete csg
	if (u3d_PolyInsideOrEqualPoly(poly, polyhard, tolerance))
	{
//		g_LogDlg.Log("mah");
		return 0;
	}

	work->FromPoly(*poly);      // work = *this

	for (fc = polyhard->m_Face; fc; fc = fc->Next)
	{
		if (!u3d_FaceCSGPoly(fc, poly, tolerance))
		{
			continue;
		}

		collide = true;

		pl = fc->Plane;

		result = csglist.AddHead();

		work->Split(pl, result, WORLD_GRID);

		if (!work->IsSolid())
		{
			break;                      // nothing to csg anymore... quit
		}
	} //for

	delete work;

	// no face csg detected, so polys didn't collide
	if (!collide)
	{
		result = csglist.AddHead();

		result->FromPoly(*poly);

		csglist.RemoveNonSolid();

		created = csglist.GetNum();

		csglist.MoveToPolyList(results);

		return created;
	}

	// normal csg
	csglist.RemoveNonSolid();

	if (csglist.GetHead() == NULL)
	{
		return 0;
	}

	created = csglist.GetNum();

	csglist.MoveToPolyList(results);

	return created;
}

bool u3d_PolySmallerThan_Fast(CNmPoly *poly, float k)
{
	CNmVec3 min, max;

	poly->GetBox(min, max);

	max -= min;

	return (max.x < k) || (max.y < k) || (max.z < k);
}

bool u3d_PolySmallerThan(CNmPoly *poly, float k)        // FIXME, gives errors in room_10_error.map
{
	S_POINT *pt;
	S_PLANE *pl;
	CNmFace *fc;
	float dist;

	// for every face...
	for (fc = poly->m_Face; fc; fc = fc->Next)
	{
		pl = fc->Plane;

		// for every point of poly that does not behave to fc...
		for (pt = poly->m_Point; pt; pt = pt->next)
		{
			// check if is it too near to fc ...
//			if (!fc->PointGetByPtr(pt))
			if (!fc->FcPtPtrGetByVecPtr(&pt->vec))
			{
				dist = pl->d - (pt->vec * pl->n);
				if (dist < k)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool u3d_PolyTouch(CNmPoly *poly1, CNmPoly *poly2, CNmFace *& fc1t, CNmFace *& fc2t, float tolerance)
{
	if (!u3d_PolyCollision_Fast(poly1, poly2, tolerance))
	{
		return false;
	}

	if (!u3d_PolyFaceInvShare(poly1, poly2, fc1t, fc2t, tolerance))
	{
		return false;
	}

	return (u3d_FaceShareFace(fc1t, fc2t, tolerance)); // || u3d_FaceToFace(fc1t,fc2t,tolerance));
}

bool u3d_PolyPlaneTouch_Fast(CNmPoly *poly, S_PLANE *pl, float tolerance)
{
	CNmVec3 min, max, origin;

	if (!poly->GetBox(min, max))
	{
		return false;
	}

	max -= min;

	poly->GetOrigin(&origin);

	return (2 * (float)fabs((origin * pl->n) - pl->d) < (max.GetLength() + tolerance));
}

bool u3d_PolyPlaneIntersect(CNmPoly *poly, S_PLANE *pl, float tolerance)  // FIXME , should be ok
{
	S_POINT *pt;
	S_POINT *ptstart;
	bool sign;
	bool inside;
	float dist;

	ptstart = NULL;

	for (pt = poly->m_Point; pt; pt = pt->next)
	{
		dist = (pt->vec * pl->n) - pl->d;

		if ((dist > tolerance) || (dist < -tolerance))
		{
			ptstart = pt->next;
			sign    = dist > 0;
			break;
		}
	}

	// rare, all points 'inside' a plane, only a THIN poly does it, and it should not exist!
	if (!pt)
	{
		#ifndef CONSOLE_MODE
			#ifdef LOG_PolyPlaneIntersect
		if (!pt)
		{
			g_LogDlg.Log("u3d_PolyPlaneIntersect: Thin poly inside a plane: ", poly->GetMsg());
		}
		g_LogDlg.Log("u3d_PolyPlaneIntersect: Poly points:");
		for (pt = poly->m_Point; pt; pt = pt->next)
		{
			g_LogDlg.Log("> ", pt->p);
		}
		g_LogDlg.Log("u3d_PolyPlaneIntersect: Plane: ", pl->n * pl->d, pl->n);
			#endif
		#endif
		return false;
	}

	if (!ptstart)   // last point reached only it is outside the plane
	{
		#ifndef CONSOLE_MODE
			#ifdef LOG_PolyPlaneIntersect
		if ((pt) && (!pt->next))
		{
			g_LogDlg.Log("u3d_PolyPlaneIntersect: one point outside: ", poly->GetMsg());
		}
		g_LogDlg.Log("u3d_PolyPlaneIntersect: Poly points:");
		for (pt = poly->m_Point; pt; pt = pt->next)
		{
			g_LogDlg.Log("> ", pt->p);
		}
		g_LogDlg.Log("u3d_PolyPlaneIntersect: Plane: ", pl->n * pl->d, pl->n);
			#endif
		#endif
		return false;
	}

	inside = true;

	for (pt = ptstart; pt; pt = pt->next)
	{
		dist = (pt->vec * pl->n) - pl->d;

		// only points 'outside' the plane
		if ((dist > tolerance) || (dist < -tolerance))
		{
			inside = false;
			if ((dist > 0) != sign)
			{
				return true;
			}
		}
	}

	return false;
}

POS u3d_PolyInsidePlane(CNmPoly *poly, S_PLANE *pl, float tolerance)  //TESTME
{
	S_POINT *pt;
	float dist;
	int pts_out = 0;
	int pts_in  = 0;
	int pts_mid = 0;

	for (pt = poly->m_Point; pt; pt = pt->next)
	{
		switch (u3d_PointInsidePlane(&pt->vec, pl, &dist, tolerance))
		{
		case (POS_OUT):
			if (pts_in != 0)
			{
				return POS_MID;             // intersect plane or all in mid (impossible)
			}
			pts_out++;
			break;
		case (POS_IN):
			if (pts_out != 0)
			{
				return POS_MID;             // intersect plane or all in mid (impossible)
			}
			pts_in++;
			break;
		case (POS_MID):
			pts_mid++;
			break;
		}
	}

	// all in mid ? impossible but never known
	if (pts_mid == poly->m_PointNum)
	{
		return POS_MID;
	}

	// some mid and out ?
	if (pts_out != 0)
	{
		return POS_OUT;
	}

	// some mid and in ?
	/*if (pts_in != 0)*/ return POS_IN;

	// out and in impossible, switch take care of it
}

bool u3d__PolyCollision_Fast(CNmPoly *poly1, CNmPoly *poly2, float tolerance)
{
	float dist = (poly1->m_pre_Origin - poly2->m_pre_Origin).GetLength();

	// ray test
	if (dist > (poly1->m_pre_Ray + poly2->m_pre_Ray + tolerance))
	{
		return false;
	}

	// box test
	return u3d_BoxCollision(&poly1->m_pre_Min, &poly1->m_pre_Max, &poly2->m_pre_Min, &poly2->m_pre_Max, tolerance);
}

bool u3d_PolyCollision_Fast(CNmPoly *poly1, CNmPoly *poly2, float tolerance)
{
	CNmVec3 min1, max1, min2, max2;

	poly1->GetBox(min1, max1);
	poly2->GetBox(min2, max2);

	return u3d_BoxCollision(&min1, &max1, &min2, &max2, tolerance);
}

bool u3d_PolyCollision(CNmPoly *poly1, CNmPoly *poly2, float tolerance)
{
	CNmPoly poly;
	S_POINT *pt;

	// poly1 point is inside poly2 ?
	for (pt = poly1->m_Point; pt; pt = pt->next)
	{
		if (u3d_PointInsideOrEqualPoly(&pt->vec, poly2, tolerance))
		{
			return true;
		}
	}

	// poly2 point is inside poly1 ?
	for (pt = poly2->m_Point; pt; pt = pt->next)
	{
		if (u3d_PointInsideOrEqualPoly(&pt->vec, poly1, tolerance))
		{
			return true;
		}
	}

	// the last resort, the slow one final test :(
	poly.FromPoly(*poly1);
	return poly.Intersect(poly2);
}

bool u3d_PolyFaceCollision(CNmPoly *poly, CNmFace *fc1, float tolerance)
{
	CNmFace *fc;
	CNmVec3 risu;
	S_FACEPOINT *item;
	int i;

	// face point inside poly
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		if (u3d_PointInsidePoly(item->vecptr, poly, tolerance))
		{
			return true;
		}
		item = item->next;
	}

	// face middle point inside poly
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		risu = (*item->vecptr + *item->next->vecptr) / 2;
		if (u3d_PointInsidePoly(&risu, poly, tolerance))
		{
			return true;
		}
		item = item->next;
	}

	// face edge intersect poly face
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		for (fc = poly->m_Face; fc; fc = fc->Next)
		{
			if (u3d_EdgeFaceIntersect(item->vecptr, item->next->vecptr, fc, &risu, tolerance))
			{
				return true;
			}
		}
		item = item->next;
	}

	// poly face edge intersect face
	// clear work flag
	for (fc = poly->m_Face; fc; fc = fc->Next)
	{
		item = fc->m_FcPtHead;
		for (i = 0; i < fc->m_NumFcPts; i++)
		{
			item->work = 0;
			item       = item->next;
		}
	}

	// intersect test
	for (fc = poly->m_Face; fc; fc = fc->Next)
	{
		item = fc->m_FcPtHead;
		for (i = 0; i < fc->m_NumFcPts; i++)
		{
			if (item->work)
			{
				item = item->next;
				continue;
			}

			item->work           = 1;
			item->nearitem->work = 1;

			if (u3d_EdgeFaceIntersect(item->vecptr, item->next->vecptr, fc1, &risu, tolerance))
			{
				return true;
			}

			item = item->next;
		}
	}

	return false;
}

bool u3d_PolyFaceInvShare(CNmPoly *poly1, CNmPoly *poly2, CNmFace *& fc1, CNmFace *& fc2, float tolerance)
{
	for (fc1 = poly1->m_Face; fc1; fc1 = fc1->Next)
	{
		for (fc2 = poly2->m_Face; fc2; fc2 = fc2->Next)
		{
			if (u3d_PlaneInverted(fc1->Plane, fc2->Plane, tolerance))
			{
				return true;
			}
		}
	}

	return false;
}

/*
Returns the number of faces that lie on the same plane
*/
int u3d_PolyFaceShare(CNmPoly *poly1, CNmPoly *poly2, float tolerance)
{
	CNmFace *fc1;
	CNmFace *fc2;
	int result = 0;

	for (fc1 = poly1->m_Face; fc1; fc1 = fc1->Next)
	{
		for (fc2 = poly2->m_Face; fc2; fc2 = fc2->Next)
		{
			if (u3d_PlaneCompare(fc1->Plane, fc2->Plane, tolerance))
			{
				result++;
			}
		}
	}

	return result;
}

bool u3d_PolyInsideOrEqualPoly(CNmPoly *poly1, CNmPoly *poly2, float tolerance)
{
	S_POINT *pt;
	for (pt = poly1->m_Point; pt; pt = pt->next)
	{
		if (!u3d_PointInsideOrEqualPoly(&pt->vec, poly2, tolerance))
		{
			return false;
		}
	}
	return true;
}

bool u3d_PolyInsidePrismFace(CNmPoly *poly1, CNmFace *fc1, float tolerance)   //FIXME
{
	return false;
}

float u3d_det(CNmVec3 *v0, CNmVec3 *v1, CNmVec3 *v2)
{
//	DETBYNmVec3(va,vb,vc,a);

	return (v0->x * (v1->y * v2->z - v1->z * v2->y) +
	        v0->y * (v1->z * v2->x - v1->x * v2->z) +
	        v0->z * (v1->x * v2->y - v1->y * v2->x));
}

/*
    WARNING:	using u3d_Angle to determine a comparision, could give 2 planes equals even if
                are one the inverse of the other (angle 0 = angle 180 for acos())
*/
bool u3d_PlaneCompare(S_PLANE *pl1, S_PLANE *pl2, float tolerance)    // FIXME, fixed
{
	return
	    ((float)fabs(pl1->d - pl2->d) < tolerance) &&       // poca distanza tra le distanze
	    u3d_VecCompare_All(&pl1->n, &pl2->n, tolerance) ;   // poco angolo (= poca distanza tra le normali!)
}

void u3d_PlaneInvert(S_PLANE *pl)
{
	pl->n.Invert();
	pl->d = -pl->d;
}

bool u3d_PlaneInverted(S_PLANE *pl1, S_PLANE *pl2, float tolerance)
{
	S_PLANE pl = *pl1;

//	u3d_PlaneInvert(&pl);
	pl.n.Invert();
	pl.d = -pl.d;

	return u3d_PlaneCompare(&pl, pl2, tolerance);
}

bool u3d_PlanesToPoint(S_PLANE *pl1, S_PLANE *pl2, S_PLANE *pl3, CNmVec3 *v)
{
/*
    CNmVec3 n1(pl1->n);
    CNmVec3 n2(pl2->n);
    CNmVec3 n3(pl3->n);
    float delta;
    float x,y,z;
    DETBYNmVec3(n1,n2,n3,delta);
    if (delta==0) return false;
    delta = 1.f / delta;
    CNmVec3 q1( pl1->d, pl1->n.y, pl1->n.z);
    CNmVec3 q2( pl2->d, pl2->n.y, pl2->n.z);
    CNmVec3 q3( pl3->d, pl3->n.y, pl3->n.z);
    DETBYNmVec3(q1,q2,q3,x);
    v->x = x * delta;
    q1.Set( pl1->n.x, pl1->d, pl1->n.z);
    q2.Set( pl2->n.x, pl2->d, pl2->n.z);
    q3.Set( pl3->n.x, pl3->d, pl3->n.z);
    DETBYNmVec3(q1,q2,q3,y);
    v->y = y * delta;
    q1.Set( pl1->n.x, pl1->n.y, pl1->d);
    q2.Set( pl2->n.x, pl2->n.y, pl2->d);
    q3.Set( pl3->n.x, pl3->n.y, pl3->d);
    DETBYNmVec3(q1,q2,q3,z);
    v->z = z * delta;
    return true;
*/

	float delta = u3d_det(&pl1->n, &pl2->n, &pl3->n);
	if (delta == 0)
	{
		return false;
	}
	delta = 1.f / delta;
	CNmVec3 q1(pl1->d, pl1->n.y, pl1->n.z);
	CNmVec3 q2(pl2->d, pl2->n.y, pl2->n.z);
	CNmVec3 q3(pl3->d, pl3->n.y, pl3->n.z);
	v->x = u3d_det(&q1, &q2, &q3) * delta;
	q1.Set(pl1->n.x, pl1->d, pl1->n.z);
	q2.Set(pl2->n.x, pl2->d, pl2->n.z);
	q3.Set(pl3->n.x, pl3->d, pl3->n.z);
	v->y = u3d_det(&q1, &q2, &q3) * delta;
	q1.Set(pl1->n.x, pl1->n.y, pl1->d);
	q2.Set(pl2->n.x, pl2->n.y, pl2->d);
	q3.Set(pl3->n.x, pl3->n.y, pl3->d);
	v->z = u3d_det(&q1, &q2, &q3) * delta;
	return true;

}

void u3d_PlanePointProjection(S_PLANE *pl, CNmVec3 *v, CNmVec3 *risu)
{
	*risu = pl->n * (pl->d - (*v * pl->n)) + *v;
}

/*
Distance can be negative (plane behind the ray)
*/
bool u3d_PlaneRayIntersection(S_PLANE *pl, CNmVec3 *v, CNmVec3 *dir, CNmVec3 *risu, float *distance)
{
	float dot = *dir * pl->n;

	if (fabs(dot) < WORLD_GRID)
	{
		return false;
	}

	*distance = (pl->d - (*v * pl->n)) / dot;

	*risu = *v + *dir * *distance;  // TESTME

	return true;
}

/*
Must be coplanar
*/
bool u3d_RayIntersection(CNmVec3 *a, CNmVec3 *adir, CNmVec3 *b, CNmVec3 *bdir, CNmVec3 *risu, float *distance)
{
/*	float da,db;

    da = *a * *adir;
    db = *b * *bdir;
*/

	return false;
}

// are two faces one appicciata all'altra ?
// face are assumed to be on the same plane (use u3d_PolyPlaneInvShare(...) first)
// but if they share all points, they obviously share the plane too.
bool u3d_FaceToFace(CNmFace *fc1, CNmFace *fc2, float tolerance)
{
	bool stop;

	// m_NumFcPts test
	if (fc1->m_NumFcPts != fc2->m_NumFcPts)
	{
		return false;
	}

	// Planes Inverted test (not done)


	S_FACEPOINT *start2;
	S_FACEPOINT *start1;
	S_FACEPOINT *item1;
	S_FACEPOINT *item2;

	start1 = fc1->m_FcPtHead;
	start2 = fc2->FcPtPtrGetByVec(start1->vecptr);

	if (!start2)
	{
		return false;
	}

	item2 = start2->prev;   // PREV !!! item2 va all'indietro, item1 in avanti , le faccie sono opposte.
	stop  = false;
	for (item1 = start1->next; !stop && item1; stop = (item1 = item1->next) == start1)
	{
		if (!u3d_VecCompare_All(item1->vecptr, item2->vecptr, tolerance))
		{
			return false;
		}

		item2 = item2->prev;
	}

	return true;
}

/*
Note:
the two faces must be on the same plane and must have the normal inverted,
that is a face must see the other face. (use u3d_PolyPlaneInvShare(pl1,pl2,fc1,fc2,t))
*/
bool u3d__FaceShareFace(CNmFace *fc1, CNmFace *fc2, float tolerance)
{
	CNmVec3 v;
	S_FACEPOINT *item;
	S_FACEPOINT *item1;
	S_FACEPOINT *item2;
	int i, i1, i2;

	// face origin inside check
	if (u3d_PointInsideOrEqualFace(&fc1->m_pre_Origin, fc2, tolerance))
	{
		return true;
	}

	if (u3d_PointInsideOrEqualFace(&fc2->m_pre_Origin, fc1, tolerance))
	{
		return true;
	}

	// face point inside check
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		if (u3d_PointInsideFace(item->vecptr, fc2, tolerance))
		{
			return true;
		}

		item = item->next;
	}

	item = fc2->m_FcPtHead;
	for (i = 0; i < fc2->m_NumFcPts; i++)
	{
		if (u3d_PointInsideFace(item->vecptr, fc1, tolerance))
		{
			return true;
		}

		item = item->next;
	}

	// edge middle point inside check
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		v = (*item->vecptr + *item->next->vecptr) / 2;      // FIXME , precalculate this

		if (u3d_PointInsideFace(&v, fc2, tolerance))
		{
			return true;
		}

		item = item->next;
	}

	item = fc2->m_FcPtHead;
	for (i = 0; i < fc2->m_NumFcPts; i++)
	{
		v = (*item->vecptr + *item->next->vecptr) / 2;      // FIXME , precalculate this

		if (u3d_PointInsideFace(&v, fc1, tolerance))
		{
			return true;
		}

		item = item->next;
	}

	// edge intersect check
	item1 = fc1->m_FcPtHead;
	for (i1 = 0; i1 < fc1->m_NumFcPts; i1++)
	{

		item2 = fc2->m_FcPtHead;
		for (i2 = 0; i2 < fc2->m_NumFcPts; i2++)
		{
			if (u3d_EdgeIntersect(item1, item2, tolerance))
			{
				return true;
			}

			item2 = item2->next;
		}

		item1 = item1->next;
	}

	return false;
}

/*
Note:
the two faces must be on the same plane and must have the normal inverted,
that is a face must see the other face. (use u3d_PolyPlaneInvShare(pl1,pl2,fc1,fc2,t))
*/
bool u3d_FaceShareFace(CNmFace *fc1, CNmFace *fc2, float tolerance)
{
	S_FACEPOINT *item;
	S_FACEPOINT *item1;
	S_FACEPOINT *item2;
	int i, i1, i2;
	CNmVec3 outn, v;

	// face origin inside check
	v = fc1->GetOrigin();
	if (u3d_PointInsideOrEqualFace(&v, fc2, tolerance))
	{
		return true;
	}

	v = fc2->GetOrigin();
	if (u3d_PointInsideOrEqualFace(&v, fc1, tolerance))
	{
		return true;
	}

	// face point inside check
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		if (u3d_PointInsideFace(item->vecptr, fc2, tolerance))
		{
			return true;
		}

		item = item->next;
	}

	item = fc2->m_FcPtHead;
	for (i = 0; i < fc2->m_NumFcPts; i++)
	{
		if (u3d_PointInsideFace(item->vecptr, fc1, tolerance))
		{
			return true;
		}

		item = item->next;
	}

	// edge middle point inside check
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		v = (*item->vecptr + *item->next->vecptr) / 2;      // FIXME , precalculate this

		if (u3d_PointInsideFace(&v, fc2, tolerance))
		{
			return true;
		}

		item = item->next;
	}

	item = fc2->m_FcPtHead;
	for (i = 0; i < fc2->m_NumFcPts; i++)
	{
		v = (*item->vecptr + *item->next->vecptr) / 2;      // FIXME , precalculate this

		if (u3d_PointInsideFace(&v, fc1, tolerance))
		{
			return true;
		}

		item = item->next;
	}

	// edge intersect check
	item1 = fc1->m_FcPtHead;
	for (i1 = 0; i1 < fc1->m_NumFcPts; i1++)
	{

		item2 = fc2->m_FcPtHead;
		for (i2 = 0; i2 < fc2->m_NumFcPts; i2++)
		{
			if (u3d_EdgeIntersect(item1, item2, tolerance))
			{
				return true;
			}

			item2 = item2->next;
		}

		item1 = item1->next;
	}

	return false;
}

bool u3d_FaceInsideOrEqualPoly(CNmFace *fc1, CNmPoly *poly1, float tolerance)
{
	S_FACEPOINT *item;
	int i;

	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{

		if (!u3d_PointInsideOrEqualPoly(item->vecptr, poly1, tolerance))
		{
			return false;
		}

		item = item->next;
	}

	return true;
}

bool u3d_FaceInsidePoly(CNmFace *fc1, CNmPoly *poly1, float tolerance)
{
	S_FACEPOINT *item;
	int i;

	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{

		if (!u3d_PointInsidePoly(item->vecptr, poly1, tolerance))
		{
			return false;
		}

		item = item->next;
	}

	return true;
}

/*
Arbitrary oriented faces intersection
*/
bool u3d_FaceIntersect(CNmFace *fc1, CNmFace *fc2, float tolerance)
{
	CNmVec3 ea1, ea2, eb1, eb2, dir;
	S_FACEPOINT *item1;
	S_FACEPOINT *item2;
	S_FACEPOINT *itema1;
	S_FACEPOINT *itema2;
	S_FACEPOINT *itemb1;
	S_FACEPOINT *itemb2;
	float a1, a2, b1, b2, min, max, work;
	int i;
	bool got1;
	bool got2;

	itema1 = NULL;
	itema2 = NULL;
	itemb1 = NULL;
	itemb2 = NULL;

	// parallel planes ? if so return false
	if (u3d_VecCompare(&fc1->Plane->n, &fc2->Plane->n, tolerance))
	{
		return false;
	}

	got1 = false;
	got2 = false;

	//	Now calulating edge ea1-ea2 resulting from the intersection from
	//  fc1 with fc2->Plane

	// getting ea1 ...
	item1 = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{

		if (fabs((*item1->vecptr * fc2->Plane->n) - fc2->Plane->d) < tolerance)
		{
			ea1    = *item1->vecptr;
			itema1 = item1;
			got1   = true;
			break;
		}
		else
		if (u3d_EdgePlaneIntersect(item1->vecptr, item1->next->vecptr, fc2->Plane, &ea1, tolerance))
		{
			got1 = true;
			break;
		}

		item1 = item1->next;
	}

	if (!got1)
	{
		return false;
	}

	// getting ea2 ...
	item1 = item1->next;
	for (i = i + 1; i < fc1->m_NumFcPts; i++)
	{

		if (fabs((*item1->vecptr * fc2->Plane->n) - fc2->Plane->d) < tolerance)
		{
			ea2    = *item1->vecptr;
			itema2 = item1;
			got2   = true;
			break;
		}
		else
		if (u3d_EdgePlaneIntersect(item1->vecptr, item1->next->vecptr, fc2->Plane, &ea2, tolerance))
		{
			got2 = true;
			break;
		}

		item1 = item1->next;
	}

	if (!got2)
	{
		return false;
	}

	// case of edge inside plane
	if (((itema1 != NULL) && (itema2 != NULL)) &&
	    ((itema1->next == itema2) || (itema2->next == itema1))
	    )
	{
		return false;
	}

	got1 = false;
	got2 = false;

	//	Now calulating edge eb1-eb2 resulting from the intersection from
	//  fc2 with fc1->Plane

	// getting eb1 ...
	item2 = fc2->m_FcPtHead;
	for (i = 0; i < fc2->m_NumFcPts; i++)
	{

		if (fabs((*item2->vecptr * fc1->Plane->n) - fc1->Plane->d) < tolerance)
		{
			eb1    = *item2->vecptr;
			itemb1 = item2;
			got1   = true;
			break;
		}
		else
		if (u3d_EdgePlaneIntersect(item2->vecptr, item2->next->vecptr, fc1->Plane, &eb1, tolerance))
		{
			got1 = true;
			break;
		}

		item2 = item2->next;
	}

	if (!got1)
	{
		return false;
	}

	// getting eb2 ...
	item2 = item2->next;
	for (i = i + 1; i < fc2->m_NumFcPts; i++)
	{

		if (fabs((*item2->vecptr * fc1->Plane->n) - fc1->Plane->d) < tolerance)
		{
			eb2    = *item2->vecptr;
			itemb2 = item2;
			got2   = true;
			break;
		}
		else
		if (u3d_EdgePlaneIntersect(item2->vecptr, item2->next->vecptr, fc1->Plane, &eb2, tolerance))
		{
			got2 = true;
			break;
		}

		item2 = item2->next;
	}

	if (!got2)
	{
		return false;
	}

	// case of edge inside plane
	if (((itemb1 != NULL) && (itemb2 != NULL)) &&
	    ((itemb1->next == itemb2) || (itemb2->next == itemb1))
	    )
	{
		return false;
	}

	// projecting along direction of intersecting line

	dir = ea2 - ea1;
	dir.Normalize();

	work = ea1 * dir;

	a1 = 0;
	a2 = (ea2 * dir) - work;
	b1 = (eb1 * dir) - work;
	b2 = (eb2 * dir) - work;

	min = u1d_Min(a1, u1d_Min(a2, u1d_Min(b1, b2)));
	max = u1d_Max(a1, u1d_Max(a2, u1d_Max(b1, b2)));

	if ((max - min + tolerance) < (fabs(a2 - a1) + fabs(b2 - b1)))
	{
		return true;
	}

	return false;

}

/*
Note:
the two faces must be on the same plane and must have the normal inverted,
that is a face must see the other face. (use u3d_PolyPlaneInvShare(pl1,pl2,fc1,fc2,t))
*/
bool u3d_FaceInsideFace(CNmFace *fc1, CNmFace *fc2, float tolerance)
{
	S_FACEPOINT *item;
	int i;

	// face point inside check
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		if (!u3d_PointInsideFace(item->vecptr, fc2, tolerance))
		{
			return false;
		}

		item = item->next;
	}

	return true;
}

/*
Note:
the two faces must be on the same plane and must have the normal inverted,
that is a face must see the other face. (use u3d_PolyPlaneInvShare(pl1,pl2,fc1,fc2,t))
is not the same of FaceToFace(here some points can be inside, others in the boundary)
*/
bool u3d_FaceInsideOrEqualFace(CNmFace *fc1, CNmFace *fc2, float tolerance)
{
	S_FACEPOINT *item;
	int i;

	// face point inside check
	item = fc1->m_FcPtHead;
	for (i = 0; i < fc1->m_NumFcPts; i++)
	{
		if (!u3d_PointInsideOrEqualFace(item->vecptr, fc2, tolerance))
		{
			return false;
		}

		item = item->next;
	}

	return true;
}

/*
Plane normal points outside
*/
POS u3d_PointInsidePlane(CNmVec3 *pt, S_PLANE *pl, float *distance, float tolerance)
{
	*distance = (*pt * pl->n) - pl->d;
	if (*distance < -tolerance)
	{
		return POS_IN;
	}
	if (*distance > tolerance)
	{
		return POS_OUT;
	}
	return POS_MID;
}


bool u3d_PointInPlanes(CNmVec3 *pt, S_PLANE *plane, float tolerance)
{
	S_PLANE *pl;
	float dist;

	for (pl = plane; pl; pl = pl->next)
	{
		if (u3d_PointInsidePlane(pt, pl, &dist, tolerance) == POS_OUT)
		{
			return false;
		}
	}
	return true;
}

bool u3d_PointInsideOrEqualPoly(CNmVec3 *p, CNmPoly *poly, float tolerance)
{
	CNmFace *fc;
	float dist;

	for (fc = poly->m_Face; fc; fc = fc->Next)
	{
		if (u3d_PointInsidePlane(p, fc->Plane, &dist, tolerance) == POS_OUT)
		{
			return false;
		}
	}

	return true;
}

bool u3d_PointInsidePoly(CNmVec3 *p, CNmPoly *poly, float tolerance)
{
	CNmFace *fc;
	float dist;

	for (fc = poly->m_Face; fc; fc = fc->Next)
	{
		if (u3d_PointInsidePlane(p, fc->Plane, &dist, tolerance) != POS_IN)
		{
			return false;
		}
	}

	return true;
}
/*
int u3d_SegmentPlaneClip(CNmVec3 &a,CNmVec3 &b,S_PLANE *plane)
{
    float _a,_b,delta;
    CNmVec3 ap,ba,v;
    S_PLANE aux;

//	aux.p = plane->p;
//	aux.p = plane->p;
    aux.n = plane->n;
    aux.p += aux.n;
    v = a - aux.p;
    ap = v;
    _a = v*aux.n;
    v = b - aux.p;
    _b = v*aux.n;

    if ((_a>0)&&(_b>0)) return 1;
    if ((_a<=0)&&(_b<=0)) return 1; // full clip

    v = b - a;
    ba = v;
    delta = v*aux.n;

// if (delta==0) error("delta=0");

    ap= -ap;
    delta = (ap*aux.n)/delta;
    ba*=delta;
    v=a+ba;

    if (_a<0) a=v;
    if (_b<0) b=v;

    return 1;
}
*/

bool u3d__PointInsideOrEqualFace(CNmVec3 *v, CNmFace *f, float tolerance)
{
	S_FACEPOINT *item;
	int i;

	item = f->m_FcPtHead;

	for (i = 0; i < f->m_NumFcPts; i++)
	{

		if (((*v - *item->vecptr) * item->pre_dirout) > tolerance)
		{
			return false;
		}

		item = item->next;
	} // for

	return true;
}

bool u3d_PointInsideOrEqualFace(CNmVec3 *v, CNmFace *f, float tolerance)
{
	S_FACEPOINT *item;
	CNmVec3 dirout;
	int i;

	item = f->m_FcPtHead;

	for (i = 0; i < f->m_NumFcPts; i++)
	{
		dirout = (*item->next->vecptr - *item->vecptr) ^ f->Plane->n;
		dirout.Normalize();

		if (((*v - *item->vecptr) * dirout) > tolerance)
		{
			return false;
		}

		item = item->next;
	} // for

	return true;
}

//
// differs only by the sign tolerance + and -. could be done only 1 with a 'soglia' that can be + or -
//
bool u3d__PointInsideFace(CNmVec3 *v, CNmFace *f, float tolerance)
{
	S_FACEPOINT *item;
	int i;

	item = f->m_FcPtHead;

	for (i = 0; i < f->m_NumFcPts; i++)
	{

		if (((*v - *item->vecptr) * item->pre_dirout) > -tolerance)
		{
			return false;
		}

		item = item->next;
	} // for

	return true;
}

bool u3d_PointInsideFace(CNmVec3 *v, CNmFace *f, float tolerance)
{
	S_FACEPOINT *item;
	CNmVec3 dirout;
	int i;

	item = f->m_FcPtHead;

	for (i = 0; i < f->m_NumFcPts; i++)
	{
		dirout = (*item->next->vecptr - *item->vecptr) ^ f->Plane->n;
		dirout.Normalize();

		if (((*v - *item->vecptr) * dirout) > -tolerance)
		{
			return false;
		}

		item = item->next;
	} // for

	return true;
}

/*
Must lie on the same plane
*/
bool u3d__EdgeIntersect(S_FACEPOINT *item1, S_FACEPOINT *item2, float tolerance)
{
	float dist_a1;
	float dist_b1;
	float dist_a2;
	float dist_b2;
	bool sign_a1;
	bool sign_b1;
	bool sign_a2;
	bool sign_b2;

	//////

	dist_a2 = (*item2->vecptr       - *item1->vecptr) * item1->pre_dirout;
	dist_b2 = (*item2->next->vecptr - *item1->vecptr) * item1->pre_dirout;

	if ((float)fabs(dist_a2) < tolerance)
	{
		return false;
	}
	if ((float)fabs(dist_b2) < tolerance)
	{
		return false;
	}

	sign_a2 = dist_a2 > 0;
	sign_b2 = dist_b2 > 0;

	if (sign_a2 == sign_b2)
	{
		return false;
	}

	/////

	dist_a1 = (*item1->vecptr       - *item2->vecptr) * item2->pre_dirout;
	dist_b1 = (*item1->next->vecptr - *item2->vecptr) * item2->pre_dirout;

	if ((float)fabs(dist_a1) < tolerance)
	{
		return false;
	}
	if ((float)fabs(dist_b1) < tolerance)
	{
		return false;
	}

	sign_a1 = dist_a1 > 0;
	sign_b1 = dist_b1 > 0;

	if (sign_a1 == sign_b1)
	{
		return false;
	}

	//////

	return true;
}

bool u3d_EdgeIntersect(S_FACEPOINT *item1, S_FACEPOINT *item2, float tolerance)
{
	float dist_a1;
	float dist_b1;
	float dist_a2;
	float dist_b2;
	bool sign_a1;
	bool sign_b1;
	bool sign_a2;
	bool sign_b2;
	CNmVec3 dirout, n;

	n = (*item1->next->vecptr - *item1->vecptr) ^ (*item2->next->vecptr - *item2->vecptr);
	n.Normalize();

	//////

	dirout = (*item1->next->vecptr - *item1->vecptr) ^ n;
	dirout.Normalize();

	dist_a2 = (*item2->vecptr       - *item1->vecptr) * dirout;
	dist_b2 = (*item2->next->vecptr - *item1->vecptr) * dirout;

	if ((float)fabs(dist_a2) < tolerance)
	{
		return false;
	}
	if ((float)fabs(dist_b2) < tolerance)
	{
		return false;
	}

	sign_a2 = dist_a2 > 0;
	sign_b2 = dist_b2 > 0;

	if (sign_a2 == sign_b2)
	{
		return false;
	}

	/////

	dirout = (*item2->next->vecptr - *item2->vecptr) ^ n;
	dirout.Normalize();

	dist_a1 = (*item1->vecptr       - *item2->vecptr) * dirout;
	dist_b1 = (*item1->next->vecptr - *item2->vecptr) * dirout;

	if ((float)fabs(dist_a1) < tolerance)
	{
		return false;
	}
	if ((float)fabs(dist_b1) < tolerance)
	{
		return false;
	}

	sign_a1 = dist_a1 > 0;
	sign_b1 = dist_b1 > 0;

	if (sign_a1 == sign_b1)
	{
		return false;
	}

	//////

	return true;
}

bool u3d_EdgePlaneIntersect(CNmVec3 *a, CNmVec3 *b, S_PLANE *pl, CNmVec3 *risu, float tolerance)
{
	float dummy;
	float dist_a = (*a * pl->n) - pl->d;
	float dist_b = (*b * pl->n) - pl->d;

	if ((fabs(dist_a) < tolerance) || (fabs(dist_b) < tolerance))       // inside the plane
	{
		return false;
	}

	if ((dist_a > 0) == (dist_b > 0))       // same space half (no intersection)
	{
		return false;
	}

	CNmVec3 dir;

	dir = *b - *a;
	dir.Normalize();

	return u3d_PlaneRayIntersection(pl, a, &dir, risu, &dummy);
}

bool u3d_EdgeFaceIntersect(CNmVec3 *a, CNmVec3 *b, CNmFace *fc, CNmVec3 *risu, float tolerance)
{
	CNmVec3 p;

	if (!u3d_EdgePlaneIntersect(a, b, fc->Plane, &p, tolerance))
	{
		return false;
	}

	return u3d_PointInsideFace(&p, fc, tolerance);
}

bool u3d_PlaneCollideTray(S_PLANE& pl, CNmVec3& start, CNmVec3& dir, CNmVec3& result)
{
	float k = dir * pl.n;
	if (k == 0.0f)
	{
		return false;
	}
	result = start + dir * (pl.d - (start * pl.n)) / k;
	return true;
}

bool u3d_SquareFaceRayIntersection(CNmVec3& a, CNmVec3& b, CNmVec3& c, CNmVec3& d, CNmVec3& eye, CNmVec3& ray, CNmVec3& risu, float& distance)
{
	S_PLANE plane;
	CNmVec3 ra, ba, cb, dc, ad, ain, bin, cin, din;
	CNmVec3 p;
	float dist;

	ra = a - eye;

	plane.n = (c - a) ^ (b - a);
	plane.n.Normalize();
	plane.d = plane.n * a;

	// behind
	if ((plane.n * ray) < 0)
	{
		return false;
	}

	if (!u3d_PlaneRayIntersection(&plane, &eye, &ray, &p, &dist))
	{
		return false;
	}

	ba = b - a;
	cb = c - b;
	dc = d - c;
	ad = a - d;

	ain = ba ^ plane.n;
	bin = cb ^ plane.n;
	cin = dc ^ plane.n;
	din = ad ^ plane.n;

	// is inside?
	if ((((p - a) * ain) > 0) && (((p - b) * bin) > 0) && (((p - c) * cin) > 0) && (((p - d) * din) > 0))
	{
		risu     = p;
		distance = dist;
		return true;
	}

	return false;
}

bool u3d_BoxRayIntersection(CNmVec3& vMin, CNmVec3& vMax, CNmVec3& eye, CNmVec3& ray, CNmVec3& risu, float& distance)
{
	CNmVec3 a, b, c, d;

	//TopFace
	a.Set(vMin.x, vMin.y, vMax.z);
	b.Set(vMin.x, vMax.y, vMax.z);
	c = vMax;
	d.Set(vMax.x, vMin.y, vMax.z);

	if (u3d_SquareFaceRayIntersection(a, b, c, d, eye, ray, risu, distance))
	{
		return true;
	}

	//BottomFace
	a.Set(vMax.x, vMin.y, vMin.z);
	b.Set(vMax.x, vMax.y, vMin.z);
	c.Set(vMin.x, vMax.y, vMin.z);
	d = vMin;

	if (u3d_SquareFaceRayIntersection(a, b, c, d, eye, ray, risu, distance))
	{
		return true;
	}

	//XZ (0,-1,0)
	a.Set(vMax.x, vMin.y, vMin.z);
	b.Set(vMax.x, vMax.y, vMin.z);
	c.Set(vMin.x, vMax.y, vMin.z);
	d = vMin;

	if (u3d_SquareFaceRayIntersection(a, b, c, d, eye, ray, risu, distance))
	{
		return true;
	}

	// YZ (-1,0,0)
	a = vMin;
	b.Set(vMin.x, vMax.y, vMin.z);
	c.Set(vMin.x, vMax.y, vMax.z);
	d.Set(vMin.x, vMin.y, vMax.z);

	if (u3d_SquareFaceRayIntersection(a, b, c, d, eye, ray, risu, distance))
	{
		return true;
	}

	//XZ (0,1,0)
	a.Set(vMin.x, vMax.y, vMin.z);
	b.Set(vMax.x, vMax.y, vMin.z);
	c = vMax;
	d.Set(vMin.x, vMax.y, vMax.z);

	if (u3d_SquareFaceRayIntersection(a, b, c, d, eye, ray, risu, distance))
	{
		return true;
	}

	// YZ (1,0,0)
	a.Set(vMax.x, vMax.y, vMin.z);
	b.Set(vMax.x, vMin.y, vMin.z);
	c.Set(vMax.x, vMin.y, vMax.z);
	d = vMax;

	if (u3d_SquareFaceRayIntersection(a, b, c, d, eye, ray, risu, distance))
	{
		return true;
	}

	return false;
}

void u3d_glCube(float x, float y, float z, float sideX, float sideY, float height)
{
	float sx  = sideX / 2;
	float sxx = -sx;
	float sy  = sideY / 2;
	float syy = -sy;
	float hh  = height;

	glPushMatrix();

	glTranslated(x, y, z);

	glBegin(GL_QUADS);
	glVertex3d(sx, sy, hh);
	glVertex3d(sxx, sy, hh);
	glVertex3d(sxx, syy, hh);
	glVertex3d(sx, syy, hh);

	glVertex3d(sxx, sy, 0);
	glVertex3d(sx, sy, 0);
	glVertex3d(sx, syy, 0);
	glVertex3d(sxx, syy, 0);

	glVertex3d(sx, sy, 0);
	glVertex3d(sx, sy, hh);
	glVertex3d(sx, syy, hh);
	glVertex3d(sx, syy, 0);

	glVertex3d(sxx, syy, 0);
	glVertex3d(sxx, syy, hh);
	glVertex3d(sxx, sy, hh);
	glVertex3d(sxx, sy, 0);

	glVertex3d(sxx, syy, 0);
	glVertex3d(sx, syy, 0);
	glVertex3d(sx, syy, hh);
	glVertex3d(sxx, syy, hh);

	glVertex3d(sxx, sy, hh);
	glVertex3d(sx, sy, hh);
	glVertex3d(sx, sy, 0);
	glVertex3d(sxx, sy, 0);
	glEnd();

	glPopMatrix();
}

void u3d_glCube(CNmVec3& min, CNmVec3& max)
{
	u3d_glCube((max.x + min.x) / 2, (max.y + min.y) / 2, min.z, max.x - min.x, max.y - min.y, max.z - min.z);
}

///////////////////////////////
//// MISCELLANEOUS 3D UTILITIES - END
///////////////////////////////
