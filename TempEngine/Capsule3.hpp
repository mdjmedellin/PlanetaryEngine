#pragma once
#ifndef CAPSULE3_H
#define CAPSULE3_H

#include "Vector3.hpp"

class Capsule3
{
public:

	Vector3 startPoint, endPoint;
	float radius;

	Capsule3();
	Capsule3( const Capsule3& toCopy);
	Capsule3( const Vector3& startPoint, const Vector3& endPoint, float radius );
	~Capsule3();
};

#endif