//////////////
// NMVEC2.H //
//////////////

#ifndef NMVEC2_H
#define NMVEC2_H

class CNmVec2
{
public:
	float x, y;

	CNmVec2();
	CNmVec2(const CNmVec2&);
	CNmVec2(float, float);
	virtual ~CNmVec2();

	void        Set(float, float);
	CNmVec2&    operator=(const CNmVec2&);
	CNmVec2 operator-();
	CNmVec2&    operator+=(const CNmVec2&);
	CNmVec2&    operator-=(const CNmVec2&);
	CNmVec2&    operator*=(float);
	CNmVec2&    operator/=(float);

	float       GetLength();
	void        Normalize();
	void        Invert();
//	CNmVec2&	Rotate(const CNmVec2&, float);
//	CNmVec2&	RotateTo(const CNmVec2&, float);
//	CNmVec2&	RotateAxis(const CNmVec2&, float);

	bool    IsNull();
};

bool operator==(const CNmVec2&, const CNmVec2&);
CNmVec2 operator+(const CNmVec2&, const CNmVec2&);
CNmVec2 operator-(const CNmVec2&, const CNmVec2&);
float operator*(const CNmVec2& v1, const CNmVec2& v2);
CNmVec2 operator*(const CNmVec2& v1, float f);
CNmVec2 operator/(const CNmVec2& v1, float f);
float operator^(const CNmVec2& v1, const CNmVec2& v2);

#endif
