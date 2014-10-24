#include "IntVector2.h"
#include "MathUtilities.hpp"

//constructors
//
IntVector2::IntVector2()
	: x( 0 )
	, y( 0 )
{
}

IntVector2::IntVector2( const IntVector2& vectorToCopy )
	: x( vectorToCopy.x )
	, y( vectorToCopy.y )
{
}

IntVector2::IntVector2( int x, int y )
	: x( x )
	, y( y )
{
}

IntVector2::~IntVector2(void)
{
}

//Setters
//
void IntVector2::SetXY( int newXValue, int newYValue)
{
	x = newXValue;
	y = newYValue;
}

//scalers
//
void IntVector2::ScaleUniform( float scale )
{
	x = GetNearestInt( scale * x );
	y = GetNearestInt( scale * y );
}

void IntVector2::ScaleNonUniform(float xScale, float yScale)
{
	x = GetNearestInt( x * xScale );
	y = GetNearestInt( y * yScale );
}


//Rotation
//
void IntVector2::Rotate90Degrees()
{
	Rotate90Degrees( x, y );
}

void IntVector2::RotateMinus90Degrees()
{
	RotateMinus90Degrees( x, y );
}

void IntVector2::Reverse()
{
	x = -x;
	y = -y;
}

void IntVector2::GetXY( int& xHolder, int& yHolder ) const
{
	xHolder = x;
	yHolder = y;
}

float IntVector2::CalculateLength() const
{
	return std::sqrt( float( x * x ) + float( y * y ) );
}

float IntVector2::CalculateLengthSquared() const
{
	return ( float( x * x ) + float( y * y ) );
}

//Operators
//
void IntVector2::operator=( const IntVector2& rhs )
{
	x = rhs.x;
	y = rhs.y;
}

void IntVector2::operator+=( const IntVector2& rhs )
{
	x += rhs.x;
	y += rhs.y;
}

void IntVector2::operator-=( const IntVector2& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
}

void IntVector2::operator*=( float scale )
{
	x = GetNearestInt( x * scale );
	y = GetNearestInt( y * scale );
}

void IntVector2::operator/=( float inverseScale )
{
	if( inverseScale == 0.f )
	{
		return;
	}

	float scale = 1.f / inverseScale;

	x = GetNearestInt( x * scale );
	y = GetNearestInt( y * scale );
}

IntVector2 IntVector2::operator+( const IntVector2& rhs ) const
{
	return IntVector2 ( ( x + rhs.x ), ( y + rhs.y ) );
}

IntVector2 IntVector2::operator-( const IntVector2& rhs ) const
{
	return IntVector2 ( ( x - rhs.x ), ( y - rhs.y ) );
}

IntVector2 IntVector2::operator-( ) const
{
	return IntVector2 ( -x , -y );
}

IntVector2 IntVector2::operator*( float scale ) const
{
	return IntVector2 ( GetNearestInt( x * scale ) , GetNearestInt( y * scale ) );
}

IntVector2 IntVector2::operator/( float inverseScale ) const
{
	if( inverseScale == 0.f )
	{
		return *this;
	}

	float scale = ( 1.f / inverseScale );
	return IntVector2 ( GetNearestInt( x * scale ) , GetNearestInt( y * scale ) );
}

bool IntVector2::operator==( const IntVector2& rhs ) const
{
	return ( x == rhs.x && y == rhs.y );
}

bool IntVector2::operator!=( const IntVector2& rhs ) const
{
	return ( x != rhs.x || y != rhs.y );
}

float IntVector2::DotProduct( const IntVector2& vectorB ) const
{
	return ( float( x * vectorB.x ) + float( y * vectorB.y ) );
}

IntVector2 operator*( const float scale, const IntVector2& vector )
{
	IntVector2 toReturn = vector;
	toReturn.ScaleUniform( scale );
	return toReturn;
}