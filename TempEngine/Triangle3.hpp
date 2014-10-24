#pragma once
#ifndef TRIANGLE3_H
#define TRIANGLE3_H

#include "Vector3.hpp"

class Triangle3
{
public:
	Triangle3();
	explicit Triangle3( const Vector3& point1, const Vector3& point2, const Vector3& point3 );
	Triangle3( const Triangle3& toCopy );
	~Triangle3();

protected:
	Vector3 point1, point2, point3;
};

#endif