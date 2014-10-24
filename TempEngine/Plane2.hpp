#pragma once
#ifndef PLANE2_H
#define PLANE2_H

#include "Vector2.hpp"

class Plane2
{
public:
	Vector2 normal;
	float distanceFromOrigin;

	Plane2();
	explicit Plane2( const Vector2& normal, float distanceFromOrigin );
	Plane2( const Plane2& toCopy );
	~Plane2();
};

#endif