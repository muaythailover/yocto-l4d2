#include "sdk.h"

#include <math.h>
#include <float.h>

// For vec_t, put this somewhere else?

// For rand(). We really need a library!
#include <stdlib.h>

#ifndef _X360
// For MMX intrinsics
#include <xmmintrin.h>
#endif

#include "minmax.h"

#pragma once

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

class q_angle
{
public:
	q_angle(void)
	{
		init();
	}

	q_angle(float X, float Y, float Z)
	{
		init(X, Y, Z);
	}

	q_angle(const float * clr)
	{
		init(clr[0], clr[1], clr[2]);
	}

	void init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		pitch = ix;
		yaw = iy;
		roll = iz;
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	q_angle& operator+=(const q_angle & v)
	{
		pitch += v.pitch;
		yaw += v.yaw;
		roll += v.roll;
		return *this;
	}

	q_angle& operator-=(const q_angle & v)
	{
		pitch -= v.pitch;
		yaw -= v.yaw;
		roll -= v.roll;
		return *this;
	}

	q_angle& operator*=(float fl)
	{
		pitch *= fl;
		yaw *= fl;
		roll *= fl;
		return *this;
	}

	q_angle& operator*=(const q_angle & v)
	{
		pitch *= v.pitch;
		yaw *= v.yaw;
		roll *= v.roll;
		return *this;
	}

	q_angle& operator/=(const q_angle & v)
	{
		pitch /= v.pitch;
		yaw /= v.yaw;
		roll /= v.roll;
		return *this;
	}

	q_angle& operator+=(float fl)
	{
		pitch += fl;
		yaw += fl;
		roll += fl;
		return *this;
	}

	q_angle& operator/=(float fl)
	{
		pitch /= fl;
		yaw /= fl;
		roll /= fl;
		return *this;
	}

	q_angle& operator-=(float fl)
	{
		pitch -= fl;
		yaw -= fl;
		roll -= fl;
		return *this;
	}

	q_angle& operator=(const q_angle & vOther)
	{
		pitch = vOther.pitch;
		yaw = vOther.yaw;
		roll = vOther.roll;
		return *this;
	}

	q_angle operator-(void) const
	{
		return q_angle(-pitch, -yaw, -roll);
	}

	q_angle operator+(const q_angle & v) const
	{
		return q_angle(pitch + v.pitch, yaw + v.yaw, roll + v.roll);
	}

	q_angle operator-(const q_angle & v) const
	{
		return q_angle(pitch - v.pitch, yaw - v.yaw, roll - v.roll);
	}

	q_angle operator*(float fl) const
	{
		return q_angle(pitch * fl, yaw * fl, roll * fl);
	}

	q_angle operator*(const q_angle & v) const
	{
		return q_angle(pitch * v.pitch, yaw * v.yaw, roll * v.roll);
	}

	q_angle operator/(float fl) const
	{
		return q_angle(pitch / fl, yaw / fl, roll / fl);
	}

	q_angle operator/(const q_angle & v) const
	{
		return q_angle(pitch / v.pitch, yaw / v.yaw, roll / v.roll);
	}

	float Length() const
	{
		return sqrt(pitch * pitch + yaw * yaw + roll * roll);
	}

	float LengthSqr(void) const
	{
		return (pitch * pitch + yaw * yaw + roll * roll);
	}

	bool IsZero(float tolerance = 0.01f) const
	{
		return (pitch > -tolerance && pitch < tolerance &&
			yaw > -tolerance && yaw < tolerance &&
			roll > -tolerance && roll < tolerance);
	}

	float pitch;
	float yaw;
	float roll;
};

inline q_angle operator*(float lhs, const q_angle & rhs)
{
	return rhs * lhs;
}

inline q_angle operator/(float lhs, const q_angle & rhs)
{
	return rhs / lhs;
}


class vector
{
public:
	float x, y, z;
	vector(void);
	vector(float X, float Y, float Z);
	void init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
	bool IsValid() const;
	float operator[](int i) const;
	float& operator[](int i);
	inline void Zero();
	bool operator==(const vector & v) const;
	bool operator!=(const vector & v) const;
	__forceinline vector& operator+=(const vector & v);
	__forceinline vector& operator-=(const vector & v);
	__forceinline vector& operator*=(const vector & v);
	__forceinline vector& operator*=(float s);
	__forceinline vector& operator/=(const vector & v);
	__forceinline vector& operator/=(float s);
	__forceinline vector& operator+=(float fl);
	__forceinline vector& operator-=(float fl);
	inline float Length() const;

