#include "AABB2.hpp"

//Constructors
//
AABB2::AABB2()
	:	mins( Vector2( 0.f, 0.f ) )
	,	maxs( Vector2( 0.f, 0.f ) )
{}

AABB2::AABB2( const Vector2& lowerLeftPoint, const Vector2& upperRightPoint )
	:	mins( lowerLeftPoint )
	,	maxs( upperRightPoint )
{}

AABB2::AABB2( const AABB2& toCopy )
	:	mins( toCopy.mins )
	,	maxs( toCopy.maxs )
{}

AABB2::AABB2( float minX, float minY, float maxX, float maxY )
	:	mins( Vector2( minX, minY ) )
	,	maxs( Vector2( maxX, maxY ) )
{}

AABB2::~AABB2()
{
}


//Setters
//
void AABB2::setMins( const Vector2& newMins )
{
	mins = newMins;
	float temp = 0.f;

	if( mins.x > maxs.x )
	{
		temp = mins.x;
		mins.x = maxs.x;
		maxs.x = temp;
	}
	
	if( mins.y > maxs.y )
	{
		temp = mins.y;
		mins.y = maxs.y;
		maxs.y = temp;
	}
}

void AABB2::setMins( float minX, float minY )
{
	mins = Vector2( minX, minY );

	if( minX > maxs.x )
	{
		mins.x = maxs.x;
		maxs.x = minX;
	}

	if( minY > maxs.y )
	{
		mins.y = maxs.y;
		maxs.y = minY;
	}
}

void AABB2::setMaxs( const Vector2& newMaxs )
{
	maxs = newMaxs;
	float temp = 0.f;

	if( mins.x > maxs.x )
	{
		temp = mins.x;
		mins.x = maxs.x;
		maxs.x = temp;
	}

	if( mins.y > maxs.y )
	{
		temp = mins.y;
		mins.y = maxs.y;
		maxs.y = temp;
	}
}

void AABB2::setMaxs( float maxX, float maxY )
{
	maxs = Vector2( maxX, maxY );

	if( mins.x > maxX )
	{
		maxs.x = mins.x;
		mins.x = maxX;
	}

	if( mins.y > maxY )
	{
		maxs.y = mins.y;
		mins.y = maxY;
	}
}

void AABB2::setCenter( const Vector2& newCenter )
{
	Vector2 distanceMinToCenter = getWidthAndHeight() * .5f;
	mins = newCenter - distanceMinToCenter;
	maxs = newCenter + distanceMinToCenter;
}

void AABB2::setCenter( float centerX, float centerY )
{
	Vector2 newCenter( centerX, centerY );
	Vector2 distanceMinToCenter = getWidthAndHeight() * .5f;
	mins = newCenter - distanceMinToCenter;
	maxs = newCenter + distanceMinToCenter;
}


//getters
//
Vector2 AABB2::getMins() const
{
	return mins;
}

Vector2 AABB2::getMaxs() const
{
	return maxs;
}


//Checkers
//
bool AABB2::checkIfIsInside( const Vector2& point ) const
{
	return ( point.x > mins.x 
				&& point.x < maxs.x 
				&& point.y > mins.y
				&& point.y < maxs.y);
}

bool AABB2::checkIfIsInterpenetrating( const AABB2& boxToCheck ) const
{
	return !( ( boxToCheck.mins.x >= maxs.x ) 
				|| ( boxToCheck.maxs.x <= mins.x ) 
				|| ( boxToCheck.mins.y >= maxs.y ) 
				|| ( boxToCheck.maxs.y <= mins.y ) );
}

bool AABB2::checkIfEdgeIsTouching( const AABB2& boxtoCheck ) const
{
	return ( ( ( boxtoCheck.maxs.x == mins.x )
		&& ( ( boxtoCheck.mins.y <= mins.y && boxtoCheck.maxs.y >= maxs.y ) )
		|| ( ( boxtoCheck.mins.y >= mins.y && boxtoCheck.mins.y <= maxs.y ) ) 
		|| ( ( boxtoCheck.maxs.y >= mins.y && boxtoCheck.maxs.y <= maxs.y ) ) )
	|| ( ( boxtoCheck.mins.x == maxs.x )
		&& ( ( boxtoCheck.mins.y <= mins.y && boxtoCheck.maxs.y >= maxs.y ) )
		|| ( ( boxtoCheck.mins.y >= mins.y && boxtoCheck.mins.y <= maxs.y ) ) 
		|| ( ( boxtoCheck.maxs.y >= mins.y && boxtoCheck.maxs.y <= maxs.y ) ) )
	|| ( ( boxtoCheck.maxs.y == mins.y )
		&& ( ( boxtoCheck.mins.x <= mins.x && boxtoCheck.maxs.x >= maxs.x ) )
		|| ( ( boxtoCheck.mins.x >= mins.x && boxtoCheck.mins.x <= maxs.x ) ) 
		|| ( ( boxtoCheck.maxs.x >= mins.x && boxtoCheck.maxs.x <= maxs.x ) ) )
	|| ( ( boxtoCheck.mins.y == maxs.y )
		&& ( ( boxtoCheck.mins.x <= mins.x && boxtoCheck.maxs.x >= maxs.x ) )
		|| ( ( boxtoCheck.mins.x >= mins.x && boxtoCheck.mins.x <= maxs.x ) ) 
		|| ( ( boxtoCheck.maxs.x >= mins.x && boxtoCheck.maxs.x <= maxs.x ) ) ) );
}

float AABB2::getDistanceCenterToCenter( const AABB2& boxToCheckAgainst ) const
{
	Vector2 distanceCenterToCenter = getCenter() - boxToCheckAgainst.getCenter();
	return distanceCenterToCenter.CalculateLength();
}

