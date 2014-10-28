#include "Vector4.h"

Vector4::Vector4()
	:	x(0.f)
	,	y(0.f)
	,	z(0.f)
	,	w(0.f)
{};

Vector4::Vector4( float x, float y, float z, float w )
	:	x( x )
	,	y( y )
	,	z( z )
	,	w( w )
{};

Vector4& Vector4::operator*=( float scale )
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;

	return *this;
}

Vector4 Vector4::operator*( float scale ) const
{
	Vector4 returnVec(x, y, z, w);

	returnVec.x *= scale;
	returnVec.y *= scale;
	returnVec.z *= scale;
	returnVec.w *= scale;

	return returnVec;
}

Vector4 Vector4::operator+( const Vector4& rhs ) const
{
	return Vector4 ( ( x + rhs.x ), ( y + rhs.y), ( z + rhs.z ), ( w + rhs.w) );
}

Vector4 operator*(const float scale, const Vector4& vector)
{
	Vector4 toReturn = vector;
	toReturn *= scale;
	return toReturn;
}