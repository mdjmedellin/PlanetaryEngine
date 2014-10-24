#include "Vector3.hpp"
#include "Vector2.hpp"
#include "MathUtilities.hpp"
#include <cmath>

//constructors
//
Vector3::Vector3()
	:	x( 0.f )
	,	y( 0.f )
	,	z( 0.f )
{}

Vector3::Vector3( const Vector3& vectorToCopy )
	:	x( vectorToCopy.x )
	,	y( vectorToCopy.y )
	,	z( vectorToCopy.z )
{}

Vector3::Vector3( float x, float y, float z )
	:	x( x )
	,	y( y )
	,	z( z )
{}

Vector3::Vector3(  const Vector2& vector2D )
	:	x( vector2D.x )
	,	y( vector2D.y )
	,	z( 0.f )
{}

Vector3::~Vector3()
{}

float Vector3::normalize()
{
	float radialDistanceSquared = ( x * x ) + ( y * y ) + ( z * z );

	if( radialDistanceSquared == 0.f )
	{
		return 0.f;
	}

	float radialDistance = std::sqrt( radialDistanceSquared );
	float inverseScale = 1.0f / radialDistance;

	x *= inverseScale;
	y *= inverseScale;
	z *= inverseScale;

	return radialDistance;
}

float Vector3::calculateRadialDistanceSquared() const
{
	return ( ( x * x ) + ( y * y ) + ( z * z ) );
}

float Vector3::calculateRadialDistance() const
{
	return sqrt( calculateRadialDistanceSquared() );
}

//Operators
//
Vector3& Vector3::operator=( const Vector3& rhs )
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
}

Vector3& Vector3::operator+=( const Vector3& rhs )
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

