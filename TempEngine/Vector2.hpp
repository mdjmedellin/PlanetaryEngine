#pragma once
#ifndef VECTOR2_H
#define VECTOR2_H

//includes
#include <cmath>
#include "MathUtilities.hpp"
//

class Vector2
{
private:
	inline void RotateRadians( float& x, float& y, float radiansToRotateBy );
	inline void Rotate90Degrees( float& x, float& y );
	inline void RotateMinus90Degrees( float& x, float& y );
	inline float SetLength( float& x, float& y, float newLength );

public:
	float x;
	float y;
	
	//constructors
	//
	Vector2();
	Vector2( const Vector2& vectorToCopy );
	explicit Vector2( float x, float y );
	~Vector2();


	float NormalizeVector();


	//Setters
	//
	void SetXY( float newXValue, float newYValue );
	
	void SetUnitLengthAndYawRadians( float yawRadians );
	void SetUnitLengthAndYawDegrees( float yawDegrees );
	void SetLengthAndYawRadians( float length, float yawRadians );
	void SetLengthAndYawDegrees( float length, float yawDegrees );
	
	float SetLength( float newLength );


	//Scalers
	//
	void ScaleUniform( float scale );
	void ScaleNonUniform( float xScale, float yScale );


	//Rotation
	//
	void RotateDegrees( float degreesToRotateBy );
	void RotateRadians( float radiansToRotateBy );
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void Reverse();
	

	void GetXY( float& xHolder, float& yHolder ) const;

	float CalculateLength() const;
	float CalculateLengthSquared() const;
	float CalculateYawRadians() const;
	float CalculateYawDegrees() const;

	//Operators
	//
	void operator=( const Vector2& rhs );
	void operator+=( const Vector2& rhs );
	void operator-=( const Vector2& rhs );
	void operator*=( float scale );
	void operator/=( float inverseScale );
	Vector2 operator+( const Vector2& rhs ) const;
	Vector2 operator-( const Vector2& rhs ) const;
	Vector2 operator-() const;
	Vector2 operator*( float scale ) const;
	Vector2 operator/( float inverseScale ) const;
	bool operator==( const Vector2& rhs ) const;
	bool operator!=( const Vector2& rhs ) const;
	const float& operator[]( int index ) const;
	float& operator[]( int index );

	float DotProduct( const Vector2& vectorB ) const;

};

Vector2 operator*(const float scale, const Vector2& vector);

inline void Vector2::RotateRadians( float& x, float& y, float radiansToRotateBy )
{
	if ( fmod( radiansToRotateBy, ( PI / 2.f ) ) == 0.f )
	{
		while( radiansToRotateBy > 0.f)
		{
			Rotate90Degrees( x, y );
			radiansToRotateBy -= ( PI / 2.f );
		}
		while( radiansToRotateBy < 0.f)
		{
			RotateMinus90Degrees( x, y );
			radiansToRotateBy += ( PI / 2.f );
		}
	}
	else
	{
		float currentRadians = atan2( y, x );
		float length = sqrt( ( x * x ) + ( y * y ) );

		x = length * cos( currentRadians + radiansToRotateBy);
		y = length * sin( currentRadians + radiansToRotateBy );
	}
}

inline void Vector2::Rotate90Degrees( float& x, float& y )
{
	float tempX = x;

	x = -y;
	y = tempX;
}

inline void Vector2::RotateMinus90Degrees( float& x, float& y )
{
	float tempY = y;
	
	y = -x;
	x = tempY;
}

inline float Vector2::SetLength( float& x, float& y, float newLength )
{
	float lengthSquared = (x * x) + (y * y);

	if(lengthSquared == 0.f)
	{
		return 0.f;
	}

	float length = std::sqrt(lengthSquared);
	float inverseScale = newLength/length;

	x *= inverseScale;
	y *= inverseScale;

	return length;
}

#endif