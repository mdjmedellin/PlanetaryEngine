#pragma once
#ifndef AABB3_H
#define AABB3_H

#include "Vector3.hpp"

class AABB3
{
public:

	AABB3();
	explicit AABB3( const Vector3& lowerLeftPoint, const Vector3& upperRightPoint );
	AABB3( const AABB3& toCopy );
	~AABB3();

	void setMins( const Vector3& newMins );
	void setMins( float minX, float minY, float minsZ );
	void setMaxs( const Vector3& newMaxs );
	void setMaxs( float maxX, float maxY, float maxsZ );
	void setCenter( const Vector3& newCenter );
	void setCenter( float centerX, float centerY, float centerZ );

	Vector3 getMins() const;
	Vector3 getMaxs() const;

	bool checkIfIsInside( const Vector3& point ) const;
	bool checkIfIsInterpenetrating( const AABB3& boxToCheck ) const;
	bool checkIfIAmEntirelyWithin( const AABB3& boxToCheck ) const;

	float calcDistanceCenterToCenter( const AABB3& boxToCheckAgainst ) const;
	Vector3 calcCenter() const;
	float getHeight() const;
	float getWidth() const;
	float getDepth() const;
	float calcVolume() const;
	float calcLengthOfDiagonalSquared() const;
	float calcLengthOfDiagonal() const;

	void scaleUniformFromCenter( float scale );
	void scaleUniformFromMins( float scale );
	void scaleUniformFromMaxs( float scale );
	void scaleNonUniformFromCenter( float scaleX, float scaleY, float scaleZ );
	void scaleNonUniformFromMins( float scaleX, float scaleY, float scaleZ );
	void scaleNonUniformFromMaxs( float scaleX, float scaleY, float scaleZ );

	void displace( float displacementX, float displacementY, float displacementZ );
	void stretchToEnclosePoint( const Vector3& pointToEnclose );
	void stretchToEncloseBox( const AABB3& boxToEnclose );

	void operator=( const AABB3& toCopy );
	void operator+=( float padding );
	void operator-=( float padding );
	void operator*=( float scale );
	void operator/=( float inverseScale );

	AABB3 operator-() const;
	AABB3 operator*( float scale ) const;

	bool operator==( const AABB3& toCheckAgainst ) const;
	bool operator!=( const AABB3& toCheckAgainst ) const;

protected:
	Vector3 mins, maxs;
};

#endif