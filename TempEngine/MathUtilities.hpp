#pragma once

#ifndef MATHUTILITIES_H
#define MATHUTILITIES_H

//constants
static const float PI = 3.14159265359f;
static const float RADIANS_TO_DEGREES = 57.2957795131f;
static const float DEGREES_TO_RADIANS = 0.01745329251f;
//

//includes
#include <vector>
#include <string>
//
class Vector2;
class Vector3;
class Vector4;

float RandZeroToOne();

int GetNearestInt(float);

float RoundFloatToNearestInt(float);

float GetMaxFloat(float, float);
float GetMaxFloat(float, float, float);
float GetMaxFloat(float, float, float, float);

float GetMinFloat(float, float);
float GetMinFloat(float, float, float);
float GetMinFloat(float, float, float, float);

int GetMaxInt(int, int);
int GetMaxInt(int, int, int);
int GetMaxInt(int, int, int, int);

int GetMinInt(int, int);
int GetMinInt(int, int, int);
int GetMinInt(int, int, int, int);

float ClampFloatWithinRange(float minimumFloatValue, float maximumFloatValue, float toClamp);

int ClampIntWithinRange(int minimumIntValue, int maximumIntValue, int intToClamp);

float ConvertDegreesToRadians(float);
float ConvertRadiansToDegrees(float);

float CalcAngularDistanceDegrees(float, float);
float CalcAngularDistanceRadians(float, float);

float CalcAngularDisplacementDegrees(float, float);
float CalcAngularDisplacementRadians(float, float);

bool CheckIfIsPowerOfTwo(int);

float InterpolateFloat(float startFloatValue, float endFloatValue, float fractionComplete);

int InterpolateInt(int, int, float);

// Filter Functions
//
float SmoothStartFilter( float value );
float SmoothStopFilter( float value );
float SmoothStepFilter( float value );
float LinearFilter( float value );
float SlingshotFilter( float value );

// Range Mapping
//
typedef float ( *FilterFunctionPtr )( float value );
float RangeMapFloat( float currentValue, float currentStartingPoint, float currentEndPoint,
						float newStartingPoint, float newEndPoint, bool clamp,
						FilterFunctionPtr filterFunction = LinearFilter);



// Method used on the construction of a convex polygon
//
bool isPointBehindFace( const Vector2& faceStart, const Vector2& faceEnd, const Vector2& pointToCheck );

Vector2 getNormal( const Vector2& startPoint, const Vector2& endPoint );

float dotProduct( const Vector2& vectorA, const Vector2& vectorB );

float dotProduct( const Vector3& vectorA, const Vector3& vectorB );

float getSign( float value );

inline float wrap( float x, float min, float max )
{
	if( x < min )
	{
		return max - ( min - x );
	}
	if( x > max )
	{
		return min + ( x - max );
	}

	return x;
}

inline float wrapComplex( float x, float min, float max )
{
	float diff = max - min;
	while( x < min - diff )
	{
		x += diff;
	}
	while( x > max + diff )
	{
		x -= diff;
	}

	if( x < min )
	{
		return max - ( min - x );
	}
	if( x > max )
	{
		return min + ( x - max );
	}

	return x;
}


//new file management methods
void separateData( const char* dataString, std::vector< std::string >& dataStrings, std::vector< float >& weightsOfData );
void removeWhiteSpace( std::string& line, bool front );
void removeFrontAndBackWhiteSpace( std::string& textBlock );
std::vector< std::string > splitString( const std::string& dataString, const char& key );


template< typename real >
real clamp( real value, real min, real max )
{
	return value < min ? min : value > max ? max : value;
}

inline float GetMaxFloat(float a, float b)
{
	if (a >= b)
	{
		return a;
	}
	else
		return b;
}

inline float GetMinFloat(float a, float b)
{
	return (-GetMaxFloat(-a,-b));
}

inline int GetMaxInt(int a, int b)
{
	return (int)(GetMaxFloat(float(a),float(b)));
}

inline int GetMinInt(int a, int b)
{
	return (int)(GetMinFloat(float(a),float(b)));
}


template< typename fn_t >
inline void forEachDirection ( const fn_t& fn )
{
	// 0 <=> x, 1<=> y, 2 <=> z
	//
	for( int axis = 0; axis < 3; ++axis )
	{
		for( int sign = -1; sign <= 1; sign +=2 )
		{
			Vector3  normal;
			normal[ axis ] = (float) sign;

			fn( axis, sign, normal );
		}
	}
}

unsigned int toRGBA( float r, float g, float b, float a = 1.0f );

#endif