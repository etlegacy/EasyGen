//////////////
// NMVEC2.CPP //
//////////////
#include "NmVec2.h"

#include <math.h>

bool operator==(const CNmVec2& v1, const CNmVec2& v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y);
}

CNmVec2 operator+(const CNmVec2& v1, const CNmVec2& v2)
{
	CNmVec2 v3(v1);
	v3 += v2;
	return v3;
}

CNmVec2 operator-(const CNmVec2& v1, const CNmVec2& v2)
{
	CNmVec2 v3(v1);
	v3 -= v2;
	return v3;
}

CNmVec2 operator*(const CNmVec2& v, float f)
{
	CNmVec2 w(v);
	w *= f;
	return w;
}

float operator*(const CNmVec2& v1, const CNmVec2& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y);
}

CNmVec2 operator/(const CNmVec2& v, float f)
{
	CNmVec2 w(v);
	w /= f;
	return w;
}

float operator^(const CNmVec2& v1, const CNmVec2& v2)
{
	return (v1.x * v2.y) - (v1.y * v2.x);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNmVec2::CNmVec2()
{
}

CNmVec2::CNmVec2(const CNmVec2& v)
{
	x = v.x;
	y = v.y;
}

CNmVec2::CNmVec2(float xx, float yy)
{
	x = xx;
	y = yy;
}

CNmVec2::~CNmVec2()
{

}

void CNmVec2::Set(float xx, float yy)
{
	x = xx;
	y = yy;
}

CNmVec2& CNmVec2::operator=(const CNmVec2& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

CNmVec2 CNmVec2::operator-()
{
	CNmVec2 w;
	w.x = -x;
	w.y = -y;
	return w;
}

CNmVec2& CNmVec2::operator+=(const CNmVec2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

CNmVec2& CNmVec2::operator-=(const CNmVec2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

CNmVec2& CNmVec2::operator*=(float f)
{
	x *= f;
	y *= f;
	return *this;
}

CNmVec2& CNmVec2::operator/=(float f)
{
	x /= f;
	y /= f;
	return *this;
}

float CNmVec2::GetLength()
{
	return (float)sqrt((x * x) + (y * y));
}

void CNmVec2::Normalize()
{
	float l;
	l  = 1.0f / (float)sqrt((x * x) + (y * y));
	x *= l;
	y *= l;
}

void CNmVec2::Invert()
{
	x = -x;
	y = -y;
}

/*
// basic rotation, no normalization, no ortogonalization.
CNmVec2& CNmVec2::Rotate(const CNmVec2& v, float angle)
{
    CNmVec2 u = v;
    *this = (*this * (float)cos(angle)) + (u * (float)sin(angle));
    return *this;
}

CNmVec2& CNmVec2::RotateTo(const CNmVec2& v, float angle)
{
    CNmVec2 ax,ay,az;
    ax = *this;
    ay = v;
    az = ax^ay;
//	if (az==0) return *this;
    ay = az^ax;
    ay.Normalize();
    ay *= ax.GetLength();
    az = (ax*(float)cos(angle))+(ay*(float)sin(angle));
    *this = az;
//	x=az.x;
//	y=az.y;
//	z=az.z;
    return *this;
}

CNmVec2& CNmVec2::RotateAxis(const CNmVec2& v, float angle)
{
    CNmVec2 ax,ay,axis,along_axis;
    float axf;

    axis = v;
    axis.Normalize();

    ay = axis ^ *this;
    ay.Normalize();

    ax = ay ^ axis;

    axf = *this * ax;

    ax *= axf;
    ay *= axf;

    ax.Rotate( ay, angle );

    along_axis = axis * ( *this * axis );

    *this = along_axis + ax;

    return *this;
}
*/
bool CNmVec2::IsNull()
{
	return (x == 0.0f) && (y == 0.0f);
}