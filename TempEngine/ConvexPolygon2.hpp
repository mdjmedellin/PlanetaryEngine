#pragma once

#ifndef CONVEXPOLYGON2_H
#define CONVEXPOLYGON2_H

#include <vector>

class Vector2;

class ConvexPolygon2
{
public:
	ConvexPolygon2();
	ConvexPolygon2( const std::vector<Vector2>& points );
	ConvexPolygon2( const ConvexPolygon2& toCopy );
	~ConvexPolygon2();

	void addPoint(const Vector2& newPoint );
	void addPoints( const std::vector< Vector2 >& pointsToAdd );
	std::vector< Vector2 > getPoints() const;

protected:
	std::vector< Vector2 > m_Points;
};

#endif