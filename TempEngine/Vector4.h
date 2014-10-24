#pragma once
#ifndef VECTOR4_H
#define VECTOR4_H

class Vector4
{
public:
	float x, y, z, w;

	Vector4( float x, float y, float z, float w )
		:	x( x )
		,	y( y )
		,	z( z )
		,	w( w )
	{};

	inline bool operator==( const Vector4& rhs )
	{
		return ( x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w );
	}
};

#endif