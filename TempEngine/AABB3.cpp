#include "AABB3.hpp"
#include <cmath>

//Constructors
//
AABB3::AABB3()
	:	mins( Vector3() )
	,	maxs( Vector3() )
{}

AABB3::AABB3( const Vector3& frontLowerLeftPoint, const Vector3& backUpperRightPoint )
	:	mins( frontLowerLeftPoint )
	,	maxs( backUpperRightPoint )
{}

AABB3::AABB3( const AABB3& toCopy )
	:	mins( toCopy.mins )
	,	maxs( toCopy.maxs )
{}

AABB3::~AABB3()
{
}


//Setters
//
void AABB3::setMins( const Vector3& newMins )
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

	if( mins.z > maxs.z )
	{
		temp = mins.z;
		mins.z = maxs.z;
		maxs.z = temp;
	}
}

void AABB3::setMins( float minX, float minY, float minZ )
{
	mins = Vector3( minX, minY, minZ );

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

	if( minZ > maxs.z )
	{
		mins.z = maxs.z;
		maxs.z = minZ;
	}
}

void AABB3::setMaxs( const Vector3& newMaxs )
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

	if( mins.z > maxs.z )
	{
		temp = mins.z;
		mins.z = maxs.z;
		maxs.z = temp;
	}
}

void AABB3::setMaxs( float maxX, float maxY, float maxZ )
{
	maxs = Vector3( maxX, maxY, maxZ );

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

	if( mins.z > maxZ )
	{
		maxs.z = mins.z;
		mins.z = maxZ;
	}
}

void AABB3::setCenter( const Vector3& newCenter )
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;

	Vector3 distanceMinToCenter = Vector3( width, height, depth ) * .5f;
	mins = newCenter - distanceMinToCenter;
	maxs = newCenter + distanceMinToCenter;
}

void AABB3::setCenter( float centerX, float centerY, float centerZ )
{
	Vector3 newCenter( centerX, centerY, centerZ );

	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;

	Vector3 distanceMinToCenter = Vector3( width, height, depth ) * .5f;
	mins = newCenter - distanceMinToCenter;
	maxs = newCenter + distanceMinToCenter;
}


//getters
//
Vector3 AABB3::getMins() const
{
	return mins;
}

Vector3 AABB3::getMaxs() const
{
	return maxs;
}


//Checkers
//
bool AABB3::checkIfIsInside( const Vector3& point ) const
{
	return ( point.x > mins.x 
		&& point.x < maxs.x 
		&& point.y > mins.y
		&& point.y < maxs.y
		&& point.z > mins.z
		&& point.z < maxs.z);
}

bool AABB3::checkIfIsInterpenetrating( const AABB3& boxToCheck ) const
{
	return !( ( boxToCheck.mins.x >= maxs.x ) 
		|| ( boxToCheck.maxs.x <= mins.x ) 
		|| ( boxToCheck.mins.y >= maxs.y ) 
		|| ( boxToCheck.maxs.y <= mins.y )
		|| ( boxToCheck.mins.z >= maxs.z )
		|| ( boxToCheck.maxs.z <= mins.z ));
}

bool AABB3::checkIfIAmEntirelyWithin( const AABB3& boxToCheck ) const
{
	return ( ( maxs.x < boxToCheck.maxs.x )
		&& ( maxs.y < boxToCheck.maxs.y )
		&& ( maxs.z < boxToCheck.maxs.z )
		&& ( mins.x > boxToCheck.mins.x )
		&& ( mins.y > boxToCheck.mins.y )
		&& ( mins.z > boxToCheck.mins.z ) );
}

float AABB3::calcDistanceCenterToCenter( const AABB3& boxToCheckAgainst ) const
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;
	Vector3 distanceMinToCenter = Vector3( width, height, depth ) * .5f;

	Vector3 distanceCenterToCenter = mins + distanceMinToCenter - boxToCheckAgainst.calcCenter();
	return distanceCenterToCenter.calculateRadialDistance();
}

Vector3 AABB3::calcCenter() const
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;
	Vector3 distanceFromMinToCenter = Vector3( width, height, depth ) * .5f;

	return ( mins + distanceFromMinToCenter );
}

float AABB3::getHeight() const
{
	return ( maxs.y - mins.y );
}

float AABB3::getWidth() const
{
	return ( maxs.x - mins.x );
}

float AABB3::getDepth() const
{
	return ( maxs.z - mins.z );
}
float AABB3::calcVolume() const
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;

	return ( width * height * depth );
}

float AABB3::calcLengthOfDiagonalSquared() const
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;
	return ( ( width * width ) + ( height * height ) + ( depth * depth ) );
}

float AABB3::calcLengthOfDiagonal() const
{
	float lengthOfDiagonalSquared = calcLengthOfDiagonalSquared();
	return sqrt( lengthOfDiagonalSquared );
}

