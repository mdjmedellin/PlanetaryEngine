#include "Capsule3.hpp"


Capsule3::Capsule3()
	:	startPoint( Vector3() )
	,	endPoint( Vector3() )
	,	radius( 0.f )
{}

Capsule3::Capsule3( const Capsule3& toCopy)
	:	startPoint( toCopy.startPoint )
	,	endPoint( toCopy.endPoint )
	,	radius( toCopy.radius )
{}

Capsule3::Capsule3( const Vector3& startPoint, const Vector3& endPoint, float radius )
	:	startPoint( startPoint )
	,	endPoint( endPoint )
	,	radius( radius )
{}

Capsule3::~Capsule3()
{}