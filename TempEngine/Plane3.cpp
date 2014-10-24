#include "Plane3.hpp"


Plane3::Plane3()
	:	normal( Vector3() )
	,	distanceFromOrigin( 0.f )
{}

Plane3::Plane3( const Vector3& normal, float distanceFromOrigin )
	:	normal( normal )
	,	distanceFromOrigin( distanceFromOrigin )
{
	setNormal( normal );
}

Plane3::Plane3( const Plane3& toCopy )
	:	normal( toCopy.normal )
	,	distanceFromOrigin( toCopy.distanceFromOrigin )

{
	setNormal( normal );
}

Plane3::~Plane3()
{}


bool Plane3::isPointInFront( const Vector3& pointToCheck ) const
{
	if( pointToCheck.DotProduct( normal ) > distanceFromOrigin )
	{
		return true;
	}

	return false;
}

bool Plane3::isPointInBack( const Vector3& pointToCheck ) const
{
	if( pointToCheck.DotProduct( normal ) < distanceFromOrigin )
	{
		return true;
	}

	return false;
}

bool Plane3::isPointOnPlane( const Vector3& pointToCheck ) const
{
	if( pointToCheck.DotProduct( normal ) == distanceFromOrigin )
	{
		return true;
	}

	return false;
}

float Plane3::getDistanceAwayFromOrigin() const
{
	return distanceFromOrigin;
}

Vector3 Plane3::getNormal() const
{
	return normal;
}

void Plane3::setNormal( const Vector3& newNormal )
{
	normal = newNormal;

	float length = normal.normalize();
	if( length == 0.f )
	{
		distanceFromOrigin = 0.f;
	}
	else
	{
		distanceFromOrigin /= length;
	}
}

float Plane3::setdistanceFromOrigin( float newDistanceFromOrigin )
{
	float toReturn = distanceFromOrigin;
	distanceFromOrigin = newDistanceFromOrigin;
	return toReturn;
}

float Plane3::displacePlaneAlongNormal( float planeDisplacement )
{
	float oldDistance = distanceFromOrigin;

	distanceFromOrigin += planeDisplacement;
	return oldDistance;
}

void Plane3::reverseNormal()
{
	setNormal( -normal );
	distanceFromOrigin = -distanceFromOrigin;
}

bool Plane3::isFacingPositive() const
{
	return ( normal.DotProduct( Vector3( 1.f, 1.f, 1.f ) ) >= 0.f );
}

bool Plane3::operator==( const Plane3& rhs ) const
{
	return ( distanceFromOrigin == rhs.distanceFromOrigin 
			&& normal == rhs.normal );
}

bool Plane3::operator!=( const Plane3& rhs ) const
{
	return ( distanceFromOrigin != rhs.distanceFromOrigin 
			|| normal == rhs.normal );
}