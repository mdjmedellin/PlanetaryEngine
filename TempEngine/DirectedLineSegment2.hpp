#pragma once

#include "Vector2.hpp"

class DirectedLineSegment2
{
public:
	Vector2 startingPoint;
	Vector2 endPoint;

	DirectedLineSegment2();
	explicit DirectedLineSegment2( const Vector2& startingPoint, const Vector2& endPoint );
	explicit DirectedLineSegment2( float startXCord, float startYCord, float endXCord, float endYCord );
	DirectedLineSegment2( const DirectedLineSegment2& toCopy );
	~DirectedLineSegment2();

	Vector2 getPointAtParametric( float parametric ) const;
	float getLengthSquared() const;
	float getLength() const;

	void displace( float displacementX, float displacementY );

	void operator=( const DirectedLineSegment2& toCopy );

	DirectedLineSegment2 operator-() const;

	bool operator==( const DirectedLineSegment2& toCheckAgainst ) const;
	bool operator!=( const DirectedLineSegment2& toCheckAgainst ) const;
};

