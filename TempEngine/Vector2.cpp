#include "Vector2.hpp"

//constructors
//
Vector2::Vector2()
	: x( 0.f )
	, y( 0.f )
{
}

Vector2::Vector2( const Vector2& vectorToCopy )
	: x( vectorToCopy.x )
	, y( vectorToCopy.y )
{
}

Vector2::Vector2( float x, float y )
	: x( x )
	, y( y )
{
}

Vector2::~Vector2(void)
{
}


//
float Vector2::NormalizeVector()
{
	return SetLength( x, y, 1.0f );
}


//Setters
//
void Vector2::SetXY(float newXValue, float newYValue)
{
	x = newXValue;
	y = newYValue;
}

void Vector2::SetUnitLengthAndYawRadians( float yawRadians )
{
	x = cos( yawRadians );
	y = sin( yawRadians );
}

void Vector2::SetUnitLengthAndYawDegrees( float yawDegrees )
{
	float yawRadians = yawDegrees * ( PI / 180.f );

	x = cos( yawRadians );
	y = sin( yawRadians );
}

void Vector2::SetLengthAndYawRadians( float length, float yawRadians )
{
	x = length * cos( yawRadians );
	y = length * sin( yawRadians );
}

void Vector2::SetLengthAndYawDegrees( float length, float yawDegrees )
{
	float yawRadians = yawDegrees * ( PI / 180.f );

	x = length * cos( yawRadians );
	y = length * sin( yawRadians );
}

float Vector2::SetLength( float newLength )
{
	return SetLength( x, y, newLength );
}


//scalers
//
void Vector2::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
}

void Vector2::ScaleNonUniform(float xScale, float yScale)
{
	x *= xScale;
	y *= yScale;
}


//Rotation
//
void Vector2::RotateDegrees( float degreesToRotateBy )
{
	float radiansToRotateBy = degreesToRotateBy * ( PI / 180.f );

	RotateRadians( x, y, radiansToRotateBy );
}

void Vector2::RotateRadians( float radiansToRotateBy )
{
	RotateRadians( x, y, radiansToRotateBy );
}

void Vector2::Rotate90Degrees()
{
	Rotate90Degrees( x, y );
}

void Vector2::RotateMinus90Degrees()
{
	RotateMinus90Degrees( x, y );
}

void Vector2::Reverse()
{
	x = -x;
	y = -y;
}


//
void Vector2::GetXY( float& xHolder, float& yHolder ) const
{
	xHolder = x;
	yHolder = y;
}


float Vector2::CalculateLength() const
{
	return std::sqrt( ( x * x ) + ( y * y ) );
}

float Vector2::CalculateLengthSquared() const
{
	return ( ( x * x ) + ( y * y ) );
}

float Vector2::CalculateYawRadians() const
{
	return atan2( y, x );
}

float Vector2::CalculateYawDegrees() const
{
	return ( atan2( y, x ) * ( 180.f / PI ) );
}

//Operators
//
void Vector2::operator=( const Vector2& rhs )
{
	x = rhs.x;
	y = rhs.y;
}

void Vector2::operator+=( const Vector2& rhs )
{
	x += rhs.x;
	y += rhs.y;
}

void Vector2::operator-=( const Vector2& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vector2::operator*=( float scale )
{
	x *= scale;
	y *= scale;
}

void Vector2::operator/=( float inverseScale )
{
	if( inverseScale == 0.f )
	{
		return;
	}

	float scale = 1.f / inverseScale;

	x *= scale;
	y *= scale;
}

Vector2 Vector2::operator+( const Vector2& rhs ) const
{
	return Vector2 ( ( x + rhs.x ), ( y + rhs.y ) );
}

Vector2 Vector2::operator-( const Vector2& rhs ) const
{
	return Vector2 ( ( x - rhs.x ), ( y - rhs.y ) );
}

Vector2 Vector2::operator-( ) const
{
	return Vector2 ( -x , -y );
}

Vector2 Vector2::operator*( float scale ) const
{
	return Vector2 ( ( x * scale ) , ( y * scale ) );
}

Vector2 Vector2::operator/( float inverseScale ) const
{
	if( inverseScale == 0.f )
	{
		return *this;
	}

	float scale = ( 1.f / inverseScale );
	return Vector2 ( ( x * scale ) , ( y * scale ) );
}

bool Vector2::operator==( const Vector2& rhs ) const
{
	return ( x == rhs.x && y == rhs.y );
}

bool Vector2::operator!=( const Vector2& rhs ) const
{
	return ( x != rhs.x || y != rhs.y );
}

float Vector2::DotProduct( const Vector2& vectorB ) const
{
	return ( ( x * vectorB.x ) + ( y * vectorB.y ) );
}

const float& Vector2::operator[]( int index ) const
{
	return *((&x) + index);
}


float& Vector2::operator[]( int index )
{
	return *((&x) + index);
}

Vector2 operator*( const float scale, const Vector2& vector )
{
	Vector2 toReturn = vector;
	toReturn.ScaleUniform( scale );
	return toReturn;
}