#pragma once
#ifndef SPHERE3_H
#define SPHERE3_H

#include "Vector3.hpp"

class Sphere3
{
public:
	Sphere3();
	explicit Sphere3( const Vector3& center, float radius );
	Sphere3( const Sphere3& toCopy);
	~Sphere3();

protected:
	Vector3 center;
	float radius;
};

#endif