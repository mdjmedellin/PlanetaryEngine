#pragma once
#ifndef VERTEX_H
#define VERTEX_H

//includes
#include "Vector3.hpp"
#include "Vector4.h"
#include "Vector2.hpp"
//

class Vertex
{
public:
	float x, y, z;
	float u, v;
	Vector3 normal;
	Vector4 rgba;
	Vector3 tangent;
	Vector3 bitangent;

	Vertex( const Vector3& pos )
		:	x( pos.x )
		,	y( pos.y )
		,	z( pos.z )
		,	rgba( Vector4( 1.f, 1.f, 1.f, 1.f ) )
	{
		normal = pos;
		normal.normalize();
		u = pos.x;
		v = 1.f-pos.y;
	}

	Vertex( const Vector3& pos,
				const Vector2& texCoords )
				:	x( pos.x )
				,	y( pos.y )
				,	z( pos.z )
				,	u( texCoords.x )
				,	v( texCoords.y )
				,	rgba( Vector4( 1.f, 1.f, 1.f, 1.f ) )
	{
		normal = pos;
		normal.normalize();
	}

	bool operator== ( const Vertex& rhs )
	{
		if( rhs.x == x  && rhs.y == y && rhs.z == z
			&& rhs.u == u && rhs.v == v 
			&& normal == rhs.normal 
			&& rgba == rhs.rgba
			&& tangent == rhs.tangent
			&& bitangent == rhs.bitangent )
		{
			return true;
		}
		return false;
	}

	void setPosition( const Vector3& pos )
	{
		x = pos.x;
		y = pos.y;
		z = pos.z;
	}

	Vector3 getPosition() const
	{
		return Vector3( x, y, z );
	}

	void createSurfaceTangents( const Vertex& previousVertex, const Vertex& nextVertex )
	{
		Vector3 P01 = nextVertex.getPosition() - getPosition();
		Vector3 P02 = previousVertex.getPosition() - getPosition();

		float V01 = nextVertex.v - v;
		float U01 = nextVertex.u - u;

		float V02 = previousVertex.v - v;
		float U02 = previousVertex.u - u;

		float Determinant = ( U01 * V02 ) - ( U02 * V01 );

		if( Determinant == 0.f )
		{
			return;
		}

		float invDeterminant = 1.f / Determinant;
		tangent = invDeterminant * ( ( V02 * P01 ) - ( V01 * P02 ) );
		bitangent = invDeterminant * ( ( U01 * P02 ) - ( U02 * P01 ) );

		Vector3 bitangentCross = normal.CrossProduct( tangent );
		bitangentCross.normalize();
		tangent = bitangentCross.CrossProduct( normal );

		if( bitangentCross.DotProduct( bitangent ) > 0.f )
		{
			bitangentCross *= -1.f;
		}

		bitangent = bitangentCross;
	}
};

#endif