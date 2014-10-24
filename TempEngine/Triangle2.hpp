#pragma once

#include "Vector2.hpp"

class Triangle2
{
public:
	Triangle2();
	explicit Triangle2( Vector2 point1, Vector2 point2, Vector2 point3 );
	Triangle2( const Triangle2& toCopy );
	~Triangle2(void);

protected:
	Vector2 points[3];
};

