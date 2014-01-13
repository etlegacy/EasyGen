// Tris.cpp: implementation of the CTris class.
//
//////////////////////////////////////////////////////////////////////

#include "EasyGen.h"
#include "Tris.h"

#include "Nemo3D.h"
#include "NmVec2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTris::CTris()
{
	m_vx[0]     = m_vy[0] = m_vx[1] = m_vy[1] = m_vx[2] = m_vy[2] = NULL;
	m_pFace     = NULL;
	m_bExcluded = false;
}

CTris::~CTris()
{

}

void CTris::SetVertex(int v, float *x, float *y, int k, float *s, float *t, S_COLORDATA *col)
{
	m_vx[v] = x;
	m_vy[v] = y;
	m_vk[v] = k;

	m_vs[v] = s;
	m_vt[v] = t;

	m_vcol[v] = col;
}

void CTris::SetZ(float az, float bz, float cz)
{
	m_vz[0] = az;
	m_vz[1] = bz;
	m_vz[2] = cz;
}

void CTris::CalcNormal()
{
	CNmVec3 a, b, c;

	a.Set(*m_vx[0], *m_vy[0], m_vz[0]);
	b.Set(*m_vx[1], *m_vy[1], m_vz[1]);
	c.Set(*m_vx[2], *m_vy[2], m_vz[2]);

	m_Normal = (b - a) ^ (c - a);
}

void CTris::SetColor(float r, float g, float b)
{
	m_Color.colR = r;
	m_Color.colG = g;
	m_Color.colB = b;
}

void CTris::SetMisc(float *steps, float *stept, void *pFace)
{
	m_steps = steps;
	m_stept = stept;
	m_pFace = pFace;
}

void CTris::DrawAlphaGL(float a_alpha, float b_alpha, float c_alpha)
{
	glBegin(GL_TRIANGLES);

//	glColor4f(1.f,1.f,1.f,a_alpha);
	glColor4f(m_Color.colR, m_Color.colG, m_Color.colB, a_alpha);
	glTexCoord2f(*m_vs[0], *m_vt[0]);
	glVertex3f(*m_vx[0], *m_vy[0], m_vz[0]);

//	glColor4f(1.f,1.f,1.f,b_alpha);
	glColor4f(m_Color.colR, m_Color.colG, m_Color.colB, b_alpha);
	glTexCoord2f(*m_vs[1], *m_vt[1]);
	glVertex3f(*m_vx[1], *m_vy[1], m_vz[1]);

//	glColor4f(1.f,1.f,1.f,c_alpha);
	glColor4f(m_Color.colR, m_Color.colG, m_Color.colB, c_alpha);
	glTexCoord2f(*m_vs[2], *m_vt[2]);
	glVertex3f(*m_vx[2], *m_vy[2], m_vz[2]);

	glEnd();
}

void CTris::GetPlane(S_PLANE *plane)
{
	CNmVec3 a, b, c;

	a.Set(*m_vx[0], *m_vy[0], m_vz[0]);
	b.Set(*m_vx[1], *m_vy[1], m_vz[1]);
	c.Set(*m_vx[2], *m_vy[2], m_vz[2]);

	plane->n = (c - a) ^ (b - a);
	plane->n.Normalize();
	plane->d = plane->n * a;
}

// Grid->UpdZ required
bool CTris::CalcFlat()
{
	float delta = 2.f;
	return m_bFlat = ((fabs(m_vz[0] - m_vz[1]) < delta) && (fabs(m_vz[1] - m_vz[2]) < delta) && (fabs(m_vz[2] - m_vz[0]) < delta));
}

// m_Normal is required to be valid
// a = 90..0
// b = 90 - a = 0..90
bool CTris::CalcSlope(float angle_limit)
{
	CNmVec3 v2 = m_Normal;

	if ((m_Normal.x == 0.f) && (m_Normal.y == 0.f))
	{
		return m_bSlope = false;
	}

	v2.z = 0.0f;

	// FIXME
	return m_bSlope = (90.f - u_rad2deg(u3d_Angle(&m_Normal, &v2))) > angle_limit;
}

bool CTris::GetFlat()
{
	return m_bFlat;
}

bool CTris::GetSlope()
{
	return m_bSlope;
}

bool CTris::PtInTris(float x, float y)
{
	CNmVec2 a(*m_vx[0], *m_vy[0]);
	CNmVec2 b(*m_vx[1], *m_vy[1]);
	CNmVec2 c(*m_vx[2], *m_vy[2]);

	CNmVec2 p(x, y);

	if ((((p - a) ^ (b - a)) < 0.01f) && (((p - b) ^ (c - b)) < 0.01f) && (((p - c) ^ (a - c)) < 0.01f))
	{
		return true;
	}

	return false;
}

void CTris::RotateTo(CTris *trg)
{
	// a = b
	m_vcol[0] = m_vcol[1];
	m_vk[0]   = m_vk[1];
	m_vs[0]   = m_vs[1];
	m_vt[0]   = m_vt[1];
	m_vx[0]   = m_vx[1];
	m_vy[0]   = m_vy[1];
	m_vz[0]   = m_vz[1];

	// b = c
	m_vcol[1] = m_vcol[2];
	m_vk[1]   = m_vk[2];
	m_vs[1]   = m_vs[2];
	m_vt[1]   = m_vt[2];
	m_vx[1]   = m_vx[2];
	m_vy[1]   = m_vy[2];
	m_vz[1]   = m_vz[2];

	// c = trg->b
	m_vcol[2] = trg->m_vcol[1];
	m_vk[2]   = trg->m_vk[1];
	m_vs[2]   = trg->m_vs[1];
	m_vt[2]   = trg->m_vt[1];
	m_vx[2]   = trg->m_vx[1];
	m_vy[2]   = trg->m_vy[1];
	m_vz[2]   = trg->m_vz[1];

	CalcNormal();
}
