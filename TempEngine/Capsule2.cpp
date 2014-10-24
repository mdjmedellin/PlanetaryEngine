#include "Capsule2.hpp"


Capsule2::Capsule2()
	:	startPoint( Vector2() )
	,	endPoint( Vector2() )
	,	radius( 0.f )
{}

Capsule2::Capsule2( const Capsule2& toCopy)
	:	startPoint( toCopy.startPoint )
	,	endPoint( toCopy.endPoint )
	,	radius( toCopy.radius )
{}

Capsule2::Capsule2( const Vector2& startPoint, const Vector2& endPoint, float radius )
	:	startPoint( startPoint )
	,	endPoint( endPoint )
	,	radius( radius )
{}

Capsule2::~Capsule2()
{}