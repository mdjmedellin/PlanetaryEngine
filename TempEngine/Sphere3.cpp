#include "Sphere3.hpp"


Sphere3::Sphere3()
	:	center( Vector3() )
	,	radius( 0.f )
{}

Sphere3::Sphere3( const Vector3& center, float radius )
	:	center( center )
	,	radius( radius )
{}

Sphere3::Sphere3( const Sphere3& toCopy)
	:	center( toCopy.center )
	,	radius( toCopy.radius )
{}

Sphere3::~Sphere3()
{}