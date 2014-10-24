#pragma once
#include <math.h>
#ifndef VECTOR3_H
#define VECTOR3_H

class Vector2;

class Vector3
{
public:
	float x, y, z;

	inline float trim( float toTrim )
	{
		if( fabs( toTrim ) < 0.0000001f )
		{
			return 0.f;
		}
		else
			return toTrim;
	}

	Vector3();
	Vector3( const Vector3& toCopy );
	explicit Vector3( float x, float y, float z );
	explicit Vector3( const Vector2& vector2D );
	~Vector3();

	//getter
	//
	void getXYZ( float& xHolder, float& yHolder, float& zHolder ) const;


	//setters
	//
	void setXYZ( float newXValue, float newYValue, float newZValue );
	float setRadialDistance( float newRadialDistance );
	void setPitchAndYawDegrees( float angleYawDegrees, float anglePitchDegrees );
	void setPitchAndYawRadians( float angleYawRadians, float anglePitchRadians );
	void setUnitLengthAndYawPitchRadians( float angleYawRadians, float anglePitchRadians );
	void setUnitLengthAndYawPitchDegrees( float angleYawDegrees, float anglePitchDegrees );
	void setLengthAndYawPitchRadians( float length, float angleYawRadians, float anglePitchRadians );
	void setLengthAndYawPitchDegrees( float length, float angleYawDegrees, float anglePitchDegrees );

	//
	//
	void reflectOverNormal( const Vector3& normal );
	Vector3 CrossProduct( const Vector3& vectorB ) const;

	void reverse();


	//scalers
	//
	void scaleUniform( float scale );
	void scaleNonUniform( float scaleX, float scaleY, float scaleZ );

	//
	float normalize();

	//calc
	//
	float calculateRadialDistanceSquared() const;
	float calculateRadialDistance() const;
	void calculateYawPitchRadians( float& yawRadiansHolder, float& pitchRadiansHolder ) const;
	void calculateYawPitchDegrees( float& yawDegreesHolder, float& pitchDegreesHolder ) const;

	//Operators
	//
	Vector3& operator=( const Vector3& rhs );
	Vector3& operator+=( const Vector3& rhs );
	Vector3& operator-=( const Vector3& rhs );
	Vector3& operator*=( float scale );
	Vector3& operator/=( float inverseScale );
	Vector3 operator+( const Vector3& rhs ) const;
	Vector3 operator-( const Vector3& rhs ) const;
	Vector3 operator-() const;
	Vector3 operator*( float scale ) const;
	Vector3 operator/( float inverseScale ) const;
	bool operator==( const Vector3& rhs ) const;
	bool operator!=( const Vector3& rhs ) const;

	float DotProduct( const Vector3& vectorB ) const;

	float operator[](  int i ) const;
	float& operator[](  int i );
};

Vector3 operator*(const float scale, const Vector3& vector);

#endif