Vector3& Vector3::operator-=( const Vector3& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

Vector3& Vector3::operator*=( float scale )
{
	x *= scale;
	y *= scale;
	z *= scale;

	return *this;
}

Vector3& Vector3::operator/=( float inverseScale )
{
	if( inverseScale != 0.f )
	{
		float scale = 1.f / inverseScale;

		x *= scale;
		y *= scale;
		z *= scale;
	}
	return *this;
}

Vector3 Vector3::operator+( const Vector3& rhs ) const
{
	return Vector3 ( ( x + rhs.x ), ( y + rhs.y), ( z + rhs.z ) );
}

Vector3 Vector3::operator-( const Vector3& rhs ) const
{
	return Vector3 ( ( x - rhs.x ), ( y - rhs.y ), ( z - rhs.z ) );
}

Vector3 Vector3::operator-( ) const
{
	return Vector3 ( -x , -y, -z );
}

Vector3 Vector3::operator*( float scale ) const
{
	return Vector3 ( ( x * scale ), ( y * scale ), ( z * scale ) );
}

Vector3 Vector3::operator/( float inverseScale ) const
{
	if( inverseScale == 0.f )
	{
		return *this;
	}

	float scale = ( 1.f / inverseScale );
	return Vector3 ( ( x * scale ) , ( y * scale ), ( z * scale ) );
}

bool Vector3::operator==( const Vector3& rhs ) const
{
	return ( x == rhs.x && y == rhs.y && z == rhs.z );
}

bool Vector3::operator!=( const Vector3& rhs ) const
{
	return ( x != rhs.x || y != rhs.y || z != rhs.z );
}

float Vector3::DotProduct( const Vector3& vectorB ) const
{
	return ( ( x * vectorB.x ) + ( y * vectorB.y ) + ( z * vectorB.z ) );
}

void Vector3::setXYZ( float newXValue, float newYValue, float newZValue )
{
	x = newXValue;
	y = newYValue;
	z = newZValue;
}

float Vector3::setRadialDistance( float newRadialDistance )
{
	float radialDistanceSquared = ( x * x ) + ( y * y ) + ( z * z );

	if( radialDistanceSquared == 0.f )
	{
		return 0.f;
	}

	float radialDistance = std::sqrt( radialDistanceSquared );
	float inverseScale = newRadialDistance / radialDistance;

	x *= inverseScale;
	y *= inverseScale;
	z *= inverseScale;

	return radialDistance;
}

void Vector3::setPitchAndYawDegrees( float angleYawDegrees, float anglePitchDegrees )
{
	//calculating radian equivalent of angles
	//
	float angleYawRadians = angleYawDegrees * ( PI / 180.f );
	float anglePitchRadians = anglePitchDegrees * ( PI / 180.f );

	//calculating the angles that will be used to calculate x, y, and z
	float cosYaw = (cos( angleYawRadians ));
	float sinYaw = (sin( angleYawRadians ));

	float cosPitch = (cos( anglePitchRadians ));
	float sinPitch = (sin( anglePitchRadians ));

	//obtaining the radial distance of the vector
	float radialDistanceSquared = x * x + y * y + z * z;
	
	if ( radialDistanceSquared == 0.f )
	{
		return;
	}
	else
	{
		float radialDistance = std::sqrt( radialDistanceSquared );

		z = radialDistance * sinPitch;
		y = radialDistance * sinYaw * cosPitch;
		x = radialDistance * cosYaw * cosPitch;
	}
}

void Vector3::scaleUniform( float scale )
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void Vector3::scaleNonUniform( float scaleX, float scaleY, float scaleZ )
{
	x *= scaleX;
	y *= scaleY;
	z *= scaleZ;
}

void Vector3::setPitchAndYawRadians( float angleYawRadians, float anglePitchRadians )
{
	//calculating the angles that will be used to calculate x, y, and z
	float cosYaw = (cos( angleYawRadians ));
	float sinYaw = (sin( angleYawRadians ));

	float cosPitch = (cos( anglePitchRadians ));
	float sinPitch = (sin( anglePitchRadians ));

	//obtaining the radial distance of the vector
	float radialDistanceSquared = x * x + y * y + z * z;

	if ( radialDistanceSquared == 0.f )
	{
		return;
	}
	else
	{
		float radialDistance = std::sqrt( radialDistanceSquared );

		z = radialDistance * sinPitch;
		y = radialDistance * sinYaw * cosPitch;
		x = radialDistance * cosYaw * cosPitch;
	}
}

void Vector3::calculateYawPitchRadians( float& yawRadiansHolder, float& pitchRadiansHolder ) const
{
	yawRadiansHolder = atan2( y, x );

	float lengthXY = std::sqrt( x * x + y * y );
	pitchRadiansHolder = atan2( z, lengthXY );
}

void Vector3::calculateYawPitchDegrees( float& yawDegreesHolder, float& pitchDegreesHolder ) const
{
	yawDegreesHolder = ( atan2( y, x ) * ( 180.f / PI ) );

	float lengthXY = std::sqrt( x * x + y * y );
	pitchDegreesHolder = ( atan2( z, lengthXY ) * ( 180.f / PI ) );
}

void Vector3::setUnitLengthAndYawPitchRadians( float angleYawRadians, float anglePitchRadians )
{
	float cosYaw = (cos( angleYawRadians ));
	float sinYaw = (sin( angleYawRadians ));
	float cosPitch = (cos( anglePitchRadians ));
	float sinPitch = (sin( anglePitchRadians ));
	x = cosYaw * cosPitch;
	y = sinYaw * cosPitch;
	z = sinPitch;
}

void Vector3::setUnitLengthAndYawPitchDegrees( float angleYawDegrees, float anglePitchDegrees )
{
	//convert degrees to radians
	float angleYawRadians = angleYawDegrees * ( PI / 180.f );
	float anglePitchRadians = anglePitchDegrees * ( PI / 180.f );

	float cosYaw = (cos( angleYawRadians ));
	float sinYaw = (sin( angleYawRadians ));
	float cosPitch = (cos( anglePitchRadians ));
	float sinPitch = (sin( anglePitchRadians ));

	x = cosYaw * cosPitch;
	y = sinYaw * cosPitch;
	z = sinPitch;
}

void Vector3::setLengthAndYawPitchRadians( float length, float angleYawRadians, float anglePitchRadians )
{
	float cosYaw = (cos( angleYawRadians ));
	float sinYaw = (sin( angleYawRadians ));
	float cosPitch = (cos( anglePitchRadians ));
	float sinPitch = (sin( anglePitchRadians ));

	x = length * cosYaw * cosPitch;
	y = length * sinYaw * cosPitch;
	z = length * sinPitch;
}

void Vector3::setLengthAndYawPitchDegrees( float length, float angleYawDegrees, float anglePitchDegrees )
{
	//convert degrees to radians
	float angleYawRadians = angleYawDegrees * ( PI / 180.f );
	float anglePitchRadians = anglePitchDegrees * ( PI / 180.f );

	float cosYaw = (cos( angleYawRadians ));
	float sinYaw = (sin( angleYawRadians ));
	float cosPitch = (cos( anglePitchRadians ));
	float sinPitch = (sin( anglePitchRadians ));

	x = length * cosYaw * cosPitch;
	y = length * sinYaw * cosPitch;
	z = length * sinPitch;
}

void Vector3::getXYZ( float& xHolder, float& yHolder, float& zHolder ) const
{
	xHolder = x;
	yHolder = y;
	zHolder = z;
}

void Vector3::reverse()
{
	x = -x;
	y = -y;
	z = -z;
}

void Vector3::reflectOverNormal( const Vector3& normal )
{
	float dotProductResult = dotProduct( *this, normal );
	Vector3 partOfThisAlongNormal = normal * dotProductResult;

	*this = *this - partOfThisAlongNormal - partOfThisAlongNormal;
}

Vector3 operator*( const float scale, const Vector3& vector )
{
	Vector3 toReturn = vector;
	toReturn *= scale;
	return toReturn;
}

float Vector3::operator[]( int i ) const
{
	switch( i )
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	case 2:
		return z;
		break;
	default:
		return 0.0f;
		break;
	}
}

float& Vector3::operator[](  int i )
{
	switch( i )
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	case 2:
		return z;
		break;
	default:
		return x;
		break;
	}
}

Vector3 Vector3::CrossProduct( const Vector3& vectorB ) const
{
	Vector3 toReturn;

	toReturn.x = ( y * vectorB.z ) - ( z * vectorB.y );
	toReturn.y = ( z * vectorB.x ) - ( x * vectorB.z );
	toReturn.z = ( x * vectorB.y ) - ( y * vectorB.x );

	return toReturn;
}