void AABB3::scaleUniformFromCenter( float scale )
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;
	Vector3 distanceFromMinsToCenter = Vector3( width, height, depth ) * .5f;

	Vector3 centerCoords = mins + distanceFromMinsToCenter;

	distanceFromMinsToCenter *= scale;

	mins = centerCoords - distanceFromMinsToCenter;
	maxs = centerCoords + distanceFromMinsToCenter;
}

void AABB3::scaleUniformFromMins( float scale )
{
	Vector3 difference = maxs - mins;
	difference *= scale;
	maxs = mins + difference;
}

void AABB3::scaleUniformFromMaxs( float scale )
{
	Vector3 difference = maxs - mins;
	difference *= scale;
	mins = maxs - difference;
}

void AABB3::scaleNonUniformFromCenter( float scaleX, float scaleY, float scaleZ )
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;
	Vector3 distanceFromMinsToCenter = Vector3( width, height, depth ) * .5f;

	Vector3 centerCoords = mins + distanceFromMinsToCenter;

	distanceFromMinsToCenter.x *= scaleX;
	distanceFromMinsToCenter.y *= scaleY;
	distanceFromMinsToCenter.z *= scaleZ;

	mins = centerCoords - distanceFromMinsToCenter;
	maxs = centerCoords + distanceFromMinsToCenter;
}

void AABB3::scaleNonUniformFromMins( float scaleX, float scaleY, float scaleZ )
{
	Vector3 difference = maxs - mins;
	difference.x *= scaleX;
	difference.y *= scaleY;
	difference.z *= scaleZ;

	maxs = mins + difference;
}

void AABB3::scaleNonUniformFromMaxs( float scaleX, float scaleY, float scaleZ )
{
	Vector3 difference = maxs - mins;
	difference.x *= scaleX;
	difference.y *= scaleY;
	difference.z *= scaleZ;

	mins = maxs - difference;
}

void AABB3::displace( float displacementX, float displacementY, float displacementZ )
{
	Vector3 displacementVector( displacementX, displacementY, displacementZ );
	mins += displacementVector;
	maxs += displacementVector;
}

void AABB3::stretchToEnclosePoint( const Vector3& pointToEnclose )
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

	if( pointToEnclose.z < mins.z )
	{
		mins.z = pointToEnclose.z;
	}
	else if( pointToEnclose.z > maxs.z )
	{
		maxs.z = pointToEnclose.z;
	}
}

void AABB3::stretchToEncloseBox( const AABB3& boxToEnclose )
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

	if( boxToEnclose.mins.z < mins.z )
	{
		mins.z = boxToEnclose.mins.z;
	}
	if( boxToEnclose.maxs.z > maxs.z )
	{
		maxs.z = boxToEnclose.maxs.z;
	}
}

void AABB3::operator=( const AABB3& toCopy )
{
	mins = toCopy.mins;
	maxs = toCopy.maxs;
}

void AABB3::operator+=( float padding )
{
	mins.x -= padding;
	mins.y -= padding;
	mins.z -= padding;

	maxs.x += padding;
	maxs.y += padding;
	maxs.z += padding;
}

void AABB3::operator-=( float padding )
{
	mins.x += padding;
	mins.y += padding;
	mins.z += padding;

	maxs.x -= padding;
	maxs.y -= padding;
	maxs.z -= padding;
}

void AABB3::operator*=( float scale )
{
	mins *= scale;
	maxs *= scale;
}

void AABB3::operator/=( float inverseScale )
{
	if( inverseScale == 0.f )
	{
		//if the user attempts to divide by 0
		//don't do anything
		//
		return;
	}

	float scale = 1 / inverseScale;
	mins *= scale;
	maxs *= scale;
}

AABB3 AABB3::operator-() const
{
	return AABB3( ( -maxs ), ( -mins ) );
}

AABB3 AABB3::operator*( float scale ) const
{
	float width = maxs.x - mins.x;
	float height = maxs.y - mins.y;
	float depth = maxs.z - mins.z;
	Vector3 distanceFromMinsToCenter = Vector3( width, height, depth ) * .5f;

	Vector3 centerCoords = mins + distanceFromMinsToCenter;

	distanceFromMinsToCenter *= scale;

	return AABB3( ( centerCoords - distanceFromMinsToCenter ), ( centerCoords + distanceFromMinsToCenter ) );
}

bool AABB3::operator==( const AABB3& toCheckAgainst ) const
{
	return ( mins == toCheckAgainst.mins && maxs == toCheckAgainst.maxs );
}

bool AABB3::operator!=( const AABB3& toCheckAgainst ) const
{
	return ( mins != toCheckAgainst.mins || maxs != toCheckAgainst.maxs );
}