#pragma once
#ifndef AABB2_H
#define AABB2_H

#include "Vector2.hpp"

class AABB2
{
public:

	AABB2();
	explicit AABB2( const Vector2& lowerLeftPoint, const Vector2& upperRightPoint );
	explicit AABB2( float minX, float minY, float maxX, float maxY );
	AABB2( const AABB2& toCopy );
	~AABB2();

	void setMins( const Vector2& newMins );
	void setMins( float minX, float minY );
	void setMaxs( const Vector2& newMaxs );
	void setMaxs( float maxX, float maxY );
	void setCenter( const Vector2& newCenter );
	void setCenter( float centerX, float centerY );

	Vector2 getMins() const;
	Vector2 getMaxs() const;

	bool checkIfIsInside( const Vector2& point ) const;
	bool checkIfIsInterpenetrating( const AABB2& boxToCheck ) const;
	bool checkIfEdgeIsTouching( const AABB2& boxtoCheck ) const;

	float getDistanceCenterToCenter( const AABB2& boxToCheckAgainst ) const;
	Vector2 getCenter() const;
	Vector2 getWidthAndHeight() const;
	float getArea() const;
	float getPerimeter() const;
	float getLengthOfDiagonalSquared() const;
	float getLengthOfDiagonal() const;

	void scaleUniformFromCenter( float scale );
	void scaleUniformFromMins( float scale );
	void scaleUniformFromMaxs( float scale );
	void scaleNonUniformFromCenter( float scaleX, float scaleY );
	void scaleNonUniformFromMins( float scaleX, float scaleY );
	void scaleNonUniformFromMaxs( float scaleX, float scaleY );

	void displace( float displacementX, float displacementY );
	void expandToEnclosePoint( const Vector2& pointToEnclose );
	void expandToEncloseBox( const AABB2& boxToEnclose );

	void operator=( const AABB2& toCopy );
	void operator+=( float padding );
	void operator-=( float padding );
	void operator*=( float scale );
	void operator/=( float inverseScale );

	AABB2 operator-() const;
	AABB2 operator*( float scale ) const;

	bool operator==( const AABB2& toCheckAgainst ) const;
	bool operator!=( const AABB2& toCheckAgainst ) const;


protected:
	Vector2 mins, maxs;
};

#endif