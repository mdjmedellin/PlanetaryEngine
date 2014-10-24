#include "Disc2.hpp"
#include "MathUtilities.hpp"

//Constructors
//
Disc2::Disc2()
	:	center( Vector2( 0.f, 0.f) )
	,	radius( 0.f )
{}

Disc2::Disc2( const Vector2& center, float radius )
	:	center( center )
	,	radius( radius )
{}

Disc2::Disc2( float xCord, float yCord, float radius )
	:	center( Vector2( xCord, yCord ) )
	,	radius( radius )
{}

Disc2::Disc2( const Disc2& toCopy )
	:	center( toCopy.center )
	,	radius( toCopy.radius )
{}

Disc2::~Disc2()
{}


//Setters
//
float Disc2::setRadius( float newRadius )
{
	float toReturn = radius;
	
	radius = newRadius;
	if( radius < 0.f )
	{
		radius = -radius;
	}

	return toReturn;
}

void Disc2::setCenter( const Vector2& newCenter )
{
	center = newCenter;
}

void Disc2::setCenter( float centerX, float centerY )
{
	center = Vector2( centerX, centerY );
}


//Getters
//
Vector2 Disc2::getCenter() const
{
	return center;
}

float Disc2::getRadius() const
{
	return radius;
}


//Checkers
//
bool Disc2::checkIfIsInside( const Vector2& point ) const
{
	Vector2 distanceFromCenterToPoint = center - point;
	float lengthSquared = ( distanceFromCenterToPoint.x * distanceFromCenterToPoint.x ) +
							( distanceFromCenterToPoint.y * distanceFromCenterToPoint.y );
	float radiusSquared = ( radius * radius );

	return ( lengthSquared < radiusSquared );
}

bool Disc2::checkIfIsInterpenetrating( const Disc2& discToCheck ) const
{
	Vector2 distanceFromCenterToCenter = center - discToCheck.center;
	float lengthSquared = ( distanceFromCenterToCenter.x * distanceFromCenterToCenter.x ) +
							( distanceFromCenterToCenter.y * distanceFromCenterToCenter.y );
	
	float sumOfRadius = radius + discToCheck.radius;
	float sumOfRadiusSquared = sumOfRadius * sumOfRadius;

	return ( lengthSquared < sumOfRadiusSquared );

}

bool Disc2::checkIfEdgeIsTouching( const Disc2& discToCheck ) const
{
	Vector2 distanceFromCenterToCenter = center - discToCheck.center;
	float lengthSquared = ( ( distanceFromCenterToCenter.x * distanceFromCenterToCenter.x ) +
							( distanceFromCenterToCenter.y * distanceFromCenterToCenter.y ) );

	float sumOfRadius = radius + discToCheck.radius;
	float sumOfRadiusSquared = sumOfRadius * sumOfRadius;

	return ( lengthSquared == sumOfRadiusSquared );
}


//Measurement Calculations
//
float Disc2::calculateDistanceFromEdgeToPoint( const Vector2& pointToCheck ) const
{
	Vector2 distanceCenterToCenter = center - pointToCheck;
	float distanceSquared = ( ( distanceCenterToCenter.x * distanceCenterToCenter.x ) +
								( distanceCenterToCenter.y * distanceCenterToCenter.y ) );

	float distance = sqrt( distanceSquared );

	return distance - radius;
}

float Disc2::calculateDistanceFromEdgeToEdge( const Disc2& discToCheck ) const
{
	Vector2 distanceCenterToCenter = center - discToCheck.center;
	float distanceSquared = ( ( distanceCenterToCenter.x * distanceCenterToCenter.x ) +
								( distanceCenterToCenter.y * distanceCenterToCenter.y ) );

	float distance = sqrt( distanceSquared );

	return ( distance - radius - discToCheck.radius );
}

float Disc2::getLengthSquaredCenterToCenter( const Disc2& discToCheckAgainst ) const
{
	Vector2 distanceCenterToCenter = center - discToCheckAgainst.center;
	return ( ( distanceCenterToCenter.x * distanceCenterToCenter.x ) +
			( distanceCenterToCenter.y * distanceCenterToCenter.y ) );
}

float Disc2::getLengthCenterToCenter( const Disc2& discToCheckAgainst ) const
{
	float lengthSquared = getLengthSquaredCenterToCenter( discToCheckAgainst );
	return sqrt( lengthSquared );
}

float Disc2::getArea() const
{
	return ( PI * radius * radius );
}

float Disc2::getCircumference() const
{
	return (2.f * PI) * radius;
}


//Modifiers
//
void Disc2::changeScale( float scale )
{
	radius *= scale;
}

void Disc2::displace( float displacementX, float displacementY )
{
	Vector2 displacementVector( displacementX, displacementY );
	center += displacementVector;
}

void Disc2::expandToEnclosePoint( const Vector2& pointToEnclose )
{
	Vector2 distanceFromCenterToPoint = center - pointToEnclose;
	float lengthSquared = ( distanceFromCenterToPoint.x * distanceFromCenterToPoint.x ) +
							( distanceFromCenterToPoint.y * distanceFromCenterToPoint.y );
	float radiusSquared = ( radius * radius );

	if ( lengthSquared > radiusSquared )
	{
		radius = sqrt( lengthSquared );
	}
}

void Disc2::expandToEncloseDisc( const Disc2& discToEnclose )
{
	Vector2 distanceFromCenterToCenter = center - discToEnclose.center;
	float lengthSquared = ( distanceFromCenterToCenter.x * distanceFromCenterToCenter.x ) +
							( distanceFromCenterToCenter.y * distanceFromCenterToCenter.y );
	float length = sqrt( lengthSquared ) + discToEnclose.radius;

	if( length > radius )
	{
		radius = length;
	}
}


//operators
//
void Disc2::operator=( const Disc2& toCopy )
{
	center = toCopy.center;
	radius = toCopy.radius;
}

void Disc2::operator+=( float padding )
{
	radius += padding;
}

void Disc2::operator-=( float padding )
{
	radius -= padding;
}

void Disc2::operator*=( float scale )
{
	radius *= scale;
}

void Disc2::operator/=( float inverseScale )
{
	if ( inverseScale == 0.f )
	{
		radius = 0.f;
	}
	else
	{
		radius /= inverseScale;
	}
}

Disc2 Disc2::operator-() const
{
	return Disc2( -center, radius );
}

Disc2 Disc2::operator*( float scale ) const
{
	return Disc2( center, ( radius * scale ) );
}

bool Disc2::operator==( const Disc2& toCheckAgainst ) const
{
	return ( center == toCheckAgainst.center && radius == toCheckAgainst.radius );
}

bool Disc2::operator!=( const Disc2& toCheckAgainst ) const
{
	return ( center != toCheckAgainst.center || radius != toCheckAgainst.radius );
}