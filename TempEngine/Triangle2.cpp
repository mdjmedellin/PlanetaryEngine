#include "Triangle2.hpp"
#include <algorithm>

Triangle2::Triangle2()
{
	std::fill_n( points, 3, Vector2() );
}

Triangle2::Triangle2( Vector2 point1, Vector2 point2, Vector2 point3 )
{
	std::fill_n( points, 3, Vector2() );
	points[0] = point1;
	points[1] = point2;
	points[2] = point3;
}

Triangle2::Triangle2( const Triangle2& toCopy )
{
	points[0] = toCopy.points[0];
	points[1] = toCopy.points[1];
	points[2] = toCopy.points[2];
}

Triangle2::~Triangle2(void)
{
}
