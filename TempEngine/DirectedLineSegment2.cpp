#include "DirectedLineSegment2.hpp"


//Constructors
//
DirectedLineSegment2::DirectedLineSegment2()
	:	startingPoint( Vector2( 0.f, 0.f ) )
	,	endPoint( Vector2( 0.f, 0.f ) )
{}

DirectedLineSegment2::DirectedLineSegment2( const Vector2& startingPoint, const Vector2& endPoint )
	:	startingPoint( startingPoint )
	,	endPoint( endPoint )
{}

DirectedLineSegment2::DirectedLineSegment2( const DirectedLineSegment2& toCopy )
	:	startingPoint( toCopy.startingPoint )
	,	endPoint( toCopy.endPoint )
{}

DirectedLineSegment2::DirectedLineSegment2( float startXCord, float startYCord, float endXCord, float endYCord )
	:	startingPoint( Vector2( startXCord, startYCord ) )
	,	endPoint( Vector2( endXCord, endYCord ) )
{}

DirectedLineSegment2::~DirectedLineSegment2()
{}


//Getters
//
Vector2 DirectedLineSegment2::getPointAtParametric( float parametric ) const
{
	return ( ( startingPoint * ( 1.f - parametric ) ) + ( endPoint * ( parametric ) ) );
}

float DirectedLineSegment2::getLengthSquared() const
{
	Vector2 difference = endPoint - startingPoint;
	return ( ( difference.x * difference.x ) + ( difference.y * difference.y ) );
}

float DirectedLineSegment2::getLength() const
{
	return sqrt( getLengthSquared() );
}


//Modifiers
//
void DirectedLineSegment2::displace( float displacementX, float displacementY )
{
	Vector2 displacementVector( displacementX, displacementY );
	startingPoint += displacementVector;
	endPoint += displacementVector;
}


//Operators
//
void DirectedLineSegment2::operator=( const DirectedLineSegment2& toCopy )
{
	startingPoint = toCopy.startingPoint;
	endPoint = toCopy.endPoint;
}

DirectedLineSegment2 DirectedLineSegment2::operator-() const
{
	return DirectedLineSegment2( ( -startingPoint ), ( -endPoint ) );
}

bool DirectedLineSegment2::operator==( const DirectedLineSegment2& toCheckAgainst ) const
{
	return ( startingPoint == toCheckAgainst.startingPoint && endPoint == toCheckAgainst.endPoint );
}

bool DirectedLineSegment2::operator!=( const DirectedLineSegment2& toCheckAgainst ) const
{
	return ( startingPoint != toCheckAgainst.startingPoint || endPoint != toCheckAgainst.endPoint );
}