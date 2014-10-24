#include "Plane2.hpp"


Plane2::Plane2()
	:	normal( Vector2() )
	,	distanceFromOrigin( 0.f )
{}

Plane2::Plane2( const Vector2& normal, float distanceFromOrigin )
	:	normal( normal )
	,	distanceFromOrigin( distanceFromOrigin )
{}

Plane2::Plane2( const Plane2& toCopy )
	:	normal( toCopy.normal )
	,	distanceFromOrigin( toCopy.distanceFromOrigin )
{}

Plane2::~Plane2()
{}