#pragma once

#include "Vector2.hpp"

class Disc2
{
public:
	Disc2();
	explicit Disc2( const Vector2& center, float radius );
	explicit Disc2( float xCord, float yCord, float radius );
	Disc2( const Disc2& toCopy );
	~Disc2();

	float setRadius( float newRadius );
	void setCenter( const Vector2& newCenter );
	void setCenter( float centerX, float centerY );

	Vector2 getCenter() const;
	float getRadius() const;

	bool checkIfIsInside( const Vector2& point ) const;
	bool checkIfIsInterpenetrating( const Disc2& discToCheck ) const;
	bool checkIfEdgeIsTouching( const Disc2& discToCheck ) const;

	float calculateDistanceFromEdgeToPoint( const Vector2& pointToCheck ) const;
	float calculateDistanceFromEdgeToEdge( const Disc2& discToCheck ) const;
	float getLengthSquaredCenterToCenter( const Disc2& discToCheckAgainst ) const;
	float getLengthCenterToCenter( const Disc2& discToCheckAgainst ) const;
	float getArea() const;
	float getCircumference() const;

	void changeScale( float scale );

	void displace( float displacementX, float displacementY );
	void expandToEnclosePoint( const Vector2& pointToEnclose );
	void expandToEncloseDisc( const Disc2& discToEnclose );

	void operator=( const Disc2& toCopy );
	void operator+=( float padding );
	void operator-=( float padding );
	void operator*=( float scale );
	void operator/=( float inverseScale );

	Disc2 operator-() const;
	Disc2 operator*( float scale ) const;

	bool operator==( const Disc2& toCheckAgainst ) const;
	bool operator!=( const Disc2& toCheckAgainst ) const;


protected:
	Vector2 center;
	float radius;
};

