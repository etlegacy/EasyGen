// Tris.h: interface for the CTris class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIS_H__359BEB9A_E938_46BA_A429_5F3153B55665__INCLUDED_)
#define AFX_TRIS_H__359BEB9A_E938_46BA_A429_5F3153B55665__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EasyGen.h"
////////
#include "Nemo3D.h"
#include "NmVec3.h"

class CTris
{
public:
	CTris();
	virtual ~CTris();

	void RotateTo(CTris *trg);
	bool PtInTris(float x, float y);
	void DrawAlphaGL(float a_alpha, float b_alpha, float c_alpha);
	void GetPlane(S_PLANE *plane);

	bool CalcFlat();
	bool CalcSlope(float angle_limit);

	bool GetFlat();
	bool GetSlope();

	float       *m_vx[3];
	float       *m_vy[3];
	int         m_vk[3];
	float       m_vz[3];
	float       *m_vs[3];
	float       *m_vt[3];
	S_COLORDATA *m_vcol[3];

	float       *m_steps;
	float       *m_stept;
	CNmVec3     m_Normal;
	void        *m_pFace;       // (S_FACEDATAEX*)
	S_COLORDATA m_Color;        // uso m_g così vanno giù
	bool        m_bFlat;        // flat?
	bool        m_bSlope;       // steep?
	bool        m_bExcluded;    // export it?

	void    SetVertex(int v, float *x, float *y, int k, float *s, float *t, S_COLORDATA *col);
	void    SetColor(float, float, float);
	void    SetMisc(float *steps, float *stept, void *pFace);
	void    SetExcluded(bool value)
	{
		m_bExcluded = value;
	}
	bool    GetExcluded()
	{
		return m_bExcluded;
	}
	void *GetFacePtr()
	{
		return m_pFace;
	}
	void    CalcNormal();
	void    SetZ(float az, float bz, float cz);
};

#endif // !defined(AFX_TRIS_H__359BEB9A_E938_46BA_A429_5F3153B55665__INCLUDED_)
