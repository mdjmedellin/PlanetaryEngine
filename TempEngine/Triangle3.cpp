#include "Triangle3.hpp"


Triangle3::Triangle3()
	:	point1( Vector3() )
	,	point2( Vector3() )
	,	point3( Vector3() )
{}

Triangle3::Triangle3( const Vector3& point1, const Vector3& point2, const Vector3& point3 )
	:	point1( point1 )
	,	point2( point2 )
	,	point3( point3 )
{}

Triangle3::Triangle3( const Triangle3& toCopy )
	:	point1( toCopy.point1 )
	,	point2( toCopy.point2 )
	,	point3( toCopy.point3 )
{}

Triangle3::~Triangle3()
{}