#pragma once
#ifndef CAPSULE2_H
#define CAPSULE2_H

#include "Vector2.hpp"

class Capsule2
{
public:

	Vector2 startPoint, endPoint;
	float radius;

	Capsule2();
	Capsule2( const Capsule2& toCopy);
	Capsule2( const Vector2& startPoint, const Vector2& endPoint, float radius );
	~Capsule2(void);
};

#endif