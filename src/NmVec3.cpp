////////////////
// NMVEC3.CPP //
////////////////
#include "NmVec3.h"

#include <math.h>

bool operator==(const CNmVec3& v1, const CNmVec3& v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

CNmVec3 operator+(const CNmVec3& v1, const CNmVec3& v2)
{
	CNmVec3 v3(v1);
	v3 += v2;
	return v3;
}

CNmVec3 operator-(const CNmVec3& v1, const CNmVec3& v2)
{
	CNmVec3 v3(v1);
	v3 -= v2;
	return v3;
}

float operator*(const CNmVec3& v1, const CNmVec3& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

CNmVec3 operator*(const CNmVec3& v, float f)
{
	CNmVec3 w(v);
	w *= f;
	return w;
}

CNmVec3 operator/(const CNmVec3& v, float f)
{
	CNmVec3 w(v);
	w /= f;
	return w;
}

CNmVec3 operator^(const CNmVec3& v1, const CNmVec3& v2)
{
	CNmVec3 w;
	w.x = (v1.y * v2.z) - (v1.z * v2.y);
	w.y = (v1.z * v2.x) - (v1.x * v2.z);
	w.z = (v1.x * v2.y) - (v1.y * v2.x);
	return w;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNmVec3::CNmVec3()
{
}

CNmVec3::CNmVec3(const CNmVec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

CNmVec3::CNmVec3(float xx, float yy, float zz)
{
	x = xx;
	y = yy;
	z = zz;
}

CNmVec3::~CNmVec3()
{

}

bool CNmVec3::Compare(const CNmVec3& vector)
{
	return x == vector.x && y == vector.y && z == vector.z;
}

bool CNmVec3::Compare(const CNmVec3& vector, float tolerance)
{
	return (*this - vector).GetLength() < tolerance;
/*	CNmVec3 c = *a - *b;

    if (c.x<0) c.x = -c.x;
    if (c.y<0) c.y = -c.y;
    if (c.z<0) c.z = -c.z;

    if (	(c.x>tolerance) ||
            (c.y>tolerance) ||
            (c.z>tolerance) ||
            (( c.x + c.y ) > tolerance) ||
            (( c.x + c.z ) > tolerance) ||
            (( c.y + c.z ) > tolerance) ||
            (( c.x + c.y + c.z ) > tolerance)
        ) return false;

    float tol_2 = tolerance * tolerance;

    return (c.x*c.x + c.y*c.y + c.z*c.z) < tol_2;*/
}

void CNmVec3::Set(float xx, float yy, float zz)
{
	x = xx;
	y = yy;
	z = zz;
}

CNmVec3& CNmVec3::operator=(const CNmVec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

CNmVec3 CNmVec3::operator-()
{
	CNmVec3 w;
	w.x = -x;
	w.y = -y;
	w.z = -z;
	return w;
}

CNmVec3& CNmVec3::operator+=(const CNmVec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

CNmVec3& CNmVec3::operator-=(const CNmVec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

CNmVec3& CNmVec3::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

CNmVec3& CNmVec3::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

float CNmVec3::GetLength()
{
	return (float)sqrt((x * x) + (y * y) + (z * z));
}

void CNmVec3::Normalize()
{
	float l;
	l  = 1.0f / (float)sqrt((x * x) + (y * y) + (z * z));
	x *= l;
	y *= l;
	z *= l;
}

void CNmVec3::Invert()
{
	x = -x;
	y = -y;
	z = -z;
}

// basic rotation, no normalization, no ortogonalization.
CNmVec3& CNmVec3::Rotate(const CNmVec3& v, float angle)
{
	CNmVec3 u = v;
	*this = (*this * (float)cos(angle)) + (u * (float)sin(angle));
	return *this;
}

CNmVec3& CNmVec3::RotateTo(const CNmVec3& v, float angle)
{
	CNmVec3 ax, ay, az;
	ax = *this;
	ay = v;
	az = ax ^ ay;
//	if (az==0) return *this;
	ay = az ^ ax;
	ay.Normalize();
	ay   *= ax.GetLength();
	az    = (ax * (float)cos(angle)) + (ay * (float)sin(angle));
	*this = az;
	return *this;
}

CNmVec3& CNmVec3::RotateAxis(const CNmVec3& v, float angle)
{
/*
    CNmVec3 v1,app,axis,vt,aux;

    v1 = *this;
    app = v;

    app.Normalize();
    axis = app;						// ...of rotation
    app *= v1*app;					// axis-component of v1  = v1a
    vt = v1 - app;					// tangential part of v1 = v1t
//	if (vt==0) return *this;         // no rotation perormed.
    aux= axis^vt;					// "aux" 2nd basis of rotation
    aux.Normalize();
    aux *= vt.length();
    vt.Rotate(aux,angle);			// rotating v1t to aux
    vt += app;						// recostructing v1
    *this = vt;
    return *this;
*/
	CNmVec3 ax, ay, axis, along_axis;
	float   axf;

	axis = v;
	axis.Normalize();

	ay = axis ^ *this;
	ay.Normalize();

	ax = ay ^ axis;

	axf = *this * ax;

	ax *= axf;
	ay *= axf;

	ax.Rotate(ay, angle);

	along_axis = axis * (*this * axis);

	*this = along_axis + ax;

	return *this;
}

CNmVec3& CNmVec3::Rot90_x()
{
	float tmp = y;
	y = -z;
	z = y;
	return *this;
}

CNmVec3& CNmVec3::Rot90_y()
{
	float tmp = z;
	z = -x;
	x = z;
	return *this;
}

CNmVec3& CNmVec3::Rot90_z()
{
	float tmp = x;
	x = -y;
	y = x;
	return *this;
}

CNmVec3& CNmVec3::Rotate_x(float angle)
{
	float a1   = y;
	float a2   = z;
	float cosa = (float)cos(angle);
	float sina = (float)sin(angle);
	y = a1 * cosa - a2 * sina;
	z = a2 * cosa + a1 * sina;
	return *this;
}

CNmVec3& CNmVec3::Rotate_y(float angle)
{
	float a1   = z;
	float a2   = x;
	float cosa = (float)cos(angle);
	float sina = (float)sin(angle);
	z = a1 * cosa - a2 * sina;
	x = a2 * cosa + a1 * sina;
	return *this;
}

CNmVec3& CNmVec3::Rotate_z(float angle)
{
	float a1   = x;
	float a2   = y;
	float cosa = (float)cos(angle);
	float sina = (float)sin(angle);
	x = a1 * cosa - a2 * sina;
	y = a2 * cosa + a1 * sina;
	return *this;
}

bool CNmVec3::IsNull()
{
	return (x == 0.0f) && (y == 0.0f) && (z == 0.0f);
}