Vector2 AABB2::getCenter() const
{
	Vector2 distanceFromMinToCenter = getWidthAndHeight() * .5f;

	return ( mins + distanceFromMinToCenter );
}

Vector2 AABB2::getWidthAndHeight() const
{
	return Vector2( ( maxs.x - mins.x ), (maxs.y - mins.y ) );
}

float AABB2::getArea() const
{
	Vector2 widthAndHeight = getWidthAndHeight();
	return ( widthAndHeight.x * widthAndHeight.y );
}

float AABB2::getPerimeter() const
{
	Vector2 widthAndHeight = getWidthAndHeight();
	return ( widthAndHeight.x * 2.f + widthAndHeight.y * 2.f );
}

float AABB2::getLengthOfDiagonalSquared() const
{
	Vector2 widthAndHeight = getWidthAndHeight();
	return ( ( widthAndHeight.x * widthAndHeight.x ) + ( widthAndHeight.y * widthAndHeight.y ) );
}

float AABB2::getLengthOfDiagonal() const
{
	float lengthOfDiagonalSquared = getLengthOfDiagonalSquared();
	return sqrt( lengthOfDiagonalSquared );
}

void AABB2::scaleUniformFromCenter( float scale )
{
	Vector2 centerCoords = getCenter();
	Vector2 distanceToCenter = getWidthAndHeight() * .5f;
	distanceToCenter *= scale;

	mins = centerCoords - distanceToCenter;
	maxs = centerCoords + distanceToCenter;
}

void AABB2::scaleUniformFromMins( float scale )
{
	Vector2 difference = maxs - mins;
	difference *= scale;
	maxs = mins + difference;
}

void AABB2::scaleUniformFromMaxs( float scale )
{
	Vector2 difference = maxs - mins;
	difference *= scale;
	mins = maxs - difference;
}

void AABB2::scaleNonUniformFromCenter( float scaleX, float scaleY )
{
	Vector2 centerCoords = getCenter();
	Vector2 distanceToCenter = getWidthAndHeight() * .5f;
	distanceToCenter.x *= scaleX;
	distanceToCenter.y *= scaleY;

	mins = centerCoords - distanceToCenter;
	maxs = centerCoords + distanceToCenter;
}

void AABB2::scaleNonUniformFromMins( float scaleX, float scaleY )
{
	Vector2 difference = maxs - mins;
	difference.x *= scaleX;
	difference.y *= scaleY;
	maxs = mins + difference;
}

void AABB2::scaleNonUniformFromMaxs( float scaleX, float scaleY )
{
	Vector2 difference = maxs - mins;
	difference.x *= scaleX;
	difference.y *= scaleY;
	mins = maxs - difference;
}

void AABB2::displace( float displacementX, float displacementY )
{
	Vector2 displacementVector( displacementX, displacementY );
	mins += displacementVector;
	maxs += displacementVector;
}

void AABB2::expandToEnclosePoint( const Vector2& pointToEnclose )
{
	if( pointToEnclose.x < mins.x )
	{
		mins.x = pointToEnclose.x;
	}
	else if( pointToEnclose.x > maxs.x )
	{
		maxs.x = pointToEnclose.x;
	}

	if( pointToEnclose.y < mins.y )
	{
		mins.y = pointToEnclose.y;
	}
	else if( pointToEnclose.y > maxs.y )
	{
		maxs.y = pointToEnclose.y;
	}
}

void AABB2::expandToEncloseBox( const AABB2& boxToEnclose )
{
	if( boxToEnclose.mins.x < mins.x )
	{
		mins.x = boxToEnclose.mins.x;
	}
	if( boxToEnclose.maxs.x > maxs.x )
	{
		maxs.x = boxToEnclose.maxs.x;
	}

	if( boxToEnclose.mins.y < mins.y )
	{
		mins.y = boxToEnclose.mins.y;
	}
	if( boxToEnclose.maxs.y > maxs.y )
	{
		maxs.y = boxToEnclose.maxs.y;
	}
}

void AABB2::operator=( const AABB2& toCopy )
{
	mins = toCopy.mins;
	maxs = toCopy.maxs;
}

void AABB2::operator+=( float padding )
{
	mins.x -= padding;
	mins.y -= padding;
	maxs.x += padding;
	maxs.y += padding;
}

void AABB2::operator-=( float padding )
{
	mins.x += padding;
	mins.y += padding;
	maxs.x -= padding;
	mins.y -= padding;
}

void AABB2::operator*=( float scale )
{
	mins *= scale;
	maxs *= scale;
}

void AABB2::operator/=( float inverseScale )
{
	if( inverseScale == 0.f )
	{
		return;
	}

	float scale = 1 / inverseScale;
	mins *= scale;
	maxs *= scale;
}

AABB2 AABB2::operator-() const
{
	return AABB2( ( -maxs ), ( -mins ) );
}

AABB2 AABB2::operator*( float scale ) const
{
	Vector2 centerCoord = getCenter();
	Vector2 distanceFromMinToCenter = getWidthAndHeight() * .5f;

	distanceFromMinToCenter *= scale;
	Vector2 newMins = centerCoord - distanceFromMinToCenter;
	Vector2 newMaxs = centerCoord + distanceFromMinToCenter;

	return AABB2( newMins, newMaxs );
}

bool AABB2::operator==( const AABB2& toCheckAgainst ) const
{
	return ( mins == toCheckAgainst.mins && maxs == toCheckAgainst.maxs );
}

bool AABB2::operator!=( const AABB2& toCheckAgainst ) const
{
	return ( mins != toCheckAgainst.mins || maxs != toCheckAgainst.maxs );
}