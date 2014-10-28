#pragma once
#ifndef VECTOR4_H
#define VECTOR4_H

class Vector4
{
public:
	float x, y, z, w;

	Vector4();
	Vector4( float x, float y, float z, float w );

	inline bool operator==( const Vector4& rhs )
	{
		return ( x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w );
	}

	Vector4& operator*=( float scale );
	Vector4 operator*( float scale ) const;
	Vector4 operator+( const Vector4& rhs ) const;
};

Vector4 operator*(const float scale, const Vector4& vector);

#endif