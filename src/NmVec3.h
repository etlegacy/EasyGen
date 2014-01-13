//////////////
// NMVEC3.H //
//////////////

#ifndef NMVEC3_H
#define NMVEC3_H

class CNmVec3
{
public:
	float x, y, z;

	CNmVec3();
	CNmVec3(const CNmVec3&);
	CNmVec3(float, float, float);
	virtual ~CNmVec3();

	void        Set(float, float, float);
	CNmVec3&    operator=(const CNmVec3&);
	CNmVec3 operator-();
	CNmVec3&    operator+=(const CNmVec3&);
	CNmVec3&    operator-=(const CNmVec3&);
	CNmVec3&    operator*=(float);
	CNmVec3&    operator/=(float);

	bool        Compare(const CNmVec3&);
	bool        Compare(const CNmVec3&, float tolerance);

	float       GetLength();
	void        Normalize();
	void        Invert();

	CNmVec3&    Rotate(const CNmVec3&, float);
	CNmVec3&    RotateTo(const CNmVec3&, float);
	CNmVec3&    RotateAxis(const CNmVec3&, float);

	CNmVec3&    Rot90_x();
	CNmVec3&    Rot90_y();
	CNmVec3&    Rot90_z();

	CNmVec3&    Rotate_x(float);  //  arbitrary rotation along X-axis
	CNmVec3&    Rotate_y(float);  //  arbitrary rotation along Y-axis
	CNmVec3&    Rotate_z(float);  //  arbitrary rotation along Z-axis

	bool    IsNull();
};

bool operator==(const CNmVec3&, const CNmVec3&);
CNmVec3 operator+(const CNmVec3&, const CNmVec3&);
CNmVec3 operator-(const CNmVec3&, const CNmVec3&);
float operator*(const CNmVec3& v1, const CNmVec3& v2);
CNmVec3 operator*(const CNmVec3& v, float f);
CNmVec3 operator/(const CNmVec3& v, float f);
CNmVec3 operator^(const CNmVec3& v1, const CNmVec3& v2);

#endif
