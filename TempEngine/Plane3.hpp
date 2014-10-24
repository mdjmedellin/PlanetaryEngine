#pragma once
#ifndef PLANE3_H
#define PLANE3_H

#include "Vector3.hpp"

class Plane3
{
public:
	Plane3();
	explicit Plane3( const Vector3& normal, float distanceFromOrigin );
	Plane3( const Plane3& toCopy );
	~Plane3();

	bool isPointInFront( const Vector3& pointToCheck ) const;
	bool isPointInBack( const Vector3& pointToCheck ) const;
	bool isPointOnPlane( const Vector3& pointToCheck ) const;

	float getDistanceAwayFromOrigin() const;
	Vector3 getNormal() const;

	void setNormal( const Vector3& newNormal);
	float setdistanceFromOrigin( float newDistanceFromOrigin );
	float displacePlaneAlongNormal( float planeDisplacement );
	void reverseNormal();

	bool isFacingPositive() const;

	bool operator==( const Plane3& rhs ) const;
	bool operator!=( const Plane3& rhs ) const;

protected:
	Vector3 normal;
	float distanceFromOrigin;
};

#endif