	__forceinline float LengthSqr(void) const
	{
		CHECK_VALID(*this);
		return (x * x + y * y + z * z);
	}

	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
	}

	float NormalizeInPlace();
	__forceinline float DistToSqr(const vector & vOther) const;
	float Dot(const vector & vOther) const;
	float Length2D(void) const;
	float Length2DSqr(void) const;
	vector& operator=(const vector & vOther);
	vector operator-(void) const;
	vector operator+(const vector & v) const;
	vector operator-(const vector & v) const;
	vector operator*(const vector & v) const;
	vector operator/(const vector & v) const;
	vector operator*(float fl) const;
	vector operator/(float fl) const;
};

//===============================================
inline void vector::init(float ix, float iy, float iz)
{
	x = ix;
	y = iy;
	z = iz;
	CHECK_VALID(*this);
}

//===============================================
inline vector::vector(float X, float Y, float Z)
{
	x = X;
	y = Y;
	z = Z;
	CHECK_VALID(*this);
}

//===============================================
inline vector::vector(void)
{
}

//===============================================
inline void vector::Zero()
{
	x = y = z = 0.0f;
}

//===============================================
inline void vectorClear(vector & a)
{
	a.x = a.y = a.z = 0.0f;
}

//===============================================
inline vector& vector::operator=(const vector & vOther)
{
	CHECK_VALID(vOther);
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	return *this;
}

//===============================================
inline float& vector::operator[](int i)
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}

//===============================================
inline float vector::operator[](int i) const
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}

//===============================================
inline bool vector::operator==(const vector & src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}

//===============================================
inline bool vector::operator!=(const vector & src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}

//===============================================
__forceinline void vectorCopy(const vector & src, vector & dst)
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

//===============================================
__forceinline vector& vector::operator+=(const vector & v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

//===============================================
__forceinline vector& vector::operator-=(const vector & v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

//===============================================
__forceinline vector& vector::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline vector& vector::operator*=(const vector & v)
{
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline vector& vector::operator+=(float fl)
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline vector& vector::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline vector& vector::operator/=(float fl)
{
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
__forceinline vector& vector::operator/=(const vector & v)
{
	CHECK_VALID(v);
	Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}

//===============================================
inline float vector::Length(void) const
{
	CHECK_VALID(*this);

	float root = 0.0f;

	float sqsr = x * x + y * y + z * z;

	__asm
		{
		sqrtss xmm0, sqsr
		movss root, xmm0
		}

	return root;
}

//===============================================
inline float vector::Length2D(void) const
{
	CHECK_VALID(*this);

	float root = 0.0f;

	float sqst = x * x + y * y;

	__asm
		{
		sqrtss xmm0, sqst
		movss root, xmm0
		}

	return root;
}

//===============================================
inline float vector::Length2DSqr(void) const
{
	return (x * x + y * y);
}

//===============================================
inline vector CrossProduct(const vector & a, const vector & b)
{
	return vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

//===============================================
float vector::DistToSqr(const vector & vOther) const
{
	vector delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr();
}

//===============================================
inline float vector::NormalizeInPlace()
{
	float sqrlen = LengthSqr() + 1.0e-10f, invlen;
	__m128 xx = _mm_load_ss(&sqrlen);
	__m128 xr = _mm_rsqrt_ss(xx);
	__m128 xt;
	xt = _mm_mul_ss(xr, xr);
	xt = _mm_mul_ss(xt, xx);
	xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
	xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
	xr = _mm_mul_ss(xr, xt);
	_mm_store_ss(&invlen, xr);
	vector & v = *this;
	v.x *= sqrlen;
	v.y *= sqrlen;
	v.z *= sqrlen;
	return sqrlen * invlen;
}

//===============================================
inline vector vector::operator+(const vector & v) const
{
	vector res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

//===============================================
inline vector vector::operator-(const vector & v) const
{
	vector res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}

//===============================================
inline vector vector::operator*(float fl) const
{
	vector res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}

//===============================================
inline vector vector::operator*(const vector & v) const
{
	vector res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;
}

//===============================================
inline vector vector::operator/(float fl) const
{
	vector res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}

//===============================================
inline vector vector::operator/(const vector & v) const
{
	vector res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}

inline float vector::Dot(const vector & vOther) const
{
	const vector & a = *this;

	return (a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
}
